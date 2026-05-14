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

// --- GAME STATE ---
int playerChoices[13] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}; 
int currentPlayer = 1;
bool gameStarted = false;
bool shiftActive = false; 
bool isIdle = true; 
bool isMuted = false; 
unsigned long lastInputTime = 0; // NEW: Timer to prevent auto-selection

// --- ANIMATION MASTER ---
unsigned long animSwitchDelay = 20000; 
unsigned long lastSwitchTime = 0;
unsigned long lastAnimUpdate = 0;
int activeAnimType = 0; 

// --- ANIMATION VARS ---
int sHead = 1, sBody = 0, sFood = 5; 
int scPos = 1; bool scForward = true;
int breatheVal = 0; bool breatheUp = true;
int kPos = 1; bool kForward = true;
int spiralCount = 0;
int pp1 = 1, pp2 = 7; bool pp1Dir = true, pp2Dir = true;

void setup() {
  Serial.begin(9600); 
  lcd.init();
  lcd.backlight();
  for (int i = 0; i < 4; i++) {
    pinMode(rows[i], OUTPUT);
    pinMode(cols[i], OUTPUT);
    digitalWrite(rows[i], HIGH);
    digitalWrite(cols[i], LOW);
  }
  pinMode(buzzer, OUTPUT);
  sFood = random(1, 13);
  showStatus();
}

void loop() {
  char key = keypad.getKey();
  if (Serial.available() > 0) key = Serial.read(); 

  if (key) {
    // Add a 300ms global debounce to prevent ghosting between column switches
    if (millis() - lastInputTime < 300) return; 
    lastInputTime = millis();

    if (isIdle) {
      if (key == 'N') {
        isMuted = !isMuted;
        if (!isMuted) tone(buzzer, 1200, 50); 
      }
      else if (key == 'S') {
        isIdle = false;
        shiftActive = false; // Reset shift state
        playTone(1000, 50);
        showStatus();
      }
    } else {
      playTone(1000, 50);
      handleInput(key);
    }
  }

  if (!gameStarted) {
    if (isIdle) {
      if (millis() - lastSwitchTime > animSwitchDelay) {
        activeAnimType++;
        if (activeAnimType > 5) activeAnimType = 0;
        lastSwitchTime = millis();
        resetAnimStates();
      }
      switch(activeAnimType) {
        case 0: runSnake(); break;
        case 1: runScanner(); break;
        case 2: runBreathing(); break;
        case 3: runKnightRider(); break;
        case 4: runSpiral(); break;
        case 5: runPingPong(); break;
      }
    } else {
      for (int p = 1; p <= 12; p++) {
        if (playerChoices[p] != -1) {
          if ((millis() / 300) % 2 == 0) spinFrame(playerChoices[p]);
        }
      }
    }
  }
}

void playTone(int freq, int duration) {
  if (!isMuted) tone(buzzer, freq, duration);
}

// --- ANIMATIONS ---
void resetAnimStates() { spiralCount = 0; breatheVal = 0; pp1 = 1; pp2 = 7; }
void runSnake() {
  if (millis() - lastAnimUpdate > 150) {
    lastAnimUpdate = millis();
    sBody = sHead; sHead++;
    if (sHead > 12) sHead = 1;
    playTone(1500, 5);
    if (sHead == sFood) { playTone(800, 30); sFood = random(1, 13); }
  }
  spinFrame(sHead);
  if ((millis() % 10) < 5) spinFrame(sBody); 
  if ((millis() % 20) < 5) spinFrame(sFood);
}
void runScanner() {
  if (millis() - lastAnimUpdate > 100) {
    lastAnimUpdate = millis();
    if (scForward) scPos++; else scPos--;
    if (scPos >= 12) scForward = false;
    if (scPos <= 1) scForward = true;
    playTone(200 + (scPos * 50), 10);
  }
  spinFrame(scPos);
}
void runBreathing() {
  if (millis() - lastAnimUpdate > 30) {
    lastAnimUpdate = millis();
    if (breatheUp) breatheVal++; else breatheVal--;
    if (breatheVal >= 15) breatheUp = false;
    if (breatheVal <= 1) breatheUp = true;
  }
  for (int i = 1; i <= 12; i++) if ((millis() % 16) < breatheVal) spinFrame(i);
}
void runKnightRider() {
  if (millis() - lastAnimUpdate > 80) {
    lastAnimUpdate = millis();
    if (kForward) kPos++; else kPos--;
    if (kPos >= 12) kForward = false;
    if (kPos <= 1) kForward = true;
    playTone(1000 + (kPos * 20), 5);
  }
  spinFrame(kPos);
  int trail = kForward ? kPos - 1 : kPos + 1;
  if (trail < 1) trail = 12; if (trail > 12) trail = 1;
  if ((millis() % 5) < 2) spinFrame(trail);
}
void runSpiral() {
  if (millis() - lastAnimUpdate > 200) {
    lastAnimUpdate = millis();
    spiralCount++;
    if (spiralCount > 13) spiralCount = 0;
    if (spiralCount < 13) playTone(400 + (spiralCount * 100), 20);
    else playTone(200, 100); 
  }
  for (int i = 1; i <= spiralCount; i++) spinFrame(i);
}
void runPingPong() {
  if (millis() - lastAnimUpdate > 120) {
    lastAnimUpdate = millis();
    if(pp1Dir) pp1++; else pp1--;
    if(pp1 >= 12) { pp1Dir = false; playTone(600, 10); }
    if(pp1 <= 1) { pp1Dir = true; playTone(600, 10); }
    if(pp2Dir) pp2++; else pp2--;
    if(pp2 >= 12) { pp2Dir = false; playTone(900, 10); }
    if(pp2 <= 1) { pp2Dir = true; playTone(900, 10); }
  }
  spinFrame(pp1);
  if ((millis() % 10) < 5) spinFrame(pp2);
}

