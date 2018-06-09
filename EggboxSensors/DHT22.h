#pragma once
class DHT22
{
public:
	DHT22(const unsigned char readPin);
	float temperature;
	float humidity;
	const unsigned char pin;
	String errorMessage;
	// Returns true if was able to read temperature & humidity, false otherwise
	bool read();
	void setup();
private:
	bool setupDone;
	enum DHT_Message { success = 0, timeoutError, checksumError };
	DHT_Message readDHTxx(byte data[5], unsigned long startTime, unsigned long timeout);
};

