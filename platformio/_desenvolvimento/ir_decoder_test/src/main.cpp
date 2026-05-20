#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Tcl.h>

// Connect the Signal (S) pin of your IR transmitter to GPIO 4
const uint16_t kIrLed = 12; // GPIO pin for the IR LED (4, 12 ou 18 são comuns para ESP32)

// Initialize the TCL AC object
IRTcl112Ac ac(kIrLed);

void setup() {
  Serial.begin(115200);
  delay(500);
  
  // Start the IR sender
  ac.begin();
  Serial.println("Setting up TCL AC to turn ON...");
  delay(1000);

  // Set the AC state
  ac.setPower(true);             // Turn ON
  ac.setMode(kTcl112AcCool);     // Set mode to Cool
  ac.setTemp(22);                // Target 24°C
  //ac.setFanSpeed(kTcl112AcFanMed);// Set fan speed to Medium
  //ac.setSwingV(kTcl112AcSwingVOn);// Turn vertical swing ON

  // Send the command
  Serial.println("Sending IR command...");
  ac.send();
}

void loop() {
  // Nothing to do here
  delay(10000);
}
