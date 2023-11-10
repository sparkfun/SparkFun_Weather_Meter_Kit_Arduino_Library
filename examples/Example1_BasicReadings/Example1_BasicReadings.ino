#include "SparkFun_Weather_Meter_Kit_Arduino_Library.h"

// Below are the pin definitions for each sensor of the weather meter kit

// Pins for Weather Carrier with ESP32 Processor Board
int windDirectionPin = 35;
int windSpeedPin = 14;
int rainfallPin = 27;

// Pins for the Weather Shield with SparkFun RedBoard Qwiic or Arduino Uno
// int windDirectionPin = A0;
// int windSpeedPin = 3;
// int rainfallPin = 2;

// Create an instance of the weather meter kit
SFEWeatherMeterKit weatherMeterKit(windDirectionPin, windSpeedPin, rainfallPin);

void setup()
{
    // Begin serial
    Serial.begin(115200);
    Serial.println("SparkFun Weather Meter Kit Example 1 - Basic Readings");
    Serial.println();
    Serial.println("Note - this example demonstrates the minimum code required");
    Serial.println("for operation, and may not be accurate for your project.");
    Serial.println("It is recommended to check out the calibration examples.");

    // Expected ADC values have been defined for various platforms in the
    // library, however your platform may not be included. This code will check
    // if that's the case
#ifdef SFE_WMK_PLAFTORM_UNKNOWN
    // The platform you're using hasn't been added to the library, so the
    // expected ADC values have been calculated assuming a 10k pullup resistor
    // and a perfectly linear 16-bit ADC. Your ADC likely has a different
    // resolution, so you'll need to specify it here:
    weatherMeterKit.setADCResolutionBits(10);
    
    Serial.println("Unknown platform! Please edit the code with your ADC resolution!");
    Serial.println();
#endif

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
