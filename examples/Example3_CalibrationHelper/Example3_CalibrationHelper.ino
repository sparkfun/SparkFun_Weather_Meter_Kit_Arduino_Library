#include "SparkFun_Weather_Meter_Kit_Arduino_Library.h"

// Pins for each sensor of the weather meter kit
int windDirectionPin = 35;
int windSpeedPin = 14;
int rainfallPin = 27;

// Create an instance of the weather meter kit
SFEWeatherMeterKit weatherMeterKit(windDirectionPin, windSpeedPin, rainfallPin);

// Here we create a struct to hold all the calibration parameters
SFEWeatherMeterKitCalibrationParams calibrationParams;

void setup()
{
    // Begin serial
    Serial.begin(115200);
    Serial.println("SparkFun Weather Meter Kit Example 3 - Calibration Helper");

    // We'll be changing the calibration parameters one at a time, so we'll get
    // all the default values now
    calibrationParams = weatherMeterKit.getCalibrationParams();

    // Begin weather meter kit
    weatherMeterKit.begin();

    // Run the calibration helper
    runCalibrationHelper();

    Serial.println();
    Serial.println("Calibration done! Enter any key to continue");
    waitForUserInput();
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

void runCalibrationHelper()
{
    // Run the helpers for each sensor
    runVaneCalibrationHelper();
    runRainfallCalibrationHelper();
    runAnemometerCalibrationHelper();
}

void runVaneCalibrationHelper()
{
    Serial.println();
    Serial.println("Wind vane calibration!");

    // Pull up resistor value
    Serial.println();
    Serial.println("The wind vane has several switches, each with different resistors");
    Serial.println("connected to GND. This library assumes there's an external");
    Serial.println("pull up resistor of known value creating a voltage divider,");
    Serial.println("which allows the internal resistor value to be measured. Enter");
    Serial.println("the pull up resistor value in Ohms (enter 10000 if not sure)");
    waitForUserInput();
    calibrationParams.windDirPullUpVal = Serial.parseInt();

    // Max ADC value
    Serial.println();
    Serial.println("The output of the voltage divider is measured with the ADC,");
    Serial.println("so the resolution needs to be known. Enter the max value of");
    Serial.println("your microcontroller's ADC (eg. for an 8-bit ADC, enter 255)");
    waitForUserInput();
    calibrationParams.windDirMaxADC = Serial.parseInt();

    // Set the pull up resistor and max ADC values
    weatherMeterKit.setCalibrationParams(calibrationParams);

    Serial.println();
    Serial.println("Some of the wind vane resistor values are quite close to each");
    Serial.println("other. If your ADC behaves non-linearly, this can result in");
    Serial.println("incorrect resistance measurements. This can be compensated by");
    Serial.println("setting the expected resistance for each angle, which this part");
    Serial.println("will walk you through. Hold the wind vane at the specified");
    Serial.println("angle, then enter any key once steady. Pay close attention to");
    Serial.println("the measured resistance to see when it changes, especially");
    Serial.println("around the 22.5 degree increments (they're very narrow)!");
    Serial.println("Enter any key to begin.");

    // Wait for user to begin
    waitForUserInput();

    // Loop through all angles
    for (int i = 0; i < WMK_NUM_ANGLES; i++)
    {
        // Compute current angle
        float currentAngle = i * (360.0 / WMK_NUM_ANGLES);

        // Loop until user requests to continue
        clearUserInput();
        while (Serial.available() == 0)
        {
            Serial.print("Hold wind vane at ");
            Serial.print(currentAngle, 1);
            Serial.print(" degrees. Enter any key when in position.");
            Serial.print("    Measured resistance (Ohms): ");
            Serial.print(weatherMeterKit.getVaneResistance(), 1);
            Serial.print("    Measured direction (degrees): ");
            Serial.println(weatherMeterKit.getWindDirection(), 1);

            // Print moderately quickly so user can catch any brief changes
            delay(100);
        }

        // Set this as the new expected resistance for this angle
        float measuredResistance = weatherMeterKit.getVaneResistance();
        calibrationParams.vaneResistances[i] = measuredResistance;
        weatherMeterKit.setCalibrationParams(calibrationParams);

        // Print value for user to see
        Serial.println();
        Serial.print("Setting expected resistance for ");
        Serial.print(currentAngle);
        Serial.print(" degrees to ");
        Serial.print(measuredResistance, 0);
        Serial.println(" Ohms");
        Serial.println();

        // Wait a bit so user can read it
        delay(1000);
    }

    // Print the resistance saved for each angle again so the user has it all in
    // one place
    Serial.println();
    Serial.println("Here are the resistances set for each angle:");
    Serial.println();
    for (int i = 0; i < WMK_NUM_ANGLES; i++)
    {
        // Compute current angle
        float currentAngle = i * (360.0 / WMK_NUM_ANGLES);

        // Print this angle / resistance pair
        Serial.print(currentAngle, 1);
        Serial.print(" degrees: ");
        Serial.print(calibrationParams.vaneResistances[i]);
        Serial.println(" Ohms");
    }

    Serial.println();
    Serial.println("Wind vane calibration complete!");
}

void runRainfallCalibrationHelper()
{
    Serial.println();
    Serial.println("Rainfall calibration!");

    // Rainfall calibration
    Serial.println();
    Serial.println("The rainfall detector contains a small cup that collects rain");
    Serial.println("water. When the cup fills, the water gets dumped out and a");
    Serial.println("counter is incremented. The exact volume of this cup needs to");
    Serial.println("be known to get an accurate measurement of the total rainfall.");
    Serial.println("To calibrate this value, you'll need to pour a known volume");
    Serial.println("of water into the rainfall detector, and the cup volume will");
    Serial.println("be calculated. The rate at which the water is poured can");
    Serial.println("affect the measurement, so go very slowly to simulate actual");
    Serial.println("rain rather than dumping it all at once!");
    Serial.println("Enter any key once you're ready to begin");

    // Wait for user to begin
    waitForUserInput();

    // User is ready, reset the rainfall counter
    weatherMeterKit.resetTotalRainfall();

    Serial.println();
    Serial.println("Begin pouring!");
    Serial.println();

    // Wait for user to finish
    clearUserInput();
    while (Serial.available() == 0)
    {
        Serial.print("Enter any key once finished pouring.");
        Serial.print("    Number of counts: ");
        Serial.print(weatherMeterKit.getRainfallCounts());
        Serial.print("    Measured rainfall (mm): ");
        Serial.println(weatherMeterKit.getTotalRainfall(), 1);

        // Print slowly
        delay(1000);
    }

    Serial.println();
    Serial.println("Now enter the volume of water poured in mL");
    waitForUserInput();
    int totalWaterML = Serial.parseInt();

    // Convert ml to mm^3
    int totalWaterMM3 = totalWaterML * 1000;

    // Divide by collection area of rainfall detector. It's about 50mm x 110mm,
    // resulting in a collection area of about 550mm^2
    float totalRainfallMM = totalWaterMM3 / 550.0;

    // Divide by number of counts
    float mmPerCount = totalRainfallMM / weatherMeterKit.getRainfallCounts();

    // Set this as the new mm per count
    calibrationParams.mmPerRainfallCount = mmPerCount;
    weatherMeterKit.setCalibrationParams(calibrationParams);

    // Print value for user to see
    Serial.println();
    Serial.print("Setting mm per count to ");
    Serial.println(mmPerCount, 4);

    Serial.println();
    Serial.println("Rainfall calibration complete!");
}

void runAnemometerCalibrationHelper()
{
    Serial.println();
    Serial.println("Anemometer calibration!");
    
    Serial.println();
    Serial.println("This part will require you to place the anemometer in a");
    Serial.println("constant wind stream for a few seconds, and you'll need to");
    Serial.println("know or the wind speed or measure it with a calibrated");
    Serial.println("anemometer (these can be purchased for relatively low cost).");
    Serial.println("Enter the number of seconds you wish to run this calibration.");
    Serial.println("Longer will be more accurate, but the wind speed is more");
    Serial.println("likely to fluctuate (10 seconds is recommended)");
    waitForUserInput();
    int calibrationSeconds = Serial.parseInt();

    // Set filter measurement period as requested
    calibrationParams.windSpeedMeasurementPeriodMillis = 1000 * calibrationSeconds;
    weatherMeterKit.setCalibrationParams(calibrationParams);
    
    Serial.println();
    Serial.println("Now place the anemometer in a constant wind stream, and");
    Serial.println("enter any key when ready to begin calibration");
    waitForUserInput();

    // Reset the wind speed filter to start the calibration period
    weatherMeterKit.resetWindSpeedFilter();

    // Wait for calibration period to end
    Serial.println();
    for(int i = 0; i < calibrationSeconds; i++)
    {
        // Print time remaining
        Serial.print("Seconds remaining: ");
        Serial.println(calibrationSeconds - i);

        // 1 second intervals
        delay(1000);
    }

    // Wait just a bit longer to make sure the filter window has passed
    delay(500);

    // Store total number of wind speed counts
    uint32_t windCounts = weatherMeterKit.getWindSpeedCounts();
    
    // Reset measurement period back to default
    calibrationParams.windSpeedMeasurementPeriodMillis = 1000;
    
    Serial.println();
    Serial.println("Calibration period finished! Enter the average wind speed");
    Serial.println("during the calibration period in kph");
    waitForUserInput();
    float windSpeed = Serial.parseFloat();

    // Calculate kph per count per second
    calibrationParams.kphPerCountPerSec = windSpeed * windCounts / calibrationSeconds;
    weatherMeterKit.setCalibrationParams(calibrationParams);

    // Print value for user to see
    Serial.println();
    Serial.print("Setting kph per count per second to ");
    Serial.println(calibrationParams.kphPerCountPerSec, 2);

    Serial.println();
    Serial.println("Anemometer calibration complete!");
}

void clearUserInput()
{
    // Ensure all previous characters have come through
    delay(100);

    // Throw away all previous charaters
    while (Serial.available() != 0)
    {
        Serial.read();
    }
}

void waitForUserInput()
{
    // Remove previous user input
    clearUserInput();

    // Wait for user to input something
    while (Serial.available() == 0)
    {
        // Nothing to do, keep waiting
    }
}