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

bool isdevice1Switched = false;
bool isdevice2Switched = false;
bool isdevice3Switched = false;

bool isAutomatic = true;

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

void ChangeDeviceState(u16_t device, u16_t newState, String reason)
{
  String json = String("{\"Device\" : \"") + device + "\"," +
                "\"Reason\":\"" + reason + "\"," +
                "\"PreviosState\" : \"" + digitalRead(device) + "\"," +
                "\"NewState\" : \"" + newState + "\"}";
  SendJSON(host, url_save, json);
  digitalWrite(device, newState);
}
void handle_ManualChange()
{
  isAutomatic = false;
  for (int i = 0; i < server.args(); i++)
  {
    if (server.argName(i) == "dev1")
    {
      if (server.arg(i) == "0")
        ChangeDeviceState(LED1_OUT, LOW, "Changed manually via GET");
      else if (server.arg(i) == "1")
        ChangeDeviceState(LED1_OUT, HIGH, "Changed manually via GET");
    }
    else if (server.argName(i) == "dev2")
    {
      if (server.arg(i) == "0")
        ChangeDeviceState(LED2_OUT, LOW, "Changed manually via GET");
      else if (server.arg(i) == "1")
        ChangeDeviceState(LED2_OUT, HIGH, "Changed manually via GET");
    }
    else if (server.argName(i) == "dev3")
    {
      if (server.arg(i) == "0")
        ChangeDeviceState(LED3_OUT, LOW, "Changed manually via GET");
      else if (server.arg(i) == "1")
        ChangeDeviceState(LED3_OUT, HIGH, "Changed manually via GET");
    }
    else if (server.argName(i) == "automatic")
    {
      if (server.arg(i) == "true") isAutomatic = true;
    }
  }
  server.send(200, "text/plain", "Done!");
}
void loop()
{
  server.handleClient();

  int s1 = analogRead(SENSOR_1);
  int s2 = analogRead(SENSOR_2);
  int s3 = analogRead(SENSOR_3);

  if (isAutomatic)
  {

    if ((s1 > sensor1Bound) && !isdevice1Switched)
    {
      ChangeDeviceState(LED1_OUT, HIGH, "Higher temp bound is reached");
      isdevice1Switched = true;
    }

    else if ((s1 < sensor1Bound) && isdevice1Switched)
    {
      ChangeDeviceState(LED1_OUT, LOW, "Lower temp bound is reached");
      isdevice1Switched = false;
    }
    if ((s2 > sensor2Bound) && !isdevice2Switched)
    {
      ChangeDeviceState(LED2_OUT, HIGH, "Higher moist bound is reached");
      isdevice2Switched = true;
    }

    else if ((s2 < sensor2Bound) && isdevice2Switched)
    {
      ChangeDeviceState(LED2_OUT, LOW, "Lower moist bound is reached");
      isdevice2Switched = false;
    }
    if ((s3 > sensor3Bound) && !isdevice3Switched)
    {
      ChangeDeviceState(LED3_OUT, HIGH, "Higher brightness bound is reached");
      isdevice3Switched = true;
    }

    else if ((s3 < sensor3Bound) && isdevice3Switched)
    {
      ChangeDeviceState(LED3_OUT, LOW, "Lower brightness bound is reached");
      isdevice3Switched = false;
    }
    delay(1000);
  }
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
  server.on("/setState", handle_ManualChange);

  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");

  int s1 = analogRead(SENSOR_1);
  int s2 = analogRead(SENSOR_2);
  int s3 = analogRead(SENSOR_3);

  if (s1 > sensor1Bound)
  {
    ChangeDeviceState(LED1_OUT, HIGH, "Initial change");
    isdevice1Switched = true;
  }
  if (s2 > sensor2Bound)
  {
    ChangeDeviceState(LED2_OUT, HIGH, "Initial change");
    isdevice2Switched = true;
  }
  if (s3 > sensor3Bound)
  {
    ChangeDeviceState(LED3_OUT, HIGH, "Initial change");
    isdevice3Switched = true;
  }
  String reason = "Initial setup";

  String json = String("{\"Reason\":\"") + reason + "\"," +
                "\"Temp\" : \"" + s1 + "\"," +
                "\"Temp\" : \"" + s2 + "\"," +
                "\"Brightness\" : \"" + s3 + "\"," +
                "\"Device1\" : \"" + isdevice1Switched + "\"," +
                "\"Device2\" : \"" + isdevice2Switched + "\"," +
                "\"Device3\" : \"" + isdevice3Switched + "\"}";
  SendJSON(host, url_save, json);
}