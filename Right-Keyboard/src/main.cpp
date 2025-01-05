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
long prevTime = 0;

const int columns[] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6};
const int rows[] = {ROW_1, ROW_2, ROW_3, ROW_4};
const int rowsCount = 4;
const int colCount = 6;

std::map<uint8_t, long> lastTime;

uint8_t keys[rowsCount][colCount] = {
  {'y', 'u', 'i', 'o', 'p', KEY_BACKSPACE},
  {'h', 'j', 'k', 'l', ';', '\''},
  {'n', 'm', ',', '.', '/', KEY_LEFT_ALT},
  {' ', 'L', 'X', '*', '*', '*'}
};

uint8_t layer1[rowsCount][colCount] = {
  {'7', '8', '9', '0', '-', '='},
  {KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_UP_ARROW, KEY_RIGHT_ARROW, '*', '\\'},
  {KEY_HOME, KEY_END, KEY_PAGE_UP, KEY_PAGE_DOWN, '*', '*'},
  {' ', 'L',  KEY_LEFT_GUI, '*', '*', '*'}
};


uint8_t layer2[rowsCount][colCount] = {
  {KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12},
  {'*', KEY_MEDIA_VOLUME_DOWN, KEY_MEDIA_VOLUME_UP, KEY_MEDIA_MUTE, '*', '*', '*'},
  {'*', '*', '*', '*', '*', '*'},
  {' ', 'L', 'X', '*', '*', '*'},
};

// SPECIAL KEYS
bool specialKeysStatus[] = { false, false}; // LAYER, LAYER2
std::set<uint8_t> specialKeys = {'L', '*', 'X'};

void checkPriorityKeys () {
  digitalWrite(ROW_4, LOW);
  // Check Layer
  if (!digitalRead(COL_2) && !specialKeysStatus[0]) {
    specialKeysStatus[0] = true;
  } else {
    if (digitalRead(COL_2)) specialKeysStatus[0] = false;
  }
  digitalWrite(ROW_4, HIGH);
} 

void setup() {
  // Serial 
  Serial.begin(115200);
  Serial.println("Right Keyboard Started");

  // Bluetooth
  bleKeyboard.setName("Right ESP32");
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
        if (!specialKeysStatus[0] && millis() - lastTime[keys[i][j]] < debounceTime) {
          continue;
        } else if (!specialKeysStatus[0]) {
          lastTime.erase(keys[i][j]);
        }
        // Pressed too soon
        if (specialKeysStatus[0] && millis() - lastTime[layer1[i][j]] < debounceTime) {
          continue;
        } else if (specialKeysStatus[0]) {
          lastTime.erase(layer1[i][j]);
        }

        // Already checked
        if (specialKeys.count(keys[i][j])) continue;

        // Pressed
        if (!digitalRead(columns[j])) {
          if (!specialKeysStatus[0]) { 
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








