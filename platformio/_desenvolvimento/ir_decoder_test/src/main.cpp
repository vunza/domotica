#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <IRremoteESP8266.h>
#include <ir_Tcl.h>

// Wi-Fi Credentials
const char* ssid = "TPLINK";
const char* password = "gregorio@2012";

// Hardware configuration
const uint16_t kIrLed = 12; 
IRTcl112Ac ac(kIrLed);

// Web Server operating on port 80
WebServer server(80);

// Global state variables to mirror the AC memory
uint8_t currentTemp = 22;
bool currentPower = true;

// HTML layout dynamically generated to display current status and buttons
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<style>body{font-family:Arial,sans-serif; text-align:center; background:#f4f4f9; padding:20px;}";
  html += ".btn{display:inline-block; padding:15px 25px; margin:10px; font-size:18px; color:white; border:none; border-radius:5px; cursor:pointer; text-decoration:none;}";
  html += ".btn-on{background:#2ecc71;} .btn-off{background:#e74c3c;} .btn-temp{background:#3498db;}";
  html += ".status{font-size:22px; margin-bottom:30px; color:#333;}</style></head><body>";
  
  html += "<h2>TCL AC Web Controller</h2>";
  html += "<div class='status'>Estado: <strong>" + String(currentPower ? "LIGADO" : "DESLIGADO") + "</strong> | Temp: <strong>" + String(currentTemp) + "&deg;C</strong></div>";
  
  html += "<a href='/toggle' class='btn " + String(currentPower ? "btn-off" : "btn-on") + "'>" + String(currentPower ? "Desligar" : "Ligar") + "</a><br><br>";
  html += "<a href='/tempup' class='btn btn-temp'>Aumentar Temp (+)</a>";
  html += "<a href='/tempdown' class='btn btn-temp'>Diminuir Temp (-)</a>";
  
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// Handler: Power Toggle Button
void handleToggle() {
  currentPower = !currentPower;
  //ac.setPower(currentPower);
  //ac.setLight(!ac.getLight());
  //ac.setSwingHorizontal(kTcl112AcSwingVOff);
  //ac.setSwingVertical(kTcl112AcSwingVOff);
  ac.send();
  
  Serial.printf("Power state updated to: %s and sent.\n", currentPower ? "ON" : "OFF");
  server.sendHeader("Location", "/");
  server.send(303); // Redirect back to root page
}

// Handler: Increase Temperature
void handleTempUp() {
  if (currentTemp < 30) { // Safety boundary check
    currentTemp++;
    ac.setTemp(currentTemp);
    ac.send();
    Serial.printf("Temperature raised to %d C and sent.\n", currentTemp);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

// Handler: Decrease Temperature
void handleTempDown() {
  if (currentTemp > 16) { // Safety boundary check
    currentTemp--;
    ac.setTemp(currentTemp);
    ac.send();
    Serial.printf("Temperature lowered to %d C and sent.\n", currentTemp);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  
  // Initialize IR library
  ac.begin();
  
  // Apply initial boot settings to the library RAM state block
  //ac.setPower(currentPower);             
  ac.setMode(kTcl112AcCool);     
  ac.setTemp(currentTemp);                
  ac.setFan(kTcl112AcFanMed);     
  ac.send();
  Serial.println("Initial boot IR command transmitted.");

  // Connect to the local Wi-Fi Network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP()); // Print the IP to access via browser

  // Map URLs to handler functions
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/tempup", handleTempUp);
  server.on("/tempdown", handleTempDown);

  // Initialize the Web Server
  server.begin();
  Serial.println("HTTP Server active.");
}

void loop() {
  // Listen for incoming client browser requests constantly
  server.handleClient();
  delay(2); // Short delay to yield CPU slice to background wireless tasks
}
