#include "GP2Y1010.h"



GP2Y1010::GP2Y1010(const unsigned char analogPin, const unsigned char dataPin): analogPin(analogPin), dataPin(dataPin)
{
	setupDone = false;
}

void GP2Y1010::setup()
{
	pinMode(dataPin, OUTPUT);
	setupDone = true;
}

void GP2Y1010::read(int numberOfTries)
{
	dustDensity = 0;
	do
	{
		read();
	}
	while (dustDensity == 0 && --numberOfTries);
}

void GP2Y1010::read()
{
	if (!setupDone)
		setup();

	float voltageOutputMeasured;
		digitalWrite(AIR_SENSOR_DIGITAL_PIN, LOW); //Power on the led inside the air quality sensor
		delayMicroseconds(AIR_SENSOR_SAMPLING_TIME);
		voltageOutputMeasured = analogRead(AIR_SENSOR_ANALOG_PIN);

		delayMicroseconds(AIR_SENSOR_DELTA_TIME);
		digitalWrite(AIR_SENSOR_DIGITAL_PIN, HIGH); //Turning off the led of the air sensor
		delayMicroseconds(AIR_SENSOR_SLEEP_TIME);

		float calcVoltage = voltageOutputMeasured * (5.0 / 1024);
		// linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
		// Chris Nafis (c) 2012
		if (calcVoltage >= 0.583)
			dustDensity = 0.17 * calcVoltage - 0.1;
		//if (debug)
		//{
		//	Serial.print("Raw Signal Value (0-1023): ");
		//	Serial.print(voltageOutputMeasured);

		//	Serial.print(" - Voltage: ");
		//	Serial.print(calcVoltage);

		//	Serial.print(" - Dust Density: ");
		//	Serial.println(dustDensity);
		//}
}
