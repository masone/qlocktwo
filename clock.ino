#include "Wire.h"
#include "math.h"
#include "RTClib.h"
#include "LPD8806.h"
#include "SPI.h" // lpd8806 dependency

// RTC SCL (yellow): Analog in 5
// RTC SDA (blue): Analog in 4

int ledDataPin  = 6; // LED DI (blue)
int ledClockPin = 7; // LED CI (yellow)
int lightPin = 1; // Photoresistor, Analog
int buttonPin = 4; // Push button, digital

DateTime now;
int intensity = 50;
int min_intensity = 10;
int max_intensity = intensity;
int nLEDs = 118;
int reset = 1;

RTC_DS1307 rtc;
LPD8806 leds = LPD8806(nLEDs, ledDataPin, ledClockPin);  

void (*words_hour[24])();
void (*words_minute[12])();

void setup () {
  Serial.begin(57600);
  pinMode(buttonPin, INPUT);
 
  Serial.println("run");
  
  init_leds();
  init_rtc();
}

void loop() {
  now = rtc.now();
  
  reset_leds();  
  setup_words();
  adapt_intensity();  
  check_reset();
  
  if(digitalRead(buttonPin) == HIGH){
    test_words();
  }else{
    display_time();
  }

  delay(1000);
}

void print_time(int hour, int minute, int second){
  Serial.print(hour, DEC);
  Serial.print(':');
  Serial.print(minute, DEC);
  Serial.print(':');
  Serial.print(second, DEC);
  Serial.println();
}

void init_leds(){
  leds.begin();
  leds.show();
}

void init_rtc(){
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();

  if (!rtc.isrunning()) {
    error();
    // following line sets the RTC to the date & time this sketch was compiled
    Serial.println("RTC is NOT running!");
  }
}

void error(){
  leds.setPixelColor(0, 255, 0, 0);
  leds.show();
}

void check_reset(){
  if(reset == 1 && digitalRead(buttonPin) == HIGH){    
    Serial.println(F("Resetting time to..."));
    Serial.println(__TIME__);
    Serial.println(__DATE__);
    rtc.adjust(DateTime(__DATE__, __TIME__)); // Reset time to the compilation time
  }

  reset = 0;
}

void adapt_intensity(){
  intensity = map(analogRead(lightPin), 0, 1023, min_intensity, max_intensity);
}

void display_time(){
  int hour = now.hour();
  int minute = now.minute();
  int second = now.second();

  int rest_minute = (minute % 5);
  int rounded_minute = minute-rest_minute;

  print_time(hour, minute, second);
  
  _il_est();
  _heures();
  show_hour(hour);
  show_minute(rounded_minute);
  show_minute_edges(rest_minute);
  
  leds.show();
}

void setup_words(){
  words_hour[0] = _minuit;  
  words_hour[1] = _une;
  words_hour[2] = _deux;
  words_hour[3] = _trois;
  words_hour[4] = _quatre;
  words_hour[5] = _cinq;  
  words_hour[6] = _six;  
  words_hour[7] = _sept;  
  words_hour[8] = _huit;  
  words_hour[9] = _neuf;
  words_hour[10] = _dix;
  words_hour[11] = _onze;
  words_hour[12] = _midi;
  words_hour[13] = _une;  
  words_hour[14] = _deux;
  words_hour[15] = _trois;  
  words_hour[16] = _quatre;  
  words_hour[17] = _cinq; 
  words_hour[18] = _six;  
  words_hour[19] = _sept;  
  words_hour[20] = _huit;  
  words_hour[21] = _neuf;
  words_hour[22] = _dix;
  words_hour[23] = _onze; 

  words_minute[0] = _nul;
  words_minute[5] = _cinq2;
  words_minute[10] = _dix2;
  words_minute[15] = _et_quart;
  words_minute[20] = _vingt;
  words_minute[25] = _vingt_cinq;
  words_minute[30] = _et_demie;
  words_minute[35] = _moins_vingt_cinq;
  words_minute[40] = _moins_vingt;
  words_minute[45] = _moins_le_quart;
  words_minute[50] = _moins_dix;
  words_minute[55] = _moins_cinq;  
}

void test_words(){
  test_edges();
  test_hours();
  test_minutes();
}

void test_hours(){
  for(int i=0; i < 24; i++) {
    reset_leds();
    show_hour(i);
    leds.show();
    delay(3000);
  }
}

void test_minutes(){
  for(int i=0; i < 200; i++) {
    reset_leds();    
    show_minute(i*5);
    leds.show();    
    delay(3000);
  }
}

void test_edges(){
  for(int i=1; i <= 4; i++) {
    reset_leds();
    show_minute_edges(i);
    leds.show();
    delay(1000);  
  }
}

