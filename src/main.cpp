// Библиотека Ардуино
#include "Arduino.h"

#include "IPAddress.h"
#include "painlessMesh.h"

#include <AsyncTCP.h>

#include <ESPAsyncWebServer.h>

// Класс работы с файлом канфигурации
#include "config.h"

#define HOSTNAME "HTTP_BRIDGE"

#define FILE_CONFIG "/config.json"

int LED = 22;

// Prototype
void receivedCallback( const uint32_t &from, const String &msg );
IPAddress getlocalIP();

Scheduler     userScheduler; // to control your personal tas

painlessMesh  mesh;
AsyncWebServer server(80);
IPAddress myIP(0,0,0,0);
IPAddress myAPIP(0,0,0,0);

// Зажигаем или гасим светодиод
Task BuildLedBlink(1000, TASK_FOREVER, []() {
	digitalWrite(LED, !digitalRead(LED));
});

void setup() {
	Serial.begin(115200);

	pinMode (LED, OUTPUT);

	Config gate_config(FILE_CONFIG);
	config_data_t config_data;

	gate_config.printConfig();
	gate_config.loadConfig(config_data);

	mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION | MESH_STATUS | APPLICATION );  // set before init() so that you can see startup messages

	// Channel set to 6. Make sure to use the same channel for your mesh and for you other
	// network (STATION_SSID)
	mesh.init( config_data.mesh_prefix, config_data.mesh_password, config_data.mesh_port, WIFI_AP_STA, 6 );
	mesh.onReceive(&receivedCallback);

	mesh.stationManual(config_data.wifi_ssid, config_data.wifi_password);
	mesh.setHostname(HOSTNAME);

	// Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
	mesh.setRoot(true);
	// This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
	mesh.setContainsRoot(true);

	myAPIP = IPAddress(mesh.getAPIP());
	Serial.println("My AP IP is " + myAPIP.toString());

	//Async webserver
	//server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
	//	request->send(SPIFFS, "/index.html");
	//});
	server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

	server.begin();

	userScheduler.addTask(BuildLedBlink);
	BuildLedBlink.enable();
	
}
 
void loop() {
	mesh.update();
	if(myIP != getlocalIP()){
		myIP = getlocalIP();
		Serial.println("My IP is " + myIP.toString());
	}
	userScheduler.execute();
}

void receivedCallback( const uint32_t &from, const String &msg ) {
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}