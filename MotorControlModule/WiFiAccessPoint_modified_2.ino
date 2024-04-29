#include <WiFi.h>
int ledPin1 = 18;
int ledPin2 = 19;
int ledPin3 = 25;
int ledPin4 = 26;
const char* ssid = "yourAP";
const char* password = "yourPassword";
WiFiServer server(80);

void setup() {
    Serial.begin(115200);
    pinMode(ledPin1,OUTPUT);
    pinMode(ledPin2,OUTPUT);
    pinMode(ledPin3,OUTPUT);
    pinMode(ledPin4,OUTPUT);
    digitalWrite(ledPin1,HIGH);
    digitalWrite(ledPin2,HIGH);
    digitalWrite(ledPin3,HIGH);
    digitalWrite(ledPin4,HIGH); 
    WiFi.softAP(ssid, password);
    server.begin();
    Serial.println("Access Point Started");
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());
}

String sender;            // Variable to store the sender
double angle; 
bool control = false;            // Variable to store the angle

bool handleMessage(String currentLine){
    bool received = false;

    if (currentLine.startsWith("sender=")) {
        sender = currentLine.substring(7); 
    }
    //second '\n' is found, angle received
    else if (currentLine.startsWith("angle=")) {
        String angleStr = currentLine.substring(6);
        angle = angleStr.toFloat();
        received = true;
    }
    return received;
}

void messageSummary(){
    Serial.print("Message from ");
    Serial.print(sender);
    Serial.print(": Angle received = ");
    Serial.println(angle, 2);
}

void loop() 
{
    int MAX_CLIENTS= 8;

    WiFiClient client[MAX_CLIENTS];
    bool conn[MAX_CLIENTS];       // tells whether client slot is available or not
    for(int i=0; i< MAX_CLIENTS; i++){
      conn[i] = false;
    }

    double angle_telescope = 0;
    double angle_roof = 0;  
    double direction =0; 
    double offset=0;
    double delta =0;

    long prev_millis = millis();
    long prev_millis2= millis();
    while(1){
      delay(1);

      // add client to available slot
      int avail = -1;
      for(int i=0; i<MAX_CLIENTS; i++)
      {
        if(conn[i] == false){
          avail = i;
          break;
        }
      }

      // check if client is one of existing ones
      if(avail>=0)
      {
        client[avail] =  server.available();// Listen for incoming clients

        if(client[avail]){
          bool repeat = false;
          for(int i=0; i<MAX_CLIENTS; i++){
            if(conn[i] && client[avail].remoteIP() == client[i].remoteIP()){
              repeat = true;
            }
          }
          if(!repeat){
            conn[avail] = true;
            Serial.println("New Client."+String(avail)); // Print a message out the serial port
            Serial.println(client[avail].remoteIP());
          }
        }        
      }

      // loop through all clients
      for(int i=0; i<MAX_CLIENTS; i++)
      {
        if (!conn[i] ) {continue;} 
        if (!client[i].connected() || !client[i] ) {
            // Serial.println("Client Disconnected.");
            prev_millis2 = millis();
            conn[i] = false;
            client[i].stop(); // Close the connection
            continue;
        }   
        // Loop only while the client's connected

        sender = "";                   // Variable to store the sender
        angle = 0;                   // Variable to store the angle
        String currentLine = "";       // Make a String to hold incoming data from the client
        bool received = false;         // variable to indicate message is received
        
        while (client[i].available()) { // If there are bytes to read from the client,
            // Serial.println("h2");
            char c = client[i].read(); // Read a byte, then
            // Check for end of line or message field
            if (c == '\n' || c == '\r' || c== '/') { 
                // Process the currentLine if it's not empty
                if (currentLine.length() > 0) {
                    received = handleMessage(currentLine);
                    currentLine = ""; // Clear the current line for the next message
                }
                // If we received an empty line and have both sender and angle, process them
                if ( !sender.isEmpty() && received) {
                    messageSummary();
                    if(sender=="telescope"){
                      angle_telescope = angle;
                    }
                    else if (sender == "roof"){
                      angle_roof = angle;
                    }
                    else if(sender=="device"){
                       control = true;
                       direction = angle;
                    }
                    break;                    
                }
            } else {
                currentLine += c; // Add it to the end of the currentLine
            }
        }
      }
    if(millis()-prev_millis>1500)
    {
      if (control == false)
      {  
          delta =  (angle_telescope - angle_roof)-offset;
          Serial.println("Delta: "+String(delta)); 
          Serial.println("Offset: "+String(offset));
          if (delta > 5) {
            digitalWrite(ledPin1,LOW);  
            digitalWrite(ledPin2,LOW);
            digitalWrite(ledPin3,HIGH);
            digitalWrite(ledPin4,HIGH);
            Serial.println("************************ Moving left ***********************");      
          }
          else if (delta < -5) {
            digitalWrite(ledPin3,LOW);  
            digitalWrite(ledPin4,LOW);
            digitalWrite(ledPin1,HIGH);
            digitalWrite(ledPin2,HIGH); 
            Serial.println("************************ Moving right ***********************");             
          }
          else
          {
            digitalWrite(ledPin1,HIGH);
            digitalWrite(ledPin2,HIGH);
            digitalWrite(ledPin3,HIGH);
            digitalWrite(ledPin4,HIGH);    
            Serial.println("************************ Not moving ***********************");      
          }
               
        
      }
      else if(control == true)
      {  
         int tmp = angle;
          Serial.println(direction);
          if (direction == 1) 
            {
              digitalWrite(ledPin1,LOW);  
              digitalWrite(ledPin2,LOW);
              digitalWrite(ledPin3,HIGH);
              digitalWrite(ledPin4,HIGH);
              Serial.println("************************ Moving left ***********************");      
            }
          else if (direction == -1)
            {
              digitalWrite(ledPin3,LOW);  
              digitalWrite(ledPin4,LOW);
              digitalWrite(ledPin1,HIGH);
              digitalWrite(ledPin2,HIGH); 
              Serial.println("************************ Moving right ***********************");             
            }
          else if (direction == 0)
            {
              digitalWrite(ledPin1,HIGH);
              digitalWrite(ledPin2,HIGH);
              digitalWrite(ledPin3,HIGH);
              digitalWrite(ledPin4,HIGH);    
              Serial.println("************************ Not rrrrrrrrrrrrrrrrrr ***********************");
              delta =  (angle_telescope - angle_roof)-offset;
              offset = delta;     
              control = false; 
            }          
      }
      prev_millis=millis();   
    }
    } 
}
   // always while
