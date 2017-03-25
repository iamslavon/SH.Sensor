#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

#define DHTPIN 14
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
WiFiServer server(80);

const char* ssid = "your ssid";
const char* password = "wifi pass";
const String host = "0.0.0.0";
const String postUrl = "http://urlToApi";
IPAddress ip(192, 168, 1, 50);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
const int houseId = 1;
const int roomId = 1;
const int temperatureSensorId = 1;
const int huminitySensorId = 2;

struct Dht22 {
	int Temperature;
	int Humidity;
};

void setup() {
	Serial.begin(115200);
	delay(100);
	dht.begin();
	ConnectToWifi();
	server.begin();
}

void loop() {
	WiFiClient client = server.available();
	if (!client) {
		return;
	}

	while (!client.available()) {
		delay(1);
	}

	String request = client.readStringUntil('\r');
	client.flush();

	if (request.indexOf("/getdata") != -1) {
		Dht22 data;

		do {
			data = ReadDht();
		} while (data.Humidity > 100 || data.Temperature > 100);
		
    String response = ComposeJsonForDht(data);
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println();
    client.print(response);
	}
	else {
		Serial.println("invalid request");
		client.stop();
		return;
	}

	client.flush();
}

void ConnectToWifi() {
	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, password);
	WiFi.config(ip, gateway, subnet);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

String CreateJson(int houseId, int roomId, int sensorId, int value) {
	String json = "{HouseId: ";
	json += houseId;
	json += ", RoomId: ";
	json += roomId;
	json += ", SensorId: ";
	json += sensorId;
	json += ", Value: ";
	json += value;
	json += "}";

	return json;
}

String ComposeJsonForDht(Dht22 data) {
	String json = "[";
	json += CreateJson(houseId, roomId, huminitySensorId, data.Humidity);
	json += ", ";
	json += CreateJson(houseId, roomId, temperatureSensorId, data.Temperature);
	json += "]";

	return json;
}

Dht22 ReadDht() {
	Dht22 dht22;
	dht22.Humidity = dht.readHumidity();
	dht22.Temperature = dht.readTemperature();

	return dht22;
}
