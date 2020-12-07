/*------------------------------------------------*/
/*Configuration type*/

#define CONFIG_ENABLE_WEBCONFIG 1

//Replaces all values in the webconfig with the ConfigStatic.h values when CONFIG_ENABLE_WEBCONFIG is enabled
//#define CONFIG_OVERWRITE_WEBCONFIG 1

 
/*------------------------------------------------*/
/*Logging level*/

#define LOGLEVEL LOG_LEVEL_INFOS
//#define LOGLEVEL LOG_LEVEL_DEBUG
//#define LOGLEVEL LOG_LEVEL_VERBOSE

/*------------------------------------------------*/
/*Main static configuration*/
//This cannot be changed via web inteface

#define CONFIG_LED_SPI_CHIPSET WS2801 //Comment out for clockless
//#define CONFIG_LED_CLOCKLESS_CHIPSET WS2812B //Comment in for clockless
//#define FASTLED_ALLOW_INTERRUPTS 0 //Comment in if clockless stripe (ex. WS2812B) is flickering
//#define CONFIG_LED_PWM 1 //Comment in if PWM Stripe

#define CONFIG_PORTS_R_PORT 12
#define CONFIG_PORTS_G_PORT 13
#define CONFIG_PORTS_B_PORT 14



#define CONFIG_PORTS_DATA_PORT 0

#define CONFIG_MQTT_MQTT_SERVER "192.168.43.21"
#define CONFIG_MQTT_MQTT_CLIENT "B6-00-000"
#define CONFIG_MQTT_MQTT_PORT 1883
#define CONFIG_MQTT_TOPIC "/nodes/B6/00/001"

#define CONFIG_WIFI_SSID

#define CONFIG_CO2_RZERO 0
#define CONFIG_CO2_RESISTOR 10
/*------------------------------------------------*/
/*Main configuration*/
//You can leave it empty and override it via the web interface on port 80
#define CONFIG_WIFI_SSID "ssid"
#define CONFIG_WIFI_PASSWORD "password"

//#define CONFIG_WIFI_STATIC_IP 1
//const byte CONFIG_WIFI_IP[] = {192, 168, 0, 100};
//const byte CONFIG_WIFI_SUBNET[] = {255, 255, 255, 0};
//const byte CONFIG_WIFI_DNS[] = {192, 168, 0, 1};

#define CONFIG_WIFI_HOSTNAME "ESP8266-CO2-SPOT"
