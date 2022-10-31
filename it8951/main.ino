#include <WiFi.h>

const char *SSID = "xxx";
const char *PWD = "xxx";
const int PAYLOAD_BYTES = 28000;
unsigned char image_buffer[PAYLOAD_BYTES];
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void notifyClients(uint32_t id) {
  Serial.println("Responding to client...");
  ws.text(id, "ok");
}

StaticJsonDocument<PAYLOAD_BYTES> jsonDoc;
char finalData[PAYLOAD_BYTES];

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len, uint32_t id) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  strcat(finalData, (char*)data);
  uint64_t currentLen = len + info->index;
  if (currentLen == info->len) {
    finalData[currentLen] = 0;
    DeserializationError error = deserializeJson(jsonDoc, finalData);
    if (error) {
      return;
    }  else {
      renderImage();
      notifyClients(id);
    }
  }
}

void renderImage() {
  setImage(jsonDoc["image"], jsonDoc["x"], jsonDoc["y"], jsonDoc["width"], jsonDoc["height"]);
  jsonDoc = NULL;
  memset(finalData, 0, sizeof finalData);
  memset(image_buffer, 0, sizeof image_buffer);
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len, client->id());
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  ws.onEvent(onEvent);
  server.on("/display/sleep", HTTP_GET, [](AsyncWebServerRequest *request){
    IT8951SystemRun();
    request->send(200, "text/plain", "display is sleeping");
  });
  server.on("/display/standBy", HTTP_GET, [](AsyncWebServerRequest *request){
    IT8951StandBy();
    request->send(200, "text/plain", "display is sleeping");
  });
  server.on("/display/systemRun", HTTP_GET, [](AsyncWebServerRequest *request){
    IT8951Sleep();
    request->send(200, "text/plain", "display has woken up");
  });
  server.on("/display/clear", HTTP_GET, [](AsyncWebServerRequest *request){
    IT8951DisplayArea(0,0, gstI80DevInfo.usPanelW, gstI80DevInfo.usPanelH, 0);
    request->send(200, "text/plain", "display has been cleared");
  });
  server.addHandler(&ws);
  // Start server
  server.begin();
}

void setup(void)
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("Display init ...");
    if(!display_begin()) {
        Serial.println("Display init failed");
        Serial.println("Aborting and retrying in 1.0s, this is not a crash");
        delay(1000);
        exit(1);
    };
    Serial.println("Display init ok");
    // clear screen => make it white
    IT8951DisplayArea(0,0, gstI80DevInfo.usPanelW, gstI80DevInfo.usPanelH, 0);
    bool connected = connectToWiFi();
    if (connected == true) {
      Serial.println("Connected to wifi !");
        initWebSocket();
    }
}

void loop() {
  ws.cleanupClients();
}

void clearImage() {
  Serial.println("Clearing display ....");
  IT8951DisplayArea(0,0, gstI80DevInfo.usPanelW, gstI80DevInfo.usPanelH, 0);
}

void setImage(String image, int x, int y, int width, int height) {
  Serial.print("X value given to diaplay : ");
  Serial.println(x);
  stringToPixelArray(image);
  display_buffer(image_buffer, x, y, width, height);
}

void stringToPixelArray(String dataString) {
  char* buffer = const_cast<char*>(dataString.c_str());
  //image_buffer = reinterpret_cast<unsigned char*>(buffer);
  
  int i=0;
  char *p = strtok(buffer, ",");
  while(p != NULL) {
    image_buffer[i++] = strtoul(p, NULL, 16);
    p = strtok(NULL, ",");
  }
}

bool connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(SSID);
  WiFi.begin(SSID, PWD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
 
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
  return true;
}
