#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#define pi 3.14159265359

WiFiClient client;
Adafruit_MPU6050 mpu;

long pitch =0;

const int FLEX_PIN0 = 36; // Pin connected to voltage divider output
const int FLEX_PIN2 = 34;
const float VCC = 3.3; // Measured voltage of Ardunio 5V line
const float R_DIV = 9700.0; // Measured resistance of 10k resistor
const float STRAIGHT_RESISTANCE = 26600.0; // resistance when straight
const float BEND_RESISTANCE = 60000.0; // resistance at 90 deg


float flexangle(const int FLEX_PIN){
  // Read the ADC, and calculate voltage and resistance from it
  int flexADC = analogRead(FLEX_PIN);
  float flexV = flexADC * VCC / 1023.0;
  float flexR = R_DIV * (VCC / flexV - 1.0);
  //Serial.println("Resistance: " + String(flexR) + " ohms");
  // Use the calculated resistance to estimate the sensor's
  // bend angle:
  float angle = map(flexR, STRAIGHT_RESISTANCE, BEND_RESISTANCE,0, 90.0);
  //Serial.println("Bend: " + String(angle) + " degrees");
  //Serial.println();
  return angle;
}


unsigned long previousMillis = 0; 
const long interval = 1500;
int s=0;
const char* ssid = "Aditya";
const char* password = "humanbeing1";
//const char* host = "maker.ifttt.com";
//const char* apiKey = "eWM0sGTpzTn75DFbWHHthV6DfaCgNpylFv5zZ9TmCEo";
String server = "http://maker.ifttt.com";
String eventName = "Smart_Glove";
String IFTTT_Key = "eWM0sGTpzTn75DFbWHHthV6DfaCgNpylFv5zZ9TmCEo";
String IFTTTUrl="http://maker.ifttt.com/trigger/Smart_Glove/with/key/eWM0sGTpzTn75DFbWHHthV6DfaCgNpylFv5zZ9TmCEo";


void setup() {

  Serial.begin(115200);
  pinMode(FLEX_PIN0, INPUT);
  pinMode(FLEX_PIN2, INPUT);
  pinMode(23,OUTPUT);
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100); 
}

void IFTTT(String text)
{
  String url = server + "/trigger/" + eventName + "/with/key/" + IFTTT_Key + "?value1=" + text;  
  Serial.println(url);
  //Start to send data to IFTTT
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  http.begin(url); //HTTP

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void loop() {

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  Serial.flush();
  
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      String text;
      if( (flexangle(FLEX_PIN0)<=-89.0) && (flexangle(FLEX_PIN2)>-89.0)){
        text="A";
        IFTTT(text);
      }
      if( (flexangle(FLEX_PIN0)>-89.0) && (flexangle(FLEX_PIN2)<=-89.0)){
        text="B";
        IFTTT(text);
      }
      Serial.println(text);
      Serial.print(String(flexangle(FLEX_PIN0)) + " " + String(flexangle(FLEX_PIN2)));
      }
  currentMillis = millis();
  while(millis() < (currentMillis + 1000 )){}
  pitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y*a.acceleration.y + a.acceleration.z*a.acceleration.z));   // y axis
  
  if (pitch == 1){
    IFTTT("Need_Help_!!!");
    Serial.println("Need_Help_!!!");
    digitalWrite(23,HIGH);
    delay(3000);
    digitalWrite(23,LOW);
  } else if (pitch == -1){
    IFTTT("Yes");
    Serial.println("Yes");  
  }
  Serial.print("Pitch : ");
  Serial.println(pitch);
  
}  
