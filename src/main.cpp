#include <Arduino.h>
#include <main.h>

bool debug = true;

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);
AM2320 am2320(&Wire);

void setup(){
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

  Serial.begin(115200);
  Wire.begin();
  wifiInit();
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);
  Serial.println("Sensor Chip ............. AM2320");
}

void loop(){
   if (!mqttClient.connected()) {       // Comprueba la conexión con el broker
    reconnect();
  }

  if((millis() - sensorPreviousTime) > 1500) { // Sensa cada 3 segundos
    sensorPreviousTime = millis(); 
    am2320_sensor();
  }
  if((millis() - sendDataPreviousTime) > 1500) { // Publica cada 3 segundos
    sendDataPreviousTime = millis();
    sendData();
  }

  mqttClient.loop();

  controlRGB();
}

void wifiInit(){  /*Inicialización del WiFi*/
  Serial.printf("\nConectándose a %s\n", ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
      delay(100);
  }

  Serial.printf("\nConectado a %s\n", ssid);
  Serial.printf("Dirección IPv4 .................. %u.%u.%u.%u\n", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);

}

void callback(char* topic, byte* payload, unsigned int length) {

  // TODO: Si el switch está apagado, sería ideal que no reciba los datos del LED. Es inútil cambiar el color al LED apagado
  // TODO: Optimizar el envío y recepción de datos.

  String incoming = "";
  char payload_stringR[length + 1];
  char payload_stringG[length + 1];
  char payload_stringB[length + 1];
  // ! char payload_string[length + 1];

 for (size_t i = 0; i < length; i++) { // Crea el payload, concatenando los datos y luego lo convierte de byte a string
    incoming.concat((char)payload[i]);
  }
  Serial.printf("Mensaje recibido ->  %s\n", topic);
  Serial.println("Mensaje-> " + incoming ); //imprime el mensaje que recibe como string


  if (strcmp(topic, topic_subscribe_SW) == 0){ // Comprueba si se está suscripto al topic en que se publica
    led_switch_payload = incoming.toInt(); // Convierte el payload de string a int
    Serial.printf("Mensaje Brightness -> %i\n", led_switch_payload );
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
    Serial.printf("Mensaje Brightness -> %i\n", led_brightness_payload);
    //! Serial.println(led_brightness_payload);
  }
  else if (strcmp(topic, topic_subscribe_Color) == 0){
    incoming.remove(0, 4);
    incoming.remove(incoming.length() - 1);
    char incomingArray[50];
    incoming.toCharArray(incomingArray, 25);
    char* rgb[9];
    char* ptr = NULL;
    byte index = 0;
    ptr = strtok(incomingArray, ", ");
    while (ptr != NULL) {
      rgb[index] = ptr;
      index++;
      ptr = strtok(NULL, ", ");
    }
    red_payload = atoi(rgb[0]);
    green_payload = atoi(rgb[1]);
    blue_payload = atoi(rgb[2]);
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
      Serial.printf("Fallo al conectarse al broker\nRC = %s\n", mqttClient.state());
      Serial.print("Intentando nuevamente en 5 segundos..."); // Espera 5 segundos antes de reintentar
      delay(5000);
    }
  }
}

void sendData(){  // Envía los datos al broker
    mqttClient.publish(topic_publish_TEMP, (char*)sensed_temperature); // Envia datos al topic de temperatura
    if(debug) Serial.print(*sensed_temperature);
    mqttClient.publish(topic_publish_HUM, (char*)sensed_humidity); // Envia datos al topic de humedad
    if(debug) Serial.print(*sensed_humidity);
  }

void am2320_sensor() {
      switch(am2320.Read()) {   // Lee el sensor
        case 1:
          if(debug) Serial.println("Sensor offline");
          break;
        case 0:
          temperature = am2320.cTemp;   // Guarda la temperatura.
          humidity = am2320.Humidity;   // y la humedad
          dtostrf(temperature, 6, 2, sensed_temperature);  // Convierte el float a string, y lo guarda en un buffer para posteriormente mandarlo por MQTT
          dtostrf(humidity, 6, 2, sensed_humidity);
          if(debug){
            Serial.printf("\tHumidity = %f%\n", am2320.Humidity);
            Serial.printf("\tTemperature = %f°C\n", am2320.cTemp);
          }
          break;
    }
}

void controlRGB(){
  if(led_brightness_payload >= 1){        // Si el switch esta activado, envia los datos del pwm al led
    ledcWrite(ledChannel_R, red_payload);
    ledcWrite(ledChannel_G, green_payload);
    ledcWrite(ledChannel_B, blue_payload);
  }
  else if(led_brightness_payload == 0){    // Apaga led
    ledcWrite(ledChannel_R, 0);
    ledcWrite(ledChannel_G, 0);
    ledcWrite(ledChannel_B, 0);    
  }

}
