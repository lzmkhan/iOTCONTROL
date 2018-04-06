 /*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <ESP8266WiFi.h>

const char* ssid     = "Komoyaro";
const char* password = "Ishwarya1";

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
  
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(D0PIN, OUTPUT);
  pinMode(D1PIN, OUTPUT);
  pinMode(2, OUTPUT); // Notification LED
}

int value = 0;


void connectOpenWifi(){
 if(WiFi.status() != WL_CONNECTED) {
  Serial.print("Searching for open wifi... ");
  int n = WiFi.scanNetworks();
  if(n == 0){
    Serial.println("No networks found");
    digitalWrite(2, HIGH);
  }
  else{
    Serial.println("Networks found!");
    for (int i = 0; i < n; ++i) {
      if(WiFi.encryptionType(i) == ENC_TYPE_NONE) {
        Serial.println("Found non-encrypted network. Trying to connect");
        Serial.print(WiFi.SSID(i));
        Serial.print(" ");
        Serial.print(WiFi.encryptionType(i));
        Serial.println();
        /* Clear previous modes. */
        WiFi.softAPdisconnect();
        WiFi.disconnect();
        WiFi.mode(WIFI_STA);
        delay(WIFI_DELAY);
        WiFi.begin(ssid);
        digitalWrite(2, LOW);
        break;
      }
    }
  }
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  ++value;

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

