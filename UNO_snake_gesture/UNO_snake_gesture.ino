/*
    Example project reading from an ADPD1080
    breakout board connected to an angle sensor
    connected to a 16x16 dotstar matrix

    Implements the classic game Snake
    Using the on chip gesture engine as the controller


    Wiring:
      Matrix
        CI    : 13
        DI    : 11

      ADPD 1080
        SDA   : SDA
        SCL   : SCL
        GPIO0 : 2



    Tim Chase For ADI
    AUG 2018
    Updated OCT 2019 to use ADPD1080 libary

*/

#include <Wire.h>

// class to talk to 1080 gesture sensor
// this could be written as an arduino libary that could be distributed

#include <adpd1080.h>
#include <SPI.h>
#include <Adafruit_DotStar.h>
Adafruit_DotStar strip = Adafruit_DotStar(256, DOTSTAR_BGR);

// used to track user input
int8_t dx, dy;

// all of thes are of length 2
// arr[0] holds the X value
// arr[1] holds the Y value

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

	pinMode(2, INPUT);

	adpd.begin(adpd.GESTURE);
	strip.setBrightness(5);
	strip.begin();
	strip.clear();
	strip.show();
	delay(100);

	// seed random
	randomSeed(analogRead(0));

	// place apple at random place
	apple[0] = random(16);
	apple[1] = random(16);

	// play win animation at start
	win();
	strip.clear();


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
			strip.clear();
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

	strip.show();

	// clear screen for next frame
	strip.clear();

	delay(250);

}


// used to deal with the ziz-zag nature of the display
void pixel(uint8_t x, uint8_t y, uint32_t color) {

	uint16_t index = 0;
	index += x * 16;
	index += (x % 2) ? ( 15 - y) : y;
	strip.setPixelColor(index, color);
}

// cool win animation
void win() {
	uint32_t cols[] = {0xFF0000, 0xFF00, 0xFF};
	// red firework
	uint32_t c = cols[0];
	strip.clear();
	for (uint8_t i = 0; i < 11; i++) {
		pixel(i, 15 - i, 0xFF0000);
		strip.show();
		delay(100);
		strip.clear();
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
		strip.show();
		delay(100);
	}
	// Green firework
	c = cols[1];
	strip.clear();
	for (uint8_t i = 0; i < 11; i++) {
		pixel(15 - i, 15 - i, 0xFF0000);
		strip.show();
		delay(100);
		strip.clear();
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
		strip.show();
		delay(100);
	}
	// blue firework
	c = cols[2];
	strip.clear();
	for (uint8_t i = 0; i < 11; i++) {
		pixel(7 + i % 2 , 15 - i, 0xFF0000);
		strip.show();
		delay(100);
		strip.clear();
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
		strip.show();
		delay(100);
	}
}













//


