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

// CODE FOR ARDUINO 1
// Game play: LEDs and Buttons

#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

// PIN & STATE INITIALIZATION ---------------------------

// global var for number of LEDs and Buttons (always equivalent)
const int numOptions = 4;

// 4x LEDs
int ledPins[numOptions] = {13, 10, 7, 4};
int ledStates[numOptions];

// 4x Buttons
int buttonPins[numOptions] = {12, 9, 6, 3};
int prevButtonStates[numOptions];

// OTHER VARS ---------------------------

// Variable to read incoming serial data into 
int incomingByte;

// Keeping track of time, instead of using delay()
unsigned long previousMillis = 0;
//const long interval = 500;

// Game Play Vars 

// Track status of game (currently playing or not)
bool inGame = false;

// Track game difficulty, number also assigned to length of pattern ( 4 : easy, 6 : medium, 8 : hard)
int gameDiff = 0;


void setup() {
  // INITTIALIZE INPUTS/OUTPUTS & STATES ---------------------------

  for (int i = 0; i < numOptions; i++) {
    // 4x LEDs as outputs
    pinMode(ledPins[i], OUTPUT);
    // 4x LEDs initialize state
    ledStates[i] = LOW;
    digitalWrite(ledPins[i], ledStates[i]);

    // 4x Buttons as inputs 
    pinMode(buttonPins[i], INPUT);
    // 4x Buttons initialize state
    prevButtonStates[i] = LOW;
  }

  // Initialize serial communication
  Serial.begin(9600);

  // Seed the random number generator with the current time 
  srand(time(NULL));

}

  //// each loop is an entire game
void loop() {

  // 1. Read in any incoming serial data --------------------------------
  readSerialData();

  // 2. begin game --------------------------------
  while(inGame) {

    // array to hold memory sequence
    int correctSequence[gameDiff];
    // randomly generate memory sequence
    for (int i = 0; i < gameDiff; i++) {
      correctSequence[i] = rand() % numOptions; // random num from 0 - 3
    }

    // transition 
    wait(500);
    blinkLights(); // round beginning/finishing animation
    wait(500);

    // 2.1 display LED sequence --------------------------------
    displaySequence(correctSequence);
    
    // 2.2 obtain user sequence --------------------------------
    // if false is returned, user failed. end game.
    if (!obtainUserInputs(correctSequence)) {
      flashLights(); // game ending animation
      inGame = false;
      Serial.print('F');
      break;
    }

    // player completed round
    // send signal to increment score
    Serial.print(gameDiff);
  
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

    // read 4/6/8, for difficultly set by player
    // convert from ASCII digit to integer
    incomingByte = incomingByte - '0';

    // set difficulty accordingly
    gameDiff = incomingByte;

    // begin game
    inGame = true;

  }
}

// blinks all lights 3 times, indicating beginning of round
void blinkLights() {
  
  for(int h = 0; h < 3; h++) {
    // turn on LEDs
    for(int i = 0; i < numOptions; i++) {
      ledStates[i] = HIGH;
      digitalWrite(ledPins[i], ledStates[i]);
    }

    wait(200);

    // turn off LEDs
    for(int i = 0; i < numOptions; i++) {
      ledStates[i] = LOW;
      digitalWrite(ledPins[i], ledStates[i]);
    }

    wait(200);
  }
}

// display LED sequence to player
void displaySequence(int correctSequence[]) {
  // iterate to light each LED
  for(int i = 0; i < gameDiff; i++) {
    // get the index of LED in sequence 
    int currLED = correctSequence[i];

    // turn on LED
    ledStates[currLED] = HIGH;
    digitalWrite(ledPins[currLED], ledStates[currLED]);

    wait(600);

    // turn off LED
    ledStates[currLED] = LOW;
    digitalWrite(ledPins[currLED], ledStates[currLED]);
    wait(200);
  }
}

// listen for and obtain button inputs
// compares input to correct sequence
bool obtainUserInputs(int correctSequence[]) {
  int currPlayerInput = -1;  // to store current player input

    // iterate for each input needed
    for (int i = 0; i < gameDiff; i++) {
      bool inputReceived = false;

      // wait until a button is pressed
      while (!inputReceived) {
        for (int j = 0; j < numOptions; j++) {
          int buttonState = digitalRead(buttonPins[j]);

          // check if button is pressed (LOW -> HIGH transition)
          if (buttonState == HIGH && prevButtonStates[j] == LOW) {
            inputReceived = true;
            currPlayerInput = j;

            // make corresponding LED visible
            ledStates[j] = HIGH;
            digitalWrite(ledPins[j], ledStates[j]);

            // debounce time
            wait(500); 

            // wait until the button is released
            while (digitalRead(buttonPins[j]) == HIGH) {}

            // turn off corresponding LED visible
            ledStates[j] = LOW;
            digitalWrite(ledPins[j], ledStates[j]);

            // update previous button state
            prevButtonStates[j] = LOW;
          } else {
            prevButtonStates[j] = buttonState;  // update state normally
          }
        }
      }

      // compare received input with correct sequence
      if (currPlayerInput != correctSequence[i]) { return false; }

    }
    return true;  // all inputs were correct
}

// flash all lights, indicating end of game
void flashLights() {
  
  for(int h = 0; h < 3; h++) {
    // turn on then off each LED
    for(int i = 0; i < numOptions; i++) {
      // turn on LED
      ledStates[i] = HIGH;
      digitalWrite(ledPins[i], ledStates[i]);
      wait(100); // stay on for 100 ms
      // turn off LED
      ledStates[i] = LOW;
      digitalWrite(ledPins[i], ledStates[i]);
    }

    wait(100);
  }
}
