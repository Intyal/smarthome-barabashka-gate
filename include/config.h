// Библиотека для работы с JSON
#include "ArduinoJson.h"
// Библиотека для работы с файловой системой
#include "FS.h"
// файловая система памяти SPI Flash
#include "SPIFFS.h"

struct config_data_t
{
	char wifi_ssid[32];
	char wifi_password[32];

    char mesh_prefix[32];
	char mesh_password[32];
    uint16_t mesh_port;
};

class Config {
public:
    Config(const char* file);
	void printConfig();
    bool loadConfig(config_data_t& data);

protected:
	char _file_name[64];
};