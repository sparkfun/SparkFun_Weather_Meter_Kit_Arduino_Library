#include "SparkFun_Weather_Meter_Kit_Arduino_Library.h"

// Pins for each sensor of the weather meter kit
int windDirectionPin = 35;
int windSpeedPin = 14;
int rainfallPin = 27;

// Pins for the Weather Shield
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

    // The library assumes a 12-bit ADC, but if yours is different, you can set
    // the resolution here
    weatherMeterKit.setADCResolutionBits(12);

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
