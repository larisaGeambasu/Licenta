/*
  Placa: AI-Thinker ESP32-CAM
*/

#include "src/OV2640.h" // biblioteca pentru camera
#include <WiFi.h>  //biblioteca pentru WiFi
#include <WebServer.h> //biblioteca pentru serverul web 
#include <WiFiClient.h> //biblioteca pentru client Wifi

#define MODEL_CAMERA_AI_THINKER

#include "pini_camera.h"

// Numele si parola retelei WiFi
#define SSID1 "DIGI-Nt7T"
#define PWD1 "RAh664hNTy"

OV2640 camera;

WebServer server(80); // Initializez serverul sa ruleze pe portul 80

// Configurarea headerului HTTP pentru streaming
const char HEADER[] = "HTTP/1.1 200 OK\r\n" \
                      "Access-Control-Allow-Origin: *\r\n" \
                      "Content-Type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n";
const char BOUNDARY[] = "\r\n--123456789000000000000987654321\r\n";
const char CTNTTYPE[] = "Content-Type: image/jpeg\r\nContent-Length: ";
const int hdrLen = strlen(HEADER);
const int bdrLen = strlen(BOUNDARY);
const int cntLen = strlen(CTNTTYPE);

// Functie pentru gestionarea streamului 
void gestioneaza_stream_jpg(void)
{
  char buf[32];
  int dimensiune;

  WiFiClient client = server.client();

  client.write(HEADER, hdrLen);
  client.write(BOUNDARY, bdrLen);

  while (true)
  {
    if (!client.connected()) break; // Verifica daca clientul este conectat
    camera.run();
    dimensiune = camera.getSize();
    client.write(CTNTTYPE, cntLen);
    sprintf(buf, "%d\r\n\r\n", dimensiune);
    client.write(buf, strlen(buf));
    client.write((char *)camera.getfb(), dimensiune);
    client.write(BOUNDARY, bdrLen);
  }
}

// Functie pentru gestionarea cererilor neidentificate
void cereri_notFound()
{
  String mesaj = "Serverul functioneaza!\n\n";
  mesaj += "URI: ";
  mesaj += server.uri();
  mesaj += "\nMetoda: ";
  mesaj += (server.method() == HTTP_GET) ? "GET" : "POST";
  mesaj += "\nArgumente: ";
  mesaj += server.args();
  mesaj += "\n";
  server.send(200, "text / plain", mesaj);
}

void setup()
{ Serial.begin(115200);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000; // Seteaza frecventa XCLK la 20 MHz
  config.pixel_format = PIXFORMAT_JPEG; // Seteaza formatul pixelilor la JPEG

  // Parametrii frame-ului
  config.frame_size = FRAMESIZE_QVGA; // Seteaza dimensiunea frame-ului la QVGA
  config.jpeg_quality = 12; // Seteaza calitatea JPEG la 12
  config.fb_count = 2; // Numarul de frame buffer-uri

  camera.init(config); // Initializeaza camera cu configuratia setata

  IPAddress ip;

  WiFi.mode(WIFI_STA); // Seteaza modul WiFi la Station
  WiFi.begin(SSID1, PWD1); // Se conecteaza la reteaua WiFi
  while (WiFi.status() != WL_CONNECTED) // Asteapta pana cand se stabileste conexiunea WiFi
  {
    delay(500);
    Serial.print(F(".")); // Afiseaza un punct pe monitor pentru fiecare incercare
  }
  ip = WiFi.localIP();
  Serial.println(F("WiFi conectat")); // Mesaj de confirmare in urma conectarii
  Serial.println("");
  Serial.println(ip);
  Serial.print("Link pentru streaming video: http://");
  Serial.print(ip);
  Serial.println("/mjpeg/1");
  server.on("/mjpeg/1", HTTP_GET, gestioneaza_stream_jpg); // Seteaza calea pentru stream-ul video
  server.onNotFound(cereri_notFound); // Seteaza calea pentru cereri neidentificate
  server.begin(); // Porneste serverul
}

void loop()
{
  server.handleClient(); // Gestioneaza cererile clientilor
}