void reset_leds(){
  for(int i=0; i < leds.numPixels(); i++) {
    light_off(i+1);
  }
}

void light_on(int i){  
  leds.setPixelColor(i-1, color());
}

uint32_t color(){
  int month = now.month();
  int day = now.day();
  int second = now.second();

  if((day == 8 && month == 4) || (day == 6 && month == 10)){
    return Wheel(map(second, 0, 60, 0, 384));
  } else {
    return leds.Color(intensity, intensity, intensity);  
  }
}

void light_off(int i){
  leds.setPixelColor(i-1, 0);
}

void show_hour(int i){
  (*words_hour[i])();
}  

void show_minute(int i){
  (*words_minute[i])();
}

void show_minute_edges(int i){
  switch (i) {
  case 1:
    _m();
    break;
  case 2:
    _mm();
    break;
  case 3:
    _mmm();
    break;
  case 4:
    _mmmm();
    break;
  }
}

/* times */
void _m(){
  light_on(115);
}
void _mm(){
  _m();
  light_on(118);
}
void _mmm(){
  _mm();
  light_on(111);
}
void _mmmm(){
  _mmm();
  light_on(114);
}

void _heures(){
  light_on(55);
  light_on(66);  
  light_on(75);  
  light_on(86);  
  light_on(95);  
  light_on(106);  
}
void _il_est(){
  light_on(1);
  light_on(20);

  light_on(40);
  light_on(41);
  light_on(60);
}
void _une(){
  light_on(43);
  light_on(58);
  light_on(63);  
}
void _deux(){
  light_on(80);
  light_on(81);
  light_on(100);  
  light_on(101);  
}
void _trois(){
  light_on(62);
  light_on(79);
  light_on(82);
  light_on(99);
  light_on(102);
}
void _quatre(){
  light_on(2);
  light_on(19);
  light_on(22);
  light_on(39);
  light_on(42);
  light_on(59);
}
void _cinq(){
  light_on(77);
  light_on(84);
  light_on(97);
  light_on(104);
}
void _six(){
  light_on(44);
  light_on(57);
  light_on(64);
}
void _sept(){
  light_on(78);
  light_on(83);
  light_on(98);
  light_on(103);
}
void _huit(){
  light_on(4);
  light_on(17);
  light_on(24);
  light_on(37);
}
void _neuf(){
  light_on(3);
  light_on(18);
  light_on(23);
  light_on(38);
}
void _dix(){
  light_on(25);
  light_on(36);
  light_on(45);
}
void _onze(){
  light_on(6);
  light_on(15);
  light_on(26);
  light_on(35);
}
void _midi(){
  light_on(5);
  light_on(16);
  light_on(25);  
  light_on(36);  
}
void _minuit(){
  light_on(56);
  light_on(65);
  light_on(76);
  light_on(85);
  light_on(96);
  light_on(105);
}

void _nul(){
}
void _cinq2(){
  light_on(69);
  light_on(72);
  light_on(89);
  light_on(92);
}
void _dix2(){
  light_on(87);
  light_on(94);
  light_on(107);
}
void _et(){
  light_on(8);
  light_on(13);
}
void _le(){
  light_on(67);
  light_on(74);
}
void _quart(){
  light_on(33);
  light_on(48);
  light_on(53);
  light_on(68);
  light_on(73);
}
void _et_quart(){
  _et();
  _quart();
}
void _vingt(){
  light_on(9);
  light_on(12);
  light_on(29);
  light_on(32);
  light_on(49);
}
void _vingt_cinq(){
  _vingt();
  light_on(52);
  _cinq2();
}
void _et_demie(){
  light_on(10);
  light_on(11);

  light_on(31);
  light_on(50);
  light_on(51);  
  light_on(70);
  light_on(71);
}

void _moins(){
  light_on(7);
  light_on(14);
  light_on(27);
  light_on(34);
  light_on(47);
}
void _moins_vingt_cinq(){
  _moins();
  _vingt_cinq();
}
void _moins_vingt(){
  _moins();
  _vingt();
}
void _moins_le_quart(){
  _moins();
  _le();
  _quart();
}
void _moins_dix(){
  _moins();
  _dix2();
}
void _moins_cinq(){
  _moins();
  _cinq2();
}


// Input a value 0 to 384 to get a color value.
// The colours are a transition r - g -b - back to r
uint32_t Wheel(int WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //Red down
      g = WheelPos % 128;      // Green up
      b = 0;                  //blue off
      break; 
    case 1:
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      r = 0;                  //red off
      break; 
    case 2:
      b = 127 - WheelPos % 128;  //blue down 
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      break; 
  }
  return(leds.Color(r,g,b));
}
