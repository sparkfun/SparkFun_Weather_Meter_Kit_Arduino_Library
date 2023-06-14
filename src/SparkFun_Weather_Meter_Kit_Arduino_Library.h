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
    uint32_t windDirPullUpVal;
    uint32_t windDirMaxADC;
    float vaneResistances[WMK_NUM_ANGLES];

    // Rainfall
    float mmPerRainfallCount;
    uint32_t minMillisPerRainfall;

    // Wind speed
    uint32_t windSpeedMeasurementPeriodMillis;
    float kphPerCountPerSec;
};

class SFEWeatherMeterKit
{
  public:
    // Constructor
    SFEWeatherMeterKit(int windDirectionPin, int windSpeedPin, int rainfallPin);
    void begin();

    // Data collection
    float getWindDirection();
    float getWindSpeed();
    float getTotalRainfall();

    // Sensor calibration params
    SFEWeatherMeterKitCalibrationParams getCalibrationParams();
    void setCalibrationParams(SFEWeatherMeterKitCalibrationParams params);

    // Helper functions. These can be helpful for sensor calibration
    float getVaneResistance();
    uint32_t getWindSpeedCounts();
    uint32_t getRainfallCounts();
    void resetWindSpeedFilter();
    void resetTotalRainfall();

  private:
    // Updates wind speed
    static void updateWindSpeed();

    // Interrupt handlers
    static void windSpeedInterrupt();
    static void rainfallInterrupt();

    // Pins for each sensor
    int _windDirectionPin;
    int _windSpeedPin;
    int _rainfallPin;

    // Sensor calibration parameters
    static SFEWeatherMeterKitCalibrationParams _calibrationParams;

    // Reciprocal of values that need division
    static float _windDirMaxADCRecirpocal;
    static float _windSpeedMeasurementPeriodMillisRecirpocal;

    // Variables to track mesaurements
    static float _windSpeed;
    static uint32_t _rainfallCounts;
    static uint32_t _windCounts;
    static uint32_t _lastWindSpeedMillis;
    static uint32_t _lastRainfallMillis;
};

#endif