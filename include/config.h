// Библиотека для работы с JSON
#include "ArduinoJson.h"
// Библиотека для работы с файловой системой
#include "FS.h"
// файловая система памяти SPI Flash
#include "SPIFFS.h"

struct config_data_t
{
	char ssid[32];
	char password[64];

    char prefix[32];
	char password[32];
    uint16_t port;
};

class Config {
public:
    Config(char *file);
	void printConfig();
    bool loadConfig();
    String param(String key);

protected:
	char _file_name[64];
    config_data_t _data;
};