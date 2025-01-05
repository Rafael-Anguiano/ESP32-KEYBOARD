#include <Arduino.h>
#include <BleKeyboard.h>
#include <map>
#include <set>

#define ROW_1 15
#define ROW_2 2
#define ROW_3 4
#define ROW_4 16
#define COL_1 17
#define COL_2 5
#define COL_3 18
#define COL_4 19
#define COL_5 21
#define COL_6 3

BleKeyboard bleKeyboard;

int debounceTime = 110;

const int columns[] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6};
const int rows[] = {ROW_1, ROW_2, ROW_3, ROW_4};
const int rowsCount = 4;
const int colCount = 6;

std::map<uint8_t, long> lastTime;

// Keyboard default layout
uint8_t keys[rowsCount][colCount] = {
  {KEY_ESC, 'q', 'w', 'e', 'r', 't'},
  {KEY_TAB, 'a', 's', 'd', 'f', 'g'},
  {KEY_LEFT_SHIFT, 'z', 'x', 'c', 'v', 'b'},
  {'*', '*', '*', KEY_LEFT_CTRL, 'L', KEY_RETURN}
};

uint8_t layer1[rowsCount][colCount] = {
  {'1', '2', '3', '4', '5', '6'},
  {'`', '*', '*', '*', '*', '*'},
  {KEY_LEFT_SHIFT, '*', '*', '*', '*', '*'},
  {'*', '*', '*', KEY_LEFT_GUI, 'L', 'X'}
};

uint8_t layer2[rowsCount][colCount] = {
  {KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6},
  {'*', '*', '*', '*', '*', '*'},
  {'*', '*', '*', '*', '*', '*'},
  {'*', '*', '*', '*', 'L', 'X'},
};

// SPECIAL KEYS
bool specialKeysStatus[] = {false, false, false, false}; // CTRL, ALT, SHIFT, LAYER
std::set<uint8_t> specialKeys = {KEY_LEFT_CTRL, KEY_LEFT_ALT, KEY_LEFT_SHIFT, 'L', '*', 'X'};

void checkPriorityKeys () {
  digitalWrite(ROW_4, LOW);
  // Check Layer
  if (!digitalRead(COL_5) && !specialKeysStatus[3]) {
    specialKeysStatus[3] = true;
  } else {
    if (digitalRead(COL_5)) specialKeysStatus[3] = false;
  }

  // Check Ctrl
  if (!digitalRead(COL_4) && !specialKeysStatus[0]) {
    bleKeyboard.press(KEY_LEFT_CTRL);
    specialKeysStatus[0] = true;
  } else {
    if (digitalRead(COL_4) && specialKeysStatus[0]) bleKeyboard.release(KEY_LEFT_CTRL);
    specialKeysStatus[0] = false;
  }

  digitalWrite(ROW_4, HIGH); 
  
  // Check Shift
  digitalWrite(ROW_3, LOW);
  if (!digitalRead(COL_1) && !specialKeysStatus[2]) {
    bleKeyboard.press(KEY_LEFT_SHIFT);
    specialKeysStatus[2] = true;
  } else {
    if (digitalRead(COL_1) && specialKeysStatus[2]) bleKeyboard.release(KEY_LEFT_SHIFT);
    specialKeysStatus[2] = false;
  }
  digitalWrite(ROW_3, HIGH);
}

void setup() {
  // Serial 
  Serial.begin(115200);
  Serial.println("Left Keyboard Started");

  // Bluetooth
  bleKeyboard.setName("Left ESP32");
  bleKeyboard.begin();

  // Pin Mode
  for (int pin : rows) pinMode(pin, OUTPUT);
  for (int pin : columns) pinMode(pin, INPUT_PULLUP);
  // Pin Definition
  for (int pin : rows) digitalWrite(pin, HIGH);
}

void loop() {
  if(bleKeyboard.isConnected()) {
    checkPriorityKeys();
    for (int i=0; i<rowsCount; i++) {
      digitalWrite(rows[i], LOW);
      for (int j = 0; j < colCount; j++) {
        // Pressed too soon
        if (!specialKeysStatus[3] && millis() - lastTime[keys[i][j]] < debounceTime) {
          continue;
        } else if (!specialKeysStatus[3]) {
          lastTime.erase(keys[i][j]);
        }
        // Pressed too soon
        if (specialKeysStatus[3] && millis() - lastTime[layer1[i][j]] < debounceTime) {
          continue;
        } else if (specialKeysStatus[3]) {
          lastTime.erase(layer1[i][j]);
        }

        // Already checked
        if (specialKeys.count(keys[i][j])) continue;

        // Pressed
        if (!digitalRead(columns[j])) {
          if (!specialKeysStatus[3]) { 
            bleKeyboard.press(keys[i][j]);
            bleKeyboard.release(keys[i][j]);
            // Reset Time
            lastTime.erase(keys[i][j]);
            lastTime[keys[i][j]] = millis();
          } else {
            bleKeyboard.press(layer1[i][j]);
            bleKeyboard.release(layer1[i][j]);
            // Reset Time
            lastTime.erase(layer1[i][j]);
            lastTime[layer1[i][j]] = millis();
          }
        }
      }
      digitalWrite(rows[i], HIGH);
    }
  } 
}
