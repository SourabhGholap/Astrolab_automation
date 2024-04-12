/*
 *  This sketch sends a message to a TCP server
 *
 */

#include <WiFi.h>
#include <WiFiMulti.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <QMC5883LCompass.h>
 
QMC5883LCompass compass;

WiFiMulti WiFiMulti;

void setup()
{
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network
    WiFiMulti.addAP("RUSHI", "23112002");

    Serial.println();
    Serial.println();
    Serial.print("Waiting for WiFi... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);


  
  Serial.begin(9600);
  // Initialize device with i2c 0x0D address
  compass.init();
  delay(100);
}


void loop()
{
//    const uint16_t port = 80;
//    const char * host = "192.168.1.1"; // ip or dns
     int x, y, z;
 
          // Read compass values
    compass.read();
 
          // Return XYZ readings
    x = compass.getX();
    y = compass.getY();
    z = compass.getZ();
    const uint16_t port = 80;
    const char * host = "192.168.4.1"; // ip or dns
    String sender = "telescope";



    Serial.print("Connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    if (!client.connect(host, port)) {
        Serial.println("Connection failed.");
        Serial.println("Waiting 5 seconds before retrying...");
        delay(5000);
        return;
    }
   int azimut = compass.getAzimuth();
          
   float bearing = compass.getBearing(azimut);
   if(azimut<0)
      azimut = 360+azimut;
   Serial.print(" Azimuth: ");Serial.print(azimut);



    double angle = azimut;
    long dt = 10;
    unsigned long prev_millis = 0;
    unsigned long curr_millis = 0;
    unsigned long new_millis = 0;

    // This will send a request to the server
    //uncomment this line to send an arbitrary string to the server
    //client.print("Send this data to the server");
    //uncomment this line to send a basic document request to the server
    while(1){
    
      delay(dt);
      new_millis=millis();
      curr_millis = new_millis;
      if(curr_millis-prev_millis > 500){
        Serial.print(String(angle)+"\n");
        // GET /index.html HTTP/1.1\n\n
        client.print("GET /sender=telescope/angle="+String(angle)+"/ HTTP/1.1\n\n");
        prev_millis = curr_millis;
      }
      // break;
    }
    delay(3000);
}
