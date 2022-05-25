//APPLETON INNOVATIONS, VISAKHAPATNAM, RAILWAY NEW COLONY
#include "FS.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include<WiFiClientSecure.h>
#include<DHT.h>

#define BUFFER_LEN  256
long lastMsg = 0;
char msg[BUFFER_LEN];

WiFiClientSecure espClient;

DHT dht(D2,DHT22);

// NETWORK VALUES.

const char* ssid = "Unit 2"; //Provide your SSID
const char* password = "Basementguys$"; // Provide Password

//sensor variables
int t=0;
int h=0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// callback func begins

void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
Serial.print(topic);
Serial.print("] ");

//INNER FOR LOOP BEGINS

for (int i = 0; i < length; i++) {
Serial.print((char)payload[i]);
}

Serial.println();
}

const char* AWS_endpoint = "a2llfeu6oxp87m-ats.iot.us-east-2.amazonaws.com"; //MQTT broker ip


PubSubClient client(AWS_endpoint, 8883, callback, espClient); //set MQTT port number to 8883 as per //standard



//wifi func begins


void setup_wifi() {
delay(10);
// We start by connecting to a WiFi network
//espClient.setBufferSizes(512, 512);
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.begin(ssid, password);

//while loop begins

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}

Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());

timeClient.begin();

//while loop begins


while(!timeClient.update()){
timeClient.forceUpdate();
}

espClient.setX509Time(timeClient.getEpochTime());
}







//reconnect function begins

void reconnect() {
// Loop until we're reconnected
while (!client.connected()) {
Serial.print("Attempting MQTT connection...");
// Attempt to connect
if (client.connect("ESPthing3")) {
Serial.println("connected");
// Once connected, publish an announcement...
client.publish("appleton", "hi folks");
// ... and resubscribe
client.subscribe("appleton_out");
} else {
Serial.print("failed, rc=");
Serial.print(client.state());
Serial.println(" try again in 5 seconds");

char buf[256];
espClient.getLastSSLError(buf,256);
Serial.print("WiFiClientSecure SSL error: ");
Serial.println(buf);

// Wait 5 seconds before retrying
delay(5000);
}
}
}

//
void certificates_opening_loading(){
  //PEM(Privacy Enhanced Mail) files are base64 encoded ascii files
//der (Distinguished Encoding Rules) is the binary form of certificates

//IF STATEMENT BEGINS

if (!SPIFFS.begin()) {
Serial.println("Failed to mount file system");
return;
}

Serial.print("Heap: "); 
Serial.println(ESP.getFreeHeap());

// Load certificate file
File cert = SPIFFS.open("/cert.der", "r"); //replace cert.crt eith your uploaded file name
if (!cert) {
Serial.println("Failed to open cert file");
}
else
Serial.println("Success to open cert file");

delay(1000);

if (espClient.loadCertificate(cert))
Serial.println("cert loaded");
else
Serial.println("cert not loaded");

// Load private key file
File private_key = SPIFFS.open("/private.der", "r"); //replace private eith your uploaded file name
if (!private_key) {
Serial.println("Failed to open private cert file");
}
else
Serial.println("Success to open private cert file");

delay(1000);

if (espClient.loadPrivateKey(private_key))
Serial.println("private key loaded");
else
Serial.println("private key not loaded");

// Load CA file
File ca = SPIFFS.open("/ca.der", "r"); //replace ca eith your uploaded file name
if (!ca) {
Serial.println("Failed to open ca ");
}
else
Serial.println("Success to open ca");

delay(1000);
if(espClient.loadCACert(ca)){
  Serial.println("ca loaded");
}
else{
Serial.println("ca failed");
}
Serial.print("Heap: "); 
Serial.println(ESP.getFreeHeap());
}

void sensor_code(){
 t=dht.readTemperature();
h=dht.readHumidity(); 
}




void publisher(){
sensor_code();
if (!client.connected()) {
reconnect();
}
client.loop();

long now = millis();

if (now - lastMsg > 5000) {
lastMsg = now;

String Temprature = String(t);
String Humidity = String(h);
String id = "AppletonR&D";

//the character data that c_str() returns cannot be modified because c_str returns
//a const character pointer which is a C-style string, it is suitable to pass the data
//to the functions, which accepts c-style string

//The snprintf() function is used to redirect the output of  printf() function onto a buffer

snprintf (msg, BUFFER_LEN, "{\"id\" : \"%s\", \"Temprature\" : \"%s\", \"Humidity\" : \"%s\"}", 
id.c_str(), Temprature.c_str(), Humidity.c_str());

Serial.print("Publish message: ");
Serial.println(msg);

client.publish("cityofkitchener/publish",msg);

Serial.print("Heap: "); 
Serial.println(ESP.getFreeHeap()); //Low heap can cause problems
}
}


// setup begins

void setup() {
Serial.begin(9600);
dht.begin();
//Serial.setDebugOutput(true);
setup_wifi();
delay(1000);
certificates_opening_loading();
}

//loop begins

void loop() {
publisher();
}
