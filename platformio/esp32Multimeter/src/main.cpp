

// #include <headers.h>

// #define INA226_ADDRESS 0x40

// INA226_WE ina226(INA226_ADDRESS);
// LiquidCrystal_I2C lcd(0x27, 16, 2);

// float corrente_mA = 0;
// float tensao_V = 0;

// // ======= CONFIGURAÇÃO WiFi ==========
// const char* ssid = "ESP32";
// const char* password = "12345678";

// WebServer server(80);

// // ======= FUNÇÃO PARA RETORNAR JSON =======
// void sendSensorData() {
//   String json = "{";
//   json += "\"corrente\":" + String(corrente_mA, 2) + ",";
//   json += "\"tensao\":" + String(tensao_V, 2);
//   json += "}";
//   server.send(200, "application/json", json);
// }

// // ======= PÁGINA WEB COM GAUGES =======
// const char webpage[] PROGMEM = R"rawliteral(
// <!DOCTYPE html>
// <html>
// <head>
// <meta name="viewport" content="width=device-width, initial-scale=1" />
// <title>INA226 Monitor</title>

// <style>
// body {
//   background: #111;
//   color: white;
//   font-family: Arial;
//   text-align: center;
// }
// .gauge {
//   width: 200px;
//   height: 200px;
//   border-radius: 50%;
//   margin: 20px auto;
//   background: #222;
//   border: 6px solid #444;
//   position: relative;
// }
// .value {
//   position: absolute;
//   top: 65px;
//   width: 100%;
//   font-size: 32px;
//   font-weight: bold;
// }
// .label {
//   position: absolute;
//   top: 110px;
//   width: 100%;
//   font-size: 16px;
// }
// </style>

// </head>
// <body>

// <h2>Monitor INA226</h2>

// <div class="gauge" id="gCorrente">
//   <div class="value" id="correnteValue">--</div>
//   <div class="label">Corrente (mA)</div>
// </div>

// <div class="gauge" id="gTensao">
//   <div class="value" id="tensaoValue">--</div>
//   <div class="label">Tensão (V)</div>
// </div>

// <script>
// function corGauge(valor, limite1, limite2) {
//   if (valor < limite1) return "green";
//   if (valor < limite2) return "orange";
//   return "red";
// }

// function atualizar() {
//   fetch("/data")
//     .then(response => response.json())
//     .then(data => {
//       document.getElementById("correnteValue").innerHTML = data.corrente;
//       document.getElementById("tensaoValue").innerHTML = data.tensao;

//       // cores dinâmicas
//       document.getElementById("gCorrente").style.borderColor =
//         corGauge(data.corrente, 200, 500);

//       document.getElementById("gTensao").style.borderColor =
//         corGauge(data.tensao, 3.3, 5);
//     });
// }

// setInterval(atualizar, 1000);
// </script>

// </body>
// </html>
// )rawliteral";

// // ==========================================

// void setup() {
//   Serial.begin(115200);

//   // LCD
//   lcd.init();
//   lcd.backlight();

//   // I2C + INA226
//   Wire.begin();
//   ina226.init();

//   // WiFi AP mode
//   /*WiFi.softAP(ssid, password);
//   Serial.println("WiFi iniciado");
//   Serial.println(WiFi.softAPIP());*/

//   // WIFI_STA mode
//   WiFi.mode(WIFI_STA);
//   WiFi.begin("TPLINK", "gregorio@2012");
//   Serial.println("Connecting to WiFi...");

//   // Rotas do servidor
//   server.on("/", []() {
//     server.send(200, "text/html", webpage);
//   });

//   server.on("/data", sendSensorData);

//   server.begin();
//   Serial.println("Servidor Web iniciado");
// }

// // ==========================================

// void loop() {

//   ina226.readAndClearFlags();
  
//   corrente_mA = ina226.getCurrent_mA();
//   tensao_V = ina226.getBusVoltage_V();

//   // LCD
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("I: ");
//   lcd.print(corrente_mA);
//   lcd.print(" mA");

//   lcd.setCursor(0, 1);
//   lcd.print("V: ");
//   lcd.print(tensao_V);
//   lcd.print(" V");

//   server.handleClient();
//   delay(1000);
// }




///////////////////////////////
//TESTED OK, MESURES I AND V //
///////////////////////////////
#include <headers.h>

#define INA226_I2C_ADDRESS 0x40
 
INA226_WE ina226(INA226_I2C_ADDRESS);
LiquidCrystal_I2C MyLCD(0x27, 16, 2);
float current_mA = 0;
float voltage_V = 0;
 
void setup(void) 
{
  MyLCD.init();
  MyLCD.backlight();
  MyLCD.setCursor(0, 0);
  Wire.begin();
  ina226.init();
}
 
