#include "SparkFun_Weather_Meter_Kit_Arduino_Library.h"

// Static member definitions
uint32_t SFEWeatherMeterKit::windSpeedMeasurementPeriodMillis;
float SFEWeatherMeterKit::kphPerCountPerSec;
uint32_t SFEWeatherMeterKit::minMillisPerRainfall;
float SFEWeatherMeterKit::_windSpeed;
uint32_t SFEWeatherMeterKit::_rainfallCounts;
uint32_t SFEWeatherMeterKit::_windCounts;
uint32_t SFEWeatherMeterKit::_lastWindSpeedMillis;
uint32_t SFEWeatherMeterKit::_lastRainfallMillis;

/// @brief Default constructor, sets default calibration values
SFEWeatherMeterKit::SFEWeatherMeterKit(int windDirectionPin, int windSpeedPin, int rainfallPin)
{
    // Set sensors pins
    _windDirectionPin = windDirectionPin;
    _windSpeedPin = windSpeedPin;
    _rainfallPin = rainfallPin;

    // Datasheet recommends a 10k pull up resistor, so that's what's assumed
    windDirPullUpVal = 10000;

    // Assuming an 8-bit ADC
    windDirMaxADC = 255;

    // The wind vane has 8 switces, but 2 could close at the same time,
    // resulting in 16 possible positions. The datasheet specifies all possible
    // resistance values and corresponding directions
    vaneResistances[WMK_ANGLE_0_0] = 33000;
    vaneResistances[WMK_ANGLE_22_5] = 6570;
    vaneResistances[WMK_ANGLE_45_0] = 8200;
    vaneResistances[WMK_ANGLE_67_5] = 891;
    vaneResistances[WMK_ANGLE_90_0] = 1000;
    vaneResistances[WMK_ANGLE_112_5] = 688;
    vaneResistances[WMK_ANGLE_135_0] = 2200;
    vaneResistances[WMK_ANGLE_157_5] = 1410;
    vaneResistances[WMK_ANGLE_180_0] = 3900;
    vaneResistances[WMK_ANGLE_202_5] = 3140;
    vaneResistances[WMK_ANGLE_225_0] = 16000;
    vaneResistances[WMK_ANGLE_247_5] = 14120;
    vaneResistances[WMK_ANGLE_270_0] = 120000;
    vaneResistances[WMK_ANGLE_292_5] = 42120;
    vaneResistances[WMK_ANGLE_315_0] = 64900;
    vaneResistances[WMK_ANGLE_337_5] = 21880;

    // Datasheet specifies 2.4kph of wind causes one trigger per second
    kphPerCountPerSec = 2.4;

    // Wind speed sampling interval. Longer durations have more accuracy, but
    // cause delay and can miss fast fluctuations
    windSpeedMeasurementPeriodMillis = 1000;

    // Datasheet specifies 0.2794mm of rain per trigger
    mmPerRainfallCount = 0.2794;

    // Debounce time for rainfall detector
    minMillisPerRainfall = 100;

    // Reset counters to zero
    _rainfallCounts = 0;
    _windCounts = 0;
    _windSpeed = 0;

    // Reset timers
    _lastWindSpeedMillis = millis();
    _lastRainfallMillis = millis();
}

/// @brief Sets up sensor pins
/// @param windDirectionPin Wind direction pin, must have an ADC
/// @param windSpeedPin Wind speed pin, must support interrupts
/// @param rainfallPin Rainfall pin, must support interrupts
void SFEWeatherMeterKit::begin()
{
    // Set pins to inputs
    pinMode(_windDirectionPin, INPUT);
    pinMode(_windSpeedPin, INPUT);
    pinMode(_rainfallPin, INPUT);

    // Attach interr_upt handlers
    attachInterrupt(digitalPinToInterrupt(_windSpeedPin), windSpeedInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(_rainfallPin), rainfallInterrupt, FALLING);
}

/// @brief Measures the resisance of the wind vane (assumes a voltage divider)
/// @return Measured resistance in Ohms
float SFEWeatherMeterKit::getVaneResistance()
{
    // The wind vane has 8 switches inside, each connected to a different
    // resistor going to ground. This library assumes there's an external pullup
    // resistor of a known value, creating a voltage divider like this circuit:
    //
    //              V_out
    //                |
    // V_in ----WMW---+----WMW--- GND
    //          R_up      R_vane
    //
    // By measuring V_out, we can calculate what R_vane is by using the voltage
    // divider equation:
    //
    // V_out = V_in * R_vane / (R_up + R_vane)
    //
    // Solving for R_vane, we get:
    //
    // R_vane = (V_out / V_in) * R_up / (1 - (V_out / V_in))

    // Measure V_out
    int rawADC = analogRead(_windDirectionPin);

    // Calculate the voltage ratio (V_out / V_in)
    float voltageRatio = (float)rawADC / windDirMaxADC;

    // Now calculate R_vane based on the equation above. Just need to make sure
    // we don't divide by zero!
    if ((1 - voltageRatio) != 0)
    {
        // Won't divide by zero, yay! Compute resistance from equation above
        return voltageRatio * windDirPullUpVal / (1 - voltageRatio);
    }
    else
    {
        // Uh oh, this would cause us to divide by zero! We'll just give a very
        // large number to be safe, 1M Ohm in this case
        return 1000000;
    }
}

