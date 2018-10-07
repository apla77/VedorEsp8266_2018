#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>


const char* SSID = "minha_rede"; //Seu SSID da Rede WIFI
const char* PASSWORD = "1234"; // A Senha da Rede WIFI
const char* MQTT_SERVER = "projetos-pf.ifrn.edu.br"; //Broker do Mosquitto.org


#define MQTT_CLIENT_ID          "servidor_vedor"
#define SUBSCRIBE_TOPIC         "home/test"   
 
const int CHECA_SENSOR = 10; 
uint32_t sleep_time_s = 30 * 1000000;


WiFiClient CLIENT;
PubSubClient MQTT(CLIENT);


//CONFIGURAÇÃO DA INTERFACE DE REDE
void setupWIFI() {
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Conectando na rede: ");
  Serial.println(SSID);
  while (WiFi.status() != WL_CONNECTED) {
   Serial.print(".");
   delay(500);
  }
}

void setup(void) {
 
  Serial.begin(115200);
  
  setupWIFI(); 
  MQTT.setServer(MQTT_SERVER, 1883);

   if (!MQTT.connected()) {
    Serial.println("Não estar conectado, Vai reconectaar agora!!!");
    reconectar();
   }

   Serial.println("Enviando a mensagem!!!");
   enviarNotificacao();

   delay(1000);
   ESP.deepSleep(sleep_time_s); // hiberna por um tempo determinado.
   delay(1000);
}

void reconectar() {
  while (!MQTT.connected()) {
    Serial.println("Conectando ao Broker MQTT.");
    if (MQTT.connect("ESP8266")) {
      Serial.println("Conectado com Sucesso ao Broker");
    } else {
      Serial.print("Falha ao Conectador, rc=");
      Serial.print(MQTT.state());
      Serial.println(" tentando se reconectar...");
      delay(3000);
    }
  }
}

// **************************************************************************

String sensor() {
  int sensorPin = A0;
  int sensorValue = 0;
  int cont = 0;
  String mensagem;

  for(int i = 0; i < CHECA_SENSOR; i++){
    sensorValue = analogRead(sensorPin);

    Serial.println(sensorValue);
    
    if(sensorValue > 5){ // teste usando valor do sensor igual a zero
      cont++;            
    } 
    delay(2000); 
  }
  if(cont == CHECA_SENSOR) {
    mensagem = "ABASTECIDO\"}";
  }
  else {
    mensagem = "DESABASTECIDO\"}";
  }
     return mensagem;
}

// *************************** enviarNotificacao *************************
void enviarNotificacao(){
  
  String sBuf = "{\"mac\":\"08:00:28:59:34:4D\",\"valor\":" + sensor();
 
  char buf[sBuf.length()+2];
  sBuf.toCharArray(buf,sBuf.length()+2);
  Serial.println(strlen(buf));
  Serial.println(buf);
  MQTT.publish("vedor/abast", buf);

  CLIENT.stop();  
}

// ************************************************************************
void loop(void) { }

// Fim programa
