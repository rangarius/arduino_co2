 
#include <Thread.h>
#include <ThreadController.h>
#include "BaseHeader.h"
#include <ArduinoMqttClient.h>
#include "EnhancedThread.h"
#include "WrapperMeasure.h"
#include "LoggerInit.h"
#include "WrapperWiFi.h"
#include "WrapperWebconfig.h"
#include "WrapperOTA.h"
#include "MQ135.h"

MQ135 mq = MQ135(A0);

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

    const int numReadings = 10;
    int readIndex = 0;
    int readings[numReadings];
    float total;
float averageCO2;
int _gPort;
int _rPort;
int _bPort;
LoggerInit loggerInit;

WrapperWiFi wifi;
WrapperOTA ota;

WrapperMeasure measure;

#ifdef CONFIG_ENABLE_WEBCONFIG
  WrapperWebconfig webServer;
#endif

ThreadController threadController = ThreadController();
Thread statusThread = Thread();
EnhancedThread measureThread = EnhancedThread();
EnhancedThread connectionThread = EnhancedThread();
EnhancedThread configThread = EnhancedThread();
EnhancedThread writingThread = EnhancedThread();


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
      float temp = measure.getTemp();
    float hum = measure.getHum();
    float co2;
    if(isnan(hum)){
      co2 = mq.getPPM();
      } else {
      co2 = mq.getCorrectedPPM(temp, hum);     
        }
   
  
  
        Log.debug("temp: %f", temp);
        Log.debug("hum: %f", hum);
        


    total = total - readings[readIndex];
    readings[readIndex] = (int)co2; 
    Log.debug("Reading %i", readings[readIndex]);
    total = total + readings[readIndex]; 
    readIndex = readIndex + 1; 
    if (readIndex >= numReadings) {
      readIndex = 0;
    }
    averageCO2 = total / numReadings;
      //color = measure.setColor(co2);
  float _ppm = averageCO2;


    float LEDblue = 0;
    float LEDred = (1024 * ((_ppm - 100) / 500)) / 3;
    float LEDgreen = 1024 - ((1024 * ((_ppm - 100) / 500)) / 3);
    
    if (LEDred > 1024) LEDred = 1024;
    if (LEDred < 0) LEDred = 0; 
    if (LEDgreen > 1024)LEDgreen = 1024;
    if (LEDgreen < 0) LEDgreen = 0;
    analogWrite(_gPort, LEDgreen);
    analogWrite(_rPort, LEDred);
    analogWrite(_bPort, LEDblue);

    Log.debug("CO2: %f",averageCO2);
}

float calibrate(int rPort, int gPort, int bPort) {
    int LEDblue = 1024;
    int LEDred = 0;
    int LEDgreen = 0;

    
    analogWrite(gPort, LEDgreen);
    analogWrite(rPort, LEDred);
    analogWrite(bPort, LEDblue);
          float temp = measure.getTemp();
    float hum = measure.getHum();
    float rzero;
    float cali_ppm;
    float RMax;

    while(cali_ppm < ATMOCO2) {

          if(isnan(temp)) {
              rzero = mq.getRZero();  
              
            } else {
              rzero = mq.getCorrectedRZero(temp, hum);    
              }


          float RCurrent = rzero;
          if ((((999 * RMax) + RCurrent) / 1000) > RMax) {
            RMax = (((999 * RMax) + RCurrent) / 1000); 
          }
          Log.debug("Rload: %f", mq._RLOAD);
          Log.debug("Rmax: %f", RMax);
          mq.setRZero(RMax);
          if(isnan(temp)) {
            cali_ppm = mq.getPPM();
            }
          else {
            cali_ppm = mq.getCorrectedPPM(temp, hum);
            }
      Log.debug("ppm: %f", cali_ppm);
      Log.debug("rzero: %f", RCurrent);
      delay(20);
      }


          
         
  
      
      

    return rzero;
  }


void writeResults() {
  wifi_status = wifi.status();
  Log.debug("Wifi Status: %i", wifi_status);
if (wifi_status) {
    client.connect(mqtt_server, mqtt_port);
     Log.info("Connected: %b", client.connected());
     delay(1000);
      if(!client.connected()) {
        return;
       }
        else {
           
    float temp = measure.getTemp();
    float hum = measure.getHum();
    

              //client.loop();
                 // JSON OBJECT Erstellen und mit Messwerten laden
      StaticJsonDocument<256> doc;
        
      doc["co2"] = (averageCO2 > 0) ? averageCO2 : 0;
      doc["temp"] = (temp < 1000) ? temp : 0;
      doc["hum"] = (hum > 0) ? hum : 0;

      Log.info("Temp: %f C", temp);
       Log.info("Hum: %f %", hum);
      Log.info("CO2: %f", averageCO2);
      Log.info("Topic %s", topic); 

      client.beginMessage(topic,measureJson(doc), true, 1, false);
      serializeJson(doc, client);
      if(client.endMessage()) {
        Log.info("published");
        }
      else {
        Log.info("publish went wrong");
        }
     }

} else {
  wifi.begin();
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
  uint16_t resistor;
  float rzero;
  float temp_offset;
  float hum_offset;


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
    rzero = cfg->co2.rzero;
    resistor = cfg->co2.resistor;
    topic = cfg->mqtt.topic;
    temp_offset = cfg->offset.temp;
        hum_offset = cfg->offset.hum;
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
    rzero = CONFIG_CO2_RZERO;
    resistor = CONFIG_CO2_RESISTOR;
    temp_offset = CONFIG_OFFSET_TEMP;
    hum_offset = CONFIG_OFFSET_HUM;
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

  _gPort = gPort;
  _bPort = bPort;
  _rPort = rPort; 
  wifi = WrapperWiFi(ssid, password, ip, subnet, dns);
  measure = WrapperMeasure(rPort, gPort, bPort, dataPort);
    measure.setOffsets(temp_offset, hum_offset);
      Log.info("Temp: %f C", measure.getTemp());
      Log.info("Hum: %f %", measure.getHum());
  mq.setRLOAD(resistor);

  mq.setRZero(rzero);
  if(rzero == 0 || isnan(rzero)) {
    Log.info("Starting Calibration");
    rzero = calibrate(rPort, gPort, bPort);
    Log.info("RZERO is set to: %f", rzero);
    Config::saveRZero(rzero);
  }

}
void webserver_step() {
  
  webServer.handle();
  ota.handle();
  }


void handleEvents(void) {
  webserver_step();
  threadController.run();
}


void setup(void) {
  LoggerInit loggerInit = LoggerInit(115200);
  
  initConfig();
  ota = WrapperOTA();
  

  statusThread.onRun(statusInfo);
  statusThread.setInterval(5000);
  threadController.add(&statusThread);

  measureThread.onRun(measureStep);
  measureThread.setInterval(1500);
  threadController.add(&measureThread);

  connectionThread.onRun(writeResults);
  connectionThread.setInterval(5000);
  threadController.add(&connectionThread);
  

  writingThread.onRun(writeResults);
  writingThread.setInterval(5000);
  
  
  
  wifi.begin();
    webServer = WrapperWebconfig();
    Log.info("Starting WebServer");
    webServer.begin();
    ota.begin(Config::getConfig()->wifi.hostname);
  //configThread.onRun(webserver_step);
  //configThread.setInterval(1000);
  //threadController.add(&configThread);
  
  client.setKeepAliveInterval(20000);
  client.setId(mqtt_client);

  threadController.run();
  pinMode(LED, OUTPUT);   // LED pin as output.
  Log.info("HEAP=%i", ESP.getFreeHeap());
}

void loop(void) {
  handleEvents();
}
