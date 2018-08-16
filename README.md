# ArduinoRadiationDosimeter
Repository for Geiger counter and Radiation dosimeter for Arduino plate

## Working features:
* **Radiometer mode**
+ Roentgen,
+ Sievert.
* **Dosimeter mode**
+ Roentgen per _"millis()"_,
+ Sievert per _"millis()"_.

Update screen data at 5 secs

## FAQ

**Connection PINs**

**Arduino:**
* Digital pins:
1. 2  Input for interrupts from Geiger plate,

* Analog pins:
1. A0  pin for button change power of measure,
2. A1  pin for button change measure,
3. A2  pin for button change mode (Radiomenter / Dosimeter),

4. A4  pins for i2c display **LCD1602 (LiquidCrystal_I2C)**,
5. A5  ==//==.

**Geiger plate**
1. GND
2. 5V
3. VIN  connect to diginal pin "2" from Arduiono plate
