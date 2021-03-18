#include "WrapperMeasure.h"

// CO2-Wert in der Atmosphäre, als Basis für die Kalibrierung
#define ATMOCO2 397.13


WrapperMeasure::WrapperMeasure(int rPort,int gPort,int bPort,int dataPort): dht(dataPort, DHT22){  
  _rPort = rPort;
  _gPort = gPort;
  _bPort = bPort;
  _dataPort = dataPort;
  
  dht.begin();
}


void WrapperMeasure::setColor(float co2) {
   
    float ppm = co2;


    LEDblue = 0;
    LEDred = (1024 * ((_ppm - 300) / 500)) / 3;
    LEDgreen = 1024 - ((1024 * ((_ppm - 300) / 500)) / 3);
    
    if (LEDred > 1024) LEDred = 1024;
    if (LEDred < 0) LEDred = 0; 
    if (LEDgreen > 1024)LEDgreen = 1024;
    if (LEDgreen < 0) LEDgreen = 0;
    analogWrite(_gPort, LEDgreen);
    analogWrite(_rPort, LEDred);
    analogWrite(_bPort, LEDblue);


  }



float WrapperMeasure::getTemp() {
      //TEMPERATUR & LUFTFEUCHTE AUSLESEN

    return (dht.readTemperature() + _tempOffset);
  }

float WrapperMeasure::getHum() {
      return (dht.readHumidity() + _humOffset);
  }


void WrapperMeasure::setOffsets(float temp_offset, float hum_offset) {
  Log.debug("Setting Offsets");
  Log.debug("Temp: %f",temp_offset);
    Log.debug("Hum: %f",hum_offset);
  _humOffset = hum_offset;
  _tempOffset = temp_offset;
  }
