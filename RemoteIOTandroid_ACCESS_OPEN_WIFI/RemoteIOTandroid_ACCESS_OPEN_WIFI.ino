 /*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <ESP8266WiFi.h>
#define WIFI_DELAY        500
#define LED_BUILTIN       16
const char* ssid     = "IOT";
const char* password = "12345678";

const char* host = "devicetracker.000webhostapp.com";
const char* streamId   = "Control.php";
const char* deviceId = "1";
char D0PIN = 4; //PIN d2
char D1PIN = 0; // PIN d3


void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
//  
//  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
//     would try to act as both a client and an access-point and could cause
//     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
//

  pinMode(D0PIN, OUTPUT);
  pinMode(D1PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, OUTPUT); // Notification LED
}



void connectOpenWifi(){
 if(WiFi.status() != WL_CONNECTED) {
  Serial.print("Searching for open wifi... ");
  int n = WiFi.scanNetworks();
  if(n == 0){
    Serial.println("No networks found");
    digitalWrite(2, LOW);
  }
  else{
    
    for (int i = 0; i < n; ++i) {
      if(WiFi.encryptionType(i) == ENC_TYPE_NONE) {
        Serial.println("Found non-encrypted network. Trying to connect");
        delay(200);
        Serial.print(WiFi.SSID(i));
        delay(200);
        Serial.print(" ");
        delay(200);
        Serial.print(WiFi.encryptionType(i));
        delay(200);
        Serial.println();
        /* Clear previous modes. */
        WiFi.softAPdisconnect();
        delay(200);
        WiFi.disconnect();
        delay(200);
        WiFi.mode(WIFI_STA);
        delay(WIFI_DELAY);
       // WiFi.setPhyMode(WIFI_PHY_MODE_11B);
       // WiFi.setAutoConnect(false);
        WiFi.begin();
        
        Serial.println("Connecting. Please wait...");
        static int count =0;
        while((WiFi.status() != WL_CONNECTED)){
          delay(500);
          Serial.print(".");
          count++;
          if(count>=20)
          break;
        }
        count=0;

        if((WiFi.status() == WL_CONNECTED)){
        Serial.println("");
        delay(200);
        Serial.println("WiFi connected");  
        delay(200);
        Serial.println("IP address: ");
        delay(200);
        Serial.println(WiFi.localIP());
        delay(200);
        digitalWrite(2, HIGH);
        break;
        }else{
          Serial.println("Not connected, trying other network");
        }
      }
    }
  }
}
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  //connectOpenWifi();

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/devicetrackerapis/";
  url += streamId;
  url += "?deviceid=";
  url += deviceId;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  String lines = "";
  while(client.available()){
    String line = client.readStringUntil('\r');
    lines+=line;
    //Serial.println(line);

  }

    //Serial.print("lines = " + lines);
    int startIndex = lines.indexOf("[");
    int endIndex = lines.indexOf("]");
    startIndex = startIndex + 3;
    endIndex--;
  
    //Serial.println(lines.substring(startIndex,endIndex));
     if(lines.substring(startIndex,endIndex).equals("ON")){
      Serial.print("ON");
      digitalWrite(D0PIN, LOW);
    }else{
      Serial.print("OFF");
      digitalWrite(D0PIN, HIGH);
    }
  
  Serial.println();
  Serial.println("closing connection");
}

