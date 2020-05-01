#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

#define LED1_OUT 12
#define LED2_OUT 14
#define LED3_OUT 27
#define BTN_IN 23
#define SENSOR_1 36
#define SENSOR_2 39
#define SENSOR_3 35

WebServer server(80);

//backend configuration
const char *host = "http://192.168.1.45:3000";
const char *url_save = "/url";

const char *ssid = "Nadezhda";
const char *password = "13521352ssd";

const char *PARAM_MESSAGE = "message";

int sensor1Value = 0;
int sensor2Value = 0;
int sensor3Value = 0;

const int sensor1Bound = 1000;
const int sensor2Bound = 2000;
const int sensor3Bound = 256;

String SendHTML(u16_t sensor1, u16_t sensor2, u16_t sensor3)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Smart Hydroponics</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #3498db;}\n";
  ptr += ".button-on:active {background-color: #2980b9;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>ESP32 Web Server</h1>\n";
  ptr += "<h3>Hello world!</h3>\n";
  ptr += "<p>Sensor 1 ";
  ptr += sensor1;
  ptr += "</p>\n";
  ptr += "<p>Sensor 2 ";
  ptr += sensor2;
  ptr += "</p>\n";
  ptr += "<p>Sensor 3 ";
  ptr += sensor3;
  ptr += "</p>\n";
  // if(led1stat)
  // {ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
  // else
  // {ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

  // if(led2stat)
  // {ptr +="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";}
  // else
  // {ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";}

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
void handle_OnConnect()
{
  sensor1Value = analogRead(SENSOR_1);
  sensor2Value = analogRead(SENSOR_2);
  sensor3Value = analogRead(SENSOR_3);

  Serial.println("CLient connected");
  server.send(200, "text/html", SendHTML(sensor1Value, sensor2Value, sensor3Value));
}
void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}

void SendJSON(String host, String url, String json)
{
  // Use WiFiClient class to create TCP connections
  HTTPClient http;
  http.setConnectTimeout(1000);
  http.begin(host + url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Connection", "keep-alive");

  // This will send the request to the server
  int httpResponseCode = http.POST(json);
  if (httpResponseCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] POST... code: %d\n", httpResponseCode);

    // file found at server
    if (httpResponseCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.println(payload);
    }
  }
  else
  {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
  }
  http.end();
}

void loop()
{
  server.handleClient();

  int s1 = analogRead(SENSOR_1);
  int s2 = analogRead(SENSOR_2);
  int s3 = analogRead(SENSOR_3);

  if (s1 > sensor1Bound)
    digitalWrite(LED1_OUT, HIGH);
  else
    digitalWrite(LED1_OUT, LOW);

  if (s2 > sensor2Bound)
    digitalWrite(LED2_OUT, HIGH);
  else
    digitalWrite(LED2_OUT, LOW);

  if (s3 > sensor3Bound)
    digitalWrite(LED3_OUT, HIGH);
  else
    digitalWrite(LED3_OUT, LOW);
  delay(100);
  //SendJSON(host, url_save, "");
  //delay(1000);
}

void setup()
{

  pinMode(LED1_OUT, OUTPUT);
  pinMode(LED2_OUT, OUTPUT);
  pinMode(LED3_OUT, OUTPUT);
  digitalWrite(LED1_OUT, LOW);
  digitalWrite(LED2_OUT, LOW);
  digitalWrite(LED3_OUT, LOW);
  pinMode(SENSOR_1, INPUT);
  pinMode(BTN_IN, INPUT);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
}