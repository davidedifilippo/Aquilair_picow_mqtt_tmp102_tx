
#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include <string> 
#include <SparkFunTMP102.h> // Used to send and recieve specific information from our sensor

//#include "secret.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "" ;    // your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)

// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "broker.hivemq.com";
int        port     = 1883;
const char topic[]  = "arduino/simple";

const long interval = 10000;
unsigned long previousMillis = 0;

int count = 0;
char countString[3];

TMP102 tmpSensor;

void setup() {

  Wire1.setSDA(14);
  Wire1.setSCL(15);
  Wire1.begin();
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

   /*It will return trueon success or false on failure to communicate. */
  if(!tmpSensor.begin(0x48,Wire1))
  {
    Serial.println("Cannot connect to TMP102.");
    Serial.println("Is the board connected? Is the device ID correct?");
    while(1);
  }

   Serial.println("Connected to TMP102!");
  delay(100);


  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  // mqttClient.setId("clientId");

  // You can provide a username and password for authentication
  // mqttClient.setUsernamePassword("username", "password");

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();

  float temp = tmpSensor.readTempC();
  
  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.print("temperatura,luogo=camera_1 temperatura=");
    Serial.println(temp);
    //sprintf(countString, "%d", count);
    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    //mqttClient.print("weather,location=us-midwest temperature=82 1465839830100400200"); //dataformat influx in telegraf
    mqttClient.print("weather,location=us-midwest temperature="); //dataformat senza timestamp
    mqttClient.print(temp);
    mqttClient.endMessage();

    Serial.println();

    count++;
  }
}
