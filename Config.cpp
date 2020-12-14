#include "Config.h"

ConfigStruct Config::_cfgStruct;
boolean Config::_cfgLoaded = false;

void Config::saveConfig() {
  EEPROM.begin(sizeof(ConfigStruct));
  EEPROM.put(CONFIG_START_ADDRESS, _cfgStruct);
  _cfgStruct.version = CONFIG_ACTIVE_VERSION;
  Log.info("Configuration saved at 0x%x with v%i", CONFIG_START_ADDRESS, _cfgStruct.version);
  //EEPROM.commit(); (done with end())
  EEPROM.end();
  _cfgLoaded = false;
  
}

void Config::initConfig(void) {
  if (!_cfgLoaded) {
    EEPROM.begin(sizeof(ConfigStruct));
    uint8_t version = EEPROM.read(CONFIG_START_ADDRESS);
    if (version == CONFIG_ACTIVE_VERSION) {
      EEPROM.get(CONFIG_START_ADDRESS, _cfgStruct);
      EEPROM.end();
      Log.info("Configuration read at 0x%x with v%i", CONFIG_START_ADDRESS, version);
    } else {
      //init config
      _cfgStruct.version = CONFIG_ACTIVE_VERSION;
      strncpy(_cfgStruct.wifi.hostname, "CO2-Spot", 32);
      _cfgStruct.ports.rPort = 14;
      _cfgStruct.ports.bPort = 12;
      _cfgStruct.ports.gPort = 13;
      _cfgStruct.ports.dataPort = 0;
      strncpy(_cfgStruct.mqtt.mqtt_server, "192.168.178.2", 64);
      strncpy(_cfgStruct.mqtt.mqtt_client, "CJS3-01-02.000", 32);
      strncpy(_cfgStruct.mqtt.topic, "/nodes/CJS3/01/01.022/", 64);
      _cfgStruct.mqtt.mqtt_port = 1883;
      _cfgStruct.co2.rzero = 0;
      _cfgStruct.co2.resistor = 10;
      _cfgStruct.offset.temp = 0;
      _cfgStruct.offset.hum = 0;
      EEPROM.end();
      saveConfig();
      Log.info("Configuration at 0x%x with v%i (v%i expected), new configuration created", CONFIG_START_ADDRESS, version, CONFIG_ACTIVE_VERSION);
    }
    _cfgLoaded = true;
  }
}

ConfigStruct *Config::getConfig(void) {
  initConfig();
  return &_cfgStruct;
}

void Config::loadStaticConfig(void) {
  Log.info("CFG=%s", "loadStaticConfig initConfig");
  initConfig();
  strlcpy(_cfgStruct.wifi.ssid, CONFIG_WIFI_SSID, sizeof(_cfgStruct.wifi.ssid));
  strlcpy(_cfgStruct.wifi.password, CONFIG_WIFI_PASSWORD, sizeof(_cfgStruct.wifi.password));
  strlcpy(_cfgStruct.wifi.hostname, CONFIG_WIFI_HOSTNAME, sizeof(_cfgStruct.wifi.hostname));

  #ifdef CONFIG_WIFI_STATIC_IP
    _cfgStruct.wifi.ip = ip2cfg(CONFIG_WIFI_IP);
    _cfgStruct.wifi.subnet = ip2cfg(CONFIG_WIFI_SUBNET);
    _cfgStruct.wifi.dns = ip2cfg(CONFIG_WIFI_DNS);
  #else
    _cfgStruct.wifi.ip.a = 0;
    _cfgStruct.wifi.ip.b = 0;
    _cfgStruct.wifi.ip.c = 0;
    _cfgStruct.wifi.ip.d = 0;
    
    _cfgStruct.wifi.subnet.a = 0;
    _cfgStruct.wifi.subnet.b = 0;
    _cfgStruct.wifi.subnet.c = 0;
    _cfgStruct.wifi.subnet.d = 0;
    
    _cfgStruct.wifi.dns.a = 0;
    _cfgStruct.wifi.dns.b = 0;
    _cfgStruct.wifi.dns.c = 0;
    _cfgStruct.wifi.dns.d = 0;
  #endif
  _cfgStruct.ports.rPort = CONFIG_PORTS_R_PORT;
  _cfgStruct.ports.bPort = CONFIG_PORTS_B_PORT;
  _cfgStruct.ports.gPort = CONFIG_PORTS_G_PORT;
  _cfgStruct.ports.dataPort = CONFIG_PORTS_DATA_PORT;

  strlcpy(_cfgStruct.mqtt.mqtt_server, CONFIG_MQTT_MQTT_SERVER, sizeof(_cfgStruct.mqtt.mqtt_server));
  strlcpy(_cfgStruct.mqtt.mqtt_client, CONFIG_MQTT_MQTT_CLIENT, sizeof(_cfgStruct.mqtt.mqtt_client));
  strlcpy(_cfgStruct.mqtt.topic, CONFIG_MQTT_TOPIC, sizeof(_cfgStruct.mqtt.topic));
  _cfgStruct.mqtt.mqtt_port = CONFIG_MQTT_MQTT_PORT;

  _cfgStruct.co2.rzero = CONFIG_CO2_RZERO;
  _cfgStruct.co2.resistor = CONFIG_CO2_RESISTOR;
  _cfgStruct.offset.temp = CONFIG_OFFSET_TEMP;
  _cfgStruct.offset.hum = CONFIG_OFFSET_HUM;
  saveConfig();
  Log.info("CFG=%s", "loadStaticConfig END");
}

