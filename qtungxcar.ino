#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

#define ENA  14  // Enable/speed motors Right        GPIO14(D5)
#define ENB  12  // Enable/speed motors Left         GPIO12(D6)
#define IN_1  15 // L298N in1 motors Right           GPIO15(D8)
#define IN_2  13 // L298N in2 motors Right           GPIO13(D7)
#define IN_3  2  // L298N in3 motors Left            GPIO2(D4)
#define IN_4  0  // L298N in4 motors Left            GPIO0(D3)

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
int speedCar = 1023;

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);  
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT); 

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<html>\
                                      <body>\
                                        <h1>ESP8266 Car Control</h1>\
                                        <button ontouchstart=\"sendCommand('forward')\" ontouchend=\"sendCommand('stop')\">Forward</button>\
                                        <button ontouchstart=\"sendCommand('backward')\" ontouchend=\"sendCommand('stop')\">Backward</button>\
                                        <button ontouchstart=\"sendCommand('left')\" ontouchend=\"sendCommand('stop')\">Left</button>\
                                        <button ontouchstart=\"sendCommand('right')\" ontouchend=\"sendCommand('stop')\">Right</button>\
                                        <script>\
                                          var connection = new WebSocket('ws://' + location.hostname + ':81/');\
                                          connection.onopen = function () {\
                                            console.log('WebSocket connected');\
                                          };\
                                          connection.onerror = function (error) {\
                                            console.log('WebSocket Error: ' + error);\
                                          };\
                                          connection.onmessage = function (e) {\
                                            console.log('Server: ' + e.data);\
                                          };\
                                          function sendCommand(command) {\
                                            connection.send(command);\
                                          }\
                                        </script>\
                                      </body>\
                                    </html>");
  });

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("HTTP server started");
}

void loop() {
  webSocket.loop();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  String command = "";
  for (size_t i = 0; i < length; i++) {
    command += (char) payload[i];
  }
  Serial.println("Received command: " + command);

  if (command == "forward") {
    goAhead();
  } else if (command == "backward") {
    goBack();
  } else if (command == "left") {
    goLeft();
  } else if (command == "right") {
    goRight();
  } else if (command == "stop") {
    stopCar();
  }
}

void goAhead() { 
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}

void goBack() { 
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}

void goRight() { 
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}

void goLeft() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}

void stopCar() {  
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}
