#pragma once
#define AIR_SENSOR_SAMPLING_TIME 280
#define AIR_SENSOR_DELTA_TIME 40
#define AIR_SENSOR_SLEEP_TIME 9680
class GP2Y1010
{
public:
	GP2Y1010(const unsigned char analogPin, const unsigned char dataPin);
	float dustDensity;
	String errorMessage;
	void setup();
	//Tries to read data a number of times until reading a non-zero value.
	void read(int numberOfTries);
private:
	bool setupDone;
	void read();
	const unsigned char dataPin;
	const unsigned char analogPin;
};

