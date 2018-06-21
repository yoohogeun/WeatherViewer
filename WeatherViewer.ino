/*
 *  WeatherViewr v0.1
 *  Created on: 21.06.2018
 *  Made by : Hogeun Yoo
 *            a@withmetoday.com
 *            
 */


/*
 *  Weather State
 *  
 *  01d | 01n = clear sky 
 *  02d | 02n = few clouds
 *  03d | 03n = scattered clouds
 *  04d | 04n = broken clouds
 *  09d | 09n = shower rain
 *  10d | 10n = rain
 *  11d | 11n = thunderstorm
 *  13d | 13n = snow
 *  50d | 50n = mist
 *  
 */  



#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define USE_SERIAL Serial

const char* ssid     = "arduino";
const char* password = "123456789";

String API_KEY = "3d878d688b3c20a4ea4e843c0e3f7804"; //OpenWeatherMap API Key (Name : WeatherViewer)
String CITY_ID = "1846918"; // 1835848 : SEOUL // 1846918 : ANSAN
String UNITS = "imperial";
String payload;
//String FIELD; //It's not used in this version
int state;
String HTTP_result;
int code;

ESP8266WiFiMulti WiFiMulti;
Adafruit_8x16matrix matrix = Adafruit_8x16matrix(); //for matrix

void setup() {
  

    USE_SERIAL.begin(115200);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    WiFiMulti.addAP(ssid, password);
    
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) { 
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("==============================");
    Serial.println("WiFi connected");  
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("==============================");

  matrix.begin(0x70);

  matrix.setTextSize(2);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);
  for (int8_t x=0; x>=-64; x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print(" Hello ");
    matrix.writeDisplay();
    delay(100);
  }

  for(int j = 1; j <5; j++) {
    matrixControl(j);
    delay(7000);
  }

  matrixControl(9);
  delay(7000);
  matrixControl(10);
  delay(7000);
  matrixControl(11);
  delay(7000);
  matrixControl(13);
  delay(7000);
  matrixControl(50);
  delay(7000);

  

    matrix.setTextSize(2);
    matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
    matrix.setTextColor(LED_ON);
    for (int8_t x=0; x>=-64; x--) {
      matrix.clear();
      matrix.setCursor(x,0);
      matrix.print(" START ");
      matrix.writeDisplay();
      delay(100);
    } 

}




void loop() {
    
    String HTTP_result = HTTP();    
    weather(HTTP_result);
    Serial.println(state);
    matrixControl(state);
    delay(1000000);
}





String HTTP() {
      // wait for WiFi connection
      if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;
        
        USE_SERIAL.print("[HTTP] begin...\n");        // configure traged server and url
        http.begin("http://api.openweathermap.org/data/2.5/weather?id="+CITY_ID+"&appid="+API_KEY+"&units="+UNITS);
        
        USE_SERIAL.println("[HTTP] start connection and send HTTP header");

        int httpCode = http.GET();        // httpCode will be negative on error
        if(httpCode > 0) {            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);            // file found at server
            Serial.println("==============================");
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
                return payload;
           }
        } 
        else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    }
}

void weather(String HTTP_result) {
  StaticJsonBuffer<1200> jsonBuffer;
  JsonObject& owm_data = jsonBuffer.parseObject(HTTP_result);
  if (!owm_data.success()) {
    Serial.println("Parsing failed");
    return;
  }
//  int field = owm_data["main"][FIELD]; //It's not used in this version
  String code = owm_data["weather"][0]["icon"];
  state = WeatherToState(code);
  Serial.print(code + " : ");
}

int WeatherToState(String code){
  int state = 0;

  //in Day
  if(code == "01d") {
    state = 1;
  }
  else if(code == "02d") {
    state = 2;
  }
  else if(code == "03d") {
    state = 3;
  }
  else if(code == "04d") {
    state = 4;
  }
  else if(code == "09d") {
    state = 9;
  }
  else if(code == "10d") {
    state = 10;  
  }
  else if(code == "11d") {
    state = 11;
  }
  else if(code == "13d") {
    state = 13;
  }
  else if(code == "50d") {
    state = 50;
  }

  //in Night
  else if(code == "01n") {
    state = -1;
  }
  else if(code == "02n") {
    state = -2;
  }
  else if(code == "03n") {
    state = -3;
  }
  else if(code == "04n") {
    state = -4;
  }
  else if(code == "09n") {
    state = -9;
  }
  else if(code == "10n") {
    state = -10;  
  }
  else if(code == "11n") {
    state = -11;
  }
  else if(code == "13n") {
    state = -13;
  }
  else if(code == "50n") {
    state = -50;
  }
  return state;
}