/// @brief Measures the direction of the wind vane
/// @return Wind direction in degrees
float SFEWeatherMeterKit::getWindDirection()
{
    // The wind vane has different resistance values based on the direction it's
    // pointing, so first measure that resistance
    float vaneResistance = getVaneResistance();

    // Now we'll search through all the possible resistance values to find which
    // is closest to our measurement, using a simple linear search
    float closestResistance = 1000000;
    int closestIndex = 0;
    for (int i = 0; i < WMK_NUM_ANGLES; i++)
    {
        // Compute the difference between this resistance and what we measured
        float resistanceDifference = abs(vaneResistance - vaneResistances[i]);

        // Check if this different is less than our closest so far
        if (resistanceDifference < closestResistance)
        {
            // This resistance is closer, update closest resistance and index
            closestResistance = resistanceDifference;
            closestIndex = i;
        }
    }

    // Now compute the wind direction in degrees
    float direction = closestIndex * SFE_WIND_VANE_DEGREES_PER_INDEX;

    // Return direction in degrees
    return direction;
}

/// @brief Updates the wind speed measurement windows if needed
void SFEWeatherMeterKit::updateWindSpeed()
{
    // The anemometer generates interrupts as it spins. Because these are
    // discrete pulses, we can't get an instantaneous measurement of the wind
    // speed. Instead, we need to track these signals over time and perform some
    // filtering to get an estimate of the current wind speed. There's lots of
    // ways to do this, but this library uses a modifed version of a moving
    // window filter.
    //
    // A moving window filter would require an array of values to be stored,
    // indicating when each pulse occurred. However for a fixed time window, the
    // number of pulses is unknown, so we don't know how big the array needs to
    // be. There are some solutions to this, but the one used here is to change
    // the moving time window to a static time window, which is illustrated in
    // this timing diagram with variable time between pulses:
    //
    // Pulses     | |  |   |    |     |      |       |        |
    // Window                  Last window        Current window
    // Time       ------|-----------------------|----------------|
    //                                        t_last           t_now
    //                  |---Measurement Period--|---Measurement Period--|
    //
    // A counter is used to track the number of pulses detected in the current
    // measurement window; when pulses are detected, the counter is incremented.
    // When t_now exceeds the measurement period, the total number of pulses is
    // used to calculate the average wind speed for that window. This filter
    // only outputs wind speed for the previous window, which does result in
    // delayed measuremnts, but is fine for most data logging applications since
    // logs can be synced with the measurement widows

    // Get current time
    uint32_t tNow = millis();

    // Compute time since start of current measurement window
    uint32_t dt = tNow - _lastWindSpeedMillis;

    // Check how long it's been since the start of this measurement window
    if (dt < windSpeedMeasurementPeriodMillis)
    {
        // Still within the current window, nothing to do (count is not
        // incremented here, that's done by the interrupt handler)
    }
    else
    {
        // We've passed the end of the measurement window, so we need to update
        // some things. But first, we need to check how long it's been since the
        // last time we updated, since it's possible we've not received any
        // pulses for a long time
        if (dt > (windSpeedMeasurementPeriodMillis * 2))
        {
            // Over 2 measurement periods have passed since the last update,
            // meaning the wind speed is very slow or even zero. So we'll reset
            // the wind speed and counter, and set the start of the next window
            // to be now
            _windSpeed = 0;
            _windCounts = 0;
            _lastWindSpeedMillis = millis();
        }
        else
        {
            // We've only just gone past the end of the measurement period, so
            // calculate the wind speed for the previous window. First compute
            // the counts per millisecond
            _windSpeed = (float)_windCounts / windSpeedMeasurementPeriodMillis;

            // Convert milliseconds to seconds, and counts per second to kph
            _windSpeed *= 1000 * kphPerCountPerSec;

            // Reset counter, and update time of start of next mesaurement window
            _windCounts = 0;
            _lastWindSpeedMillis += windSpeedMeasurementPeriodMillis;
        }
    }
}

/// @brief Gets the mesaured wind speed
/// @return Measured wind speed in kph
float SFEWeatherMeterKit::getWindSpeed()
{
    // Check if the wind speed needs to be updated
    updateWindSpeed();

    // Return wind speed for the previous measurement interval
    return _windSpeed;
}

/// @brief Gets the number of wind speed counts
/// @return Number of wind speed counts
uint32_t SFEWeatherMeterKit::getWindSpeedCounts()
{
    // Return total wind speed counts
    return _windCounts;
}

/// @brief Gets the number of rainfall counts
/// @return Number of rainfall counts
uint32_t SFEWeatherMeterKit::getRainfallCounts()
{
    // Return total rainfall counts
    return _rainfallCounts;
}

/// @brief Gets the total rainfall
/// @return Total rainfall in mm
float SFEWeatherMeterKit::getTotalRainfall()
{
    // Return total rainfall in mm
    return _rainfallCounts * mmPerRainfallCount;
}

/// @brief Resets the total rainfall
void SFEWeatherMeterKit::resetWindSpeedFilter()
{
    _windSpeed = 0;
    _windCounts = 0;
    _lastWindSpeedMillis = millis();
}

/// @brief Resets the total rainfall
void SFEWeatherMeterKit::resetTotalRainfall()
{
    _rainfallCounts = 0;
}

/// @brief Interrupt handler for wind speed pin
void SFEWeatherMeterKit::windSpeedInterrupt()
{
    // Check if the measurement window needs to be updated
    updateWindSpeed();

    // Increment counts in this measurement window
    _windCounts++;
}

/// @brief Interrupt handler for rainfall pin
void SFEWeatherMeterKit::rainfallInterrupt()
{
    // Debounce by checking time since last interrupt
    if ((millis() - _lastRainfallMillis) < minMillisPerRainfall)
    {
        // There's not been enough time since the last interrupt, so this is
        // likely just the switch bouncing
        return;
    }

    // Enough time has passed that this is probably a real signal instead of a
    // bounce, so update the time of the last interrupt to be now
    _lastRainfallMillis = millis();

    // Increment counter
    _rainfallCounts++;
}