#ifndef ConfigStructures_h
#define ConfigStructures_h

#define CONFIG_START_ADDRESS 0
#define CONFIG_ACTIVE_VERSION 3

typedef struct {
  uint8_t a;
  uint8_t b;
  uint8_t c;
  uint8_t d;  
} ConfigIP;

typedef struct {
  char ssid[32];
  char spacer1[32];
  char password[64];
  char spacer2[64];
  
  ConfigIP ip;
  ConfigIP subnet;
  ConfigIP dns;
  char spacer3[16];
  
  char hostname[32];
  char spacer4[128];
} ConfigWifi;

typedef struct {
  char mqtt_server[64];
  char mqtt_client[32];
  uint16_t mqtt_port;
  char topic[64];

  char spacer[59];
} ConfigMqtt;

typedef struct {
  uint16_t rPort;
  uint16_t gPort;
  uint16_t bPort;
  uint16_t dataPort;
  
  char spacer[32];
} ConfigPort;

typedef struct {
  float rzero;
  uint16_t resistor;
  char spacer[32];
  } ConfigCo2;

typedef struct {
  uint8_t version;
  ConfigWifi wifi;
  ConfigMqtt mqtt;
  ConfigPort ports;
  ConfigCo2 co2;
} ConfigStruct;

#endif