/*
 * 
 * 8X16 Matrix Control PART
 * 
 * 
 */

static const uint8_t PROGMEM //icon design
  clear_sky[] =
  { 
    B00001000,
    B01000010,
    B00011000,
    B00111101,
    B10111100,
    B00011000,
    B01000010,
    B00001000 },
  
  few_clouds[] =
  { 
    B00000000,
    B00000000,
    B00000000,
    B00000100,
    B00100001,
    B00001100,
    B01011110,
    B00011110,
    B01110001,
    B10001001,
    B10000001,
    B01111110,
    B00000000,
    B00000000,
    B00000000,
    B00000000 },
  
  scattered_clouds[] = 
  {
    B00000000,
    B01101110,
    B10010001,
    B10001001,
    B10000001,
    B01111110,
    B00000000,
    B00000000
  },
  
  broken_clouds[] =
  { 
    B00000000,
    B00111010,
    B01000101,
    B01000001,
    B01101110,
    B11111111,
    B11111111,
    B01111110 },

  shower_rain[] =
  { 
    B00101110,
    B01010001,
    B10000001,
    B10000001,
    B01111110,
    B00000000,
    B01010010,
    B10100100 },

  rain[] =
  { 
    B00000000,
    B00000000,
    B01001001,
    B10010010,
    B00000000,
    B00111100,
    B01000010,
    B10000001,
    B10000001,
    B11111111,
    B00001000,
    B00001000,
    B00001000,
    B00101000,
    B00010000,
    B00000000 },


thunderstorm[] =
  { 
    B00000001,
    B00001010,
    B00010001,
    B00110010,
    B01100000,
    B11111000,
    B00111110,
    B00001100,
    B00011000,
    B00010000,
    B00100010,
    B00000100,
    B01000010,
    B10000100,
    B01000000,
    B10000000 },

  snow[] = 
  {
    B01000000,
    B11100010,
    B01000111,
    B00010010,
    B00111000,
    B00010000,
    B01000010,
    B11100111,
    B01000010,
    B00000000,
    B00001000,
    B00011100,
    B00001000,
    B01000010,
    B11100111,
    B01000010,
    },

  mist[] = 
  {
    B01010101,
    B10101010,
    B00000000,
    B01010101,
    B10101010,
    B00000000,
    B01010101,
    B10101010
  };

void matrixControl(int state) {

  Serial.println(state);
  Serial.println(state);
  if(state == 1 || state == -1) {
    matrix.clear();
    matrix.drawBitmap(0, 4, clear_sky, 8, 8, LED_ON);
    matrix.writeDisplay();
  }
  else if(state == 2 || state == -2) {
    matrix.clear();
    matrix.drawBitmap(0, 0, few_clouds, 8, 16, LED_ON);
    matrix.writeDisplay();
  }
  else if(state == 3 || state == -3) {
    matrix.clear();
    matrix.drawBitmap(0, 4, scattered_clouds, 8, 8, LED_ON);
    matrix.writeDisplay();
  }
  else if(state == 4 || state == -4) {
    matrix.clear();
    matrix.drawBitmap(0, 3, broken_clouds, 8, 8, LED_ON);
    matrix.writeDisplay();
  }
  else if(state == 9 || state == -9) {
    matrix.clear();
    matrix.drawBitmap(0, 4, shower_rain, 8, 8, LED_ON);
    matrix.writeDisplay();
  }
  else if(state == 10 || state == -10) {
    matrix.clear();
    matrix.drawBitmap(0, 0, rain, 8, 16, LED_ON);
    matrix.writeDisplay();
  }
  else if(state == 11 || state == -11) {
    matrix.clear();
    matrix.drawBitmap(0, 0, thunderstorm, 8, 16, LED_ON);
    matrix.writeDisplay();
  }
  else if(state == 13 || state == -13) {
    matrix.clear();
    matrix.drawBitmap(0, 0, snow, 8, 16, LED_ON);
    matrix.writeDisplay();
  }
  else if(state == 50 || state == -50) {
    matrix.clear();
    matrix.drawBitmap(0, 4, mist, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(100);
  }
  else { 
    Serial.println("Error");
    matrix.setTextSize(2);
    matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
    matrix.setTextColor(LED_ON);
    for (int8_t x=0; x>=-64; x--) {
      matrix.clear();
      matrix.setCursor(x,0);
      matrix.print(" Error");
      matrix.writeDisplay();
      delay(100);
    } 
  }
}
