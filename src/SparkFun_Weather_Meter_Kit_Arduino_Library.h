#ifndef __SPARKFUN_WEATHER_METER_KIT_H__
#define __SPARKFUN_WEATHER_METER_KIT_H__

#include "Arduino.h"

// Enum to define the indexes for each wind direction
enum SFEWeatherMeterKitAngles
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

class SFEWeatherMeterKit
{
  public:
    // Constructor
    SFEWeatherMeterKit();
    void begin(int windDirectionPin, int windSpeedPin, int rainfallPin);

    // Data collection
    float getWindDirection();
    float getWindSpeed();
    float getTotalRainfall();

    // Helper functions
    float getVaneResistance();
    void resetTotalRainfall();

    // Interrupt handlers. These need to be public to be called correctly, but
    // should not be called anywhere other than the .cpp file
    void windSpeedInterrupt();
    void rainfallInterrupt();

    // Calibration settings for each of the sensors. These can be changed as
    // needed to calibrate each sensor
    int windDirPullUpVal;
    int windDirMaxADC;
    float vaneResistances[WMK_NUM_ANGLES];
    int windSpeedMeasurementPeriodMillis;
    float kphPerCountPerSec;
    float mmPerRainfallCount;
    float minMillisPerRainfall;

  private:
    // Updates wind speed
    void updateWindSpeed();

    // Pins for each sensor
    int _windDirectionPin;
    int _windSpeedPin;
    int _rainfallPin;

    // Variables to track mesaurements
    float _windSpeed;
    int _rainfallCounts;
    int _windCounts;
    uint32_t _lastWindSpeedMillis;
    uint32_t _lastRainfallMillis;
};

// This library uses interrupts. Because the interrupt handlers must be static,
// only 1 object can be created, which is done here
extern SFEWeatherMeterKit weatherMeterKit;

#endif