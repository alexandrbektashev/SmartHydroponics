
#include <WiFi.h>
#include <AsyncTCP.h>

const char* ssid     = "Nadezhda";
const char* password = "13521352ssd";

const char* streamId   = "....................";
const char* privateKey = "....................";

void setup() {
  Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:

}
