#include <Adafruit_NeoPixel.h>

 
#define LeftSignal 2     // Analog Pin 5 used as trigger for left turn-signal
#define RightSignal 3    // Analog Pin 4 used as trigger for right turn-signal
#define ReverseSignal 4  // Analog Pin 3 used as trigger for reverse light
#define PIN 9             // NeoPixel data pin  
#define BRIGHTNESS 255    // Full brightness
#define NUM_PIXELS 30      // Total number of NeoPixels
#define WIPE_SPEED 25     // Sweep animation delay.

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

int LeftBlinkerState  = 0;
int RightBlinkerState = 0;
int ReverseLightState = 0;
int runLightState     = 0;
int reverseState      = 0;
 
uint32_t AMBER = strip.Color(255, 130, 0);
uint32_t RED   = strip.Color(255, 0, 0);
uint32_t WHITE = strip.Color(255, 255, 255);
uint32_t BLUE  = strip.Color(0, 0, 255);
uint32_t OFF   = strip.Color(0, 0, 0);

/**
 * Setup pins and do fancy start-up sequence. 
 * Red/White/Blue Larson scanner. 
 */
void setup() {
  pinMode(LeftSignal, INPUT); 
  pinMode(RightSignal, INPUT);   
  pinMode(ReverseSignal, INPUT);   
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();   
  knightRider(1, WIPE_SPEED, 2, RED);  
  knightRider(1, WIPE_SPEED, 2, WHITE);
  knightRider(1, WIPE_SPEED, 2, BLUE);    
}

/**
 * Main loop
 */
void loop() {
  // Read the state of the left turn-signal voltage
  //LeftBlinkerState = digitalRead(LeftSignal);
  int LeftVoltage = digitalRead(LeftSignal);
  //float LeftVoltage = LeftBlinkerState * (5.0 / 1023.0);  

  // Read the state of the right turn-signal voltage
  //RightBlinkerState = digitalRead(RightSignal);
  int RightVoltage = digitalRead(RightSignal);
  //float RightVoltage = RightBlinkerState * (5.0 / 1023.0);  

  // Read the state of the reverse lights voltage
  //ReverseLightState = digitalRead(ReverseSignal);
  int ReverseVoltage = digitalRead(ReverseSignal);
  //float ReverseVoltage = ReverseLightState * (5.0 / 1023.0);  

  // If we have voltage clear the strip from 'run light state' 
  // and start Larson scan to the left. 
  if (LeftVoltage == HIGH) {
    runLightState = 0;
    reverseState = 0;
    clearStrip();
    leftTurn();
    delay(5);
  } else if (RightVoltage == HIGH ) {
    // If we have voltage clear the strip from 'run light state' start Larson scan to the right. 
    runLightState = 0;
    reverseState = 0;
    clearStrip();
    rightTurn();
    delay(5);
  } else if (ReverseVoltage == HIGH) {
     runLightState = 0;
     reverseState = 0;
     clearStrip();
     reverseLight();
  } else if (RightVoltage < 3.0 && LeftVoltage < 3.0 && ReverseVoltage < 3.0) { 
    // If there's < 3v (capasitor drain) revert to run light state. 
    runLight();
  }
  
  delay(100);
}

/**
 * Larson scan for right turn.
 */
void rightTurn() {
  strip.setBrightness(BRIGHTNESS);
  
  // Turn strip on. 
  for (int16_t i = NUM_PIXELS / 2; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, AMBER);
    strip.show();
    delay(10);
  }
  
  delay(300);
  
  // Turn strip off.
  for (int16_t i = NUM_PIXELS / 2; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, OFF);
    strip.show();
    delay(10);
  }
  
  delay(150);
}

/**
 * Larson scan for Left turn.
 */
void leftTurn() {
  strip.setBrightness(BRIGHTNESS);
  
  for (int16_t i = (NUM_PIXELS-1) /2 ; i > -1; i--) {
    strip.setPixelColor(i, AMBER);
    strip.show();
    delay(10);
  }

  delay(300);

  for (int16_t i = NUM_PIXELS / 2; i > -1; i--) {
    strip.setPixelColor(i, OFF);
    strip.show();
    delay(10);
  }  
  
  delay(150);
}

/**
 * Helper function to make all LEDs color (c)
 */
void all(uint32_t c) {
  for(uint16_t i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);  
  }
  strip.show();
}

/**
 * Nothing is goign on, red 'run' state.
 */
void runLight()
{
  if (runLightState == 0) {
    strip.setBrightness(BRIGHTNESS / 12);
    spread(WIPE_SPEED, RED);
    runLightState = 1;  
    all(RED);
    strip.show();
  }
}

/**
 * We're in reverse turn white. 
 */
void reverseLight()
{
  if (reverseState == 0) {
    strip.setBrightness(BRIGHTNESS);
    spread(WIPE_SPEED, WHITE);
    reverseState = 1;  
    all(WHITE);
    strip.show();
  }
}

// Function for larson scan. 
void knightRider(uint16_t cycles, uint16_t speed, uint8_t width, uint32_t color) {
  uint32_t old_val[NUM_PIXELS]; // up to 256 lights!

  for(int i = 0; i < cycles; i++){
    for (int count = 1; count < NUM_PIXELS; count++) {
      strip.setPixelColor(count, color);
      old_val[count] = color;
      for(int x = count; x>0; x--) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        strip.setPixelColor(x-1, old_val[x-1]); 
      }
      strip.show();
      delay(speed);
    }
    
    for (int count = NUM_PIXELS-1; count>=0; count--) {
      strip.setPixelColor(count, color);
      old_val[count] = color;
      for(int x = count; x<=NUM_PIXELS ;x++) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        strip.setPixelColor(x+1, old_val[x+1]);
      }
      strip.show();
      delay(speed);
    }

  }
}

// Set all LEDs to off/black. 
void clearStrip() {
  for (int i = 0; i<NUM_PIXELS; i++) {
    strip.setPixelColor(i, 0x000000); 
    strip.show();
  }
}

// Dim a color across a width of leds. 
uint32_t dimColor(uint32_t color, uint8_t width) {
   return (((color&0xFF0000)/width)&0xFF0000) + (((color&0x00FF00)/width)&0x00FF00) + (((color&0x0000FF)/width)&0x0000FF);
}

// Function to light LEDS from the center one at a time (spreading). 
void spread(uint16_t speed, uint32_t color) {
  clearStrip();
  
  delay(300);
  
  int center = NUM_PIXELS / 2;
  
  for (int x = 0; x < center; x++) {
    strip.setPixelColor(center + x, color); 
    strip.setPixelColor(center + (x*-1), color); 
    strip.show();
    delay(speed);
  }    
}
