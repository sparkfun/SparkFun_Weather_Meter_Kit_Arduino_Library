#include "SparkFun_Weather_Meter_Kit_Arduino_Library.h"

// Pins for each sensor of the weather meter kit
int windDirectionPin = 35;
int windSpeedPin = 14;
int rainfallPin = 27;

// Create an instance of the weather meter kit
SFEWeatherMeterKit weatherMeterKit(windDirectionPin, windSpeedPin, rainfallPin);

void setup()
{
    // Begin serial
    Serial.begin(115200);
    Serial.println("SparkFun Weather Meter Kit Example 2 - Manual Calibration");
    Serial.println();
    Serial.println("Note - this example demonstrates how to manually set the");
    Serial.println("calibration parameters once you know what they are for your");
    Serial.println("set up. If you don't know what values to use, check out");
    Serial.println("Example 3, which walks you through it! The values used in");
    Serial.println("this example are all defaults, so you may need to change them.");

    // Here we create a struct to hold all the calibration parameters
    SFEWeatherMeterKitCalibrationParams calibrationParams;

    // The wind vane uses a voltage divider to measure the direction, so the
    // max value of the ADC needs to be known. It defaults to 255 (8-bit ADC)
    calibrationParams.windDirMaxADC = 255;

    // In the voltage divider circuit, the wind vane has internal resistors that
    // connect to ground, meaning your circuit requires a resistor connected to
    // VCC. The value of that high-side resistor needs to be known. It defaults
    // to 10k Ohm, which is the same as what's recommended by the datasheet
    calibrationParams.windDirPullUpVal = 10000;

    // The wind vane has 8 switces, but 2 could close at the same time,
    // resulting in 16 possible positions. Some of these resistor values are
    // fairly close to each other, meaning an accurate ADC is required. However
    // some ADCs have a non-linear behavior that causes this measurement to be
    // inaccurate. To account for this, the vane resistor values can be manually
    // changed here to compensate for the non-linear behavior of the ADC
    calibrationParams.vaneResistances[WMK_ANGLE_0_0] = 33000;
    calibrationParams.vaneResistances[WMK_ANGLE_22_5] = 6570;
    calibrationParams.vaneResistances[WMK_ANGLE_45_0] = 8200;
    calibrationParams.vaneResistances[WMK_ANGLE_67_5] = 891;
    calibrationParams.vaneResistances[WMK_ANGLE_90_0] = 1000;
    calibrationParams.vaneResistances[WMK_ANGLE_112_5] = 688;
    calibrationParams.vaneResistances[WMK_ANGLE_135_0] = 2200;
    calibrationParams.vaneResistances[WMK_ANGLE_157_5] = 1410;
    calibrationParams.vaneResistances[WMK_ANGLE_180_0] = 3900;
    calibrationParams.vaneResistances[WMK_ANGLE_202_5] = 3140;
    calibrationParams.vaneResistances[WMK_ANGLE_225_0] = 16000;
    calibrationParams.vaneResistances[WMK_ANGLE_247_5] = 14120;
    calibrationParams.vaneResistances[WMK_ANGLE_270_0] = 120000;
    calibrationParams.vaneResistances[WMK_ANGLE_292_5] = 42120;
    calibrationParams.vaneResistances[WMK_ANGLE_315_0] = 64900;
    calibrationParams.vaneResistances[WMK_ANGLE_337_5] = 21880;

    // The rainfall detector contains a small cup that collects rain water. When
    // the cup fills, the water is dumped and the total rainfall is incremented
    // by some value. This value defaults to 0.2794mm of rain per count, as
    // specified by the datasheet
    calibrationParams.mmPerRainfallCount = 0.2794;

    // The rainfall detector switch can sometimes bounce, causing multiple extra
    // triggers. This input is debounced by ignoring extra triggers within a
    // time window, which defaults to 100ms
    calibrationParams.minMillisPerRainfall = 100;

    // The anemometer contains a switch that opens and closes as it spins. The
    // rate at which the switch closes depends on the wind speed. The datasheet
    // states that a wind of 2.4kph causes the switch to close once per second
    calibrationParams.kphPerCountPerSec = 2.4;

    // Because the anemometer generates discrete pulses as it rotates, it's not
    // possible to measure the wind speed exactly at any point in time. A filter
    // is implemented in the library that averages the wind speed over a certain
    // time period, which defaults to 1 second. Longer intervals result in more
    // accurate measurements, but cause delay in the measurement
    calibrationParams.windSpeedMeasurementPeriodMillis = 1000;

    // Now we can set all the calibration parameters at once
    weatherMeterKit.setCalibrationParams(calibrationParams);

    // Begin weather meter kit
    weatherMeterKit.begin();
}

void loop()
{
    // Print data from weather meter kit
    Serial.print("Wind direction (degrees): ");
    Serial.print(weatherMeterKit.getWindDirection(), 1);
    Serial.print("\t\t");
    Serial.print("Wind speed (kph): ");
    Serial.print(weatherMeterKit.getWindSpeed(), 1);
    Serial.print("\t\t");
    Serial.print("Total rainfall (mm): ");
    Serial.println(weatherMeterKit.getTotalRainfall(), 1);

    // Only print once per second
    delay(1000);
}