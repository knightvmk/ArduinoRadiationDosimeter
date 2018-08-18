
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
#define SENSITIVITY 15
#define CRITICAL_RAD_LEVEL 0.5 // в микро-Зивертах/час
#define TIME_ERROR 3.0

int service_pin = 13; // сервисный индикатор с платы
int soundPin = 11;
int switch_measure = 10;
int switch_mode = 11;
int switch_power = 12;
int switch_precision = 12;
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
double current_dose = 0; // счётчик накопленной радиации за время работы прибора

unsigned int select_rad = 1; // 0 - Зиверты, 1 - Ренгены, 2 - Рад, 3 - Бэр, 4 - Кюри, 5 - CPM для дебуга
unsigned int select_power = 0; // 0 - микро, 1 - милли, 2 - без, / в час
unsigned int select_mode = 0; // 0 - радиометр, 1 - дозиметр
//unsigned int select_precision = 0; // 0 - за 5 сек, 1 - за 10 сек

unsigned long total_rad = 0;
unsigned long time_buzzer = 60000; // пищалка при прошествии 1 минуты

unsigned int TIME_FOR_PAUSE = 5000; // default.



LiquidCrystal_I2C lcd(0x27, 16, 2); // Устанавливаем дисплей

void ShowRadiation(); // режим покателя радиации в час
void ShowDosimeter(); // режим показателя накопленной радиации за время работы прибора
void Counter(); // счётчик в режиме работы прерываний
void SwitchMeasure(); // изменение меры измерения
void SwitchMode(); // изменение режима работы радиометр-дозиметр
void SwitchPower(); // изменение единицы меры измерения (микро-мили-"без приставки")
void SwitchPricision(); // сменить время измерения радиометра с 5 на 10 сек.
void AttentionBuzzer(); // функция сигнализации при достижении порога радиации в 0.5 мкЗв/ч или 50 мкР/ч
void RefreshPrintData(); // принудительное обновление данных на экране при изменении режима работы прибора


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
  if (digitalRead(switch_mode) == LOW) SwitchMode();
  if (digitalRead(switch_measure) == LOW) SwitchMeasure();
  if (digitalRead(switch_power) == LOW) SwitchPower();
  if (digitalRead(switch_precision) == LOW) SwitchPricision();

  // работа режимов
  if (select_mode == 0) ShowRadiation();
  else  ShowDosimeter();
  if (rad_value >= CRITICAL_RAD_LEVEL) AttentionBuzzer();
}

void ShowRadiation()
{
  if (count == 0) return; // нефиг тут делать, если не было ни одного щелчка
  if ((millis() * TIME_ERROR - time_previous_measure) > TIME_FOR_PAUSE)
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
    if (TIME_FOR_PAUSE == 5000) count_per_minute = ((double)common_counter / (double)checker) * 12.0; // у нас есть показатель/час, получаем показатель/час для 2 секунд периода измерений
    else count_per_minute = ((double)common_counter / (double)checker) * 6.0;
    rad_value = count_per_minute * CONVERT_PULSE; // по-умолчанию в микро-Зивертах
    time_previous_measure = millis() * TIME_ERROR;


    switch (select_rad)
    {
      case 0: // начало Зиверт

        lcd.clear();
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
  if (millis()*TIME_ERROR - time_previous_measure_dose < 2000) return;
  // необходимо получить общую дозу за всё время работы прибора
  // режим один - показать total за время работы прибора

  unsigned long timing = millis() * TIME_ERROR; // время сейчас в мс
  time_previous_measure_dose = timing;
  if (timing - time_buzzer <= 60000)
  {
    tone(soundPin, 7000, 50);
    time_buzzer *= 2;
  }
  double rad_value = (total_rad * CONVERT_PULSE); // в микро-Зивертах/час

  double minutes = (double)timing / (60000.0 ); // время в мин

  current_dose = (rad_value / 60.0) ; // доза микро-Зиверт за количество прошедших минут


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

  lcd.setCursor(0, 1);
  lcd.print(minutes, 2);
  lcd.setCursor(8, 1);
  lcd.print("min");
  count = 0;
}

void Counter()
{
  tone(soundPin, 150, 7);
  ++count;
  ++total_rad;
  //total_rad += count; // общий выровненный показатель радиации в час
}

void SwitchMeasure()
{
  if (select_rad == 1) select_rad = 0;
  else select_rad = 1;
  RefreshPrintData();
}

void SwitchPower()
{
  if (select_power == 2) select_power = 0;
  else ++select_power;
  RefreshPrintData();
}

void SwitchMode()
{
  if (select_mode == 1) select_mode = 0;
  else select_mode = 1;
  RefreshPrintData();
}

void SwitchPricision()
{
  if (TIME_FOR_PAUSE == 5000) TIME_FOR_PAUSE = 10000;
  else TIME_FOR_PAUSE = 5000;
  if (select_mode == 0) lcd.clear();
  count = 0;
}

void AttentionBuzzer()
{
  tone(soundPin, 7000, 70);
  delay(65);
  noTone(soundPin);
  tone(soundPin, 9000, 140);
  delay(130);
  noTone(soundPin);
}

void RefreshPrintData() // экстренное принудительное обновление данных на дисплее
{
  lcd.clear();
  if (select_mode == 0)
  {
    lcd.setCursor(0, 1);
    if (select_rad == 0)
    {
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
    }
    else
    {
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
    }
  }
  else
  {
    switch (select_rad)
    {
      case 0: // Зиверты
        switch (select_power)
        {
          case 0:
            lcd.setCursor(0, 0);
            lcd.print(current_dose, 4);
            lcd.setCursor(8, 0);
            lcd.print("uSv");
            break;
          case 1:
            lcd.setCursor(0, 0);
            lcd.print(current_dose * 0.001, 4);
            lcd.setCursor(8, 0);
            lcd.print("mSv");
            break;
          case 2:
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
            lcd.setCursor(0, 0);
            lcd.print(current_dose * 100, 4);
            lcd.setCursor(8, 0);
            lcd.print("uRn");
            break;
          case 1:
            lcd.setCursor(0, 0);
            lcd.print(current_dose * 0.1, 4);
            lcd.setCursor(8, 0);
            lcd.print("mRn");
            break;
          case 2:
            lcd.setCursor(0, 0);
            lcd.print(current_dose * 0.0001, 5);
            lcd.setCursor(9, 0);
            lcd.print("Rn");
            break;
        }
        break;
    }
  }
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



