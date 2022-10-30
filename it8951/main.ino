#include <WiFi.h>

const char *SSID = "XXXXX";
const char *PWD = "XXXXX";

String currentFileName = String();

unsigned char image_buffer[20000];

File fsUploadFile;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void notifyClients() {
  Serial.println("Responding to client...");
  ws.textAll("ok");
}

StaticJsonDocument<20000> jsonDoc;
char finalData[20000];

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
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
      notifyClients();
    }
  }
}

void renderImage() {
  const String image = jsonDoc["image"];
  const int width = jsonDoc["width"];
  const int height = jsonDoc["height"];
  const int x = jsonDoc["x"];
  const int y = jsonDoc["y"];  
  setImage(image, x, y, width, height);
  jsonDoc = NULL;
  memset(finalData, 0, sizeof finalData);
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
      Serial.println("Message arrived");
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
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
        // Start server
        server.begin();
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
  stringToPixelArray(image);
  Serial.print("IMAGE BUFFER FIRST ITEM : ");
  Serial.println(image_buffer[0]);
  display_buffer(image_buffer, x, y, width, height);
}

void stringToPixelArray(String dataString) {
  Serial.print("IMAGE data : ");
  Serial.println(dataString);
  char* buffer = const_cast<char*>(dataString.c_str());
  Serial.print("IMAGE char data : ");
  Serial.println(buffer[2]);
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
