#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "RGB.h"
#include <adpd1080.h>
#include <SPI.h>
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif
#define PIN 6
#define HEIGHT 16
#define WIDTH 16

// MATRIX DECLARATION:
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16, 16, PIN,
                            NEO_MATRIX_BOTTOM     + NEO_MATRIX_LEFT +
                            NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
                            NEO_GRB            + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255)
};

int8_t dx, dy;
const uint8_t winLen = 15; // how many apple do you need to eat

uint8_t head[] = {8, 8};
uint8_t body[winLen + 1][2];
uint8_t apple[2];

// length of the snakes body
uint8_t len = 0;

// direction the snake is moving
int8_t dir[] = {1, 0};

uint32_t headColor =  0x00FF00;
uint32_t bodyColor =  0x0000FF;
uint32_t appleColor = 0xFF0000;

ADPD1080 adpd = ADPD1080(0x64);

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);
  adpd.begin(adpd.GESTURE);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(35);
  matrix.setTextColor(colors[0]);
  matrix.fillScreen(0);
  //initBoard();

    // seed random
  randomSeed(analogRead(0));

  // place apple at random place
  apple[0] = random(16);
  apple[1] = random(16);

  // play win animation at start
  win();
  matrix.fillScreen(0);
}

void loop() {

  // check if there is a new gesture
  if (!digitalRead(2)) {
    // clear dx and dy
    dx = dy = 0;

    switch (adpd.getGesture()) {
      case adpd.CLICK:
        break;
      case adpd.UP:
        dy = -1;
        break;
      case adpd.DOWN:
        dy = 1;
        break;
      case adpd.LEFT:
        dx = -1;
        break;
      case adpd.RIGHT:
        dx = 1;
        break;
    }
    adpd.clearFlags();
  }


  // determine direction
  if (abs(dx) > abs(dy)) {
    if (!dir[0]) { // only change direction if we turn
      dir[0] = abs(dx) / dx;
      dir[1] = 0;
    }
  } else if (abs(dx) < abs(dy)) {
    if (!dir[1]) { // only change direction if we turn
      dir[0] = 0;
      dir[1] = abs(dy) / dy;
    }
  }


  // if there is a body move the body
  if (len) {
    for (int8_t i = len - 1; i > 0; i--) {
      body[i][0] = body[i - 1][0];
      body[i][1] = body[i - 1][1];
    }

    body[0][0] = head[0];
    body[0][1] = head[1];
  }

  // move head
  head[0] = (head[0] + dir[0]) & 15;
  head[1] = (head[1] + dir[1]) & 15;

  // check if the snake ate his tail
  for (uint8_t i = 0; i < len; i++) {
    if ( body[i][0] == head[0] && body[i][1] == head[1]) {
      len = 0;
    }
  }

  // check if we ate an apple
  if (head[0] == apple[0] && head[1] == apple[1]) {
    if (len + 1 ==  winLen) { // you won
      win();
      matrix.fillScreen(0);
      len = 0;
      apple[0] = random(16);
      apple[1] = random(16);
      return;
    }
    len += 1;
    body[len - 1][0] = head[0];
    body[len - 1][1] = head[1];
    apple[0] = random(16);
    apple[1] = random(16);
  }

  // draw everything

  // draw apple
  pixel(apple[0], apple[1], appleColor);

  // draw head
  pixel(head[0], head[1], headColor);

  // draw body
  for (uint8_t i = 0; i < len; i++) {
    pixel(body[i][0], body[i][1], bodyColor);
  }

  matrix.show();

  // clear screen for next frame
  matrix.fillScreen(0);

  delay(250);

}

// used to deal with the ziz-zag nature of the display
void pixel(uint8_t x, uint8_t y, uint32_t color) {

  uint16_t index = 0;
  index += x * 16;
  index += (x % 2) ? ( 15 - y) : y;
  matrix.setPixelColor(index, color);
}

// cool win animation
void win() {
  uint32_t cols[] = {0xFF0000, 0xFF00, 0xFF};
  // red firework
  uint32_t c = cols[0];
  matrix.fillScreen(0);
  for (uint8_t i = 0; i < 11; i++) {
    pixel(i, 15 - i, 0xFF0000);
    matrix.show();
    delay(100);
    matrix.fillScreen(0);
  }
  pixel(11, 4, c);
  for (uint8_t i = 1; i < 4; i++) {
    pixel(11 + i, 4 + i / 3, c);
    pixel(11 - i, 4 + i / 3, c);
    pixel(11, 4 - i + i / 3, c);
    pixel(11, 4 + i, c);
    pixel(11 + i, 4 + i + i / 4, c);
    pixel(11 + i, 4 - i + i / 3, c);
    pixel(11 - i, 4 + i + i / 4, c);
    pixel(11 - i, 4 - i + i / 3, c);
    matrix.show();
    delay(100);
  }
  // Green firework
  c = cols[1];
  matrix.fillScreen(0);
  for (uint8_t i = 0; i < 11; i++) {
    pixel(15 - i, 15 - i, 0xFF0000);
    matrix.show();
    delay(100);
    matrix.fillScreen(0);
  }
  pixel(4, 4, c);
  for (uint8_t i = 1; i < 4; i++) {
    pixel(4 + i, 4 + i / 3, c);
    pixel(4 - i, 4 + i / 3, c);
    pixel(4, 4 - i + i / 3, c);
    pixel(4, 4 + i, c);
    pixel(4 + i, 4 + i + i / 4, c);
    pixel(4 + i, 4 - i + i / 3, c);
    pixel(4 - i, 4 + i + i / 4, c);
    pixel(4 - i, 4 - i + i / 3, c);
    matrix.show();
    delay(100);
  }
  // blue firework
  c = cols[2];
  matrix.fillScreen(0);
  for (uint8_t i = 0; i < 11; i++) {
    pixel(7 + i % 2 , 15 - i, 0xFF0000);
    matrix.show();
    delay(100);
    matrix.fillScreen(0);
  }
  pixel(8, 4, c);
  for (uint8_t i = 1; i < 4; i++) {
    pixel(8 + i, 4 + i / 3, c);
    pixel(8 - i, 4 + i / 3, c);
    pixel(8, 4 - i + i / 3, c);
    pixel(8, 4 + i, c);
    pixel(8 + i, 4 + i + i / 4, c);
    pixel(8 + i, 4 - i + i / 3, c);
    pixel(8 - i, 4 + i + i / 4, c);
    pixel(8 - i, 4 - i + i / 3, c);
    matrix.show();
    delay(100);
  }
}











