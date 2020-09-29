#include "config.h"

Config::Config(const char* file) {
	strcpy_P(_file_name, file);

}

void Config::printConfig() {
	Serial.println();

	if(!SPIFFS.begin(true)){
		Serial.println(F("An Error has occurred while mounting SPIFFS")); // "При монтировании SPIFFS возникла ошибка"
		return;
	}

	File file = SPIFFS.open(_file_name, "r");
	if(!file){
		Serial.println(F("Failed to open file for reading")); // "Не удалось открыть файл для чтения"
		return;
	}

	Serial.println(F("File Content:")); // "Содержимое файла:"
	while(file.available()){
		Serial.write(file.read());
	}
	file.close();
}

// Загрузка настроек устройства из файла 
bool Config::loadConfig(config_data_t& data) {
	const size_t capacity = 8 * 64;
	StaticJsonDocument<capacity> cfg;

	// Открываем файл для чтения
	if(!SPIFFS.begin(true)){
		Serial.println(F("An Error has occurred while mounting SPIFFS")); // "При монтировании SPIFFS возникла ошибка"
		return false;
	}

	File file = SPIFFS.open(_file_name, "r");
	if(!file){
		Serial.println(F("Failed to open file for reading")); // "Не удалось открыть файл для чтения"
		return false;
	}

	// Разбор JSON файла
	DeserializationError error = deserializeJson(cfg, file);
	if (error) {
		Serial.print(F("deserializeJson() failed with code "));
		Serial.println(error.c_str());
		file.close();
		return false;
	}

	// Закрываем фаил
	file.close();

	// Копирование значений из JSON в Config
	strcpy_P(data.wifi_ssid, cfg["wifi"]["ssid"] | "");
	strcpy_P(data.wifi_password, cfg["wifi"]["password"] | "");

	strcpy_P(data.mesh_prefix, cfg["mesh"]["prefix"] | "barabashka_abc123");
	strcpy_P(data.mesh_password, cfg["mesh"]["password"] | "barabashka");
	data.mesh_port = cfg["mesh"]["port"].as<int>() | 2204;

	return true;
}