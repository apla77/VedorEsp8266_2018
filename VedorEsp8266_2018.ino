#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>


const char* SSID = "minha_rede"; //Seu SSID da Rede WIFI
const char* PASSWORD = "1234"; // A Senha da Rede WIFI
const char* MQTT_SERVER = "projetos-pf.ifrn.edu.br"; //Broker do Mosquitto.org


#define MQTT_CLIENT_ID          "servidor_vedor"
#define SUBSCRIBE_TOPIC         "home/test"   
 
const int CHECA_SENSOR = 10; 
uint32_t sleep_time_s = 30 * 1000000; // equivale a 30 segundos

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

//************************ Função Setap *******************************
void setup(void) {
 
  Serial.begin(115200);
  
  setupWIFI(); 
  MQTT.setServer(MQTT_SERVER, 1883);

   if (!MQTT.connected()) {
    Serial.println("Não estar conectado, Vai reconectaar agora!!!");
    reconectar();
   }

   Serial.println("Enviando a Notificaçao!!!");
   enviarNotificacao();

   delay(1000); 
   // delay antes é necessário para o bom funcionamento do deepSleep
   
   ESP.deepSleep(sleep_time_s); // hiberna por um tempo predeterminado.
   
   // delay depois é necessário para o bom funcionamento do deepSleep
   delay(1000);
}


// *************************** Função Reconectar **********************
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


// ************************** Função Sensor ****************************
String sensor() {
  int sensorPin = A0; // Pino da porta analógica
  int sensorValue = 0;
  int cont = 0;
  String mensagem;

  for(int i = 0; i < CHECA_SENSOR; i++){
    sensorValue = analogRead(sensorPin);

    Serial.println(sensorValue);
    
    if(sensorValue > 5){ // teste usando valor do sensor igual a 5
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
     return mensagem; // retorna a string com valor, ABASTECIDO ou DESABASTECIDO
}


// ************************ Função EnviarNotificacao ************************
void enviarNotificacao(){
  
  String sBuf = "{\"mac\":\"08:00:28:59:34:4D\",\"valor\":" + sensor();
 
  char buf[sBuf.length()+2]; // Cria um Array de char do tamanho de sBuf + 2
  sBuf.toCharArray(buf,sBuf.length()+2); // Copia o conteudo de sBuf para buf
  Serial.println(strlen(buf)); // Imprime o tamanho array buf
  Serial.println(buf); // Imprime o conteudo do array buf
  MQTT.publish("vedor/abast", buf); // publica a notificação

  CLIENT.stop(); // linha pode ser retirada
}


// ***************************  Função Loop ***********************************
void loop(void) { 
  // Não faz nada, toda logica tem que estar no Setup
  // devido o fato do programa hiberna
}
// Fim programa
