#include "SparkFun_Weather_Meter_Kit_Arduino_Library.h"

// Static member definitions
SFEWeatherMeterKitCalibrationParams SFEWeatherMeterKit::_calibrationParams;
uint32_t SFEWeatherMeterKit::_windCountsPrevious;
uint32_t SFEWeatherMeterKit::_windCounts;
uint32_t SFEWeatherMeterKit::_rainfallCounts;
uint32_t SFEWeatherMeterKit::_lastWindSpeedMillis;
uint32_t SFEWeatherMeterKit::_lastRainfallMillis;
int SFEWeatherMeterKit::_windDirectionPin;
int SFEWeatherMeterKit::_windSpeedPin;
int SFEWeatherMeterKit::_rainfallPin;

/// @brief Default constructor, sets default calibration values
SFEWeatherMeterKit::SFEWeatherMeterKit(int windDirectionPin, int windSpeedPin, int rainfallPin)
{
    // Set sensors pins
    _windDirectionPin = windDirectionPin;
    _windSpeedPin = windSpeedPin;
    _rainfallPin = rainfallPin;

    // The wind vane has 8 switces, but 2 could close at the same time, which
    // results in 16 possible positions. The datasheet specifies the resistance
    // for each direction, which were used to calculate the expected ADC values
    // for a 12-bit ADC (4095 max) with a 10k pullup
    _calibrationParams.vaneADCValues[WMK_ANGLE_0_0] = 3143;
    _calibrationParams.vaneADCValues[WMK_ANGLE_22_5] = 1624;
    _calibrationParams.vaneADCValues[WMK_ANGLE_45_0] = 1845;
    _calibrationParams.vaneADCValues[WMK_ANGLE_67_5] = 335;
    _calibrationParams.vaneADCValues[WMK_ANGLE_90_0] = 372;
    _calibrationParams.vaneADCValues[WMK_ANGLE_112_5] = 264;
    _calibrationParams.vaneADCValues[WMK_ANGLE_135_0] = 738;
    _calibrationParams.vaneADCValues[WMK_ANGLE_157_5] = 506;
    _calibrationParams.vaneADCValues[WMK_ANGLE_180_0] = 1149;
    _calibrationParams.vaneADCValues[WMK_ANGLE_202_5] = 979;
    _calibrationParams.vaneADCValues[WMK_ANGLE_225_0] = 2520;
    _calibrationParams.vaneADCValues[WMK_ANGLE_247_5] = 2397;
    _calibrationParams.vaneADCValues[WMK_ANGLE_270_0] = 3780;
    _calibrationParams.vaneADCValues[WMK_ANGLE_292_5] = 3309;
    _calibrationParams.vaneADCValues[WMK_ANGLE_315_0] = 3548;
    _calibrationParams.vaneADCValues[WMK_ANGLE_337_5] = 2810;

    // Datasheet specifies 2.4kph of wind causes one trigger per second
    _calibrationParams.kphPerCountPerSec = 2.4;

    // Wind speed sampling interval. Longer durations have more accuracy, but
    // cause delay and can miss fast fluctuations
    _calibrationParams.windSpeedMeasurementPeriodMillis = 1000;

    // Datasheet specifies 0.2794mm of rain per trigger
    _calibrationParams.mmPerRainfallCount = 0.2794;

    // Debounce time for rainfall detector
    _calibrationParams.minMillisPerRainfall = 100;

    // Reset counters to zero
    _windCountsPrevious = 0;
    _windCounts = 0;
    _rainfallCounts = 0;

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
    pinMode(_windSpeedPin, INPUT_PULLUP);
    pinMode(_rainfallPin, INPUT_PULLUP);

    // Attach interr_upt handlers
    attachInterrupt(digitalPinToInterrupt(_windSpeedPin), windSpeedInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(_rainfallPin), rainfallInterrupt, RISING);
}

