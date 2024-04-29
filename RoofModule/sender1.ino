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
   
    compass.init();
    compass.calibrate();
    Serial.println();
    Serial.print("compass.setCalibrationOffsets(");
    Serial.print(compass.getCalibrationOffset(0));
    Serial.print(", ");
    Serial.print(compass.getCalibrationOffset(1));
    Serial.print(", ");
    Serial.print(compass.getCalibrationOffset(2));
    Serial.println(");");
    Serial.print("compass.setCalibrationScales(");
    Serial.print(compass.getCalibrationScale(0));
    Serial.print(", ");
    Serial.print(compass.getCalibrationScale(1));
    Serial.print(", ");
    Serial.print(compass.getCalibrationScale(2));
    Serial.println(");");
    

    Serial.println();
    Serial.println();
    WiFiMulti.addAP("RUSHI", "23112002");
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


  
  // Initialize device with i2c 0x0D address
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
        client.print("GET /sender=roof/angle="+String(angle)+"/ HTTP/1.1\n\n");
        prev_millis = curr_millis;
      }
      // break;
    }
    delay(3000);
  // int maxloops = 0;

  // //wait for the server's reply to become available
  // while (!client.available() && maxloops < 1000)
  // {
  //   maxloops++;
  //   delay(1); //delay 1 msec
  // }
  // if (client.available() > 0)
  // {
  //   //read back one line from the server
  //   String line = client.readStringUntil('\r');
  //   Serial.println(line);
  // }
  // else
  // {
  //   Serial.println("client.available() timed out ");
  // }

  //   Serial.println("Closing connection.");
  //   client.stop();

  //   Serial.println("Waiting 5 seconds before restarting...");
  //   delay(5000);
}