// --- UPDATED INPUT LOGIC ---
void handleInput(char key) {
  if (key == 'S') {
    startSpin();
  }
  else if (key == 'N') { 
    shiftActive = false; 
    currentPlayer++; 
    if (currentPlayer > 12) currentPlayer = 1; 
    showStatus(); 
  }
  else if (key == '0') {
    if (!shiftActive) { 
      shiftActive = true; 
      lcd.clear(); 
      lcd.print("P"); lcd.print(currentPlayer); lcd.print(" Shift Mode");
      lcd.setCursor(0,1); 
      lcd.print("0=10 1=11 2=12");
    } else {
      saveChoice(10);
      shiftActive = false;
    }
  }
  else if (key >= '1' && key <= '9') {
    int val = key - '0';
    if (shiftActive) {
      if (val == 1) val = 11; 
      else if (val == 2) val = 12;
      shiftActive = false; 
    }
    saveChoice(val);
  }
}

void saveChoice(int val) {
    if (val > 12 || val < 1) return;
    bool taken = false;
    for(int i = 1; i <= 12; i++) if(playerChoices[i] == val && i != currentPlayer) taken = true;
    if(taken) { 
      lcd.clear(); lcd.print("Tile Taken!"); 
      playTone(200, 300); 
      delay(1000); 
    } 
    else { 
      playerChoices[currentPlayer] = val; 
    }
    showStatus();
}

void startSpin() {
  bool ready = false;
  for(int i=1; i<=12; i++) if(playerChoices[i] != -1) ready = true;
  if(!ready) return;
  gameStarted = true;
  lcd.clear(); lcd.print("Spinning...");
  int totalSteps = random(60, 100);
  int currentDelay = 10, finalPos = 0;
  for (int step = 0; step < totalSteps; step++) {
    finalPos = (step % 12) + 1; 
    unsigned long startMillis = millis();
    while (millis() - startMillis < currentDelay) spinFrame(finalPos);
    playTone(200 + (step * 2), 15);
    if (step > totalSteps - 15) currentDelay += 30;
  }
  
  int winner = -1;
  for (int p = 1; p <= 12; p++) if (playerChoices[p] == finalPos) winner = p;
  
  lcd.clear();
  if (winner != -1) { 
    lcd.print("Player "); lcd.print(winner); lcd.print(" WINS :D"); 
    lcd.setCursor(0, 1);
    lcd.print("Winning Tile: "); lcd.print(finalPos);
    victoryTune(); 
  } 
  else { 
    lcd.print("No one won :<"); 
    lcd.setCursor(0, 1);
    lcd.print("Winning Tile: "); lcd.print(finalPos);
    sadTune(); 
  }
  
  unsigned long flickerEnd = millis() + 5000;
  while(millis() < flickerEnd) {
    if ((millis() / 200) % 2 == 0) spinFrame(finalPos);
    else for(int i=0; i<4; i++) { digitalWrite(rows[i], HIGH); digitalWrite(cols[i], LOW); }
  }
  for (int i = 1; i <= 12; i++) playerChoices[i] = -1;
  currentPlayer = 1; gameStarted = false; isIdle = true; lastSwitchTime = millis(); showStatus();
}

void showStatus() {
  lcd.clear();
  if (isIdle) { 
    lcd.print("---BATTLEBYTE---"); 
    lcd.setCursor(0, 1); 
    lcd.print("Press S to PLAY!");
  }
  else {
    lcd.print("Player "); lcd.print(currentPlayer); 
    lcd.setCursor(0, 1);
    if (playerChoices[currentPlayer] == -1) lcd.print("Pick 1-12");
    else { lcd.print("Selected Tile "); lcd.print(playerChoices[currentPlayer]); }
  }
}

void victoryTune() { int m[]={523, 659, 784, 1046, 784, 1046}; for(int i=0; i<6; i++){ playTone(m[i],150); delay(200); } }
void sadTune() { playTone(150, 500); delay(600); playTone(100, 1000); }

void spinFrame(int position) {
  for(int i=0; i<4; i++) { digitalWrite(rows[i], HIGH); digitalWrite(cols[i], LOW); }
  int r = -1, c = -1;
  if (position == 12 || position == 0) { r = 0; c = 0; } 
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
    digitalWrite(rows[r], LOW); digitalWrite(cols[c], HIGH);
    delay(1); digitalWrite(rows[r], HIGH); digitalWrite(cols[c], LOW);
  }
}
