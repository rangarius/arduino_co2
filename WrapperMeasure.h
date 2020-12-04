#ifndef WrapperMeasure_h
#define WrapperMeasure_h

#include "BaseHeader.h"
#include <PubSubClient.h>
#include <stdlib.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoJson.h>

class WrapperMeasure {
  public:
    WrapperMeasure(): dht(0, DHT22){};
    WrapperMeasure(int rPort, int bPort,int gPort,int dataPort);
    void
      begin(),
      setColor(float co2);
    float
      getCO2(void),
      getTemp(void),
      getHum(void),
      getRZERO(void),
      getResistance(void);
    
    
  private:
    int _rPort;
    int _gPort;
    int _bPort;
    int _dataPort;
    float _ppm;
    float LEDgreen;
    float LEDred;
    float LEDblue;
    DHT dht;

};

#endif
