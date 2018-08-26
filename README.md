# ArduinoRadiationDosimeter
Repository for Geiger counter and Radiation dosimeter for Arduino plate.

## Working features:
* **Radiometer mode**
+ Roentgen,
+ Sievert,
+ + Alarm by high level of radiation _over 0.5 uSv/h or 50 uRn/h_.
* **Dosimeter mode**
+ Roentgen per _"millis()"_,
+ Sievert per _"millis()"_,
+ + Alarm every minute.

Update screen data at 5 secs or 15 secs in "Precision  mode"

## FAQ

**Connection PINs**

**Arduino:**
* Digital pins:
1. **3** - Input for interrupts from Geiger plate,
2. **8** - Sound buzzer pin,
3. **12** - pin for button change power of measure,
4. **7** - pin for button change measure,
5. **4** - pin for button change mode (Radiomenter / Dosimeter).
7. **13** - pin for switch precision of measure (5 sec / 15 sec update screen data)

* Analog pins:
4. **A4** - **SDA** - pins for i2c display **LCD1602 (LiquidCrystal_I2C)**,
5. **A5** - **SCL** - ==//==.

**Geiger plate**
1. **GND**
2. **5V**
3. **VIN** - connect to diginal pin "2" from Arduiono plate




---
Last update the Readme at 26.08.2018

_tags: Arduino, Uno, Nano Pro Mini, Pro, Mini, ATmega, C++, Geiger, Radiation, Counter, Dosimeter, CAJOE, RadiationD, v1.1, RadiationD-v1.1_
