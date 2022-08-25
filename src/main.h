#include <WiFi.h>
#include <PubSubClient.h>
#include <AM2320.h>

// * --------------------------------------
// *                PINES   
// * --------------------------------------

#define RED_PIN   13
#define GREEN_PIN 12
#define BLUE_PIN  14

// * --------------------------------------
// *         VARIABLES GLOBALES   
// * --------------------------------------

byte red_payload = 0;
byte green_payload = 0;
byte blue_payload = 0;
byte led_brightness_payload = 0;

// --------------------------------------
//                PWM
// --------------------------------------

const int pwm_frec = 5000;
const int ledChannel_R = 0;
const int ledChannel_G = 1;
const int ledChannel_B = 2;
const int resolution = 8;

// * -------------------------------
// *    TEMPORIZADO CON millis()
// * -------------------------------
// TODO: Implementar temporizado con módulo de timer

unsigned long sendDataPreviousTime = 0;
unsigned long sensorPreviousTime = 0;

// * --------------------------------------
// *             CREDENCIALES   
// * --------------------------------------

// -------------------------------------
//                 WIFI
// -------------------------------------
// TODO: Descomentar las credenciales que correspondan

// const char* ssid     = "Ivoo";
// const char* password = "f42xiS3Hj";
// const char* ssid     = "Wifi 1234";
// const char* password = "carapezza3574";
const char* ssid     = "S11";
const char* password = "ger12345";

// --------------------------------------
//                  MQTT
// --------------------------------------
const char* server = "broker.hivemq.com";
const int port = 1883;
const char* Client_ID = "ubsajhfyuf3654hv64asd521454wreg";
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

void wifiInit();    /*Inicialización del WiFi*/
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void sendData();
void am2320_sensor();
