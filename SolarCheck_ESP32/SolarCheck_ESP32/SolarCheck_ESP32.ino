#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "KOMATTU";
const char* password = "parasuram";

const char* serverUrl = "http://192.168.1.4:8000/api/reading/";
void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Temporary test readings
    String data = "{\"voltage\":5.2,\"current\":0.15}";

    int responseCode = http.POST(data);

    Serial.print("HTTP Response: ");
    Serial.println(responseCode);

    Serial.println(http.getString());

    http.end();
  }

  delay(5000);
}