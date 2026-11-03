
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

// Wi-Fi credentials
const char* ssid = "REDMI Note 17 Pro 5G";
const char* password = "krishna2004";

// Django server API address
const char* serverURL = "https://solarcheck-qcjf.onrender.com/api/reading/";
Adafruit_INA219 ina219;

void setup() {
  Serial.begin(115200);

  // Initialize I2C
  Wire.begin(21, 22);

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWi-Fi connection failed!");
    Serial.print("Wi-Fi status: ");
    Serial.println(WiFi.status());
    return;
  }

  Serial.println("\nWi-Fi connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Connected Wi-Fi: ");
Serial.println(WiFi.SSID());

Serial.print("Gateway IP: ");
Serial.println(WiFi.gatewayIP());

  // Initialize INA219 sensor
  if (!ina219.begin()) {
    Serial.println("INA219 not detected!");

    while (1) {
      delay(10);
    }
  }

  Serial.println("INA219 detected!");
  Serial.println("SolarCheck system started!");
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {

    // Read voltage and current
    float busVoltage = ina219.getBusVoltage_V();
    float shuntVoltage = ina219.getShuntVoltage_mV();

    float voltage = busVoltage + (shuntVoltage / 1000.0);
    float current = ina219.getCurrent_mA() / 1000.0;

    // Calculate power
    float power = voltage * current;

    // Display sensor readings
    Serial.println("----------------------------");

    Serial.print("Bus Voltage: ");
    Serial.print(busVoltage, 3);
    Serial.println(" V");

    Serial.print("Shunt Voltage: ");
    Serial.print(shuntVoltage, 3);
    Serial.println(" mV");

    Serial.print("Panel Voltage: ");
    Serial.print(voltage, 3);
    Serial.println(" V");

    Serial.print("Current: ");
    Serial.print(current, 4);
    Serial.println(" A");

    Serial.print("Power: ");
    Serial.print(power, 4);
    Serial.println(" W");

    // Prepare JSON data
    String json = "{\"voltage\":" + String(voltage, 3) +
                  ",\"current\":" + String(current, 4) + "}";

    // Send data to Django backend
    WiFiClientSecure client;
client.setInsecure();  // Temporary testing only

HTTPClient http;

http.setConnectTimeout(20000);
http.setTimeout(120000);

http.begin(client, serverURL);
http.addHeader("Content-Type", "application/json");

Serial.println("Sending data to Render...");
Serial.println(json);

int responseCode = http.POST(json);

Serial.print("Server response: ");
Serial.println(responseCode);

if (responseCode > 0) {
  Serial.println(http.getString());
} else {
  Serial.println(http.errorToString(responseCode));
}

http.end();

    Serial.println("Sending data to Django...");
    Serial.println(json);

    Serial.print("Server response: ");
    Serial.println(responseCode);

    http.end();

  } else {
    Serial.println("Wi-Fi disconnected!");
    Serial.println("Reconnecting...");

    WiFi.reconnect();
  }

  delay(5000);
}
