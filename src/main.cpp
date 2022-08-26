#include <Arduino.h>
#include <main.h>

bool debug = true;

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);
AM2320 am2320(&Wire);

void setup(){
  Serial.begin(115200);
  Wire.begin();
  wifiInit();

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  // ------------------------------------------
  //            Configuración PWM
  // ------------------------------------------

  ledcSetup(ledChannel_R, pwm_frec, resolution);  // Configura el canal PWM
  ledcSetup(ledChannel_G, pwm_frec, resolution);
  ledcSetup(ledChannel_B, pwm_frec, resolution);
  ledcAttachPin(RED_PIN, ledChannel_R);           // Asigna el pin al canal PWM.
  ledcAttachPin(GREEN_PIN, ledChannel_G);
  ledcAttachPin(BLUE_PIN, ledChannel_B);
  delay(10);
  
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);
}

void loop(){
   if (!mqttClient.connected()) {       // Comprueba la coneccion con el broker
    reconnect();
  }
  mqttClient.loop();

  if(led_brightness_payload == 1){        // Si el switch esta activado, envia los datos del pwm al led
    ledcWrite(ledChannel_R, red_payload);
    ledcWrite(ledChannel_G, green_payload);
    ledcWrite(ledChannel_B, blue_payload);
  }
  else if(led_brightness_payload == 0){    // Apaga led
    ledcWrite(ledChannel_R, 0);
    ledcWrite(ledChannel_G, 0);
    ledcWrite(ledChannel_B, 0);    
  }
  am2320_sensor();
  sendData();
}

void wifiInit(){  /*Inicialización del WiFi*/
  Serial.printf("Conectándose a %s\n", ssid);

  // ! Serial.print("Conectándose a ");
  // ! Serial.println(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
      delay(100);
  }

  Serial.printf("\nConectado a %s\n", ssid);
  Serial.printf("Dirección IPv4 .................. %u.%u.%u.%u\n", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  // ! Serial.println("Dirección IP: ");
  // ! Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String incoming = "";
  char payload_stringR[length + 1];
  char payload_stringG[length + 1];
  char payload_stringB[length + 1];
  // ! char payload_string[length + 1];

 for (int i = 0; i < length; i++) { // Crea el payload, concatenando los datos y luego lo convierte de byte a string
    incoming.concat((char)payload[i]);
  }
  Serial.printf("Mensaje recibido ->  %s\n", topic);
  // ! Serial.print("Mensaje recibido ->  ");
  // ! Serial.print(topic);
  // ! Serial.println("");
  // ! Serial.println("Mensaje-> " + incoming ); //imprime el mensaje que recibe como string

  if(strcmp(topic, topic_subscribe_RED) == 0){ // Comprueba si se está suscripto al topic en que se publica
    memcpy(payload_stringR, payload, length);
    payload_stringR[length] = '\0';
    red_payload = atoi(payload_stringR); // Convierte el payload de string a int
    Serial.printf("Mensaje Red -> %i\n", red_payload);
    // ! Serial.println(red_payload);
  }
  else if (strcmp(topic, topic_subscribe_GREEN) == 0){
    memcpy(payload_stringG, payload, length);
    payload_stringG[length] = '\0';
    green_payload = atoi(payload_stringG);
    Serial.printf("Mensaje Green-> %i\n", green_payload);
    // ! Serial.println(green_payload);
  }
  else if (strcmp(topic, topic_subscribe_BLUE) == 0){
    memcpy(payload_stringB, payload, length);
    payload_stringB[length] = '\0';
    blue_payload = atoi(payload_stringB);
    Serial.printf("Mensaje Blue -> %i\n", blue_payload);
    // ! Serial.println(blue_payload);
  }
  else if (strcmp(topic, topic_subscribe_SW) == 0){
    led_brightness_payload = incoming.toInt(); //convierte el valor del sw en int
    Serial.printf("Mensaje Brightness -> %i\n", led_brightness_payload );
    // ! Serial.println(led_brightness_payload);
  }
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Intentando conectarse al broker MQTT...\n");
    if (mqttClient.connect(Client_ID)) {
      Serial.println("Conectado\n");
      mqttClient.subscribe(topic_subscribe); // Suscribe al topic raíz
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

void sendData(){  // Envía los datos al broker
  if((millis() - sendDataPreviousTime) > 3000) { // Publica cada 3 segundos
    sendDataPreviousTime = millis();
    mqttClient.publish(topic_publish_TEMP, (char*)sensed_temperature); // Envia datos al topic de temperatura
    mqttClient.publish(topic_publish_HUM, (char*)sensed_humidity); // Envia datos al topic de humedad
    // TODO: Implementar el cambio de float a char para enviar los datos del sensor
  }
}

void am2320_sensor() {
  if((millis() - sensorPreviousTime) > 2000) { // Sensa cada 2 segundos
    sensorPreviousTime = millis();
    if(debug){ 
      Serial.println(F("Chip = AM2320"));
      switch(am2320.Read()) {
        case 1:
          Serial.println(F("Sensor offline"));
          break;
        case 0:
          temperature = am2320.cTemp;
          humidity = am2320.Humidity;
          Serial.printf("\tHumidity = %f%\n", am2320.Humidity);
          Serial.printf("\tTemperature = %f°C\n", am2320.cTemp);
          break;
      }
    }
  } 
  // ! delay(2000);
}
