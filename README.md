# ArduinoRadiationDosimeter
Repository for Geiger counter and Radiation dosimeter for Arduino plate

## Working features:
* **Radiometer mode**
+ Roentgen,
+ Sievert.
* **Dosimeter mode**
+ Roentgen per _"millis()"_,
+ Sievert per _"millis()"_.

Update screen data at 2 secs

## FAQ

**Connection PINs**

* Arduino:
*Digital pins*:
2  - Input for interrupts from Geiger plate,

*Analog pins*:
A0 - pin for button change power of measure,
A1 - pin for button change measure,
A2 - pin for button change mode (Radiomenter / Dosimeter),

A4 - pins for i2c display **LCD1602 (LiquidCrystal_I2C)**,
A5 - ==//==.

* Geiger plate
GND
5V
VIN - connect to diginal pin "2" from Arduiono plate
