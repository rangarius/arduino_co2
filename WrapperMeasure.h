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
    WrapperMeasure(): dht(0, DHT22) {};
    WrapperMeasure(int rPort, int bPort,int gPort,int dataPort);
    void
      begin();
    float
      getCO2(void),
      getTemp(void),
      getHum(void);
      
  private:
    int _rPort;
    int _gPort;
    int _bPort;
    int _dataPort;
    float _ppm;
    float getResistance(void);
    float getPPM(void);
    float getRZero(void);
    float RCurrent;
    float RMax;
    float LEDgreen;
    float LEDred;
    float LEDblue;
    DHT dht;
    int readIndex;
    float total; // Summe aller Werte
    float average; // Durchschnittswert
};

#endif
