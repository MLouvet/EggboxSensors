#ifndef NETWORK_H
#define NETWORK_H

#define MQTT_AIR "sensor/air"
#define MQTT_TEMPERATURE "sensor/3"
#define MQTT_HUMIDITY "sensor/humidity"
#define MQTT_SERVER "192.168.137.1"


#define RTC

enum DHT_Message { success, timeoutError, checksumError };
https://github.com/256dpi/arduino-mqtt/blob/master/examples/AdafruitHuzzahESP8266/AdafruitHuzzahESP8266.ino
const bool debug = true;
char messageBuffer[100];
long lastMsg = 0;   //Horodatage du dernier message publié sur MQTT
long lastRecu = 0;
int waitTimes[] = { 5, 30, 60, 300, 600, 1800, 3600 };

typedef struct rtcStore rtcStore;
struct rtcStore
{
	int waitTimeIndex = 0;
};


#endif
