# Sensor CO2 Messung

##Verbaute Teile
* Wemos D1 mini V3
* MQ-135 auf Parent
* DHT 22 auf Parent
* RGB LED

* 5V Netzteil oder USB Anbindung

Ich habe festgestellt, dass die Kombi DHT22 / MQ-135 plus Licht der Versorgung über einen USB Hub Schwierigkeiten macht. Da scheint zu wenig Strom anzukommen. Beobachten lässt es sich, wenn der DHT22 Sensor zeitenweise keine Daten liefert



## Workflow zum Kalibrieren

* Sensor zusammenbauen
* Ohne Messung für ca 48 Stunden in einem Raum mit möglichst wenig CO2 einbrennen lassen (Strom auf MQ-135)
* Arduinocode einspielen
* Während das Licht blau leuchtet, in einem Raum mit offenem Fenster einlesen lassen
* Konfiguration über WLAN CO2-Config-2 anschließend möglich. Adresse 192.168.4.1
* R-Load musste bei unseren auf 1.0 eingestellt werden. Falls eine Anpassung notwengig sein sollte, noch einmal mit RZERO = 0 in Kalibration versetzen.


zusätzlich Benötigte Bibliotheken:

* ArduinoThread
* ArduinoJSON > 6.0.0
* MqttClient
* ARDUINO LOGGING <- ist als ZIP enthalten 