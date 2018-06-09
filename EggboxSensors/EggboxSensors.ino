#include <system.h>
#include <MQTTClient.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#include "DHT22.h"
#include "Network.h"
#include "GP2Y1010.h"

#define SECONDS_TO_MICROSECONDS 1000000
#define AIR_SENSOR_ANALOG_PIN A0
#define AIR_SENSOR_DIGITAL_PIN 0
#define DHT_SENSOR_PIN 2

DHT22 dht(DHT_SENSOR_PIN);
GP2Y1010 airSensor(AIR_SENSOR_ANALOG_PIN, AIR_SENSOR_DIGITAL_PIN);

void setup()
{
	//Setup Wi-Fi
	//Setup DHT 22
	//Setup GP2Y1010
	/* add setup code here */

	Serial.begin(115200);
	dht.setup();
	airSensor.setup();
	pinMode(LED_PIN, OUTPUT);
	pinMode(AIR_SENSOR_DIGITAL_PIN, OUTPUT);
	setupWifi();           //On se connecte au r�seau wifi
	client.setServer(MQTT_SERVER, 1883);    //Configuration de la connexion au serveur MQTT
	client.setCallback(callback);  //La fonction de callback qui est execut�e � chaque r�ception de message   
	if (debug)
		Serial.println("Setup - WiFi station only");
}



void loop()
{
	dht.read();

	//Reading temperature & humidity + error handling
	switch (readDHT22(DHT_SENSOR_PIN, &temperature, &humidity)) {
		case success:
			//Do things with the temperature here	
			Serial.print("Humidity (%): ");
			Serial.println(humidity, 2);
			Serial.print("Temperature (C): ");
			Serial.println(temperature, 2);
			if (!client.connected()) {
				reconnect();
			}
			client.loop();
			//Envoi d'un message par minute
			//if (nowTime - lastMsg > 1000 * 60) {
			//lastMsg = nowTime;
			//Lecture de la temp�rature ambiante
			{
				StaticJsonBuffer<200> jsonBuffer;
				JsonObject& root = jsonBuffer.createObject();
				// INFO: the data must be converted into a string; a problem occurs when using floats...
				root["temperature"] = (String)temperature;
				root["humidity"] = (String)humidity;
				root["particulates"] = (String)dustDensity;
				root.prettyPrintTo(Serial);
				Serial.println("");
				/*
				{
				"temperature": "23.20" ,
				"humidity": "43.70"
				}
				*/
				char data[200];
				root.printTo(data, root.measureLength() + 1);
				client.publish(MQTT_TEMPERATURE, data, true);
			}
			//if (now - lastRecu > 100) {
			//	lastRecu = now;
			//	client.subscribe("sensor/light");
			//}
			if (debug)
				Serial.println("Entering deepSleep for 60 seconds");
			//WiFi.disconnect(true);
			ESP.deepSleep(60 * SECONDS_TO_MICROSECONDS);
			delay(1000);
			break;
		case timeoutError:
			Serial.println("No response from sensor");
			break;
		case checksumError:
			Serial.println("Communication error with the sensor");
			break;
		default:
			Serial.println("Not handled value");
	}
	//DeepSleep for 60 microseconds;
}
