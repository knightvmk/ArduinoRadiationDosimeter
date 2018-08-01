#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define CONVERT_PULSE 0.00812037037037 // Конвертация (Щелчков в Минуту) в МикроЗиверты/час
#define TIME_PERIOD 10000
#define MINUTE_PERIOD 60000

int service_pin = 13; // сервисный индикатор с платы
int geiger_input = 2; // вход с платы Счетчика Гейгера
long count = 0; // счётчик
long count_per_minute = 0;
long time_previous = 0;
long time_previous_measure = 0;
long time = 0;
long count_previous = 0;
double rad_value = 0.0;
bool is_initialized = false;

int select_rad = 0; // 0 - Зиверты, 1 - Ренгены, 2 - Рад, 3 - Бэр, 4 - Кюри, 5 - CPM для дебуга
int select_power = 0; // 0 - микро, 1 - милли, 2 - без, / в час


LiquidCrystal_I2C lcd(0x27, 16, 2); // Устанавливаем дисплей


void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  interrupts();
  attachInterrupt(digitalPinToInterrupt(2), Counter, FALLING); // Прерывания для считывания пульсов трубки Гейгера-Мюллера

  Serial.begin(9600); // Вывод в ПК по COM интерфейсу // Debug

  lcd.init(); // инициализация LCD
  lcd.backlight(); // включаем подсветку
  lcd.clear(); // очистка дисплея

  lcd.setCursor(1, 0);
  lcd.print("S.T.A.L.K.E.R.");
  lcd.setCursor(6, 1);
  lcd.print("KIT");
  select_rad = 1;
  select_power = 0;
}

void loop()
{
  //if(!is_initialized) BootAnimation();
  Show_Radiation();
}

void Show_Radiation()
{
  if (millis() - time_previous_measure > TIME_PERIOD)
  {
    //digitalWrite(LED_BUILTIN, HIGH);
    count_per_minute = 6 * count;
    count = 0;

    rad_value = count_per_minute * CONVERT_PULSE; // по-умолчанию в микро-Зивертах
    time_previous_measure = millis();
    
    switch (select_rad)
    {
      case 0: // начало Зиверт

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("RADmeter mode ");

        lcd.setCursor(0, 1);
        switch (select_power)
        {
          case 0:

            lcd.print(rad_value, 4);
            lcd.setCursor(6, 1);
            lcd.print(" uSv/h");
            break;

          case 1:

            lcd.print(rad_value * 0.001, 5);
            lcd.setCursor(7, 1);
            lcd.print(" mSv/h");
            break;

          case 2:

            lcd.print(rad_value * 0.000001, 7);
            lcd.setCursor(9, 1);
            lcd.print(" Sv/h");
            break;
        }
        break;
      // конец Зиверт
      case 1: // начало Рентген

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("RADmeter mode ");

        lcd.setCursor(0, 1);
        switch (select_power)
        {
          case 0:

            lcd.print(rad_value * 100, 4);
            lcd.setCursor(6, 1);
            lcd.print(" uRn/h");
            break;

          case 1:

            lcd.print(rad_value * 0.1, 4);
            lcd.setCursor(6, 1);
            lcd.print(" mRn/h");
            break;

          case 2:

            lcd.print(rad_value * 0.0001, 5);
            lcd.setCursor(7, 1);
            lcd.print(" Rn/h");
            break;
        }
        break;
      // конец Рентген

    }
  //digitalWrite(LED_BUILTIN, LOW);
  }
}

void Counter()
{
  ++count;
}

void BootAnimation()
{
  delay(1000);
  lcd.clear();
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
  delay(4001);
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



