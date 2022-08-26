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
float temperature = 0;
float humidity = 0;
float* sensed_temperature = &temperature;
float* sensed_humidity = &humidity;


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

const char* ssid     = "Fibertel WiFi666 2.4GHz";
const char* password = "0044304973";
// const char* ssid     = "Ivoo";
// const char* password = "f42xiS3Hj";
// const char* ssid     = "Wifi 1234";
// const char* password = "carapezza3574";
// const char* ssid     = "S11";
// const char* password = "ger12345";

// --------------------------------------
//                  MQTT
// --------------------------------------
const char* server = "broker.hivemq.com";
const int port = 1883;
const char* Client_ID = "kjaf1234hgb12ghj54l234hgj15123hjgl5uyg";
const char* topic_subscribe = "/et28/redes/ig/led/#";
const char* topic_subscribe_SW = "/et28/redes/ig/led/sw";
const char* topic_subscribe_RED = "/et28/redes/ig/led/R";
const char* topic_subscribe_GREEN = "/et28/redes/ig/led/G";
const char* topic_subscribe_BLUE = "/et28/redes/ig/led/B";
const char* topic_publish_TEMP = "/et28/redes/ig/sensor/tempc";                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
const char* topic_publish_HUM  = "/et28/redes/ig/sensor/hum";


// * -------------------------------
// *           FUNCIONES
// * -------------------------------

void wifiInit();    /*Inicialización del WiFi*/
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void sendData();
void am2320_sensor();