void Config::logConfig(void) {
  initConfig();
  Log.debug("CFG Show Config");
  
  Log.debug("+WIFI+");
  Log.debug("  ssid=%s", _cfgStruct.wifi.ssid);
  Log.debug("  password=%s", _cfgStruct.wifi.password);
  Log.debug("  ip=%i.%i.%i.%i", _cfgStruct.wifi.ip.a, _cfgStruct.wifi.ip.b, _cfgStruct.wifi.ip.c, _cfgStruct.wifi.ip.d);
  Log.debug("  subnet=%i.%i.%i.%i", _cfgStruct.wifi.subnet.a, _cfgStruct.wifi.subnet.b, _cfgStruct.wifi.subnet.c, _cfgStruct.wifi.subnet.d);
  Log.debug("  dns=%i.%i.%i.%i", _cfgStruct.wifi.dns.a, _cfgStruct.wifi.dns.b, _cfgStruct.wifi.dns.c, _cfgStruct.wifi.dns.d);
  Log.debug("  hostname=%s", _cfgStruct.wifi.hostname);

  Log.debug("+PORTS+");
  Log.debug("  rPort=%i", _cfgStruct.ports.rPort);
  Log.debug("  gPort=%i", _cfgStruct.ports.gPort);
  Log.debug("  bPort=%i", _cfgStruct.ports.bPort);
  Log.debug(" dataPort=%i", _cfgStruct.ports.dataPort);

  Log.debug("+MQTT+");
  Log.debug("  mqttServer=%s", _cfgStruct.mqtt.mqtt_server);
  Log.debug("  mqttClient=%s", _cfgStruct.mqtt.mqtt_client);
  Log.debug("  mqttPort=%i", _cfgStruct.mqtt.mqtt_port);
  Log.debug("  mqttTopic=%s", _cfgStruct.mqtt.topic);
  
  Log.debug("+CO2+");
  Log.debug("  RZERO=%f", _cfgStruct.co2.rzero);
  Log.debug("  RESISTOR=%i", _cfgStruct.co2.resistor);

    Log.debug("+TEMP/HUM+");
  Log.debug("  TEMP_OFFSET=%f", _cfgStruct.offset.temp);
  Log.debug("  HUM_OFFSET=%f", _cfgStruct.offset.hum);
  
}

float Config::saveRZero(float rzero) {
  _cfgStruct.co2.rzero = rzero;
  saveConfig();
  Log.info("Calibration done; Config saved");
  }

byte *Config::cfg2ip(ConfigIP ipStruct) {
  Log.verbose("CFG=cfg2ip: %i.%i.%i.%i", ipStruct.a, ipStruct.b, ipStruct.c, ipStruct.d);
  byte *ipByte = new byte[4];
  ipByte[0] = ipStruct.a;
  ipByte[1] = ipStruct.b;
  ipByte[2] = ipStruct.c;
  ipByte[3] = ipStruct.d;
  //byte ipByte[] = { ipStruct.a, ipStruct.b, ipStruct.c, ipStruct.d };
  return ipByte;
}

ConfigIP Config::ip2cfg(const byte ip[4]) {
  Log.verbose("CFG=ip2cfg: %i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
  ConfigIP cfgIp;
  cfgIp.a = ip[0];
  cfgIp.b = ip[1];
  cfgIp.c = ip[2];
  cfgIp.d = ip[3];
  return cfgIp;
}
