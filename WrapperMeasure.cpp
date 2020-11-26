#include "WrapperMeasure.h"

// Lastwiderstand des MQ-135
#define RLOAD 10.0                 

// Parameter zur CO2 Berechnung in ppm auf Basis des Sensor-Widerstands
#define PARA 116.6020682
#define PARB 2.769034857

// CO2-Wert in der Atmosphäre, als Basis für die Kalibrierung
#define ATMOCO2 397.13

// CO2-Wert in der Atmosphäre, als Basis für die Kalibrierung
#define ATMOCO2 397.1

// Parameter zur Berechnung der Durchschnittswerte
  const int numReadings = 10;
  int readings[numReadings];

WrapperMeasure::WrapperMeasure(int rPort,int gPort,int bPort,int dataPort): dht(dataPort, DHT22) {  
  _rPort = rPort;
  _gPort = gPort;
  _bPort = bPort;
  _dataPort = dataPort;
  dht.begin();
  
 // Array zur Berechnung der Durchschnittswerte
  readIndex = 0; // Index eines einzelnen Wertes
  total = 0; // Summe aller Werte
  average = 0; // Durchschnittswert
}



float WrapperMeasure::getCO2() {
    total = total - readings[readIndex];
    readings[readIndex] = analogRead(_dataPort); 
    total = total + readings[readIndex]; 
    readIndex = readIndex + 1; 
    if (readIndex >= numReadings) {
      
      readIndex = 0;
      
      Log.verbose("PPM: %f ",getPPM());
    }
    average = total / numReadings;
    Log.verbose("numReadings %i", analogRead(_dataPort));
  
    // Volatiler Sensor, daher sanfte Kalibrierung, RCurrent nur mit 0,001%
    float RCurrent = getRZero();
    if ((((9999 * RMax) + RCurrent) / 10000) > RMax) {
      RMax = (((9999 * RMax) + RCurrent) / 10000); 
    }
    _ppm = getPPM();
    if (_ppm < ATMOCO2) {
          analogWrite(_gPort, 0);
    analogWrite(_rPort, 0);
    analogWrite(_bPort, 1024);
    }
    else {
    //LED LAMPEN anhand des Messwertes ausrichten
    LEDblue = 0;
    LEDred = (1024 * ((_ppm - 500) / 500)) / 3;
    LEDgreen = 1024 - ((1024 * ((_ppm - 500) / 500)) / 3);
    if (LEDred > 1024) LEDred = 1024;
    if (LEDred < 0) LEDred = 0; 
    if (LEDgreen > 1024)LEDgreen = 1024;
    if (LEDgreen < 0) LEDgreen = 0;
    analogWrite(_gPort, LEDgreen);
    analogWrite(_rPort, LEDred);
    analogWrite(_bPort, LEDblue);
    }
    return _ppm;
  }

float WrapperMeasure::getTemp() {
      //TEMPERATUR & LUFTFEUCHTE AUSLESEN

    return dht.readTemperature();
  }

float WrapperMeasure::getHum() {
      return dht.readHumidity();
  }

float WrapperMeasure::getResistance() {
      int val = average;
      return ((1023. / (float)val) - 1.) * RLOAD;
    }
    
// CO2 Wert in ppm berechnen. Auf Basis der Vereinfachung, dass sich nur CO2 in der Luft befindet.
float WrapperMeasure::getPPM() {
      return PARA * pow((getResistance() / RMax), -PARB);
    }
    
// RZero Widerstand des Sensors (in Kiloohm) für die Kalibrierung berechnen
float WrapperMeasure::getRZero() {
      return getResistance() * pow((ATMOCO2 / PARA), (1. / PARB));
    }
