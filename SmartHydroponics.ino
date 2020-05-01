#include <Arduino.h>
// #include <WiFi.h>
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
#include <WebServer.h>

#define LED1_OUT 12
#define LED2_OUT 14
#define LED3_OUT 27
#define BTN_IN 23
#define SENSOR_1 36

WebServer server(80);

const char *ssid = "Nadezhda";
const char *password = "13521352ssd";

const char *PARAM_MESSAGE = "message";

String SendHTML()
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

  //  if(led1stat)
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

void loop()
{
  server.handleClient();
  // put your main code here, to run repeatedly:
}

void handle_OnConnect()
{
  digitalWrite(LED1_OUT, HIGH);
  Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF");
  server.send(200, "text/html", SendHTML());
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
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
