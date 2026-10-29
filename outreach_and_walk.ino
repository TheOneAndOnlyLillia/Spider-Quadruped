#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

#define screen_width 128
#define screen_height 64

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define servo_min  150
#define servo_max  600

#define oled_reset     -1
#define screen_address 0x3C
Adafruit_SSD1306 display(screen_width, screen_height, &Wire, oled_reset);

#define numflakes     10

#define logo_height   16
#define logo_width    16
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

const int fl_shoulder = 0; const int fl_middle = 1; const int fl_tibia = 2;
const int bl_shoulder = 3; const int bl_middle = 4; const int bl_tibia = 5;
const int fr_shoulder = 6; const int fr_middle = 7; const int fr_tibia = 8;
const int br_shoulder = 9; const int br_middle = 10; const int br_tibia = 11;

const int fl_shoulder_angle = 110;
const int fl_middle_angle   = 150;
const int fl_tibia_angle    = 90;

const int bl_shoulder_angle = 10;
const int bl_middle_angle   = 50;
const int bl_tibia_angle    = 110;

const int fr_shoulder_angle = 40;
const int fr_middle_angle   = 150;
const int fr_tibia_angle    = 90;

const int br_shoulder_angle = 80;
const int br_middle_angle   = 50;
const int br_tibia_angle    = 120;

const int fl_tibia_wave_up = 50;
const int fl_tibia_wave_down = 90;

const int shoulder_ch[4]   = { fl_shoulder,       bl_shoulder,       fr_shoulder,       br_shoulder };
const int middle_ch[4]     = { fl_middle,         bl_middle,         fr_middle,         br_middle };
const int shoulder_home[4] = { fl_shoulder_angle, bl_shoulder_angle, fr_shoulder_angle, br_shoulder_angle };
const int middle_home[4]   = { fl_middle_angle,   bl_middle_angle,   fr_middle_angle,   br_middle_angle };
const char* leg_name[4]    = { "FL", "BL", "FR", "BR" };

const int shoulder_fwd_sign[4] = { +1, +1, -1, -1 };
const int middle_up_sign[4]    = { +1, -1, -1, +1 };

int cur_shoulder[4] = { fl_shoulder_angle, bl_shoulder_angle, fr_shoulder_angle, br_shoulder_angle };

const int pair_a[2] = { 0, 3 };
const int pair_b[2] = { 1, 2 };

const int diag_swing_size = 6;
const int diag_lift_size  = 10;
const int diag_steps      = 6;
const int diag_step_ms    = 20;

const int homing_stagger_ms = 60;

const bool table_mode = false;

const int bow_amount   = 15;
const int show_steps   = 8;
const int show_step_ms = 15;

const int sit_angles[4] = { fl_middle_sit, bl_middle_sit, fr_middle_sit, br_middle_sit };
const int sit_bounce_amount    = 8;
const int sit_rock_amount      = 8;
const int shoulder_wiggle_size = 10;

void setAngle(int channel, int angle) {
  angle = constrain(angle, 0, 180);
  int pulse = map(angle, 0, 180, servo_min, servo_max);
  pwm.setPWM(channel, 0, pulse);
}

void wave() {
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
  setAngle(fl_tibia, fl_tibia_wave_up);
  delay(500);
  setAngle(fl_tibia, fl_tibia_wave_down);
  delay(500);
  setAngle(fl_tibia, fl_tibia_wave_up);
  delay(500);
  setAngle(fl_tibia, fl_tibia_wave_down);
  delay(500);
  setAngle(fl_tibia, fl_tibia_wave_up);
  delay(500);
  setAngle(fl_tibia, fl_tibia_wave_down);
  delay(500);
}

void standHome() {
  setAngle(fl_shoulder, fl_shoulder_angle); delay(homing_stagger_ms);
  setAngle(bl_shoulder, bl_shoulder_angle); delay(homing_stagger_ms);
  setAngle(fr_shoulder, fr_shoulder_angle); delay(homing_stagger_ms);
  setAngle(br_shoulder, br_shoulder_angle); delay(homing_stagger_ms);
  for (int i = 0; i < 4; i++) cur_shoulder[i] = shoulder_home[i];

  setAngle(fl_middle, fl_middle_angle); delay(homing_stagger_ms);
  setAngle(bl_middle, bl_middle_angle); delay(homing_stagger_ms);
  setAngle(fr_middle, fr_middle_angle); delay(homing_stagger_ms);
  setAngle(br_middle, br_middle_angle); delay(homing_stagger_ms);

  setAngle(fl_tibia, fl_tibia_angle); delay(homing_stagger_ms);
  setAngle(bl_tibia, bl_tibia_angle); delay(homing_stagger_ms);
  setAngle(fr_tibia, fr_tibia_angle); delay(homing_stagger_ms);
  setAngle(br_tibia, br_tibia_angle); delay(homing_stagger_ms);
}

void moveAllShouldersSmooth(const int toAngle[4], int steps, int stepDelayMs) {
  int fromAngle[4];
  for (int i = 0; i < 4; i++) fromAngle[i] = cur_shoulder[i];
  for (int s = 1; s <= steps; s++) {
    for (int i = 0; i < 4; i++) {
      int angle = fromAngle[i] + (long)(toAngle[i] - fromAngle[i]) * s / steps;
      setAngle(shoulder_ch[i], angle);
    }
    delay(stepDelayMs);
  }
  for (int i = 0; i < 4; i++) cur_shoulder[i] = toAngle[i];
}

