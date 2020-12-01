 
#include <Thread.h>
#include <ThreadController.h>
#include "BaseHeader.h"
#include <ArduinoMqttClient.h>
#include "EnhancedThread.h"
#include "WrapperMeasure.h"
#include "LoggerInit.h"
#include "WrapperWiFi.h"
#include "WrapperWebconfig.h"
#define MQTT_MAX_PACKET_SIZE 512
WiFiClient espClient;

MqttClient client(espClient);

char* mqtt_server;
char* mqtt_client;
char* topic;
int mqtt_port;
 int wifi_status;
#define LED LED_BUILTIN // LED in NodeMCU at pin GPIO16 (D0) or LED_BUILTIN @Lolin32.#
#define ATMOCO2 397.1 
int ledState = LOW;


LoggerInit loggerInit;

WrapperWiFi wifi;

WrapperMeasure measure;

#ifdef CONFIG_ENABLE_WEBCONFIG
  WrapperWebconfig webServer;
#endif

Mode activeMode;
boolean autoswitch;

ThreadController threadController = ThreadController();
Thread statusThread = Thread();
EnhancedThread measureThread = EnhancedThread();
EnhancedThread connectionThread = EnhancedThread();
EnhancedThread resetThread = EnhancedThread();
EnhancedThread writingThread = EnhancedThread();
EnhancedThread calibrationThread = EnhancedThread();

void statusInfo(void) {
  if (ledState == LOW) {
    ledState = HIGH;
  } else {
    ledState = LOW;
    Log.debug("HEAP=%i", ESP.getFreeHeap());
  }
  digitalWrite(LED, ledState);
}

void measureStep() {
  float co2 = measure.getCO2();
 
  if (co2 < ATMOCO2) {
    Log.verbose("CO2: %f", co2);
    } 
  else {
    Log.debug("CO2: %f", co2);
    delay(1000);
    }
}

void connectionStep() {
  while(!client.connected()) {
    if(client.connect(mqtt_server, mqtt_port)) {
      threadController.remove(&connectionThread);
      writeResults();
      threadController.add(&writingThread);
      };
    client.poll();
    
    }
  
  }

void writeResults() {
  wifi_status = wifi.status();
  Log.debug("Wifi Status: %i", wifi_status);
if (wifi_status) {
  while(!client.connected()) {
    client.connect(mqtt_server, mqtt_port);
     Log.info("Connected: %b", client.connected());
     delay(1000);
      if(!client.connected()) {
        break;
       }
        else {
            float co2 = measure.getCO2();
    float temp = measure.getTemp();
    float hum = measure.getHum();
    
    Log.debug("co2: %f", co2);
      Log.debug("temp: %f", temp);
        Log.debug("hum: %f", hum);
              //client.loop();
                 // JSON OBJECT Erstellen und mit Messwerten laden
      StaticJsonDocument<256> doc;
        
      doc["co2"] = (co2 > 0) ? co2 : 0;
      doc["temp"] = (temp < 1000) ? temp : 0;
      doc["hum"] = (hum > 0) ? hum : 0;

      Log.info("Temp: %f C", temp);
       Log.info("Hum: %f %", hum);
      Log.info("CO2: %f", co2);
      Log.info("Topic %s", topic); 

      client.beginMessage(topic,measureJson(doc), true);
      serializeJson(doc, client);
      if(client.endMessage()) {
        Log.info("published");
        }
      else {
        Log.info("publish went wrong");
        }
     }
  }
}
  }




void initConfig(void) {
  #if defined(CONFIG_OVERWRITE_WEBCONFIG) && defined(CONFIG_ENABLE_WEBCONFIG)
    Config::loadStaticConfig();
  #endif

  const char* ssid;
  const char* password;
  const byte* ip;
  const byte* subnet;
  const byte* dns;
  uint16_t rPort;
  uint16_t gPort;
  uint16_t bPort;
  uint16_t dataPort;


  #ifdef CONFIG_ENABLE_WEBCONFIG
    //TODO Fallback
    ConfigStruct* cfg = Config::getConfig();
    
    ssid = cfg->wifi.ssid;
    password = cfg->wifi.password;
    ip = Config::cfg2ip(cfg->wifi.ip);
    subnet = Config::cfg2ip(cfg->wifi.subnet);
    dns = Config::cfg2ip(cfg->wifi.dns);
    rPort = cfg->ports.rPort;
    gPort = cfg->ports.gPort;
    bPort = cfg->ports.bPort;
    dataPort = cfg->ports.dataPort;
    mqtt_server = cfg->mqtt.mqtt_server;
    mqtt_client = cfg->mqtt.mqtt_client;
    mqtt_port = cfg->mqtt.mqtt_port;
    topic = cfg->mqtt.topic;
    Log.info("CFG=%s", "EEPROM config loaded");
    Config::logConfig();
  #else
    ssid = CONFIG_WIFI_SSID;
    password = CONFIG_WIFI_PASSWORD;
    #ifdef CONFIG_WIFI_STATIC_IP
      ip = CONFIG_WIFI_IP;
      subnet = CONFIG_WIFI_SUBNET;
      dns = CONFIG_WIFI_DNS;
    #else
      const byte empty[4] = {0};
      ip = empty;
    #endif
    gPort = CONFIG_PORTS_G_PORT;
    bPort = CONFIG_PORTS_B_PORT;
    rPort = CONFIG_PORTS_R_PORT;
    dataPort = CONFIG_PORTS_DATA_PORT;
    mqtt_server = CONFIG_MQTT_MQTT_SERVER;
    mqtt_client = CONFIG_MQTT_MQTT_CLIENT;
    mqtt_port = CONFIG_MQTT_MQTT_PORT;
    topic = CONFIG_MQTT_TOPIC;

    Log.info("CFG=%s", "Static config loaded");
  #endif
    analogWrite(rPort, 1024);
    delay(1000);
    analogWrite(rPort, 0);
    analogWrite(gPort, 1024);
    delay(1000);
    analogWrite(gPort, 0);
    analogWrite(bPort, 1024);
    delay(1000);
    analogWrite(bPort, 0);

  wifi = WrapperWiFi(ssid, password, ip, subnet, dns);
  measure = WrapperMeasure(rPort, gPort, bPort, dataPort);
            Log.info("Temp: %f C", measure.getTemp());
      Log.info("Hum: %f %", measure.getHum());
      Log.info("CO2: %f", measure.getCO2());
}

void handleEvents(void) {
  #ifdef CONFIG_ENABLE_WEBCONFIG
    webServer.handle();
  #endif
  threadController.run();
}


void setup(void) {
  LoggerInit loggerInit = LoggerInit(115200);
  
  initConfig();

  

  statusThread.onRun(statusInfo);
  statusThread.setInterval(5000);
  threadController.add(&statusThread);

  measureThread.onRun(measureStep);
  //measureThread.setInterval(100);
  threadController.add(&measureThread);

  connectionThread.onRun(writeResults);
  connectionThread.setInterval(5000);
  threadController.add(&connectionThread);
  
  writingThread.onRun(writeResults);
  writingThread.setInterval(15000);
  
  
  
  wifi.begin();
  client.setKeepAliveInterval(20000);
  client.setId(mqtt_client);
  #ifdef CONFIG_ENABLE_WEBCONFIG
    webServer = WrapperWebconfig();
    webServer.begin();
    Log.info("Starting WebServer");
  #endif
    
  threadController.run();
  pinMode(LED, OUTPUT);   // LED pin as output.
  Log.info("HEAP=%i", ESP.getFreeHeap());
}

void loop(void) {
  handleEvents();
}
