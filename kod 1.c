#include <SoftwareSerial.h>
#include <Tic.h>
#include <LiquidCrystal.h>

// Rotary encoder pinleri
const int pinCLK = 2;  // Encoder A
const int pinDT  = 3;  // Encoder B
volatile int encoderPos = 0;  // Encoder pozisyonu
volatile int lastEncoded = 0;

// SoftSerial: Arduino 10=RX, 11=TX
SoftwareSerial ticSerial(10, 11);  // Arduino 10=RX, 11=TX
TicSerial tic(ticSerial);          // Tic cihazı

// LCD pinleri: RS=13, E=12, D4=7, D5=6, D6=5, D7=4
LiquidCrystal lcd(13, 12, 7, 6, 5, 4);

void setup() {
  // Seri haberleşme ve LCD başlatma
  ticSerial.begin(9600);     
  lcd.begin(16, 2);

  // Enkoder pinleri
  pinMode(pinCLK, INPUT_PULLUP);
  pinMode(pinDT, INPUT_PULLUP);

  // Interrupt ile enkoder hareketini yakala
  attachInterrupt(digitalPinToInterrupt(pinCLK), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinDT), updateEncoder, CHANGE);

  // Tic safe start devreden çıkar
  tic.exitSafeStart();
}

void updateEncoder() {
  int MSB = digitalRead(pinCLK);
  int LSB = digitalRead(pinDT);
  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;
  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderPos++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderPos--;
  lastEncoded = encoded;

  // Pozisyonu -2000..+2000 aralığında sınırla
  if (encoderPos > 2000) encoderPos = 2000;
  if (encoderPos < -2000) encoderPos = -2000;
}

void loop() {
  // Motoru güncel pozisyona götür
  tic.setTargetPosition(encoderPos);

  // LCD ekranın sağ tarafında pozisyonu göster
  lcd.setCursor(10, 0);         // 1. satır, sağ taraf
  lcd.print("Motor Loc: ");
  lcd.print("      ");           // Önce eski değerleri temizle
  lcd.setCursor(10, 0);
  lcd.print(encoderPos);         // Yeni pozisyon yazdır
}
