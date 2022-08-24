#include <Arduino.h>
#include <main.h>

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

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

void setup(){
  //define los pines como salida
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  Serial.begin(115200); //comunicacion serie
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
   if (!mqttClient.connected()) { //comprueba la coneccion con el topic
    reconnect();
  }
  mqttClient.loop();

  if(SW == 1){ //si el switch esta activado envia los datos del pwm al led
    ledcWrite(ledChannel_R, var_R);
    ledcWrite(ledChannel_G, var_G);
    ledcWrite(ledChannel_B, var_B);
  }
  else if(SW == 0){ //pone un 0 a los pines del led si el switch esta apagado
    ledcWrite(ledChannel_R, 0);
    ledcWrite(ledChannel_G, 0);
    ledcWrite(ledChannel_B, 0);    
  }

  sendData(); //llama a la funcion para publicar en el topic
}

void wifiInit(){  //Inicialización del WiFi
  Serial.printf("Conectándose a %s\n", ssid);

  // ! Serial.print("Conectándose a ");
  // ! Serial.println(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
      delay(100);
  }

  Serial.printf("\nConectado a %s", ssid);
  Serial.printf("Dirección IPv4 .................. %s ", WiFi.localIP());

  // ! Serial.println("Dirección IP: ");
  // ! Serial.println(WiFi.localIP());
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
  Serial.printf("Mensaje recibido ->  %s\n", topic);
  // ! Serial.print("Mensaje recibido ->  ");
  // ! Serial.print(topic);
  // ! Serial.println("");
  // ! Serial.println("Mensaje-> " + incoming ); //imprime el mensaje que recibe como string

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
    SW = incoming.toInt(); //convierte el valor del sw en int
    Serial.print("Mensaje_SW->" );
    Serial.println(SW);
  }
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Intentando conectarse MQTT...\n");
    if (mqttClient.connect(Client_ID)) {
      Serial.println("Conectado\n");
      mqttClient.subscribe(topic_subscribe); //se subcribe al topic general /LED/#
    } 
    else {
      Serial.printf("Fallo al conectarse al broker\nCRC = %s\n", mqttClient.state());
      // ! Serial.print("Fallo, rc=");
      // ! Serial.print(mqttClient.state());
      Serial.print("Intentando nuevamente en 5 segundos..."); // Espera 5 segundos antes de reintentar
      delay(5000);
    }
  }
}

void sendData(){  //funcion para enviar datos
  currentTime=millis();
  if((currentTime-previousTime)>3000) { //manda los datos cada 3 segundos
    previousTime=currentTime;
    mqttClient.publish(topic_publish_TEMP, temperature); // Envia datos al topic de temperatura
    mqttClient.publish(topic_publish_HUM, humidity); // Envia datos al topic de humedad
  }
}
