#include "config.h"

Config::Config(char *file) {
	strcpy(_file_name, file);

}

void Config::printConfig() {
	Serial.println();

	if(!SPIFFS.begin(true)){
		Serial.println("An Error has occurred while mounting SPIFFS"); // "При монтировании SPIFFS возникла ошибка"
		return;
	}

	File file = SPIFFS.open(_file_name, "r");
	if(!file){
		Serial.println("Failed to open file for reading"); // "Не удалось открыть файл для чтения"
		return;
	}

	Serial.println("File Content:"); // "Содержимое файла:"
	while(file.available()){
		Serial.write(file.read());
	}
	file.close();
	};

// Загрузка настроек устройства из файла 
bool Config::loadConfig() {
	const size_t capacity = JSON_OBJECT_SIZE(5) + 32;
	StaticJsonDocument<capacity> cfg;

	// Открываем файл для чтения
	if(!SPIFFS.begin(true)){
		Serial.println("An Error has occurred while mounting SPIFFS"); // "При монтировании SPIFFS возникла ошибка"
		return false;
	}

	File file = SPIFFS.open(_file_name, "r");
	if(!file){
		Serial.println("Failed to open file for reading"); // "Не удалось открыть файл для чтения"
		return false;
	}

	// Разбор JSON файла
	DeserializationError error = deserializeJson(cfg, file);
	if (error) {
		Serial.println("Failed to deserializeJson file");
		file.close();
		return false;
	}

	// Копирование значений из JSON в Config
	strcpy_P(_wifi.ssid, cfg["wifi"]["ssid"] | "");
	strcpy_P(_wifi.password, cfg["wifi"]["password"] | "");

	strcpy_P(_mesh.prefix, cfg["mesh"]["prefix"] | "");
	strcpy_P(_mesh.password, cfg["mesh"]["password"] | "");
	_mesh.port = cfg["mesh"]["port"];

	//strlcpy(_wifi.ssid, doc["wifi"]["ssid"] | "barabashka_1", sizeof(_wifi.ssid));
	//strlcpy(_wifi.password, doc["wifi"]["password"] | "password", sizeof(_wifi.password));

	// Закрываем фаил
	file.close();
	return true;
};

String Config::param(String key) 
{ 
	return _cfg[key];
} 