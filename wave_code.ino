// made in arduino IDE 

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVO_MIN  150
#define SERVO_MAX  600

#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

//original standing leg angles
const int FL_SHOULDER = 0; const int FL_MIDDLE = 1; const int FL_TIBIA = 2;
const int BL_SHOULDER = 3; const int BL_MIDDLE = 4; const int BL_TIBIA = 5;
const int FR_SHOULDER = 6; const int FR_MIDDLE = 7; const int FR_TIBIA = 8;
const int BR_SHOULDER = 9; const int BR_MIDDLE = 10; const int BR_TIBIA = 11;

const int FL_SHOULDER_ANGLE = 90;
const int FL_MIDDLE_ANGLE   = 70;
const int FL_TIBIA_ANGLE    = 90;

const int BL_SHOULDER_ANGLE = 50;
const int BL_MIDDLE_ANGLE   = 100;
const int BL_TIBIA_ANGLE    = 110;

const int FR_SHOULDER_ANGLE = 50;
const int FR_MIDDLE_ANGLE   = 90;
const int FR_TIBIA_ANGLE    = 90;

const int BR_SHOULDER_ANGLE = 70;
const int BR_MIDDLE_ANGLE   = 50;
const int BR_TIBIA_ANGLE    = 120;

const int FL_MIDDLE_SIT = 100;
const int FR_MIDDLE_SIT = 60;
const int BL_MIDDLE_SIT = 70;
const int BR_MIDDLE_SIT = 80;

const int FL_TIBIA_WAVE_UP = 50;
const int FL_TIBIA_WAVE_DOWN = 90;

void setAngle(int channel, int angle) {
  angle = constrain(angle, 0, 180);
  int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
  pwm.setPWM(channel, 0, pulse);
}

void standHome() {
  setAngle(FL_SHOULDER, FL_SHOULDER_ANGLE); setAngle(FL_MIDDLE, FL_MIDDLE_ANGLE); setAngle(FL_TIBIA, FL_TIBIA_ANGLE);
  setAngle(BL_SHOULDER, BL_SHOULDER_ANGLE); setAngle(BL_MIDDLE, BL_MIDDLE_ANGLE); setAngle(BL_TIBIA, BL_TIBIA_ANGLE);
  setAngle(FR_SHOULDER, FR_SHOULDER_ANGLE); setAngle(FR_MIDDLE, FR_MIDDLE_ANGLE); setAngle(FR_TIBIA, FR_TIBIA_ANGLE);
  setAngle(BR_SHOULDER, BR_SHOULDER_ANGLE); setAngle(BR_MIDDLE, BR_MIDDLE_ANGLE); setAngle(BR_TIBIA, BR_TIBIA_ANGLE);
}

void sit() {
  setAngle(FL_MIDDLE, FL_MIDDLE_SIT);
  setAngle(FR_MIDDLE, FR_MIDDLE_SIT);
  setAngle(BL_MIDDLE, BL_MIDDLE_SIT);
  setAngle(BR_MIDDLE, BR_MIDDLE_SIT);
}

void wave() {
  //display code makes cute blinking face
  display.clearDisplay();

  display.drawLine(32, 28, 42, 16, SSD1306_WHITE);
  display.drawLine(33, 28, 43, 16, SSD1306_WHITE);
  display.drawLine(42, 16, 52, 28, SSD1306_WHITE);
  display.drawLine(43, 16, 53, 28, SSD1306_WHITE);

  display.drawLine(76, 28, 86, 16, SSD1306_WHITE);
  display.drawLine(77, 28, 87, 16, SSD1306_WHITE);
  display.drawLine(86, 16, 96, 28, SSD1306_WHITE);
  display.drawLine(87, 16, 97, 28, SSD1306_WHITE);

  display.fillCircle(64, 42, 12, SSD1306_WHITE);
  display.fillRect(50, 30, 28, 12, SSD1306_BLACK);

  display.fillCircle(64, 50, 5, SSD1306_BLACK);

  display.fillCircle(25, 34, 4, SSD1306_WHITE);
  display.fillCircle(103, 34, 4, SSD1306_WHITE);

  display.display();
  setAngle(FL_TIBIA, FL_TIBIA_WAVE_UP);
  delay(500);
  setAngle(FL_TIBIA, FL_TIBIA_WAVE_DOWN);
  delay(500);
  setAngle(FL_TIBIA, FL_TIBIA_WAVE_UP);
  delay(500);
  setAngle(FL_TIBIA, FL_TIBIA_WAVE_DOWN);
  delay(500);
  setAngle(FL_TIBIA, FL_TIBIA_WAVE_UP);
  delay(500);
  setAngle(FL_TIBIA, FL_TIBIA_WAVE_DOWN);
  delay(500);
}

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  standHome();
  display.clearDisplay();

  display.fillRoundRect(35, 15, 14, 20, 5, SSD1306_WHITE);
  display.fillRoundRect(79, 15, 14, 20, 5, SSD1306_WHITE);
  display.fillCircle(25, 34, 4, SSD1306_WHITE);
  display.fillCircle(103, 34, 4, SSD1306_WHITE);

  display.drawPixel(52, 44, SSD1306_WHITE);
  display.drawFastHLine(53, 45, 10, SSD1306_WHITE);
  display.drawPixel(63, 44, SSD1306_WHITE);
  display.drawFastHLine(65, 45, 10, SSD1306_WHITE);
  display.drawPixel(75, 44, SSD1306_WHITE);

  display.display();
  delay(3000);
  delay(6000);

  sit();
  delay(2000);
  wave();
}

void loop() {
  display.clearDisplay();

  display.fillRoundRect(35, 15, 14, 20, 5, SSD1306_WHITE);
  display.fillRoundRect(79, 15, 14, 20, 5, SSD1306_WHITE);
  display.fillCircle(25, 34, 4, SSD1306_WHITE);
  display.fillCircle(103, 34, 4, SSD1306_WHITE);

  display.drawPixel(52, 44, SSD1306_WHITE);
  display.drawFastHLine(53, 45, 10, SSD1306_WHITE);
  display.drawPixel(63, 44, SSD1306_WHITE);

  display.drawFastHLine(65, 45, 10, SSD1306_WHITE);
  display.drawPixel(75, 44, SSD1306_WHITE);

  display.display();
  delay(3000);

  display.clearDisplay();

  display.drawLine(32, 28, 42, 14, SSD1306_WHITE);
  display.drawLine(42, 14, 52, 28, SSD1306_WHITE);

  display.drawLine(76, 28, 86, 14, SSD1306_WHITE);
  display.drawLine(86, 14, 96, 28, SSD1306_WHITE);

  display.fillCircle(25, 34, 4, SSD1306_WHITE);
  display.fillCircle(103, 34, 4, SSD1306_WHITE);

  display.drawPixel(52, 44, SSD1306_WHITE);
  display.drawFastHLine(53, 45, 10, SSD1306_WHITE);
  display.drawPixel(63, 44, SSD1306_WHITE);
  display.drawFastHLine(65, 45, 10, SSD1306_WHITE);
  display.drawPixel(75, 44, SSD1306_WHITE);

  display.display();
  delay(250);

  delay(2000);
  wave();
}
