#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

WiFiServer server(80);

const int relayPin = 5;
const char* ssid = "ssid name";
const char* password = "password";
bool relayStatus;
IPAddress ip(192, 168, 1, 51);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void setup() {
  Serial.begin(115200);
  delay(100);
  ConnectToWifi();
  server.begin();
  pinMode(relayPin, OUTPUT);
  Switch(true);
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

  if (request.indexOf("/switch/1") != -1) {
    Switch(true);
    String response = CreateJson(true);
    SendResponse(client, response);
  }

  if (request.indexOf("/switch/0") != -1) {
    Switch(false);
    String response = CreateJson(false);
    SendResponse(client, response);
  }

  if (request.indexOf("/status") != -1) {
    String response = CreateJson(relayStatus);
    SendResponse(client, response);
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

void SendResponse(WiFiClient client, String response) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println();
  client.print(response);
}

String CreateJson(bool result) {
  String json = "{Result: ";
  json += result;
  json += "}";

  return json;
}

void Switch(bool status) {
  relayStatus = status;
  digitalWrite(relayPin, status);
}
