// MaryJo Santos, UIN: 662163618, netID: msant22
// Zainab Zaneer, UIN: 679493513, netID: zzane2
// CS 362 – Computer Design Course Project Fall 2024 
// FlashBack - a memory game

// Abstract
  // FlashBack is a memory game where players must mimic sequences 
  // of LED lights using buttons to progress through rounds. 
  // The game features adjustable difficulty levels, high score 
  // tracking, and a visual score display via an LCD. This project 
  // integrates LEDs, buttons, a potentiometer and an LCD, to create 
  // a challenging and customizable memory game experience.

// CODE FOR ARDUINO 2
// Game display and start: LCD, Potentiometer and start Button

#include <LiquidCrystal.h>

// PIN & STATE INITIALIZATION ---------------------------

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Potentiometer
int potPin = A5;

// Button
int buttonPin = 7;
int prevButtonState = LOW;

// OTHER VARS ---------------------------

// Variable to read incoming serial data into 
int incomingByte;

// Keeping track of time, instead of using delay()
unsigned long previousMillis = 0;

// Game Play Vars 

// Track status of game (currently playing or not)
bool inGame = false;

// Track game difficulty, number also assigned to length of pattern ( 4 : easy, 6 : medium, 8 : hard)
int gameDiff = 0;

// Store the current game score
int currScore = 0;

// Store the high score
int highScore = 0;


void setup() {
  // INITTIALIZE INPUTS/OUTPUTS & STATES ---------------------------

  // LCD
  lcd.begin(16, 2);

  // Button
  pinMode(buttonPin, INPUT);

  // Initialize serial communication
  Serial.begin(9600);

}

// Loop basics:

  // 1. Read in any serial data
    // F -> game end
    // number -> increment score by num

  // 2. if button pressed and game not yet started
    // start game, send signal

  // 3. LCD display in intervals:
    // if inGame false, not in game
      // update difficulty display based on pot
      // display high score
    // if inGame true, game being played
      // display current score

void loop() {
  // 1. Read in any incoming serial data --------------------------------
  readSerialData();

  // 2. Check for button press --------------------------------
  checkButtons();

  // 3. update display in intervals for visibility --------------------------------

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 300) {

    // Check if game is not being played
    if (!inGame) {
      // display game settings, according to what level is currently selected
      displayGameSettings();

    } else if (inGame) {  // If game is currently being played
      displayGameScreen();
    }

    // Update time
    previousMillis = currentMillis;
  }
  
}

// utilize time to display lights and transhitions in timely matter
void wait(long interval) {
  previousMillis = millis();
  unsigned long currentMillis = millis();
  while (currentMillis - previousMillis < interval) { currentMillis = millis(); }
}

// Read in and respond to incoming serial data
void readSerialData() {
  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();

    // if it's a capital F (Finish), set inGame to false to terminate game
    if (incomingByte == 'F') {
      inGame = false;

    } else { // read 4/6/8, player scored points. increment curr score accodingly
      // convert from ASCII digit to integer
      incomingByte = incomingByte - '0';
      // add scored points to total current game points
      currScore += incomingByte;
        
      // compare and update highScore accordingly
      if (currScore > highScore) { highScore = currScore; }
    }

  }
}

// check button for any presses
void checkButtons() {
  int buttonState = digitalRead(buttonPin); // get current state of button
  // if button state has changed (button pressed down)
  if (buttonState == HIGH && prevButtonState == LOW) {
    wait(50); // debounce

    while (buttonState == HIGH) { buttonState = digitalRead(buttonPin); }

    // check if new state is low: if button was released
    // and game is not in progress
    if (buttonState == LOW && !inGame) {
      // allow game to start
      inGame = true;
      // reset current game score
      currScore = 0;
      // Output chosen game difficulty 
      Serial.print(gameDiff);
      
    }
  }
}

// display game settings on LCD
void displayGameSettings() {
  int potVal = analogRead(potPin);   // read the potentiometer value at the input pin

  // initialize display text
  char line1[] = "easy medium hard";
  char line2[15];
  sprintf(line2, " High Score: %d", highScore);

  // use potVal to determin what level is currently selected
    // 0 - 333 : easy, 333 - 666 : medium, 666+ : hard
  if (potVal < 333) { // easy (gameDiff = 4)
    gameDiff = 4; // set game pattern length

    strcpy(line1, "EASY medium hard"); // Adjust display

  } else if (333 < potVal && potVal < 666) { // medium (gameDiff = 6)
    gameDiff = 6; // set game pattern length

    strcpy(line1, "easy MEDIUM hard"); // Adjust display

  } else if (666 < potVal) { // hard (gameDiff = 8)
    gameDiff = 8; // set game pattern length

    strcpy(line1, "easy medium HARD"); // Adjust display
  }

  // output text
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

// display for while in game
void displayGameScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Current Score:");

  // Display current score
  char line2[15];
  sprintf(line2, "      %d", currScore);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}