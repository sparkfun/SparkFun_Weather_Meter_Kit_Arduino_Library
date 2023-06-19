#ifndef __SPARKFUN_WEATHER_METER_KIT_H__
#define __SPARKFUN_WEATHER_METER_KIT_H__

#include "Arduino.h"

// Enum to define the indexes for each wind direction
enum SFEWeatherMeterKitAnemometerAngles
{
    WMK_ANGLE_0_0 = 0,
    WMK_ANGLE_22_5,
    WMK_ANGLE_45_0,
    WMK_ANGLE_67_5,
    WMK_ANGLE_90_0,
    WMK_ANGLE_112_5,
    WMK_ANGLE_135_0,
    WMK_ANGLE_157_5,
    WMK_ANGLE_180_0,
    WMK_ANGLE_202_5,
    WMK_ANGLE_225_0,
    WMK_ANGLE_247_5,
    WMK_ANGLE_270_0,
    WMK_ANGLE_292_5,
    WMK_ANGLE_315_0,
    WMK_ANGLE_337_5,
    WMK_NUM_ANGLES
};

#define SFE_WIND_VANE_DEGREES_PER_INDEX (360.0 / WMK_NUM_ANGLES)

// Calibration parameters for each sensor
struct SFEWeatherMeterKitCalibrationParams
{
    // Wind vane
    uint32_t vaneADCValues[WMK_NUM_ANGLES];

    // Wind speed
    uint32_t windSpeedMeasurementPeriodMillis;
    float kphPerCountPerSec;

    // Rainfall
    float mmPerRainfallCount;
    uint32_t minMillisPerRainfall;
};

class SFEWeatherMeterKit
{
  public:
    // Constructor
    SFEWeatherMeterKit(int windDirectionPin, int windSpeedPin, int rainfallPin);
    static void begin();

    // Data collection
    static float getWindDirection();
    static float getWindSpeed();
    static float getTotalRainfall();

    // Sensor calibration params
    static SFEWeatherMeterKitCalibrationParams getCalibrationParams();
    static void setCalibrationParams(SFEWeatherMeterKitCalibrationParams params);

    // Helper functions. These can be helpful for sensor calibration
    static uint32_t getWindSpeedCounts();
    static uint32_t getRainfallCounts();
    static void resetWindSpeedFilter();
    static void resetTotalRainfall();

  private:
    // Updates wind speed
    static void updateWindSpeed();

    // Interrupt handlers
    static void windSpeedInterrupt();
    static void rainfallInterrupt();

    // Pins for each sensor
    static int _windDirectionPin;
    static int _windSpeedPin;
    static int _rainfallPin;

    // Sensor calibration parameters
    static SFEWeatherMeterKitCalibrationParams _calibrationParams;

    // Variables to track mesaurements
    static uint32_t _windCounts;
    static uint32_t _windCountsPrevious;
    static uint32_t _rainfallCounts;
    static uint32_t _lastWindSpeedMillis;
    static uint32_t _lastRainfallMillis;
};

#endif