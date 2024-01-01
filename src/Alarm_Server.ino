/*
  ESP8266 Alarm Server v.1

  Description:
  The ESP8266 Alarm Server is designed for Hesilicon and Hesillicon-based NVR devices, 
  such as Smonet, that utilize port 15002 as their alarm server port. The server captures 
  alarm messages in JSON format and forwards them to an MQTT broker through the Wi-Fi protocol.

  Disclaimer:
  This code serves as an example program for ESP microcontrollers and is intended solely 
  for educational purposes. Using this code as a commercial or fixed solution is not recommended.

  System Requirements:
  - ESP8266 or ESP32 microcontroller
  - Wi-Fi connectivity
  - MQTT broker
  - Hesilicon or Hesillicon-based NVR devices with port 15002 for alarm messages

  Important Notes:
  - The program requires significant processing power. While ESP8266 is capable, it may take 
    approximately 20 to 30 seconds to process one message.
  - Messages received during the processing time will not be processed until the current 
    processing is complete.
  - Consider using ESP32 to reduce processing time, but it's not recommended for 
    resource-intensive tasks.

  Getting Started:
  1. Clone this repository to your local environment.
  2. Configure the necessary parameters in the code, such as Wi-Fi credentials and MQTT broker details.
  3. Upload the code to your ESP8266 or ESP32 device.
  4. Monitor the console for debugging information.

  Contributing:
  Feel free to contribute to the project by submitting bug reports, feature requests, or pull requests. 
  However, keep in mind the educational and experimental nature of this code.

  Author:
  Isuru Ranaweera
    Github: https://github.com/Isuru-rana

  License:
  This project is licensed under the MIT License (see LICENSE).

  Note: Always follow ethical and legal guidelines when working with surveillance or alarm systems. 
  This project is meant for educational purposes only.
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

const char* ssid = "your wifi ssid";
const char* password = "yourwifipassword";

const char* mqttServer = "yourmqttbrokeraddress";
const int mqttPort = 1883;
const char* mqttUsername = "mqtt-username";
const char* mqttPassword = "mqtt-password";

const int listning_port = 15002;  // Change it with your NVR alarm server port

const int bufferSize = 512;
char buffer[bufferSize];
char bufferMod[bufferSize];

#define STLED D0
#define ST_CONNECT_WiFi 100
#define ST_CONNECT_MQTT 200

unsigned long startt;
unsigned long t1;
unsigned long t2;
unsigned long t3;
unsigned long t4;

WiFiClient espClient;
PubSubClient client(espClient);

WiFiServer server(listning_port);


void setup() {
  //startt = millis();
  Serial.begin(115200);
  pinMode(STLED, OUTPUT);
  delay(1000);

  ESP.wdtDisable();
  ESP.wdtEnable(30000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    ledBlink(ST_CONNECT_WiFi);
  }

  Serial.printf("WiFi Connected: %s   IP: %d\n", ssid);

  client.setServer(mqttServer, mqttPort);
  client.connect("Alarm Server", mqttUsername, mqttPassword);

  while (!client.connected()) {
    ledBlink(ST_CONNECT_MQTT);
  }

  Serial.printf("Connected to MQTT server \n");
  server.begin();
  Serial.printf("Server Started \n");


  //t1 = millis();
  //unsigned long t1T = t1 - startt;
  //Serial.println();
  //Serial.print("Setup finished");
  //Serial.println(t1T);
}

void loop() {
  startt = 0;
  startt = millis();

  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
  }

  //client.loop();

  espClient = server.available();

  if (espClient) {
    Serial.printf("New client connected \n");
    while (espClient.connected()) {
      if (espClient.available()) {
        memset(buffer, 0, bufferSize);
        memset(bufferMod, 0, bufferSize);

        int bytesRead = espClient.readBytesUntil('\n', buffer, bufferSize - 1);
        //Serial.print(bytesRead);

        if (bytesRead > 0) {
          for (int i = 20; i < bytesRead; i++) {
            bufferMod[i - 20] = buffer[i];
            Serial.print(bufferMod[i - 20]);
          }
          //espClient.stop();
          extractDataFromJSON(bufferMod);
        }
      }
    }
    //espClient.stop();
    Serial.println("Client disconnected");
  }
/*
  t2 = millis();
  unsigned long t2T = t2 - startt;
  Serial.println();
  Serial.print("Loop Single routine");
  Serial.println(t2T);
  */
  ESP.wdtFeed();
}


