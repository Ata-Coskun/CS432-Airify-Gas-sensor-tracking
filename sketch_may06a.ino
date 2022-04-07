#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

#define  smokeD2 33
#define ORG "********"
#define DEVICE_TYPE "ESP32"
#define DEVICE_ID "************"
#define TOKEN "****************"

// Your threshold value
int sensorThres = 3000;
uint64_t chipid;

const char* ssid = "NetMASTER Uydunet-4780";
const char* password = "**************";
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char pubTopic1[] = "iot-2/evt/status1/fmt/json";
char authMethod[] = "use-token-auth";

char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

WiFiClient wifiClient;
PubSubClient client(server, 1883, NULL, wifiClient);

void setup() {

  pinMode(smokeD2, INPUT);
  Serial.begin(115200);
  Serial.println("started");

  chipid = ESP.getEfuseMac();
  Serial.printf("ESP32 Chip ID = %04X", (uint16_t)(chipid >> 32));
  Serial.printf("%08X\n", (uint32_t)chipid);
  delay(3000);

  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());

  if (!client.connected()) {
    Serial.print("Reconnecting client to ");
    Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    Serial.println("Bluemix connected");
  }
}

long lastMsg = 0;
void loop() {

  client.loop();
  long now = millis();

if (now - lastMsg > 3000) {

        int analogSensor = analogRead(smokeD2);
        Serial.print("Pin A0: ");
        Serial.println(analogSensor);
        Serial.println();
        Serial.println("Sensor is checked.");
        Serial.println();
        // Checks if it has reached the threshold value
        if (analogSensor > sensorThres)
        {
         Serial.println("Dangerous gas level.");
         Serial.println();
        }
        else
        {
         Serial.println("Quality is good.");
         Serial.println();
        }
         delay(3000);
    
      lastMsg = now;
      String payload = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
            payload += ",\"mq135_air_quality\":";
            payload += analogSensor;
            payload += "}}";
     
      Serial.print("Sending payload: ");
      Serial.println(payload);

      if (client.publish(pubTopic1, (char*) payload.c_str())) {
          Serial.println("Publish ok");
      } else {
          Serial.println("Publish failed");
      }
  }
}
