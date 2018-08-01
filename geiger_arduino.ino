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


LiquidCrystal_I2C lcd(0x27,16,2);  // Устанавливаем дисплей


void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2), Counter, RISING); // Прерывания для считывания пульсов трубки Гейгера-Мюллера

  Serial.begin(9600); // Вывод в ПК по COM интерфейсу // Debug
  
  lcd.init(); // инициализация LCD
  lcd.backlight(); // включаем подсветку
  lcd.clear(); // очистка дисплея

  lcd.setCursor(1,0);
  lcd.print("S.T.A.L.K.E.R.");
  lcd.setCursor(6,1);
  lcd.print("KIT");

}

void loop() 
{
  //lcd.setCursor(2, 0); // устанавливаем курсор на 0 строку, 3 символ
  //lcd.print("CPM"); // вывод надписи
  //lcd.setCursor(1, 1); // устанавливаем курсор на 1 строку, 1 символ
  //lcd.print(millis()/1000); // задержка
  Show_CPM_uSv();
}

void Show_CPM_uSv()
{
  if(millis()-time_previous_measure > TIME_PERIOD)
  {
    //float elapsed_millis = millis();
    //int elapsed_sec = round((elapsed_millis)/1000);
    count_per_minute = 6*count;
    //count_per_minute = count;
    count = 0;
    rad_value = count_per_minute * CONVERT_PULSE;
    time_previous_measure = millis();
    //debug zone
    //Serial.print("CPM = ");
    //Serial.print(count_per_minute, DEC);
    //Serial.print("    ");
    //Serial.print("uSv/h = ");
    //Serial.println(rad_value, 4);
    //end debug zone
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CPM = ");
    lcd.setCursor(6,0);
    lcd.print(count_per_minute);
    lcd.setCursor(0,1);
    lcd.print(rad_value, 4);
    lcd.setCursor(6,1);
    lcd.print(" uSv/h");
    //count = 0;
    //delay(3000);
    
    
  }
}

void Counter()
{
  //digitalWrite(LED_BUILTIN, HIGH);
  //delay(100);
  //digitalWrite(LED_BUILTIN, LOW);
  ++count;
}


