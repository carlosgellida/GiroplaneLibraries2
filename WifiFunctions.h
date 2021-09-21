#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

//If your want to connect to a Modem add your credentials bellow:
char ssid[] = "IZZI-2124" ; 
char pass[] = "ECA9404C2124" ;

//If instead you want to use the ESP32 as an acces point, It will be work with 
//following credentials, feel free to modify them
const char* ssid2     = "ESP32-Access-Point";
const char* password2 = "123456789";

WebSocketsClient webSocket;

WiFiClient client;

StaticJsonDocument<200> jsonDocTx;

bool wsconnected = false;
bool mensaje_recibido = true; 

void printWifiData() {

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
}

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
	const uint8_t* src = (const uint8_t*) mem;
	Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		Serial.printf("%02X ", *src);
		src++;
	}
	Serial.printf("\n");
}


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("[WSc] Disconnected!\n");
      wsconnected = false;
			break;
		case WStype_CONNECTED:
			Serial.printf("[WSc] Connected to url: %s\n", payload);
      wsconnected = true;
			// send message to server when Connected
			webSocket.sendTXT("Connected from ESP32"); 
      mensaje_recibido = false;
			break;
		case WStype_TEXT:
			// Serial.printf("[WSc] get text: %s\n", payload);
      mensaje_recibido = true; 
			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			//Serial.printf("[WSc] get binary length: %u\n", length);
			//hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}
}


void foundIP(void){
  // Esta función busca automáticamente el servidor disponible en el puerto
  // 5000 de varias IP's para localizar la IP del PC servidor
  IPAddress server(192, 168, 0, 2); 
  String iPdelPC = ""; 
  for(int i=0; i < 256; i++){
    bool connectedServer = false; 
    for(int j=2; j < 25; j++){
      iPdelPC = "192.168." + String(i) + "." + String(j) ; 
      server[2] = i; 
      server[3] = j; 
      if (client.connect(server, 5000)) {
        Serial.print("IP of PC: "); 
        Serial.println(server); 
        Serial.println("connected to server");
        connectedServer = true; 
        break;
      }
      Serial.println("Please wait, looking for the PC IP");
      delay(10) ; 
    }
    if(connectedServer){
      break ; 
    }
  }

  webSocket.begin(server, 8001, "/"); 
  
  webSocket.onEvent(webSocketEvent) ;
  webSocket.setReconnectInterval(50); 

  Serial.println("Websocket has begin"); 
}


void foundIP2(void){
  // Esta función busca el servidor cuando se está conectando al PC utilizando ESP32 como acces point

  IPAddress server(192, 168, 4, 2); 
  String iPdelPC = ""; 
  int i = 4; 
  while(true){
    bool connectedServer = false; 
    for(int j=2; j < 5; j++){
      iPdelPC = "192.168." + String(i) + "." + String(j) ; 
      server[2] = i; 
      server[3] = j; 
      if (client.connect(server, 5000)) {
        Serial.print("IP of PC: "); 
        Serial.println(server); 
        Serial.println("connected to server");
        connectedServer = true; 
        break;
      }
      Serial.println("Please wait, looking for the PC IP");
      delay(10) ; 
    }
    if(connectedServer){
        break; 
    }
  }

  webSocket.begin(server, 8001, "/"); 
  
  webSocket.onEvent(webSocketEvent) ;
  webSocket.setReconnectInterval(50); 

  Serial.println("Websocket has begin"); 
}


void taskWifiWithModem(void) {
  // Esta función inicia la conexión WIFI, y se conecta a un 
  // servidor Web Socket, esta función hace uso de un modem

  int status = WL_IDLE_STATUS;     // the Wifi radio's status
  //int WL_CONNECTED = 0; 

  while (status != 6) {

    Serial.print("Attempting to connect to WPA SSID: ");
    status = WiFi.begin(ssid, pass);
    delay(3000);

  }

  Serial.print("You're connected to the network");

  printWifiData();

  Serial.println(WiFi.localIP());

  delay(2000) ; 

  foundIP(); 
}


void taskWifiNoModem(void) {
  // Esta función inicia la conexión WIFI, y se conecta a un 
  // servidor Web Socket, esta función No hace uso de un modem

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid2, password2);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  delay(2000) ; 

  foundIP2(); 
}


void getAndSendInfo()
{
  //Declare a string where data is saved
  String output;

  //Resetar string output
  output = "";

  //Resetear documento JSON
  jsonDocTx.clear();

  //Almacenar info del quaternio en documento JSON
  jsonDocTx["QW"] = CurrQuaternion(0) ; 
  jsonDocTx["QX"] = CurrQuaternion(1) ; 
  jsonDocTx["QY"] = CurrQuaternion(2) ; 
  jsonDocTx["QZ"] = CurrQuaternion(3) ; 

  /*jsonDocTx["Lx"] = buttons.Lx ; // Agregar lectura de botones al archivo JSON
  jsonDocTx["Ly"] = buttons.Ly ; 
  jsonDocTx["Lw"] = buttons.Lw ; 
  jsonDocTx["Rx"] = buttons.Rx ; 
  jsonDocTx["Ry"] = buttons.Ry ; 
  jsonDocTx["Rw"] = buttons.Rw ; */

  jsonDocTx["dQW"] = DesiQuaternion(0) ; 
  jsonDocTx["dQX"] = DesiQuaternion(1) ; 
  jsonDocTx["dQY"] = DesiQuaternion(2) ; 
  jsonDocTx["dQZ"] = DesiQuaternion(3) ; 

  //Convertir documento JSON en un string
  serializeJson(jsonDocTx, output);

  //Enviar string con info del documento JSON
  webSocket.sendTXT(output);  
}