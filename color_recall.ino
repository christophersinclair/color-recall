/**************************************************************************
Written by Chris Sinclair

# Color Recall

### Game Rules
Input the colors displayed on the screen using the buttons in the correct order.

### Hardware
1 x Arduino Uno
1 x SSD13306 OLED display
4 x LEDs of colors red, blue, yellow, and green
4 x Buttons with colored caps of red, blue, yellow, and green

### Software
Adafruit_BusIO
Adafruit_GFX_Library
Adafruit_SSD1306
Array
**************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Array.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for address
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define COLORS_MAX 55 // 10 turns only, (10 + 9 + 8 + 7 + 6 + 5 + 4 + 3 + 2 + 1)

// Bounds on display
int x, minX;

// Set GPIO pin locations for LEDs
const int RED_LED = 0;
const int BLUE_LED = 1;
const int YELLOW_LED = 2;
const int GREEN_LED = 3;

// Set GPIO pin locations for buttons
const int RED_BTN = 4;
const int BLUE_BTN = 5;
const int YELLOW_BTN = 6;
const int GREEN_BTN = 7;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  // Initialize display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
  }

  // LEDs are output
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Pullup resistors on buttons
  pinMode(RED_BTN, INPUT_PULLUP);
  pinMode(BLUE_BTN, INPUT_PULLUP);
  pinMode(YELLOW_BTN, INPUT_PULLUP);
  pinMode(GREEN_BTN, INPUT_PULLUP);

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setTextWrap(false);
}

// Display a scrolling message on the screen
void displayScrollingMessage(char *message, int loops) {
  int numScrolls = 0;
  int maxScrolls = loops; // Scroll the message <loops> times
  int totalWidth = -12 * strlen(message);
  int x = display.width();
  
  // Loop until we've scrolled the message <loops> times
  while (numScrolls < maxScrolls) {
    for (int x_pos = x; x_pos > totalWidth; x_pos -= 5) {
      display.clearDisplay();
      display.setCursor(x_pos, 10);
      display.print(message);
      display.display();
      delay(15); // Adjust this delay to control scroll speed
    }
    numScrolls++;
  }
}

// Display a static message to the screen
void displayMessage(char *message) {
  display.clearDisplay();
  display.setCursor(10, 10);
  display.print(message);
  display.display();
}

void gameLoss() {
  char lossMessage[] = "FAIL";
  displayMessage(lossMessage);

  for(int i = 0; i < 5; i ++) {
    digitalWrite(RED_LED, HIGH);
    delay(2000);
    digitalWrite(RED_LED, LOW);
    delay(1000);
  }

  for(;;) {
    // End of game
  }
}

void gameWin() {
  char winMessage[] = "WIN!";
  displayMessage(winMessage);

  for(int i = 0; i < 10; i++) {
    digitalWrite(BLUE_LED, HIGH);
    delay(100);
    digitalWrite(RED_LED, HIGH);
    delay(100);
    digitalWrite(YELLOW_LED, HIGH);
    delay(100);
    digitalWrite(GREEN_LED, HIGH);
    delay(100);
    digitalWrite(BLUE_LED, LOW);
    delay(100);
    digitalWrite(RED_LED, LOW);
    delay(100);
    digitalWrite(YELLOW_LED, LOW);
    delay(100);
    digitalWrite(GREEN_LED, LOW);
    delay(100);
  }
}

Array<int, COLORS_MAX> initializeMemorization() {
  long randNumber;

  Array<int, COLORS_MAX> colors;

  // Initializes entire game
  for(int i = 0; i < 10; i++) {
    for(int j = 0; j < colors.size(); j++) {
      colors.push_back(colors[j]);
    }

    randNumber = random(4);
    colors.push_back(randNumber);
  }

  return colors;
}

void nextColor(int iteration) {
  if(iteration == 0) {
    displayMessage("RED");
    digitalWrite(RED_LED, HIGH);
    delay(1000);
    digitalWrite(RED_LED, LOW);
  } else if(iteration == 1) {
    displayMessage("BLUE");
    digitalWrite(BLUE_LED, HIGH);
    delay(1000);
    digitalWrite(BLUE_LED, LOW);
  } else if(iteration == 2) {
    displayMessage("YELLOW");
    digitalWrite(YELLOW_LED, HIGH);
    delay(1000);
    digitalWrite(YELLOW_LED, LOW);
  } else if(iteration == 3) {
    displayMessage("GREEN");
    digitalWrite(GREEN_LED, HIGH);
    delay(1000);
    digitalWrite(GREEN_LED, LOW);
  }
}

void waitForColor(int iteration) {
  for(;;) {
    if(digitalRead(RED_BTN) == 0) {
      if(iteration == 0) {
        break;
      } else {
        gameLoss();
      }
    } else if(digitalRead(BLUE_BTN) == 0) {
      if(iteration == 1) {
        break;
      } else {
        gameLoss();
      }
    } else if(digitalRead(YELLOW_BTN) == 0) {
      if(iteration == 2) {
        break;
      } else {
        gameLoss();
      }
    } else if(digitalRead(GREEN_BTN) == 0) {
      if(iteration == 3) {
        break;
      } else {
        gameLoss();
      }
    }
  }
}

void showOrder(int begin, int end, Array<int, COLORS_MAX> *colors) {
  int iteration;
  for(int i = begin; i < end; i++) {
    iteration = (*colors)[i];
    nextColor(iteration);

    delay(2000);
    display.clearDisplay();
  }
}

void readOrder(int begin, int end, Array<int, COLORS_MAX> *colors) {
  int iteration;
  for(int i = begin; i < end; i++) {
    iteration = (*colors)[i];
    waitForColor(iteration);
  }
}

void loop() {
  // Display welcome message
  displayScrollingMessage("Welcome to Color Recall!", 2);

  // Display rules
  displayScrollingMessage("Press the colors in the order they appear!", 1);

  Array<int, COLORS_MAX> colors = initializeMemorization();

  // Turn 1
  showOrder(0, 1, &colors);
  readOrder(0, 1, &colors);

  // Turn 2
  showOrder(1, 3, &colors);
  readOrder(1, 3, &colors);

  // Turn 3
  showOrder(3, 6, &colors);
  readOrder(3, 6, &colors);

  // Turn 4
  showOrder(6, 10, &colors);
  readOrder(6, 10, &colors);

  // Turn 5
  showOrder(15, 21, &colors);
  readOrder(15, 21, &colors);

  // Turn 6
  showOrder(21, 28, &colors);
  readOrder(21, 28, &colors);

  // Turn 7
  showOrder(28, 36, &colors);
  readOrder(28, 36, &colors);

  // Turn 8
  showOrder(36, 45, &colors);
  readOrder(36, 45, &colors);

  // Turn 9
  showOrder(45, 55, &colors);
  readOrder(45, 55, &colors);

  // If got to this point without failure, win!
  gameWin();
}
