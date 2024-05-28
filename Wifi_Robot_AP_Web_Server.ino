#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

#define ENA  14  // Chân Enable/tốc độ động cơ bên phải GPIO14(D5)
#define ENB  12  // Chân Enable/tốc độ động cơ bên trái GPIO12(D6)
#define IN_1  15 // Chân IN1 của L298N cho động cơ bên phải GPIO15(D8)
#define IN_2  13 // Chân IN2 của L298N cho động cơ bên phải GPIO13(D7)
#define IN_3  2  // Chân IN3 của L298N cho động cơ bên trái GPIO2(D4)
#define IN_4  0  // Chân IN4 của L298N cho động cơ bên trái GPIO0(D3)

const char* ssid = "your_SSID";      // Tên mạng WiFi
const char* password = "your_PASSWORD"; // Mật khẩu mạng WiFi
int speedCar = 1023; // Tốc độ xe

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
  Serial.print("Địa chỉ IP AP: ");
  Serial.println(myIP);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<html>\
                                      <head>\
                                        <style>\
                                          body {\
                                            display: flex;\
                                            justify-content: center;\
                                            align-items: center;\
                                            height: 100vh;\
                                            background-color: #f0f0f0;\
                                            margin: 0;\
                                            font-family: Arial, sans-serif;\
                                          }\
                                          .control-panel {\
                                            text-align: center;\
                                          }\
                                          .control-panel h1 {\
                                            color: #333;\
                                          }\
                                          .button {\
                                            width: 100px;\
                                            height: 100px;\
                                            margin: 10px;\
                                            font-size: 18px;\
                                            background-color: #008CBA;\
                                            color: white;\
                                            border: none;\
                                            border-radius: 10px;\
                                            transition: background-color 0.3s ease;\
                                          }\
                                          .button:active {\
                                            background-color: #005f73;\
                                          }\
                                        </style>\
                                      </head>\
                                      <body>\
                                        <div class=\"control-panel\">\
                                          <h1>Điều khiển xe ESP8266</h1>\
                                          <button class=\"button\" ontouchstart=\"sendCommand('forward')\" ontouchend=\"sendCommand('stop')\">Tiến</button><br>\
                                          <button class=\"button\" ontouchstart=\"sendCommand('left')\" ontouchend=\"sendCommand('stop')\">Trái</button>\
                                          <button class=\"button\" ontouchstart=\"sendCommand('right')\" ontouchend=\"sendCommand('stop')\">Phải</button><br>\
                                          <button class=\"button\" ontouchstart=\"sendCommand('backward')\" ontouchend=\"sendCommand('stop')\">Lùi</button>\
                                        </div>\
                                        <script>\
                                          var connection = new WebSocket('ws://' + location.hostname + ':81/');\
                                          connection.onopen = function () {\
                                            console.log('WebSocket kết nối');\
                                          };\
                                          connection.onerror = function (error) {\
                                            console.log('Lỗi WebSocket: ' + error);\
                                          };\
                                          connection.onmessage = function (e) {\
                                            console.log('Server: ' + e.data);\
                                          };\
                                          function sendCommand(command) {\
                                            connection.send(command);\
                                          }\
                                          document.addEventListener('keydown', function(event) {\
                                            if(event.key === 'ArrowUp' || event.key === 'w') {\
                                              sendCommand('forward');\
                                            } else if(event.key === 'ArrowDown' || event.key === 's') {\
                                              sendCommand('backward');\
                                            } else if(event.key === 'ArrowLeft' || event.key === 'a') {\
                                              sendCommand('left');\
                                            } else if(event.key === 'ArrowRight' || event.key === 'd') {\
                                              sendCommand('right');\
                                            }\
                                          });\
                                          document.addEventListener('keyup', function(event) {\
                                            if(['ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight', 'w', 'a', 's', 'd'].includes(event.key)) {\
                                              sendCommand('stop');\
                                            }\
                                          });\
                                        </script>\
                                      </body>\
                                    </html>");
  });

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("Máy chủ HTTP đã bắt đầu");
}

void loop() {
  webSocket.loop();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  String command = "";
  for (size_t i = 0; i < length; i++) {
    command += (char) payload[i];
  }
  Serial.println("Nhận lệnh: " + command);

  if (command == "forward") {
    diTien();
  } else if (command == "backward") {
    diLui();
  } else if (command == "left") {
    reTrai();
  } else if (command == "right") {
    rePhai();
  } else if (command == "stop") {
    dungLai();
  }
}

void diTien() { 
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}

void diLui() { 
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}

void rePhai() { 
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}

void reTrai() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}

void dungLai() {  
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}
