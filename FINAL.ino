
/*
 * Created by Driti - Make-A-Thon 4.0 Lema Labs 
 * 12 January 2020
 * Github: https://github.com/GomathiRaveendran/Driti ---- Private repository
 * 
 * This program uses load cell, load cell amplifier and two ultrasonic sensors to measure volume and weight of different items.
 * Purpose: Food Inventory (stock management)
 *
*/

#include "HX711.h" //------------------------------------- Header file for load cell 
#include <ESP8266WiFi.h> //------------------------------- Header file for connecting to the internet
#include <FirebaseArduino.h> //--------------------------- Firebase header file

//ULTRASONIC SENSOR PINS
#define TRIGGER1 5 
#define ECHO1    4

#define TRIGGER2 10
#define ECHO2    9

//LOAD CELL SENSOR PINS
const int LOADCELL_DOUT_PIN = 14;
const int LOADCELL_SCK_PIN = 13;

//FIREBASE CONNECTION PARAMETERS
#define FIREBASE_HOST "ultra-6d7e9.firebaseio.com"
#define FIREBASE_AUTH "Ib1OzFha2FYn2RN5q8rF6VYKt7ia7rTfy9snZEfz"

//WIFI CONNECTION PARAMETERS
#define WIFI_SSID "twevent"
#define WIFI_PASSWORD "rotc sv toad nice raze"
//#define WIFI_SSID ""
//#define WIFI_PASSWORD ""


float calibration_factor = 405; //------------- CALIBRATION FOR THE LOAD CELL USED IN THIS PROJECT. THIS WILL VARY FOR EVERY INDIVIDUAL SENSOR
HX711 scale;

void setup() {
  Serial.begin(115200);
  pinMode(TRIGGER1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIGGER2, OUTPUT);
  pinMode(ECHO2, INPUT);

  //connecting to load cell
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting"); 
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
  }
 Serial.println();
 Serial.print("connected: ");
 Serial.println(WiFi.localIP());

 //connect to Firebase
 Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

 //This sets the load that was present on the load cell at the moment it was executed to be "zero" grams. Load cell readings are relative.
 scale.set_scale(calibration_factor);
 scale.tare();
}

/*
 * The following loop function does three main things:-
 * 1) Prints the analog readings of the load cell in the serial monitor
 * 2) Prints the analog readings of TWO supersonic sensors in the serial monitor 
 * 3) Sends the three data values to Firebase via internet
 */
//float vol1;
float vol2;
void loop() 
{ Serial.println("*************************");
  Serial.println("Load Cell reading"); 
  if(Serial.available())
  { 
    char c = Serial.read();
    if(c == 't')
      scale.tare(); //--------------------- Setting a relative zero value in the load cell
  }
   long reading = scale.get_value();
   float wght = reading/calibration_factor;
  // wght=wght+10;
   Serial.print(wght);
  Firebase.setInt("Weight",wght); 
  Serial.println(" grams"); 
  Serial.println();
  delay(1000);

  Serial.println("*************************");
  Serial.println("Ultrasonic Sensor reading");
 
  long duration1, distance1;
  digitalWrite(TRIGGER1, LOW);  
  delayMicroseconds(2); 
   digitalWrite(TRIGGER1, HIGH);
  delayMicroseconds(10); 
   digitalWrite(TRIGGER1, LOW);
  duration1 = pulseIn(ECHO1, HIGH);
  distance1 = (duration1/2) / 29.1; 
  Serial.print("Centimeter1: ");
  Serial.println(distance1); //---distance of obstacle from the sensor, this raw data must be manipulated to find stuff like, vol. no. of boxes..etc
  // vol1 = (25-distance1)*0.55;   // Personalized calculations. The bubble top used had max. height = 25cm. and max vol = 6.6l. A simple linear eqn.
  Firebase.setInt("Distance1",distance1);
  delay(1000);
  
  long duration2, distance2;
  digitalWrite(TRIGGER2, LOW);  
  delayMicroseconds(2); 
  digitalWrite(TRIGGER2, HIGH);
  delayMicroseconds(10); 
  digitalWrite(TRIGGER2, LOW);
  duration2 = pulseIn(ECHO2, HIGH);
  distance2 = (duration2/2) / 29.1;
  Serial.print("distance2: ");
  Serial.println(distance2);
  vol2 = ((25-distance2)*0.55);
  Serial.print("Volume: ");
  Serial.println(vol2);
  Firebase.setFloat("Distance2",vol2);
  Serial.println("*************************");
  delay(1000);
  
}
