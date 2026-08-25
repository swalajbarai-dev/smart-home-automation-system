#define BLYNK_TEMPLATE_ID "---------" 
#define BLYNK_TEMPLATE_NAME "Home automation system" 
#define BLYNK_AUTH_TOKEN "------------" 
#define BLYNK_PRINT Serial 
#include <ESP8266WiFi.h> 
#include <BlynkSimpleEsp8266.h> 
#include <WiFiManager.h> 
#include <TimeLib.h> 
#include <WidgetRTC.h> 
char auth[] = BLYNK_AUTH_TOKEN; 
// -------------------- BLYNK TIMER & RTC -------------------- 
BlynkTimer timer; 
WidgetRTC rtc; 
// -------------------- RELAY PINS -------------------- 
const int relay1 = D1;   // Bulb 1 Relay 
const int relay2 = D2;   // Bulb 2 Relay 
// -------------------- MANUAL SWITCH PINS -------------------- 
const int switch1 = D5;  // Switch for Bulb 1 
25 
const int switch2 = D6;  // Switch for Bulb 2 
// -------------------- STATE VARIABLES -------------------- 
bool bulb1State = false; 
bool bulb2State = false; 
bool lastSwitch1State = HIGH; 
bool lastSwitch2State = HIGH; 
// -------------------- CONTROL FUNCTIONS -------------------- 
void setBulb1(bool state) { 
bulb1State = state; 
// Active LOW relay 
digitalWrite(relay1, state ? LOW : HIGH); 
// Sync Blynk app button 
Blynk.virtualWrite(V0, state); 
Serial.print("Bulb 1: "); 
Serial.println(state ? "ON" : "OFF"); 
} 
void setBulb2(bool state) { 
bulb2State = state; 
digitalWrite(relay2, state ? LOW : HIGH); 
26 
27 
 
  Blynk.virtualWrite(V1, state); 
 
  Serial.print("Bulb 2: "); 
  Serial.println(state ? "ON" : "OFF"); 
} 
 
// -------------------- AUTO OFF AFTER 11 PM -------------------- 
void nightAutoOff() { 
  int h = hour(); 
  int m = minute(); 
 
  // Auto OFF at 16:30 
  if (h == 13 && m >= 45) { 
 
    if (bulb1State) { 
      Serial.println("4:40 PM reached -> Turning OFF Bulb 1"); 
      setBulb1(false); 
    } 
 
    if (bulb2State) { 
      Serial.println("4:40 PM reached -> Turning OFF Bulb 2"); 
      setBulb2(false); 
    } 
  } 
} 
 
// -------------------- CHECK MANUAL SWITCHES -------------------- 
28 
 
void checkSwitches() { 
  bool currentSwitch1 = digitalRead(switch1); 
  bool currentSwitch2 = digitalRead(switch2); 
 
  // Switch 1 pressed 
  if (lastSwitch1State == HIGH && currentSwitch1 == LOW) { 
    delay(50); // Debounce 
    setBulb1(!bulb1State); 
  } 
 
  // Switch 2 pressed 
  if (lastSwitch2State == HIGH && currentSwitch2 == LOW) { 
    delay(50); // Debounce 
    setBulb2(!bulb2State); 
  } 
 
  lastSwitch1State = currentSwitch1; 
  lastSwitch2State = currentSwitch2; 
} 
 
// -------------------- BLYNK APP BUTTONS -------------------- 
BLYNK_WRITE(V0) { 
  setBulb1(param.asInt()); 
} 
 
BLYNK_WRITE(V1) { 
  setBulb2(param.asInt()); 
} 
// -------------------- SETUP -------------------- 
void setup() { 
Serial.begin(115200); 
// Relay pins 
pinMode(relay1, OUTPUT); 
pinMode(relay2, OUTPUT); 
// Keep relays OFF at startup 
digitalWrite(relay1, HIGH); 
digitalWrite(relay2, HIGH); 
// Manual switch pins 
pinMode(switch1, INPUT_PULLUP); 
pinMode(switch2, INPUT_PULLUP); 
Serial.println("Starting Home Automation System..."); 
// WiFi Auto Connect 
WiFiManager wifiManager; 
wifiManager.autoConnect("ESP8266-Setup"); 
Serial.println("WiFi Connected"); 
29 
// Connect to Blynk 
Blynk.begin(auth, WiFi.SSID().c_str(), WiFi.psk().c_str()); 
Serial.println("Blynk Connected"); 
// Start RTC 
rtc.begin(); 
// Check manual switches every 100 ms 
timer.setInterval(100L, checkSwitches); 
// Check 11 PM auto-off every 30 seconds 
timer.setInterval(30000L, nightAutoOff); 
} 
// -------------------- LOOP -------------------- 
void loop() { 
Blynk.run(); 
timer.run(); 
}