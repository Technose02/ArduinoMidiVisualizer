#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            7

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      144

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define BAUDRATE 115200

const byte REFRESH_DELAY = 10;
const byte CMD_SET_LED = NUMPIXELS;
const byte CMD_SHOW_LEDS = NUMPIXELS + 1;
const byte CMD_CLEAR_LEDS = NUMPIXELS + 2;

uint32_t colors[33];
void setupColors() {
  colors[0]  = pixels.Color(0, 0, 0);                     // off
  colors[1]  = pixels.gamma32(pixels.Color(0, 0, 180));   // blue    CH01 (w)
  colors[2]  = pixels.gamma32(pixels.Color(0, 125, 125)); // cyan    CH01 (k)
  colors[3]  = pixels.gamma32(pixels.Color(0, 180, 0));   // green   CH02 (w)
  colors[4]  = pixels.gamma32(pixels.Color(125, 125, 0)); // yellow  CH02 (k)
  colors[5]  = pixels.gamma32(pixels.Color(180, 0, 0));   // red     CH03 (w)
  colors[6]  = pixels.gamma32(pixels.Color(125, 0, 125)); // magenta CH03 (k)
  colors[7]  = pixels.gamma32(pixels.Color(0, 0, 180));   // blue    CH04 (w)
  colors[8]  = pixels.gamma32(pixels.Color(0, 125, 125)); // cyan    CH04 (k)
  colors[9]  = pixels.gamma32(pixels.Color(0, 180, 0));   // green   CH05 (w)
  colors[10] = pixels.gamma32(pixels.Color(125, 125, 0)); // yellow  CH05 (k)
  colors[11] = pixels.gamma32(pixels.Color(180, 0, 0));   // red     CH06 (w)
  colors[12] = pixels.gamma32(pixels.Color(125, 0, 125)); // magenta CH06 (k)
  colors[13] = pixels.gamma32(pixels.Color(0, 0, 180));   // blue    CH07 (w)
  colors[14] = pixels.gamma32(pixels.Color(0, 125, 125)); // cyan    CH07 (k)
  colors[15] = pixels.gamma32(pixels.Color(0, 180, 0));   // green   CH08 (w)
  colors[16] = pixels.gamma32(pixels.Color(125, 125, 0)); // yellow  CH08 (k)
  colors[17] = pixels.gamma32(pixels.Color(180, 0, 0));   // red     CH09 (w)
  colors[18] = pixels.gamma32(pixels.Color(125, 0, 125)); // magenta CH09 (k)  
  colors[19] = pixels.gamma32(pixels.Color(160,160,160)); // white   CH10 (w)
  colors[20] = pixels.gamma32(pixels.Color(80, 80, 80));  // grey    CH10 (k)  
  colors[21] = pixels.gamma32(pixels.Color(180, 0, 0));   // red     CH11 (w)
  colors[22] = pixels.gamma32(pixels.Color(125, 0, 125)); // magenta CH11 (k)
  colors[23] = pixels.gamma32(pixels.Color(0, 0, 180));   // blue    CH12 (w) // Synthesia left
  colors[24] = pixels.gamma32(pixels.Color(0, 125, 125)); // cyan    CH12 (k) // Synthesia left
  colors[25] = pixels.gamma32(pixels.Color(0, 180, 0));   // green   CH13 (w) // Synthesia right
  colors[26] = pixels.gamma32(pixels.Color(125, 125, 0)); // yellow  CH13 (k) // Synthesia right   
  colors[27] = pixels.gamma32(pixels.Color(0, 0, 180));   // blue    CH14 (w)
  colors[28] = pixels.gamma32(pixels.Color(0, 125, 125)); // cyan    CH14 (k)
  colors[29] = pixels.gamma32(pixels.Color(0, 180, 0));   // green   CH15 (w)
  colors[30] = pixels.gamma32(pixels.Color(125, 125, 0)); // yellow  CH15 (k)
  colors[31] = pixels.gamma32(pixels.Color(180, 0, 0));   // red     CH16 (w)
  colors[32] = pixels.gamma32(pixels.Color(125, 0, 125)); // magenta CH16 (k)
}

byte cmd;
byte idx;
byte col;
byte cmdPos;

byte writeAction = 0;

void showLeds() {
  pixels.show();
}

void clearLeds() {
  pixels.clear();
}

void setLed() {
  pixels.setPixelColor(idx, colors[col]);
}

void setup() {
  setupColors();  
  Serial.begin(BAUDRATE);
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.show();
  cmdPos = 0;
}

byte refreshCntr = 0;
void loop() {
  if (refreshCntr == REFRESH_DELAY) {
    showLeds();
    refreshCntr = 0;
  } else {
    refreshCntr++;
  }
  
  if (Serial.available()) {
    byte b = Serial.read();

    writeAction = 0;
    
    switch (cmdPos) {
      case 0: // Warten auf neues Kommando
        if (b < CMD_SET_LED) {
          cmd = CMD_SET_LED;
          idx = b;
          cmdPos ++;
          // in diesem Durchlauf nichts ausgeben, warte noch auf col
        } else if (b == CMD_SHOW_LEDS) {
          cmd = b;
          showLeds();
          writeAction = 1; // in diesem Durchlauf sowohl nur cmd ausgeben
        } else if (b == CMD_CLEAR_LEDS) {
          cmd = b;
          clearLeds();
          writeAction = 1; // in diesem Durchlauf sowohl nur cmd ausgeben
        }
        break;
      case 1: // Habe den Index für ein SET_LED-Kommando
              // gelesen und lese jetzt den col-Wert
        col = b;
        setLed();
        writeAction = 2; // in diesem Durchlauf sowohl idx als auch col ausgeben
        cmdPos = 0;
        break;
    }

    switch (writeAction) {
      case 1:
        Serial.write(cmd);
        break;
      case 2:
          Serial.write(idx);
          Serial.write(col);
    }
  }
}
