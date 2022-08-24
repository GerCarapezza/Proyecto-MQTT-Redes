#include <WiFi.h>
#include <PubSubClient.h>

// * --------------------------------------
// *                PINES   
// * --------------------------------------

#define RED   13
#define GREEN 12
#define BLUE  14

// * -------------------------------
// *    TEMPORIZADO CON millis()
// * -------------------------------
// TODO: Implementar temporizado con módulo de timer

unsigned long currentTime = 0;
unsigned long previousTime = 0;

// * --------------------------------------
// *             CREDENCIALES   
// * --------------------------------------

// -------------------------------------
//                 WIFI
// -------------------------------------
// TODO: Descomentar las credenciales que correspondan

const char* ssid     = "Ivoo";
const char* password = "f42xiS3Hj";
// const char* ssid     = "Wifi 1234";
// const char* password = "carapezza3574";
// const char* ssid     = "S11";
// const char* password = "ger12345";

// --------------------------------------
//                  MQTT
// --------------------------------------
const char* server = "broker.hivemq.com";
const int port = 1883;
const char* Client_ID = "ubsajhfyuf3654hv64asd521454";
const char* topic_subscribe = "/ET28/REDES/IG/LED/#";
const char* topic_subscribe_SW = "/ET28/REDES/IG/LED/SW";
const char* topic_subscribe_RED = "/ET28/REDES/IG/LED/R";
const char* topic_subscribe_GREEN = "/ET28/REDES/IG/LED/G";
const char* topic_subscribe_BLUE = "/ET28/REDES/IG/LED/B";
const char* topic_publish_TEMP = "/ET28/REDES/IG/SENSOR/TEMP";
const char* topic_publish_HUM  = "/ET28/REDES/IG/SENSOR/HUM";


// * -------------------------------
// *           FUNCIONES
// * -------------------------------

void wifiInit();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void sendData();

// * -------------------------------
// *         SENSOR AM2320
// * -------------------------------

char* temperature = "20";
char* humidity = "36";

