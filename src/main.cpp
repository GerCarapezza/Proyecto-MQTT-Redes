#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

//----------------WIFI------------------------------
const char* ssid     = "Wifi 1234";
const char* password = "carapezza3574";
// const char* ssid     = "S11";
// const char* password = "ger12345";

//---------------MQTT-------------------------------
const char* server  = "broker.hivemq.com";
const int port      = 1883;
const char* Client_ID       = "ubsajhfyuf3654hv64asd521454";
const char* topic_subscribe = "/ET28/REDES/IG/LED/#";
const char* topic_subscribe_SW   = "/ET28/REDES/IG/LED/SW";
const char* topic_subscribe_RED   = "/ET28/REDES/IG/LED/R";
const char* topic_subscribe_GREEN = "/ET28/REDES/IG/LED/G";
const char* topic_subscribe_BLUE  = "/ET28/REDES/IG/LED/B";
const char* topic_publish_TEMP   = "/ET28/REDES/IG/SENSOR/TEMP";
const char* topic_publish_HUM   = "/ET28/REDES/IG/SENSOR/HUM";

//--------------PINES-------------------------------
#define RED 13
#define GREEN 12
#define BLUE 14

//para funcion milis
unsigned long currentTime=0;
unsigned long previousTime=0;

//variables valores cada led
int var_R = 0;
int var_G = 0;
int var_B = 0;
bool SW = 0; // valor SW

//--------PWM----------------------------------------
const int freq = 5000;
const int ledChannel_R = 0;
const int ledChannel_G = 1;
const int ledChannel_B = 2;
const int resolution = 8;


//----WIFI settings----------------------------------
void wifiInit() {
  Serial.print("Conectándose a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
      delay(100);
  }
  Serial.println("");
  Serial.println("Conectado a WiFi");
  // Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String incoming = "";
  char playload_stringR[length + 1];
  char playload_stringG[length + 1];
  char playload_stringB[length + 1];
  char playload_string[length + 1];

 //---para recibir un mensaje como string----------
 for (int i = 0; i < length; i++) {
    incoming.concat((char)payload[i]);
  }
  Serial.print("Mensaje recibido ->  ");
  Serial.print(topic);
  Serial.println("");
  // Serial.println("Mensaje-> " + incoming );

 //-----------------NO USAR------------------------
  // memcpy(playload_string, payload, length);
  // playload_string[length] = '\0';
  // var = atoi(playload_string);
  // // Serial.println("Mensaje_SW-> " + var);
  // memcpy(playload_string, payload, length);
  // playload_string[length] = '\0';
  // var_R = atoi(playload_string);
  // // Serial.println("Mensaje_R-> " + var_R);
  // memcpy(playload_string, payload, length);
  // playload_string[length] = '\0';
  // var_G = atoi(playload_string);
  // // Serial.println("Mensaje_G-> " + var_G);
  // memcpy(playload_string, payload, length);
  // playload_string[length] = '\0';
  // var_B = atoi(playload_string);
  // // Serial.println("Mensaje_B-> " + var_B);

//comprueba de que topico viene el dato y lo convierte en int
  if(strcmp(topic, topic_subscribe_RED) == 0){ //comprueba led rojo
    memcpy(playload_stringR, payload, length);
    playload_stringR[length] = '\0';
    var_R = atoi(playload_stringR); // convierte de string a int
    Serial.print("Mensaje_R->");
    Serial.println(var_R);
  }
  else if (strcmp(topic, topic_subscribe_GREEN) == 0){ //comprueba led verde
    memcpy(playload_stringG, payload, length);
    playload_stringG[length] = '\0';
    var_G = atoi(playload_stringG);
    Serial.print("Mensaje_G->");
    Serial.println(var_G);
  }
  else if (strcmp(topic, topic_subscribe_BLUE) == 0){ //comprueba led azul
    memcpy(playload_stringB, payload, length);
    playload_stringB[length] = '\0';
    var_B = atoi(playload_stringB);
    Serial.print("Mensaje_B->");
    Serial.println(var_B);
  }
  else if (strcmp(topic, topic_subscribe_SW) == 0){ //comprueba switch
    SW = incoming.toInt();
    Serial.print("Mensaje_SW->" );
    Serial.println(SW);
  }


}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Intentando conectarse MQTT...");

    if (mqttClient.connect(Client_ID)) {
      Serial.println("Conectado");

      mqttClient.subscribe(topic_subscribe); //se subcribe al topic general .../LED/#

    } 
    else {
      Serial.print("Fallo, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" intentar de nuevo en 5 segundos");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void sendData(){  //funcion para enviar datos
  currentTime=millis();
  if((currentTime-previousTime)>3000) { //manda los datos cada 3 segundos
    previousTime=currentTime;
    mqttClient.publish(topic_publish_TEMP,"20");
    mqttClient.publish(topic_publish_HUM,"56");
  }
}

void setup(){
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  Serial.begin(115200);
  //config señal pwm
  ledcSetup(ledChannel_R, freq, resolution);
  ledcSetup(ledChannel_G, freq, resolution);
  ledcSetup(ledChannel_B, freq, resolution);
  //asigna cada salida a un pwm
  ledcAttachPin(RED, ledChannel_R);
  ledcAttachPin(GREEN, ledChannel_G);
  ledcAttachPin(BLUE, ledChannel_B);
  delay(10);
  wifiInit();
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);
}

void loop(){
   if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  if(SW == 1){
    ledcWrite(ledChannel_R, var_R);
    ledcWrite(ledChannel_G, var_G);
    ledcWrite(ledChannel_B, var_B);
  }
  else if(SW == 0){
    ledcWrite(ledChannel_R, 0);
    ledcWrite(ledChannel_G, 0);
    ledcWrite(ledChannel_B, 0);    
  }

  sendData();

}
