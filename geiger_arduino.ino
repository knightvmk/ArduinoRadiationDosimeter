
/*
   This source code for Geiger plate RadiationD-v1.1(CAJOE)
   and chinise Ali-Arduino Uno with 12 Mhz quartz.

   Free licence, use is it for oneself

   Author: Gorb Nikolai
   Date:   Summer 2018
   Git:    https://github.com/knightvmk/ArduinoRadiationDosimeter
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WARNING
// For chinise Ali-Arduino 1000 mills == 333 mills !!! Factor = 3.0 / TIME_ERROR

#define CONVERT_PULSE 0.00812037037037 // Конвертация (Щелчков в Минуту) в МикроЗиверты/час
#define SENSITIVITY 30
#define TIME_ERROR 3.0

int service_pin = 13; // сервисный индикатор с платы
int soundPin = 11;
int switch_measure = A1;
int switch_mode = A2;
int switch_power = A0;
int geiger_input = 2; // вход с платы Счетчика Гейгера
unsigned long count = 0; // счётчик
double count_per_minute = 0;
unsigned long time_previous_measure = 0;
unsigned long time_previous_measure_dose = 0;
unsigned long time = 0;
double rad_value = 0.0;
bool is_initialized = false;
unsigned int checker = 0; // для усренения измерений
unsigned long common_counter = 0; // для усренения измерений

unsigned int select_rad = 1; // 0 - Зиверты, 1 - Ренгены, 2 - Рад, 3 - Бэр, 4 - Кюри, 5 - CPM для дебуга
unsigned int select_power = 0; // 0 - микро, 1 - милли, 2 - без, / в час
unsigned int select_mode = 0; // 0 - радиометр, 1 - дозиметр

unsigned long total_rad = 0;
int diviser = 0; // делитель-усреднитель для дозиметрии


LiquidCrystal_I2C lcd(0x27, 16, 2); // Устанавливаем дисплей

void SwitchMeasure();
void SwitchMode();
void SwitchPower();


void setup()
{
  pinMode(soundPin, OUTPUT); //объявляем пин 3 как выход.
  pinMode(switch_measure, INPUT_PULLUP);
  pinMode(switch_mode, INPUT_PULLUP); 
  pinMode(switch_power, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  interrupts();
  attachInterrupt(digitalPinToInterrupt(geiger_input), Counter, FALLING); // Прерывания для считывания пульсов трубки Гейгера-Мюллера

  //Serial.begin(9600); // Вывод в ПК по COM интерфейсу // Debug
  lcd.init(); // инициализация LCD
  lcd.backlight(); // включаем подсветку
  lcd.clear(); // очистка дисплея

  lcd.setCursor(1, 0);
  lcd.print("S.T.A.L.K.E.R.");
  lcd.setCursor(6, 1);
  lcd.print("KIT");
}

void loop()
{
  // обработка нажатий кнопок
  if (analogRead(switch_mode) < 1000) SwitchMode();
  if (analogRead(switch_measure) < 1000) SwitchMeasure();
  if (analogRead(switch_power) < 1000) SwitchPower();
  
  // работа режимов
  if (select_mode == 0)
    Show_Radiation();
  else
  {
    ShowDosimeter();
  }
}

void Show_Radiation()
{
  if (count == 0) return; // нефиг тут делать, если не было ни одного щелчка
  if ((millis() * TIME_ERROR - time_previous_measure) > 5000)
  {
    if (checker > SENSITIVITY)
    {
      checker = 0;
      common_counter = 0;
    }
    
    common_counter += count; // общий выровненный показатель радиации в час
    total_rad += count; // для дозиметрии
    count = 0; // обнуляемся для нового периода

    ++checker; // делитель усредненного значения радиации в час

    count_per_minute = ((double)common_counter / (double)checker) * 12.0; // у нас есть показатель/час, получаем показатель/час для 2 секунд периода измерений
    rad_value = count_per_minute * CONVERT_PULSE; // по-умолчанию в микро-Зивертах
    time_previous_measure = millis() * TIME_ERROR;

jumper_radiometer:
    switch (select_rad)
    {
      case 0: // начало Зиверт

        lcd.clear();
        lcd.setCursor(0, 0);
        //lcd.print("Radiometer");

        lcd.setCursor(0, 1);
        switch (select_power)
        {
          case 0:
          
            lcd.print(rad_value, 4);
            lcd.setCursor(11, 1);
            lcd.print("uSv/h");
            break;

          case 1:

            lcd.print(rad_value * 0.001, 5);
            lcd.setCursor(11, 1);
            lcd.print("mSv/h");
            break;

          case 2:

            lcd.print(rad_value * 0.000001, 7);
            lcd.setCursor(12, 1);
            lcd.print("Sv/h");
            break;
        }
        break;
      // конец Зиверт
      case 1: // начало Рентген

        lcd.clear();
        lcd.setCursor(0, 0);
        //lcd.print("Radiometer");

        lcd.setCursor(0, 1);
        switch (select_power)
        {
          case 0:

            lcd.print(rad_value * 100, 4);
            lcd.setCursor(11, 1);
            lcd.print("uRn/h");
            break;

          case 1:

            lcd.print(rad_value * 0.1, 4);
            lcd.setCursor(11, 1);
            lcd.print("mRn/h");
            break;

          case 2:

            lcd.print(rad_value * 0.0001, 5);
            lcd.setCursor(12, 1);
            lcd.print("Rn/h");
            break;
        }
        break;
        // конец Рентген

    }
  }
}

void ShowDosimeter()
{
  if(millis()*TIME_ERROR - time_previous_measure_dose < 2000) return;
  // необходимо получить общую дозу за всё время работы прибора
  // режим один - показать total за время работы прибора

  long timing = millis() * TIME_ERROR; // время сейчас в мс
  time_previous_measure_dose = timing;
  double rad_value = (total_rad * CONVERT_PULSE); // в микро-Зивертах/час

  double minutes = (double)timing / (60000.0 ); // время в мин
  //double current_dose = (rad_value) / (60000); // по правилу пропорций
  //double current_dose = (rad_value * minutes) / (60000.0);
  //double rad_min = rad_value / 60; // микро-Зиверт в мин
  double current_dose = (rad_value / 60.0) ; // доза микро-Зиверт за количество прошедших минут
  
jumper_dosimeter:
  switch (select_rad)
  {
    case 0: // Зиверты
      switch (select_power)
      {
        case 0:

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(current_dose, 4);
          lcd.setCursor(8, 0);
          lcd.print("uSv");

          break;

        case 1:
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(current_dose * 0.001, 4);
          lcd.setCursor(8, 0);
          lcd.print("mSv");

          break;

        case 2:
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(current_dose * 0.000001, 5);
          lcd.setCursor(9, 0);
          lcd.print("Sv");

          break;

      }

      break;

    case 1: // Рентгены

      switch (select_power)
      {
        case 0:

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(current_dose * 100, 4);
          lcd.setCursor(8, 0);
          lcd.print("uRn");

          break;

        case 1:
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(current_dose * 0.1, 4);
          lcd.setCursor(8, 0);
          lcd.print("mRn");

          break;

        case 2:
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(current_dose * 0.0001, 5);
          lcd.setCursor(9, 0);
          lcd.print("Rn");

          break;

      }

      break;

  }

  //lcd.setCursor(0, 1);
  //lcd.print("Time = ");
  lcd.setCursor(0, 1);
  lcd.print(minutes, 2);
  lcd.setCursor(8, 1);
  lcd.print("min");
  count = 0;
}

void Counter()
{
  ++count;
  total_rad += count; // общий выровненный показатель радиации в час
}

void SwitchMeasure()
{
  if (select_rad == 1) select_rad = 0;
  else select_rad = 1;

  //if(select_mode==0) goto jumper_radiometer;
  //else goto jumper_dosimeter;
}

void SwitchPower()
{
  if (select_power == 2) select_power = 0;
  else ++select_power;

  //if(select_mode==0) goto jumper_radiometer;
  //else goto jumper_dosimeter;
}

void SwitchMode()
{
  if (select_mode == 1) select_mode = 0;
  else select_mode = 1;
}

void BootAnimation() // useless
{
  lcd.clear();
  lcd.setCursor(1, 10);
  lcd.print("load");
  lcd.setCursor(0, 0);
  lcd.print("-");
  delay(400);
  lcd.print("--");
  delay(400);
  lcd.print("---");
  delay(400);
  lcd.print("----");
  delay(400);
  lcd.print("-----");
  delay(400);
  lcd.print("------");
  delay(400);
  lcd.print("-------");
  delay(400);
  lcd.print("--------");
  delay(400);
  lcd.print("---------");
  delay(400);
  lcd.print("----------");
  delay(400);
  lcd.print("-----------");
  delay(400);
  lcd.print("------------");
  delay(400);
  lcd.print("-------------");
  delay(400);
  lcd.print("--------------");
  delay(400);
  lcd.print("---------------");
  delay(400);
  lcd.print("----------------");
  lcd.setCursor(1, 0);
  lcd.print("-");
  delay(400);
  lcd.print("--");
  delay(400);
  lcd.print("---");
  delay(400);
  lcd.print("----");
  delay(400);
  lcd.print("-----");
  delay(400);
  lcd.print("------");
  delay(400);
  lcd.print("-------");
  delay(400);
  lcd.print("--------");
  delay(400);
  lcd.print("---------");
  delay(400);
  lcd.print("----------");
  delay(400);
  lcd.print("-----------");
  delay(400);
  lcd.print("------------");
  delay(400);
  lcd.print("-------------");
  delay(400);
  lcd.print("--------------");
  delay(400);
  lcd.print("---------------");
  delay(400);
  lcd.print("----------------");
  is_initialized = true;
}