/// @brief Gets the current calibration parameters
/// @return Current calibration parameters
SFEWeatherMeterKitCalibrationParams SFEWeatherMeterKit::getCalibrationParams()
{
    return _calibrationParams;
}

/// @brief Sets the new calibration parameters
/// @param params New calibration parameters
void SFEWeatherMeterKit::setCalibrationParams(SFEWeatherMeterKitCalibrationParams params)
{
    // Copy the provided calibration parameters
    memcpy(&_calibrationParams, &params, sizeof(SFEWeatherMeterKitCalibrationParams));
}

/// @brief Adjusts the expected ADC values for the wind vane based on the
/// provided ADC resolution
/// @param resolutionBits Resolution of ADC in bits (eg. 8-bit, 12-bit, etc.)
void SFEWeatherMeterKit::setADCResolutionBits(uint8_t resolutionBits)
{
    for(int i = 0; i < WMK_NUM_ANGLES; i++)
    {
        int8_t bitShift = (SFE_WIND_VANE_ADC_RESOLUTION_DEFAULT) - resolutionBits;

        if(bitShift > 0)
        {
            _calibrationParams.vaneADCValues[i] >>= bitShift;
        }
        else if(bitShift < 0)
        {
            _calibrationParams.vaneADCValues[i] <<= -bitShift;
        }
    }
}

/// @brief Measures the direction of the wind vane
/// @return Wind direction in degrees
float SFEWeatherMeterKit::getWindDirection()
{
    // Measure the output of the voltage divider
    int rawADC = analogRead(_windDirectionPin);

    // Now we'll loop through all possible directions to find which is closest
    // to our measurement, using a simple linear search
    int closestDifference = 1000000;
    int closestIndex = 0;
    for (int i = 0; i < WMK_NUM_ANGLES; i++)
    {
        // Compute the difference between the ADC value for this direction and
        // what we measured
        int adcDifference = abs((int)_calibrationParams.vaneADCValues[i] - rawADC);

        // Check if this different is less than our closest so far
        if (adcDifference < closestDifference)
        {
            // This resistance is closer, update closest resistance and index
            closestDifference = adcDifference;
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
    if (dt < _calibrationParams.windSpeedMeasurementPeriodMillis)
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
        if (dt > (_calibrationParams.windSpeedMeasurementPeriodMillis * 2))
        {
            // Over 2 measurement periods have passed since the last update,
            // meaning the wind speed is very slow or even zero. So we'll reset
            // the wind speed and counter, and set the start of the next window
            // to be now
            _windCountsPrevious = 0;
            _windCounts = 0;
            _lastWindSpeedMillis = tNow;
        }
        else
        {
            // We've only just gone past the end of the measurement period, so
            // save the wind counts for the previous window, reset current
            // counter, and update time of start of next mesaurement window
            _windCountsPrevious = _windCounts;
            _windCounts = 0;
            _lastWindSpeedMillis += _calibrationParams.windSpeedMeasurementPeriodMillis;
        }
    }
}

/// @brief Gets the mesaured wind speed
/// @return Measured wind speed in kph
float SFEWeatherMeterKit::getWindSpeed()
{
    // Check if the wind speed needs to be updated
    updateWindSpeed();

    // Calculate the wind speed for the previous window. First compute the
    // counts per millisecond
    float windSpeed = (float) _windCountsPrevious / _calibrationParams.windSpeedMeasurementPeriodMillis;

    // Convert milliseconds to seconds, and counts per second to kph
    windSpeed *= 1000 * _calibrationParams.kphPerCountPerSec;

    // Return wind speed for the previous measurement interval
    return windSpeed;
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
    return _rainfallCounts * _calibrationParams.mmPerRainfallCount;
}

/// @brief Resets the total rainfall
void SFEWeatherMeterKit::resetWindSpeedFilter()
{
    _windCountsPrevious = 0;
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
    if ((millis() - _lastRainfallMillis) < _calibrationParams.minMillisPerRainfall)
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