#include <ESP8266WiFi.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#include <PubSubClient.h>

const char* ssid     = "default";
const char* password = "";

const char *mqtt_server = "35.173.27.127";
const int mqtt_port = 1883;
const char *mqtt_user = "letto";
const char *mqtt_pass = "loop333";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[25];

String to_send ="";
String topic_device = "abcd123456";
String topic1 = topic_device + "/led1";
String topic2 =  topic_device + "/led2";
#define PIN_RESET 0
#define LED_BLUE 1
#define PIN_REL 2
//*****************************
//*** DECLARACION FUNCIONES ***
//*****************************
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
WiFiManager wm;

void setup() {
	pinMode(PIN_REL, OUTPUT);
  digitalWrite(PIN_REL, LOW);
  pinMode(PIN_RESET, INPUT_PULLUP);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, HIGH);
	//Serial.begin(115200);
	randomSeed(micros());
	//setup_wifi();
  bool res;
    res = wm.autoConnect("ConectarRelayAP",""); // password protected ap

    if(!res) {
        //Serial.println("Failed to connect");
    } 
    else {
        //Serial.println("connected...yeey :)");
    }

  delay(500);
	client.setServer(mqtt_server, mqtt_port);
	client.setCallback(callback);
}

void loop() {
	if (!client.connected()) {
		reconnect();
	}

	client.loop();

	long now = millis();
	if (now - lastMsg > 10000){
		lastMsg = now;
		publish_status();
	}

  set_wifimanager();

}

//*****************************
//***    CONEXION WIFI      ***
//*****************************
void setup_wifi(){
	delay(10);
	// Nos conectamos a nuestra red Wifi
	//Serial.println();
	//Serial.print("Conectando a ");
	//Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		//Serial.print(".");
	}

	//Serial.println("");
	//Serial.println("Conectado a red WiFi!");
	//Serial.println("Dirección IP: ");
	//Serial.println(WiFi.localIP());
}



void callback(char* topic, byte* payload, unsigned int length){
	String incoming = "";
	//Serial.print("Mensaje recibido desde -> ");
	//Serial.print(topic);
	//Serial.println("");
	for (int i = 0; i < length; i++) {
		incoming += (char)payload[i];
	}
	incoming.trim();
	//Serial.println("Mensaje -> " + incoming);

	if ( incoming == "on") {
		digitalWrite(PIN_REL, HIGH);
     

	} if ( incoming == "off")  {
		digitalWrite(PIN_REL, LOW);
    
	}

  publish_status();
}

void reconnect() {

	while (!client.connected()) {
		//Serial.print("Intentando conexión Mqtt...");
		// Creamos un cliente ID
		String clientId = "esp32_";
		clientId += String(random(0xffff), HEX);
		// Intentamos conectar
		if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
			//Serial.println("Conectado!");
        digitalWrite(LED_BLUE, LOW);


			// Nos suscribimos
			client.subscribe(topic1.c_str());
			client.subscribe(topic2.c_str());
		} else {
			//Serial.print("falló :( con error -> ");
			//Serial.print(client.state());
			//Serial.println(" Intentamos de nuevo en 5 segundos");
      digitalWrite(LED_BLUE, HIGH);
			delay(5000);
		}
	}
}

void set_wifimanager(){
    if (digitalRead(PIN_RESET) == 0) {
       wm.resetSettings();
       delay(1000);
       ESP.restart();

    }
}

void publish_status(){
    if (digitalRead(PIN_REL)==LOW ){
        to_send = "apagado";
        }
    else {
        to_send= "encendido" ;
    }    

		to_send.toCharArray(msg, 25);
		//Serial.print("Publicamos mensaje -> ");
		//Serial.println(msg);
		client.publish("values", msg);

}

