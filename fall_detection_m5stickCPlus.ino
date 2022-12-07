#include "M5StickCPlus.h"
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

WiFiClient espClient;

float accX = 0;
float accY = 0;
float accZ = 0;
int counter = -1;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.println();
  M5.Lcd.setTextSize(1.5);
  M5.Lcd.setCursor(0, 50);
  M5.Lcd.print("Connecting to ");
  M5.Lcd.println(ssid);

  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }

  randomSeed(micros());

  M5.Lcd.println("");
  M5.Lcd.println("WiFi connected");
  M5.Lcd.println("IP address:");
  M5.Lcd.println(WiFi.localIP());

  // Set BLACK to the background color.
  M5.Lcd.fillScreen(BLACK);  
  M5.Lcd.setCursor(0, 0);
}

void setup(void) {

    counter = -1;

    M5.begin();
    M5.IMU.Init();

    setup_wifi();

    M5.Lcd.setRotation(1);
    M5.Lcd.fillScreen(TFT_GREEN);
    M5.Lcd.setTextColor(TFT_BLACK);

    M5.Lcd.setCursor(10, 30);
    M5.Lcd.setTextSize(2.8);
    M5.Lcd.print("Fall detector");
    M5.Lcd.setCursor(10, 70);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("Status: Normal");
    delay(1000);
}

void loop() {

  counter = counter - 1;

  if (counter == 0)
  {
    send_message();
  }
  else if(counter < 0)
  {
    counter = -1;
  }

  M5.update();

  M5.IMU.getAccelData(&accX, &accY, &accZ);
  if ( (accX*accX + accY*accY + accZ*accZ) < 0.05 ) 
  {
    M5.Lcd.fillScreen(TFT_RED);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setCursor(10, 20);
    M5.Lcd.setTextSize(2);
    M5.Lcd.print("Fall Detected!");
    M5.Lcd.setCursor(10, 50);
    M5.Lcd.println("Are you Okay?");
    M5.Lcd.setCursor(10, 90);
    M5.Lcd.println("Press Button!----->");
    counter = 35000;
  }

  if (M5.BtnA.wasReleased()) 
  {
    M5.Lcd.fillScreen(TFT_GREEN);
    M5.Lcd.setTextColor(TFT_BLACK);
    M5.Lcd.setCursor(10, 30);
    M5.Lcd.setTextSize(3);
    M5.Lcd.print("Phew!");
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(10, 80);
    M5.Lcd.print("Take Care..");
    delay(8000);
    counter = -1;

    //http-webhook-IFTTT
    HTTPClient http;
    http.begin("https://maker.ifttt.com/trigger/'Your_IFTTT_Event_Name_for_Fall_Detected_and_subject_responded'/json/with/key/'Your_IFTTT_Service_Key'");
    http.GET();
    http.end();

    setup();
  } 
}

void send_message(){

    M5.Lcd.fillScreen(TFT_ORANGE);
    M5.Lcd.setTextColor(TFT_BLACK); 
    M5.Lcd.setCursor(10, 30);
    M5.Lcd.setTextSize(3);
    M5.Lcd.print("Not okay!");
    M5.Lcd.setCursor(10, 80);
    M5.Lcd.setTextSize(2);
    M5.Lcd.print("Sending a message!");

    //http-webhook-IFTTT
    HTTPClient http;
    http.begin("https://maker.ifttt.com/trigger/Your_IFTTT_Event_Name_for_Fall_Detected_and_subject_didn't_responde'/json/with/key/'Your_IFTTT_Service_Key'");
    http.GET();
    http.end();

    delay(8000);
    setup();
}
