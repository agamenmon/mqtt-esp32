#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"          // Librairie des capteurs DHT

#define wifi_ssid "BKGPL"
#define wifi_password "bkgpl2016"
#define mqtt_server "m14.cloudmqtt.com"
#define mqtt_user "esp8266"  //s'il a été configuré sur Mosquitto
#define mqtt_password "123456" //idem

#define temperature_topic "temperature"  //Topic température
#define humidity_topic "humidity"        //Topic humidité

const uint16_t mqtt_port = 17885; //Port của CloudMQTT

//Buffer qui permet de décoder les messages MQTT reçus
char message_buff[100];

long lastMsg = 0;   //Horodatage du dernier message publié sur MQTT
long lastRecu = 0;
bool debug = false;  //Affiche sur la console si True

#define DHTPIN D4    // Pin sur lequel est branché le DHT

// Dé-commentez la ligne qui correspond à votre capteur 
//#define DHTTYPE DHT11       // DHT 11 
#define DHTTYPE DHT22         // DHT 22  (AM2302)

//Création des objets
DHT dht(DHTPIN, DHTTYPE);     
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);     //Facultatif pour le debug
  pinMode(D2,OUTPUT);     //Pin 2 D4
  setup_wifi();           //On se connecte au réseau wifi
  client.setServer(mqtt_server, mqtt_port);    //Configuration de la connexion au serveur MQTT
  client.setCallback(callback);  //La fonction de callback qui est executée à chaque réception de message   
  dht.begin();
}

// Hàm kết nối wifi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected ");
  Serial.print("Addresse IP : ");
  Serial.print(WiFi.localIP());
}

// Hàm reconnect thực hiện kết nối lại khi mất kết nối với MQTT Broker
void reconnect() {
  //Boucle jusqu'à obtenur une reconnexion
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("OK");
    } else {
      Serial.print("failed, rc= ");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Hàm call back để nhận dữ liệu
void callback(char* topic, byte* payload, unsigned int length) {

  int i = 0;
  if ( debug ) {
    Serial.println("Message recu =>  topic: " + String(topic));
    Serial.print(" | longueur: " + String(length,DEC));
  }
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);
  if ( debug ) {
    Serial.println("Payload: " + msgString);
  }
  
  if ( msgString == "ON" ) {
    digitalWrite(D2,HIGH);  
  } else {
    digitalWrite(D2,LOW);  
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  //Envoi d'un message par minute
  if (now - lastMsg > 1000 * 10) {
    lastMsg = now;
    //Lecture de l'humidité ambiante
    float h = dht.readHumidity();
    // Lecture de la température en Celcius
    float t = dht.readTemperature();

    //Inutile d'aller plus loin si le capteur ne renvoi rien
    /*
    if ( isnan(t) || isnan(h)) {
      Serial.println("Echec de lecture ! Verifiez votre capteur DHT");
      return;
    }
    */
  
    
    Serial.print("Temperature : ");
    Serial.print(t);
    Serial.print(" | Humidite : ");
    Serial.println(h);
    Serial.print("\n");
    
    client.publish(temperature_topic, String(t).c_str(), true);   //Publie la température sur le topic temperature_topic
    client.publish(humidity_topic, String(h).c_str(), true);      //Et l'humidité
  }
  if (now - lastRecu > 100 ) {
    lastRecu = now;
    client.subscribe("homeassistant/switch1");
  }
}


