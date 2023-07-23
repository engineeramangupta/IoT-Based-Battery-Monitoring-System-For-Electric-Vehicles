
#include <WiFi.h>
#include <ThingSpeak.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90614.h>
//#include <Fonts/FreeMonoBold18pt7b.h>


WiFiClient client;

long channelNumber = 2125218;
const char APIKey[] = "CM78F703S4JNB11H";
//Note channel number is api key will be provided by thinkspeak

int analogInPin1 = 36;
int analogInPin2 = 39; 
int analogInPin3 = 34;

int voltageSensorValue;
float voltage; 

int currentSensorValue;
float current;

int batPercentage;

int temperatureSensorValue;
float temperature;

//int batHealth;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float calibration = 0.30; // Check Battery voltage using multimeter & add/subtract the value

void setup() {
  Serial.begin(9600);
  WiFi.begin("AMANGUPTA", "12344321");
//  Note -  use your wifi credentials
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print("..");
  }
  Serial.println();
  Serial.println("esp32 is connected!");
  Serial.println(WiFi.localIP());
  
  ThingSpeak.begin(client);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
}
void loop() {
  //variables
  voltageSensorValue = analogRead(analogInPin1);
  currentSensorValue = analogRead(analogInPin2);
  temperatureSensorValue = analogRead(analogInPin3);

  //voltage
  voltage = (((voltageSensorValue * 3.3) / 2048.0) + calibration); //multiply by two as voltage divider network is 100K & 100K Resistor


 //battery Percentage
  batPercentage = mapfloat(voltage, 2.8, 4.2, 0, 100); //2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage
 
  if (batPercentage >= 100){
    batPercentage = 100;
  }
  if (batPercentage <= 0){
    batPercentage = 0;
  }

  //Current
  current = ((((((currentSensorValue*5)/1023.0)-2.5)/0.185)-36)*1000);

  if(current < 10){
    current = 0;
  }

  //temperature
  // temperature = (temperatureSensorValue*500.0)/1023.0;
  temperature = ((temperatureSensorValue/1024.0)*5000)/10;

  
  //if print on serial Monitor
  Serial.println("Percentage: " + (String) batPercentage);
  Serial.println("voltageSensorValue: " + (String) voltageSensorValue);
  Serial.println("voltage: " + (String) voltage);
  Serial.println("currentSensorValue: " + (String) currentSensorValue);
  Serial.println("Current: " + (String) current);
  Serial.println("temperatureSensorValue: " + (String) temperatureSensorValue);
  Serial.println("temperature: " + (String) temperature);
  Serial.println();

  //upload to ThingSpeak
  ThingSpeak.writeField(channelNumber, 1, batPercentage, APIKey);
  ThingSpeak.writeField(channelNumber, 2, voltage, APIKey);
  ThingSpeak.writeField(channelNumber, 3, current, APIKey);
  ThingSpeak.writeField(channelNumber, 4, temperature, APIKey);
//  ThingSpeak.writeField(channelNumber, 5, batHealth, APIKey);


  printText();
  delay(2000);
  display.clearDisplay();
}

//function
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max){
  return (x-in_min)* (out_max-out_min)/ (in_max-in_min)+ out_min;
}

//OLED Display
void printText() {
//  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(WHITE);
  display.setTextSize(1);

  //battery Percentage
  display.setCursor(5, 2);
  display.print("Batt_Perc : ");
  display.print(batPercentage);
  display.print("%");

  //battery Voltage
  display.setCursor(5, 17);
  display.print("Batt_Volt : ");
  display.print(voltage);
  display.print("V");

  //battery Current
  display.setCursor(5, 32);
  display.print("Batt_Curr : ");
  display.print(current);
  display.print("mA");

  //battery Temperature
  display.setCursor(5, 47);
  display.print("Batt_Temp : ");
  display.print(temperature);
  // display.drawCircle(92, 35, 3, WHITE);
  display.print("C");
  
  display.display();
}
