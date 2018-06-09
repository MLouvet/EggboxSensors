#include <MQTT.h>
#include <ESP8266WiFi.h>
#include "MAC_Identification.h"
#include "Network.h"

MQTTClient client;
WiFiClient espClient;

void setupWifi()
{
	Serial.print("\nTrying to connect to ");
	Serial.println(SSID);

	// attempt to connect to Wifi network:
	WiFi.mode(WIFI_STA);
	WiFi.begin(SSID, PASS);
	WiFi.waitForConnectResult();

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("Connected.");
	Serial.print("MAC Addr: ");
	Serial.println(WiFi.macAddress());
	Serial.print("IP Addr:  ");
	Serial.println(WiFi.localIP());
	Serial.print("Subnet:   ");
	Serial.println(WiFi.subnetMask());
	Serial.print("Gateway:  ");
	Serial.println(WiFi.gatewayIP());
	Serial.print("DNS Addr: ");
	Serial.println(WiFi.dnsIP());
	Serial.print("Channel:  ");
	Serial.println(WiFi.channel());
	Serial.print("Status: ");
	Serial.println(WiFi.status());
}

//Reconnection to the MQTT network
void reconnect() {
	//Boucle jusqu'� obtenur une reconnexion
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection...");
		// Create a random client ID
		String clientId = "ESP8266Client-";
		clientId += String(random(0xffff), HEX);
		// Attempt to connect
		if (client.connect(clientId.c_str())) {
			Serial.println("OK");
			//waitTimeIndex = 0;
		}

		String composeClientID() {
			uint8_t mac[6];
			WiFi.macAddress(mac);
			String clientId;
			clientId += "esp-";
			clientId += macToStr(mac);
			return clientId;
		}

		else {
			Serial.print("KO, erreur : ");
			Serial.print(client.state());
			Serial.println(" Waiting 5 seconds before trying to reconnect");
			delay(/*waitTimes[waitTimeIndex]*/5 * 1000);
			//waitTimeIndex = 
		}
	}
}

// D�clenche les actions � la r�ception d'un message
// D'apr�s http://m2mio.tumblr.com/post/30048662088/a-simple-example-arduino-mqtt-m2mio
void callback(char* topic, byte* payload, unsigned int length) {

	if (debug) {
		Serial.println("Message recu =>  topic: " + String(topic));
		Serial.print(" | longueur: " + String(length, DEC));
	}

	// create character buffer with ending null terminator (string)
	int i = 0;
	for (i = 0; i < length; i++) {
		messageBuffer[i] = payload[i];
	}
	messageBuffer[i] = '\0';

	String msgString = String(messageBuffer);
	if (debug) {
		Serial.println("Payload: " + msgString);
	}

	if (msgString == "ON") {
		digitalWrite(LED_PIN, HIGH);
	}
	else if (msgString == "OFF") {
		digitalWrite(LED_PIN, LOW);
	}
}