void loop(void) 
{
  ina226.readAndClearFlags();
  current_mA = ina226.getCurrent_mA();
  MyLCD.setCursor(0, 0);  // Coluna 0, linha 0
  MyLCD.print("I = ");
  MyLCD.print(current_mA);
  MyLCD.print(" mA");
  //delay(2500);

  voltage_V = ina226.getBusVoltage_V();
  MyLCD.setCursor(0, 1); // Coluna 0, linha 1
  MyLCD.print("V = ");
  MyLCD.print(voltage_V);
  MyLCD.print(" V");

  delay(2500);
  MyLCD.clear();
}



////////////////////////////////
//TESTED OK , WITHOUT LCD - I //
////////////////////////////////

// #include <Wire.h>
// #include "INA226.h"

// INA226 INA(0x40);

// // Initialising the INA226 sensor
// void setup() {
//   Serial.begin(9600);

//   Wire.begin();
//   if (!INA.begin()) { Serial.println("could not connect. Fix and Reboot"); }
//   INA.setMaxCurrentShunt(1, 0.002);
// }

// // Polling & displaying the sensor readings every 2 seconds
// void loop() {
//   Serial.print("\nBus Voltage:");
//   Serial.print(INA.getBusVoltage(), 3);

//   Serial.print(" -- Shunt Voltage(mV): ");
//   Serial.print(INA.getShuntVoltage_mV(), 3);

//   Serial.print(" -- Current(mA): ");
//   Serial.print(INA.getCurrent_mA() / 10, 3);

//   Serial.print(" -- Power(mW): ");
//   Serial.print(INA.getPower_mW(), 3);
//   delay(2000);
// }





/////////////////////////////////
//TESTED OK , WITHOUT LCD - II //
/////////////////////////////////
// #include <Wire.h>
// #include <INA226_WE.h>   
// #define I2C_ADDRESS 0x40

// INA226_WE ina226 = INA226_WE(I2C_ADDRESS);

// void setup() 
// {
//   Serial.begin(9600);
//   while (!Serial); // wait until serial comes up on Arduino Leonardo or MKR WiFi 1010
//   Wire.begin();
//   ina226.init();

//   /* Set Number of measurements for shunt and bus voltage which shall be averaged
//     Mode *     * Number of samples
//     AVERAGE_1            1 (default)
//     AVERAGE_4            4
//     AVERAGE_16          16
//     AVERAGE_64          64
//     AVERAGE_128        128
//     AVERAGE_256        256
//     AVERAGE_512        512
//     AVERAGE_1024      1024*/

//   //ina226.setAverage(AVERAGE_16); // choose mode and uncomment for change of default

//   /* Set conversion time in microseconds
//      One set of shunt and bus voltage conversion will take:
//      number of samples to be averaged x conversion time x 2

//        Mode *         * conversion time
//      CONV_TIME_140          140 µs
//      CONV_TIME_204          204 µs
//      CONV_TIME_332          332 µs
//      CONV_TIME_588          588 µs
//      CONV_TIME_1100         1.1 ms (default)
//      CONV_TIME_2116       2.116 ms
//      CONV_TIME_4156       4.156 ms
//      CONV_TIME_8244       8.244 ms  */

//   //ina226.setConversionTime(CONV_TIME_1100); //choose conversion time and uncomment for change of default

//   /* Set measure mode
//     POWER_DOWN - INA226 switched off
//     TRIGGERED  - measurement on demand
//     CONTINUOUS  - continuous measurements (default)*/

//   //ina226.setMeasureMode(CONTINUOUS); // choose mode and uncomment for change of default

//   /* Set Resistor and Current Range
//      if resistor is 5.0 mOhm, current range is up to 10.0 A
//      default is 100 mOhm and about 1.3 A*/

//   ina226.setResistorRange(0.1, 1.3); // choose resistor 0.1 Ohm and gain range up to 1.3A

//   /* If the current values delivered by the INA226 differ by a constant factor
//      from values obtained with calibrated equipment you can define a correction factor.
//      Correction factor = current delivered from calibrated equipment / current delivered by INA226*/

//   ina226.setCorrectionFactor(0.93);

//   Serial.println("INA226 Current Sensor Example Sketch - Continuous");

//   ina226.waitUntilConversionCompleted(); //if you comment this line the first data might be zero
// }

// void loop()
// {
//   float shuntVoltage_mV = 0.0;
//   float loadVoltage_V = 0.0;
//   float busVoltage_V = 0.0;
//   float current_mA = 0.0;
//   float power_mW = 0.0;