void extractDataFromJSON(const char* json) {
  DynamicJsonDocument doc(bufferSize);

  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.println("Parsing failed");
    return;
  }

  const char* address = doc["Address"];
  int channel = doc["Channel"];
  const char* descrip = doc["Descrip"];
  const char* event = doc["Event"];
  const char* serialID = doc["SerialID"];
  const char* startTime = doc["StartTime"];
  const char* status = doc["Status"];
  const char* type = doc["Type"];

  sendMQTT(address, channel, descrip, event, serialID, startTime, status, type);
}

void sendMQTT(const char* address, int channel, const char* descrip, const char* event, const char* serialID, const char* startTime, const char* status, const char* type) {

  int timeout = 30;

  char msg[512];
  char seperator[5] = ", ";

  //Convert channel int to char to usen in "strcat"
  char channelChar[3];
  itoa(channel, channelChar, 10);

  //Filter the event
  if (strcmp(event, "HumanDetect") == 0) {
    strcpy(msg, address);
    strcat(msg, seperator);
    strcat(msg, channelChar);
    strcat(msg, seperator);
    //strcat(msg, descrip);
    //strcat(msg, seperator);
    strcat(msg, event);
    strcat(msg, seperator);
    //strcat(msg, serialID);
    //strcat(msg, seperator);
    strcat(msg, startTime);
    strcat(msg, seperator);
    //strcat(msg, status);
    //strcat(msg, seperator);
    strcat(msg, type);

    espClient.stop();
    client.setServer(mqttServer, mqttPort);
    client.connect("Alarm Server", mqttUsername, mqttPassword);

    while (!client.connected() && timeout > 0) {
      ledBlink(ST_CONNECT_MQTT);
      timeout--;

      if (timeout < 1) {
        Serial.println("  timeout! Retring....");
        espClient.stop();
        timeout = 30;
      }
    }
    Serial.println("Connected to MQTT server");

    //Publish msg
    client.publish("NVRAlarm_HD", msg);
  }

  else if (strcmp(descrip, "FaceDetection,0") == 0 || strcmp(descrip, "FaceDetection,1") == 0 || strcmp(descrip, "FaceDetection,2") == 0 || strcmp(descrip, "FaceDetection,3") == 0 || strcmp(descrip, "FaceDetection,4") == 0 || strcmp(descrip, "FaceDetection,5") == 0 || strcmp(descrip, "FaceDetection,6") == 0 || strcmp(descrip, "FaceDetection,7") == 0 || strcmp(descrip, "FaceDetection,8") == 0) {
    strcpy(msg, address);
    strcat(msg, seperator);
    //strcat(msg, channelChar);
    //strcat(msg, seperator);
    strcat(msg, descrip);
    strcat(msg, seperator);
    //strcat(msg, event);
    //strcat(msg, seperator);
    //strcat(msg, serialID);
    //strcat(msg, seperator);
    strcat(msg, startTime);
    strcat(msg, seperator);
    //strcat(msg, status);
    //strcat(msg, seperator);
    strcat(msg, type);

    espClient.stop();
    client.setServer(mqttServer, mqttPort);
    client.connect("Alarm Server", mqttUsername, mqttPassword);

    while (!client.connected() && timeout > 0) {
      ledBlink(ST_CONNECT_MQTT);
      timeout--;

      if (timeout < 1) {
        Serial.println("  timeout! Retring....");
        espClient.stop();
        timeout = 30;
      }

      //Publish msg
      client.publish("NVRAlarm_FD", msg);
    }
  }
}

void reconnect() {
  int timeout = 30;
  bool status = false;

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("WiFi disconnected");

    while (WiFi.status() != WL_CONNECTED && timeout > 0) {
      ledBlink(ST_CONNECT_WiFi);
      status = true;
      timeout--;
      if (timeout < 1) {
        Serial.println("  timeout! Retring....");
        timeout = 30;
        Serial.print("WiFi disconnected");
      }
    }
  }

  if (status != false) {
    Serial.println();
    Serial.println("WiFi Connected");
    status = false;
  }

  /*
    if (WiFi.status() == WL_CONNECTED && !client.connected()) {
      Serial.println("Mqtt Server disconnected");
      while (!client.connected()) {
        ledBlink(ST_CONNECT_MQTT);
        client.connect("Test Node 1", mqttUsername, mqttPassword);
      }
      Serial.println("MQTTServer connected");
    }
    */
}

void ledBlink(int time) {
  digitalWrite(STLED, LOW);
  delay(time);
  digitalWrite(STLED, HIGH);
  delay(time);
}