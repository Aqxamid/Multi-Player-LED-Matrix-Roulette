#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int buzzer = 10;

const int rows[] = {2, 3, 4, 5}; 
const int cols[] = {6, 7, 8, 9};

const byte ROWS = 4; 
const byte COLS = 4; 

char keys[ROWS][COLS] = {
  {'1','7','4','X'}, 
  {'2','8','5','X'}, 
  {'3','9','6','X'}, 
  {'N','0','S','X'}  
};

byte rowPins[ROWS] = {11, 12, 13, A0}; 
byte colPins[COLS] = {A1, A2, A3, 1}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int playerChoices[13] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}; 
int currentPlayer = 1;
bool gameStarted = false;
bool shiftActive = false; 

void setup() {
  Serial.begin(9600); // Add this line for serial monitor
  lcd.init();
  lcd.backlight();
  for (int i = 0; i < 4; i++) {
    pinMode(rows[i], OUTPUT);
    pinMode(cols[i], OUTPUT);
    digitalWrite(rows[i], HIGH);
    digitalWrite(cols[i], LOW);
  }
  pinMode(buzzer, OUTPUT);
  showStatus();
}

void loop() {
  char key = keypad.getKey();

  // --- ADD THIS DEBUG BLOCK ---
  if (Serial.available() > 0) {
    key = Serial.read(); 
    Serial.print("Serial Input Received: ");
    Serial.println(key);
  }
  // ----------------------------

  if (key) {
    playTone(1000, 50);
    if (key == 'S') {
      startSpin();
    } 
    else if (key == 'N') {
      currentPlayer++;
      if (currentPlayer > 12) currentPlayer = 1;
      shiftActive = false; 
      showStatus();
    }
    else if (key == '0' && !shiftActive) {
      shiftActive = true;
      lcd.clear();
      lcd.print("Player "); lcd.print(currentPlayer);
      lcd.setCursor(0,1);
      lcd.print("Select 10,11,12"); 
    }
    else if (key >= '0' && key <= '9') {
      int selection = key - '0';
      if (shiftActive) {
        if (selection <= 2) selection += 10; 
        shiftActive = false;
      }

      bool alreadyTaken = false;
      for(int i = 1; i <= 12; i++) {
        if(playerChoices[i] == selection && i != currentPlayer) {
          alreadyTaken = true;
          break;
        }
      }

      if(alreadyTaken) {
        lcd.clear();
        lcd.print("Tile Taken!");
        playTone(200, 300);
        delay(1000);
        showStatus();
      } else {
        playerChoices[currentPlayer] = selection;
        showStatus();
      }
    }
  }

  if (!gameStarted) {
    for (int p = 1; p <= 12; p++) {
      if (playerChoices[p] != -1) {
        if ((millis() / 300) % 2 == 0) spinFrame(playerChoices[p]);
      }
    }
  }
}

void showStatus() {
  lcd.clear();
  lcd.print("Player "); lcd.print(currentPlayer);
  lcd.setCursor(0, 1);
  if (playerChoices[currentPlayer] == -1) {
    lcd.print("Pick tile 1-12");
  } else {
    lcd.print("Choice: "); lcd.print(playerChoices[currentPlayer]);
  }
}

void startSpin() {
  bool ready = false;
  for(int i=1; i<=12; i++) if(playerChoices[i] != -1) ready = true;
  if(!ready) return;

  gameStarted = true;
  shiftActive = false;
  lcd.clear();
  lcd.print("Spinning...");
  
  int totalSteps = random(60, 100);
  int currentDelay = 10;
  int finalPos = 0;

  for (int step = 0; step < totalSteps; step++) {
    finalPos = step % 13; // Changed to 13 to include the 12th LED
    unsigned long startMillis = millis();
    while (millis() - startMillis < currentDelay) {
      spinFrame(finalPos);
    }
    playTone(200 + (step * 2), 15);
    if (step > totalSteps - 15) currentDelay += 30;
  }

  lcd.clear();
  lcd.print("Result: "); lcd.print(finalPos);
  delay(1500);
  
  int winner = -1;
  for (int p = 1; p <= 12; p++) {
    if (playerChoices[p] == finalPos) {
      winner = p;
      break;
    }
  }

  lcd.clear();
  if (winner != -1) {
    lcd.print("Player "); lcd.print(winner); lcd.print(" WINS");
    lcd.setCursor(0, 1);
    lcd.print("CONGRATS! :D");
    victoryTune();
  } else {
    lcd.print("No one won :<");
    lcd.setCursor(0, 1);
    lcd.print("SAD D:");
    sadTune();
  }
  
  delay(5000);
  for (int i = 1; i <= 12; i++) playerChoices[i] = -1;
  currentPlayer = 1;
  gameStarted = false;
  showStatus();
}

void playTone(int freq, int duration) {
  tone(buzzer, freq, duration);
}

void victoryTune() {
  int melody[] = {523, 659, 784, 1046, 784, 1046};
  for (int i = 0; i < 6; i++) {
    tone(buzzer, melody[i], 150);
    delay(200);
  }
}

void sadTune() {
  tone(buzzer, 150, 500); delay(600);
  tone(buzzer, 100, 1000);
}

void spinFrame(int position) {
  for(int i=0; i<4; i++) { digitalWrite(rows[i], HIGH); digitalWrite(cols[i], LOW); }
  int r = -1, c = -1;

  // New Precise Mapping:
  // [12] [1] [2] [3]
  // [11] [ ] [ ] [4]
  // [10] [ ] [ ] [5]
  // [9]  [8]  [7] [6]

  if (position == 12 || position == 0) { r = 0; c = 0; } // Top Left
  else if (position == 1) { r = 0; c = 1; }
  else if (position == 2) { r = 0; c = 2; }
  else if (position == 3) { r = 0; c = 3; }
  else if (position == 4) { r = 1; c = 3; }
  else if (position == 5) { r = 2; c = 3; }
  else if (position == 6) { r = 3; c = 3; }
  else if (position == 7) { r = 3; c = 2; }
  else if (position == 8) { r = 3; c = 1; }
  else if (position == 9) { r = 3; c = 0; }
  else if (position == 10) { r = 2; c = 0; }
  else if (position == 11) { r = 1; c = 0; }

  if (r != -1 && c != -1) {
    digitalWrite(rows[r], LOW); 
    digitalWrite(cols[c], HIGH);
    delay(1);
    digitalWrite(rows[r], HIGH);
    digitalWrite(cols[c], LOW);
  }
}
