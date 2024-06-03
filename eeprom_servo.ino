#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <EEPROM.h>

// Include the HTML page
#include "PageIndex.h"

#define ServoPort D1  // Define the pin to which the servo is connected

Servo myservo;  // Create a servo object
ESP8266WebServer server(80);  // Create a web server object on port 80

// EEPROM addresses for storing Wi-Fi SSID, password, and servo status
#define EEPROM_WIFI_SSID_ADDR 0
#define EEPROM_WIFI_PASS_ADDR 64
#define EEPROM_SERVO_STATUS_ADDR 128

char ssid[32];  // Array to store Wi-Fi SSID
char password[32];  // Array to store Wi-Fi password
int servoStatus;  // Variable to store the servo position

// Function to handle the root URL "/"
void handleRoot() {
  String s = MAIN_page;  // Read the HTML page content
  server.send(200, "text/html", s);  // Send the HTML page to the client
}

// Function to handle the servo control URL "/setPOS"
void handleServo() {
  String POS = server.arg("servoPOS");  // Get the servo position from the URL parameter
  int pos = POS.toInt();  // Convert the position to an integer
  myservo.write(pos);  // Move the servo to the specified position
  delay(15);  // Short delay to ensure smooth movement
  Serial.print("Servo Angle:");
  Serial.println(pos);  // Print the servo position to the Serial Monitor
  server.send(200, "text/plain", "");  // Send an empty response to the client
}

// Function to handle the configuration save URL "/saveConfig"
void handleConfigSave() {
  String ssid_str = server.arg("ssid");  // Get the SSID from the URL parameter
  String password_str = server.arg("password");  // Get the password from the URL parameter
  String servoStatus_str = server.arg("servoStatus");  // Get the servo position from the URL parameter

  ssid_str.toCharArray(ssid, sizeof(ssid));  // Convert the SSID to a char array
  password_str.toCharArray(password, sizeof(password));  // Convert the password to a char array
  servoStatus = servoStatus_str.toInt();  // Convert the servo position to an integer

  // Save Wi-Fi configuration and servo position to EEPROM
  EEPROM.begin(512);
  EEPROM.put(EEPROM_WIFI_SSID_ADDR, ssid);
  EEPROM.put(EEPROM_WIFI_PASS_ADDR, password);
  EEPROM.put(EEPROM_SERVO_STATUS_ADDR, servoStatus);
  EEPROM.commit();
  EEPROM.end();

  // Send a response to the client
  server.send(200, "text/plain", "Configuration saved! Rebooting...");

  // Restart ESP8266 to apply new settings
  ESP.restart();
}

// Function to start the ESP8266 in Access Point (AP) mode
void startAPMode() {
  WiFi.softAP("NodeMCU_ESP8266", "12345");  // Create an AP with the specified SSID and password
  IPAddress apip = WiFi.softAPIP();  // Get the IP address of the AP
  Serial.print("AP IP address: ");
  Serial.println(apip);  // Print the AP IP address to the Serial Monitor
}

void setup() {
  Serial.begin(115200);  // Initialize Serial communication at 115200 baud rate
  delay(500);  // Short delay for stabilization

  myservo.attach(ServoPort);  // Attach the servo to the defined pin

  // Initialize EEPROM
  EEPROM.begin(512);

  // Read Wi-Fi configuration and servo status from EEPROM
  EEPROM.get(EEPROM_WIFI_SSID_ADDR, ssid);
  EEPROM.get(EEPROM_WIFI_PASS_ADDR, password);
  EEPROM.get(EEPROM_SERVO_STATUS_ADDR, servoStatus);
  EEPROM.end();

  // Start in AP mode initially
  startAPMode();

  // Check if SSID and password are available, then connect to Wi-Fi
  if (strlen(ssid) > 0 && strlen(password) > 0) {
    WiFi.begin(ssid, password);  // Begin Wi-Fi connection with the stored SSID and password
    Serial.println("Connecting to Wi-Fi");

    unsigned long startAttemptTime = millis();

    // Keep attempting to connect for 10 seconds
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());  // Print the device IP address to the Serial Monitor
      myservo.write(servoStatus);  // Set the servo to the last known position
    } else {
      Serial.println("");
      Serial.println("Failed to connect to Wi-Fi. Continuing in AP mode.");
    }
  } else {
    Serial.println("No Wi-Fi credentials found. Continuing in AP mode.");
  }

  // Define URL handlers
  server.on("/", handleRoot);  // Handle root URL
  server.on("/setPOS", handleServo);  // Handle servo control URL
  server.on("/saveConfig", handleConfigSave);  // Handle configuration save URL
  server.begin();  // Start the web server
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();  // Handle incoming client requests
}
