#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

//----------------WIFI------------------------------
const char* ssid     = "Wifi 1234";
const char* password = "carapezza3574";

//---------------MQTT-------------------------------
const char* server  = "broker.hivemq.com";
const int port      = 1883;
const char* Client_ID       = "ubsafiugijsiaifhubafdv564521454";
const char* topic_subscribe = "/ET28/REDES/GERMAN/RGB";
const char* topic_publish   = "/ET28/REDES/GERMAN/TEMPERATURA";

//--------------PINES-------------------------------
int ledpin = 26; //solo para probar

int RED = 32;
int GREEN = 35;
int BLUE = 34;


// unsigned long currentTime=0;
// unsigned long previousTime=0;

int var = 0;
// int var_R = 0;
// int var_G = 0;
// int var_B = 0;

int ledval = 0;
// int fotoval = 0;
char datos[40];


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
  Serial.print("Mensaje recibido ->");
  Serial.print(topic);
  Serial.println("");


 for (int i=0;i<length;i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensaje->" + incoming);
}



void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Intentando conectarse MQTT...");

    if (mqttClient.connect(Client_ID)) {
      Serial.println("Conectado");

      mqttClient.subscribe("/ET28/REDES/GERMAN");

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

// void sendData(){  //funcion para enviar datos
//   currentTime=millis();
//   if((currentTime-previousTime)>3000) {
//     previousTime=currentTime;
//   }
// }

void setup(){
  pinMode(ledpin,OUTPUT);
  Serial.begin(115200);
  delay(10);
  wifiInit();
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);
}

void loop()
{
   if (!mqttClient.connected()) {
    reconnect();
  }

  mqttClient.loop();



  if(var == 0)
  {
    digitalWrite(ledpin,LOW);
    Serial.print("String: ");
    Serial.println(resultS);
  }
  else if (var == 1)
  {
    digitalWrite(ledpin,HIGH);
    Serial.print("String: ");
    Serial.println(resultS);
  }

  // sendData();

}
