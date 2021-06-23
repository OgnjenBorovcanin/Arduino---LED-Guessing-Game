
const int SW_pin = 4;
const int X_pin = A0;
const int Y_pin = A1;

const int leftLed = 7;
const int botLed = 8;
const int upperLed = 9;
const int rightLed = 10;
const int greenLed = 11;
const int redLed = 12;

#define BTN_PIN 2
#define DEBOUNCE_LIMIT 200
volatile int last_interrupt_time = 0;

int xValue;
int yValue;


String strUlaz;

int gameArray[50];
int gameCounter = 4; // global counter for gameArray
boolean gameStarted = false;
boolean nextLvl = false;

int guessArrayX[50];
int guessArrayY[50];
int guessArrayLed[50];  //this array is based on guessArrayX and guessArrayY, values 1, 2, 3, 4 (LED number)
int guessCounter = 0;

boolean checkPatternMatch = false;
boolean gameOver = false;

int score = 0;
int highscore = 0;

void setup() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), btn_isr, RISING);

  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);

  pinMode(leftLed, OUTPUT);
  pinMode(botLed, OUTPUT);
  pinMode(upperLed, OUTPUT);
  pinMode(rightLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);

  Serial.println("--- Welcome to LED mini game ---");
  Serial.println("Instructions: "); 
  Serial.println("Type 'start' to start playing."); 
  Serial.println("Type 'reset' to guess again."); 
  Serial.println("");
  
}

void loop() {
  
  //game start or reset guessing
  while (Serial.available()) {
    strUlaz = Serial.readString();

    if (strUlaz == "start\n") {
      gameStarted = true;
    }
    if (strUlaz == "reset\n") { //reset guessing
      Serial.println("Guess again clean");
      resetGuessing();
    }
  }

  if(gameStarted){
    digitalWrite(redLed, LOW);
    Serial.println("Look at the LEDs!");
    Serial.println("");
    score = 0;
    resetGuessing();
    gameCounter = 4;  //only when new game starts
    randomizeLeds();
    lightUpTaskLeds();
    gameStarted = false;
  }

  if (nextLvl) {
    resetGuessing();
    randomizeLeds();
    lightUpTaskLeds();
    nextLvl = false;
  }

  if(checkPatternMatch){
    //fidning LED number for guessArrayLed[] based on guessArrayX and guessArrayY arrays
    Serial.print("Your LED pattern:     ");
    for(int p1 = 0; p1 < guessCounter; p1++){
      if(guessArrayX[p1] > 800){
        guessArrayLed[p1] = 1;
      }
      if(guessArrayX[p1] < 200){
        guessArrayLed[p1] = 3; 
      }
      if(guessArrayY[p1] > 800){
        guessArrayLed[p1] = 2;
      }
      if(guessArrayY[p1] < 200){
        guessArrayLed[p1] = 4;
      }
      Serial.print(guessArrayLed[p1]);
      Serial.print(" ");
      checkPatternMatch = false;
    }
    Serial.println("");

    
    //check if matching
    for(int p2 = 0; p2 < gameCounter; p2++){
      if(guessArrayLed[p2] != gameArray[p2]){
        gameOver = true;
      }
    }

    if(gameOver){ //wrong pattern
      digitalWrite(redLed, HIGH);
      Serial.print("Solution LED pattern: ");
      for(int p3 = 0; p3 < gameCounter; p3++){
        Serial.print(gameArray[p3]);
        Serial.print(" ");
      }
      Serial.println("");
      Serial.println("--- GAME OVER ---");
      Serial.print("Your score: ");
      Serial.println(score);
      if(score > highscore){
        highscore = score;
        Serial.println("New highscore!");
      } else {
        Serial.print("Highscore: ");
        Serial.println(highscore);
      }
      Serial.println("Type 'start' for a new game.");
      gameOver = false;
    } else {  //next level
      Serial.println("Correct! - Next level");
      Serial.println("");
      gameArray[gameCounter++] = random(1, 5);
      //clean Leds
      turnOffLeds();
      //these two are already in nextLvl if
      //resetGuessing();
      //randomizeLeds(); //randomize leds
      nextLvl = true;
      score++;
      digitalWrite(greenLed, HIGH);
      delay(2000); //LED stays on 2 sec
      digitalWrite(greenLed, LOW);
    }
  }


  xValue = analogRead(X_pin);
  yValue = analogRead(Y_pin);

//      Serial.print("X axis:");
//    Serial.println(xValue);
//    Serial.print("Y axis:");
//    Serial.println(yValue);
//    delay(2000);

  if (xValue > 800) {
    digitalWrite(leftLed, HIGH);
  } else {
    digitalWrite(leftLed, LOW);
  }

  if (xValue < 200) {
    digitalWrite(rightLed, HIGH);
  } else {
    digitalWrite(rightLed, LOW);
  }

  if (yValue < 200) {
    digitalWrite(botLed, HIGH);
  } else {
    digitalWrite(botLed, LOW);
  }

  if (yValue > 800) {
    digitalWrite(upperLed, HIGH);
  } else {
    digitalWrite(upperLed, LOW);
  }

  //test Joystick button
//  int joyButtonState = digitalRead(SW_pin);
//  if (!joyButtonState) {
//    digitalWrite(greenLed, HIGH);
//    digitalWrite(redLed, HIGH);
//  } else {
//    digitalWrite(greenLed, LOW);
//    digitalWrite(redLed, LOW);
//  }

}

void randomizeLeds(){
  for (int i = 0; i < gameCounter; i++) {
    gameArray[i] = random(1, 5); //leftLed 1, upperLed - 2, rightLed - 3, botLed - 4
  }
}

void resetGuessing(){ //clean guess arrays and guessCounter
  for(int i = 0; i < gameCounter; i++){
    guessArrayX[i] = 0;
    guessArrayY[i] = 0;
    guessArrayLed[i] = 0;
  }
  guessCounter = 0;
}


void lightUpTaskLeds() {
  //clean Leds
  turnOffLeds();

  delay(2000);  //2 sec for player to prepare
  
  int i = 0;
  for(i = 0; i < gameCounter; i++){
    switch (gameArray[i]) {
      case 1:
        digitalWrite(leftLed, HIGH);
        delay(1000);
        digitalWrite(leftLed, LOW);
        delay(200);
        break;
      case 2:
        digitalWrite(upperLed, HIGH);
        delay(1000);
        digitalWrite(upperLed, LOW);
        delay(200);
        break;
      case 3:
        digitalWrite(rightLed, HIGH);
        delay(1000);
        digitalWrite(rightLed, LOW);
        delay(200);
        break;
      case 4:
        digitalWrite(botLed, HIGH);
        delay(1000);
        digitalWrite(botLed, LOW);
        delay(200);
        break;
      default:
        break;
    }
  }
}

void turnOffLeds() {
  digitalWrite(leftLed, LOW);
  digitalWrite(upperLed, LOW);
  digitalWrite(rightLed, LOW);
  digitalWrite(botLed, LOW);
}


void btn_isr() {
  //debounce
  int interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > DEBOUNCE_LIMIT) {
    //button logic
    //Serial.println("Button clicked.");
    guessArrayX[guessCounter] = xValue; //analogRead(X_pin);
    guessArrayY[guessCounter] = yValue; //analogRead(Y_pin);
//    Serial.print("X axis:");
//    Serial.println(guessArrayX[guessCounter]);
//    Serial.print("Y axis:");
//    Serial.println(guessArrayY[guessCounter]);
    guessCounter++;
    if(guessCounter == gameCounter){
      checkPatternMatch = true;
    }
  }

  last_interrupt_time = interrupt_time;
}
