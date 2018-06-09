#include "DHT22.h"



DHT22::DHT22(const unsigned char readPin) : pin(readPin)
{
	temperature = 0.0f;
	humidity = 0.0f;
	setupDone = false;
}

// Returns true if was able to read temperature & humidity, false otherwise
bool DHT22::read()
{
	if (!setupDone)
		setup();
	byte data[5];
	DHT_Message ret = (DHT_Message)readDHTxx(data, 1, 1000);

	/* Checking communication errors */
	if (ret != success) {
		switch (ret) {
			case timeoutError: errorMessage = "Timeout reading DHT22 sensor"; break;
			case checksumError: errorMessage = "Corrupted data from DHT22 sensor"; break;
			default:errorMessage = "Unhandled DHT22 error";
		}
		return false;
	}

	/* Calculates the true humidity & temperature */
	humidity = data[0];
	humidity *= 256;
	humidity += data[1];
	humidity *= 0.1;

	temperature = data[2] & 0x7f;
	temperature *= 256;
	temperature += data[3];
	temperature *= 0.1;
	if (data[2] & 0x80) {
		temperature *= -1;
	}

	/* Ok */
	return true;
}

void DHT22::setup()
{
	pinMode(pin, INPUT_PULLUP);
	setupDone = true;
}

/**
* Low level function to read temperature & humidity (raw values) measured by the DHT22.
* Should also work with DHT11
*/
DHT22::DHT_Message DHT22::readDHTxx(byte data[5], unsigned long startTime, unsigned long timeout)
{
	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	//Might consider replacing byte as uint8_t
	//Convertion pinNumber to low level binary mask
	byte bit = digitalPinToBitMask(pin);
	byte port = digitalPinToPort(pin);
	volatile byte *ddr = (volatile uint8_t *)portModeRegister(port);	//Register MODE (I/O)
	volatile byte *out = (volatile uint8_t *)portOutputRegister(port);	//Register out (writing)
	volatile byte *in = (volatile uint8_t *)portInputRegister(port);	//Register IN (reading)

	unsigned long maxCycles = microsecondsToClockCycles(timeout);

	/*Avoids pull-up issues */
	*out |= bit;  // PULLUP
	*ddr &= ~bit; // INPUT
	delay(100);   // Lets the pull-up resistor put the data line to HIGH

				  /* Awaking the sensor */
	*ddr |= bit;  // OUTPUT
	*out &= ~bit; // LOW
	delay(startTime);	// Wait time to LOW awakening the sensor
						// N.B. It is NOT possible to use delayMicroseconds() here as a delay of more than
						// 16 milliseconds won't give a precise enough timing.

						/* Critical code section - No interruption possible */
	noInterrupts();

	/* Listening */
	*out |= bit;  // PULLUP
	delayMicroseconds(40);
	*ddr &= ~bit; // INPUT

				  /* Awaiting sensor answer */
	timeout = 0;
	while (!(*in & bit)) { /* Waiting for a LOW state */
		if (++timeout == maxCycles) {
			interrupts();
			return timeoutError;
		}
	}

	timeout = 0;
	while (*in & bit) { /* Waiting for a HIGH state */
		if (++timeout == maxCycles) {
			interrupts();
			return timeoutError;
		}
	}

	/* Reading sensor data (40 bits) */
	for (byte i = 0; i < 40; ++i) {

		/* Waiting for a LOW state */
		unsigned long cyclesLow = 0;
		while (!(*in & bit)) {
			if (++cyclesLow == maxCycles) {
				interrupts();
				return timeoutError;
			}
		}

		/* Waiting for a HIGH state */
		unsigned long cyclesHigh = 0;
		while (*in & bit) {
			if (++cyclesHigh == maxCycles) {
				interrupts();
				return timeoutError;
			}
		}

		/* If HIGH tile is greater than LOW time, it's a '1', else, it's a 0 */
		data[i / 8] <<= 1;
		if (cyclesHigh > cyclesLow) {
			data[i / 8] |= 1;
		}
	}

	/* Critical code section end */
	interrupts();

	/*
	* Data formatting :
	* [1, 0] = humidity in %
	* [3, 2] = temperature in Celsius degrees
	* [4] = checksum (humidity + temperature)
	*/

	/* Cheecking the checksum */
	byte checksum = (data[0] + data[1] + data[2] + data[3]) & 0xff;
	if (data[4] != checksum)
		return checksumError;
	return success;
}