void moveAllMiddlesSmooth(const int fromAngle[4], const int toAngle[4], int steps, int stepDelayMs) {
  for (int s = 1; s <= steps; s++) {
    for (int i = 0; i < 4; i++) {
      int angle = fromAngle[i] + (long)(toAngle[i] - fromAngle[i]) * s / steps;
      setAngle(middle_ch[i], angle);
    }
    delay(stepDelayMs);
  }
}

void liftSwingPair(const int pair[2]) {
  int liftTo[4];
  for (int i = 0; i < 4; i++) liftTo[i] = middle_home[i];
  for (int p = 0; p < 2; p++) {
    int leg = pair[p];
    liftTo[leg] = middle_home[leg] + middle_up_sign[leg] * diag_lift_size;
  }
  moveAllMiddlesSmooth(middle_home, liftTo, diag_steps, diag_step_ms);

  int fwd[4];
  for (int i = 0; i < 4; i++) fwd[i] = cur_shoulder[i];
  for (int p = 0; p < 2; p++) {
    int leg = pair[p];
    fwd[leg] = shoulder_home[leg] + shoulder_fwd_sign[leg] * diag_swing_size;
  }
  moveAllShouldersSmooth(fwd, diag_steps, diag_step_ms);

  moveAllMiddlesSmooth(liftTo, middle_home, diag_steps, diag_step_ms);
}

void pushAllShouldersBack() {
  int backAngle[4];
  for (int i = 0; i < 4; i++) {
    backAngle[i] = shoulder_home[i] - shoulder_fwd_sign[i] * diag_swing_size;
  }
  moveAllShouldersSmooth(backAngle, diag_steps, diag_step_ms);
}

void walkForward() {
  liftSwingPair(pair_a);
  liftSwingPair(pair_b);
  pushAllShouldersBack();
}

void sit() {
  moveAllMiddlesSmooth(middle_home, sit_angles, show_steps, show_step_ms);
}

void bounceFromBase(const int base[4], int amount, int times) {
  int down[4];
  for (int i = 0; i < 4; i++) down[i] = base[i] - middle_up_sign[i] * amount;

  for (int t = 0; t < times; t++) {
    moveAllMiddlesSmooth(base, down, show_steps, show_step_ms);
    moveAllMiddlesSmooth(down, base, show_steps, show_step_ms);
  }
}

void rockFromBase(const int base[4], int amount, int times) {
  int leanLeft[4];
  int leanRight[4];
  for (int i = 0; i < 4; i++) {
    leanLeft[i]  = base[i];
    leanRight[i] = base[i];
  }

  leanLeft[0] = base[0] - middle_up_sign[0] * amount;
  leanLeft[1] = base[1] - middle_up_sign[1] * amount;
  leanLeft[2] = base[2] + middle_up_sign[2] * amount;
  leanLeft[3] = base[3] + middle_up_sign[3] * amount;

  leanRight[0] = base[0] + middle_up_sign[0] * amount;
  leanRight[1] = base[1] + middle_up_sign[1] * amount;
  leanRight[2] = base[2] - middle_up_sign[2] * amount;
  leanRight[3] = base[3] - middle_up_sign[3] * amount;

  for (int t = 0; t < times; t++) {
    moveAllMiddlesSmooth(base, leanLeft, show_steps, show_step_ms);
    delay(200);
    moveAllMiddlesSmooth(leanLeft, leanRight, show_steps + 2, show_step_ms);
    delay(200);
    moveAllMiddlesSmooth(leanRight, base, show_steps, show_step_ms);
  }
}

void bowFromBase(const int base[4]) {
  int down[4];
  for (int i = 0; i < 4; i++) down[i] = base[i];
  down[0] = base[0] - middle_up_sign[0] * bow_amount;
  down[2] = base[2] - middle_up_sign[2] * bow_amount;

  moveAllMiddlesSmooth(base, down, show_steps, show_step_ms);
  delay(600);
  moveAllMiddlesSmooth(down, base, show_steps, show_step_ms);
}

void shoulderWiggle(int amount, int times) {
  int fwd[4];
  int back[4];
  for (int i = 0; i < 4; i++) {
    fwd[i]  = shoulder_home[i] + shoulder_fwd_sign[i] * amount;
    back[i] = shoulder_home[i] - shoulder_fwd_sign[i] * amount;
  }

  for (int t = 0; t < times; t++) {
    moveAllShouldersSmooth(fwd, show_steps, show_step_ms);
    moveAllShouldersSmooth(back, show_steps, show_step_ms);
  }
  moveAllShouldersSmooth(shoulder_home, show_steps, show_step_ms);
}

void showHappyFace() {
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
  delay(1000);

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
}

void performShow() {
  standHome();
  delay(800);

  sit();
  delay(800);

  showHappyFace();
  wave();

  shoulderWiggle(shoulder_wiggle_size, 3);
  rockFromBase(sit_angles, sit_rock_amount, 2);
  bounceFromBase(sit_angles, sit_bounce_amount, 3);
  bowFromBase(sit_angles);

  delay(600);
  standHome();
  delay(1500);
}

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  standHome();
  delay(4000);

  showHappyFace();
}

void loop() {
  if (table_mode) {
    performShow();
  } else {
    walkForward();
  }
}