//   ina226.readAndClearFlags();
//   shuntVoltage_mV = ina226.getShuntVoltage_mV();
//   busVoltage_V = ina226.getBusVoltage_V();
//   current_mA = ina226.getCurrent_mA();
//   power_mW = ina226.getBusPower();
//   loadVoltage_V  = busVoltage_V + (shuntVoltage_mV / 1000);

//   Serial.print("Shunt Voltage [mV]: "); Serial.println(shuntVoltage_mV);
//   Serial.print("Bus Voltage [V]: "); Serial.println(busVoltage_V);
//   Serial.print("Load Voltage [V]: "); Serial.println(loadVoltage_V);
//   Serial.print("Current[mA]: "); Serial.println(current_mA);
//   Serial.print("Bus Power [mW]: "); Serial.println(power_mW);
//   if (!ina226.overflow)
//   {
//     Serial.println("Values OK - no overflow");
//   }
//   else
//   {
//     Serial.println("Overflow! Choose higher current range");
//   }
//   Serial.println();

//   delay(3000);
// }




////////////////////////////////////
//TESTED OK, MESURES ONLY VOLTAGE //
////////////////////////////////////

// #include <Arduino.h>
// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>
// LiquidCrystal_I2C lcd(0x27,16,2);
// #include <INA226_WE.h>
// #define I2C_ADDRESS 0x40

// INA226_WE ina226(I2C_ADDRESS);
// // INA226_WE ina226 = INA226_WE(); // Alternative: sets default address 0x40

// void continuousSampling();

// void setup() {
//   Serial.begin(9600);
//   lcd.init();
//   // turn on LCD backlight                      
//   lcd.backlight();
//   Wire.begin();
//   // default parameters are set - for change check the other examples
//   ina226.init();
//   Serial.println("INA226 Current Sensor Example Sketch - PowerDown");
//   Serial.println("Continuous Sampling starts");
//   Serial.println();
// }

// void loop() {
//   for(int i=0; i<5; i++){
//     continuousSampling();
//     delay(3000);
//   }
  
//   Serial.println("Power down for 7s");
//   ina226.powerDown();
//   for(int i=0; i<7; i++){
//     Serial.print(".");
//     delay(1000);
//   }
  
//   Serial.println("Power up!");
//   Serial.println("");
//   ina226.powerUp();
// }

// void continuousSampling(){
//   float shuntVoltage_mV = 0.0;
//   float loadVoltage_V = 0.0;
//   float busVoltage_V = 0.0;
//   float current_mA = 0.0;
//   float power_mW = 0.0; 
//   float res=0.0;
//   float pes=0.0;

//   ina226.readAndClearFlags();
//   shuntVoltage_mV = ina226.getShuntVoltage_mV();
//   busVoltage_V = ina226.getBusVoltage_V();
//   current_mA = ina226.getCurrent_mA();
//   power_mW = ina226.getBusPower();
//   loadVoltage_V  = busVoltage_V + (shuntVoltage_mV/1000);
  
//   Serial.print("Shunt Voltage [mV]: "); Serial.println(shuntVoltage_mV);
//   Serial.print("Bus Voltage [V]: "); Serial.println(busVoltage_V);
//   lcd.setCursor ( 0 , 0);lcd.print("Voltage:");lcd.print(busVoltage_V);
//   Serial.print("Load Voltage [V]: "); Serial.println(loadVoltage_V);
//   Serial.print("Current[mA]: "); Serial.println(current_mA);
//   res= busVoltage_V /0.07;
//   pes=res*10;
//   lcd.setCursor ( 0 , 1);lcd.print("Percentage:");lcd.print(pes);lcd.print("%");
//   Serial.print("Bus Power [mW]: "); Serial.println(power_mW);
//   if(!ina226.overflow){
//     Serial.println("Values OK - no overflow");
//   }
//   else{
//     Serial.println("Overflow! Choose higher current range");
//   }
//   Serial.println();
// }





////////////////////////
//TESTED OK, ONLY LCD //
////////////////////////
// #include <Arduino.h>
// #include <LiquidCrystal_I2C.h>

// // set the LCD number of columns and rows
// int lcdColumns = 16;
// int lcdRows = 2;

// LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

// void setup(){
//   // initialize LCD
//   lcd.init();
//   // turn on LCD backlight                      
//   lcd.backlight();  
// }

// void loop(){
//   // set cursor to first column, first row
//   lcd.setCursor(3, 0);
//   // print message
//   lcd.print("Ola, Mundo!");
//   delay(2000);
//   // clears the display to print new message
//   lcd.clear();
//   // set cursor to first column, second row
//   lcd.setCursor(2,1);
//   lcd.print("Hello, World!");
//   delay(2000);
//   lcd.clear(); 
// }


