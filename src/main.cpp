#include "Arduino.h"
#include "IPAddress.h"
#include "painlessMesh.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"

// Класс работы с файлом канфигурации
#include "config.h"

#define HOSTNAME "HTTP_BRIDGE"

#define FILE_CONFIG "/config.json"

int LED = 22;

Scheduler deviceScheduler; // Управление задачами

painlessMesh mesh; // Объект Mesh сети
AsyncWebServer server(80); // Объект Web сервера
IPAddress myIP(0,0,0,0); // IP адрес устройства
IPAddress myAPIP(0,0,0,0); // IP адрес точки доступа

// Prototype
void receivedCallback( const uint32_t &from, const String &msg );
IPAddress getlocalIP();


// Задача по миганию светодиодом
Task BuildLedBlink(1000, TASK_FOREVER, []() {
	digitalWrite(LED, !digitalRead(LED));
});

void setup() {
	// Инициализация серийного порта
	Serial.begin(115200);

	// Выходной Пин светодиода
	pinMode(LED, OUTPUT);

	// Настройки
	//
	// Объект настроек устройства
	Config gate_config(FILE_CONFIG);
	// Данные с настройками устройства
	config_data_t config_data;
	// Вывод настроек в серийный порт
	gate_config.printConfig();
	// Загрузка настроек
	gate_config.loadConfig(config_data);

	// Mesh сеть
	//
	// Установка вывода сообщений в серийный порт
	mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION | MESH_STATUS | APPLICATION );
	// Инициализация MESH сети на канале 6. Режим точки доступа WiFi при старте устройства.
	mesh.init( config_data.mesh_prefix, config_data.mesh_password, config_data.mesh_port, WIFI_AP_STA, 6 );
	// Установка процедуры обратного вызова для сообщений, адресованных этому узлу
	mesh.onReceive(&receivedCallback);
	// Подключение к WiFi сети
	mesh.stationManual(config_data.wifi_ssid, config_data.wifi_password);
	// Установка имени узла
	mesh.setHostname(HOSTNAME);
	// Устанавливаеи узел как корневой
	mesh.setRoot(true);
	// Сообщать всем узлам что этот узел является корневым
	mesh.setContainsRoot(true);

	// Wifi сеть
	//
	// Вывод IP ареса точки достпа
	myAPIP = IPAddress(mesh.getAPIP());
	Serial.println("My AP IP is " + myAPIP.toString());

	// Web сервер
	//
	// Асинхронная обработка запросов
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
		const size_t capacity = 8 * 64;
		StaticJsonDocument<capacity> cfg;
		JsonObject root  = cfg.to<JsonObject>();
		root["entity_id"] = "test entity";
		char json_string[256];
		serializeJson(root, json_string);
        request->send(200, "text/plain", json_string);
    });
	// Статическая выдача страниц
	server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
	// Старт Web сервера
	server.begin();

	// Добавление задач в планировщик
	//
	// Светодиод
	deviceScheduler.addTask(BuildLedBlink);
	BuildLedBlink.enable();
	
}
 
void loop() {
	// Выполнение задач из планировщика объекта painlessMesh mesh
	mesh.update();

	// Проверка текущего IP устройства
	if(myIP != IPAddress(mesh.getStationIP())){
		myIP = IPAddress(mesh.getStationIP());
		Serial.println("My IP is " + myIP.toString());
	}

	// Выполнение задач из планировщика устройства
	deviceScheduler.execute();
}

// Процедура обратного вызова для сообщений, адресованных этому узлу
void receivedCallback( const uint32_t &from, const String &msg ) {
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
}
