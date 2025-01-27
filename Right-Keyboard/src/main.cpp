#include <Arduino.h>
#include <BleKeyboard.h>
#include <map>
#include <set>
// Pin Definition
#define ROW_1 13
#define ROW_2 32
#define ROW_3 22
#define ROW_4 23
#define COL_1 33
#define COL_2 25
#define COL_3 26
#define COL_4 27
#define COL_5 14
#define COL_6 12
// Constants
const int debounceTime = 160;
const int columns[] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6};
const int rows[] = {ROW_1, ROW_2, ROW_3, ROW_4};
const int rowsCount = 4;
const int colCount = 6;
BleKeyboard bleKeyboard;

// SPECIAL KEYS
bool altPressed = false;
bool guiPressed = false;
bool layer1Pressed = false;
bool layer2Pressed = false;
std::set<uint8_t> specialKeys = {KEY_LEFT_ALT, 'L', '*', 'X', KEY_LEFT_GUI};

std::map<uint8_t, long> lastTime;

uint8_t keys[rowsCount][colCount] = {
    {'y', 'u', 'i', 'o', 'p', KEY_BACKSPACE},
    {'h', 'j', 'k', 'l', ';', '\''},
    {'n', 'm', ',', '.', '/', KEY_LEFT_ALT},
    {' ', 'L', 'X', '*', '*', '*'}};

uint8_t layer1[rowsCount][colCount] = {
    {'7', '8', '9', '0', '-', '='},
    {KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_UP_ARROW, KEY_RIGHT_ARROW, '*', '\\'},
    {'[', ']', KEY_HOME, KEY_END, KEY_PAGE_UP, KEY_PAGE_DOWN},
    {' ', 'L', KEY_LEFT_GUI, '*', '*', '*'}};

uint8_t layer2[rowsCount][colCount] = {
    {KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6},
    {KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12},
    {'*', '*', '*', '*', '*', '*'},
    {' ', 'L', 'X', '*', '*', '*'},
};

void checkPriorityKeys()
{
    digitalWrite(ROW_4, LOW);
    if (!digitalRead(COL_2) && !digitalRead(COL_3))
    {
        if (!guiPressed)
            bleKeyboard.press(KEY_LEFT_GUI);
        guiPressed = true;
    }
    else
    {
        if ((digitalRead(COL_2) || digitalRead(COL_3)) && guiPressed)
            bleKeyboard.release(KEY_LEFT_GUI);
        guiPressed = false;
    }

    // Check Layer 1
    if (!digitalRead(COL_2) && !layer1Pressed)
    {
        layer1Pressed = true;
    }
    else
    {
        if (digitalRead(COL_2))
            layer1Pressed = false;
    }
    // Check Layer 2
    if (!guiPressed && !digitalRead(COL_3) && !layer2Pressed)
    {
        layer2Pressed = true;
    }
    else
    {
        if (digitalRead(COL_3) && layer2Pressed)
            layer2Pressed = false;
    }
    digitalWrite(ROW_4, HIGH);

    // Check ALT
    digitalWrite(ROW_3, LOW);
    if (!digitalRead(COL_6) && !layer1Pressed && !layer2Pressed)
    {
        if (!altPressed)
            bleKeyboard.press(KEY_LEFT_ALT);
        altPressed = true;
    }
    else
    {
        if (digitalRead(COL_6) && altPressed)
            bleKeyboard.release(KEY_LEFT_ALT);
        altPressed = false;
    }
    digitalWrite(ROW_3, HIGH);
}

void setup()
{
    // Bluetooth
    bleKeyboard.setName("Patito");
    bleKeyboard.begin();

    // Pin Mode
    for (int pin : rows)
        pinMode(pin, OUTPUT);
    for (int pin : columns)
        pinMode(pin, INPUT_PULLUP);
    // Pin Definition
    for (int pin : rows)
        digitalWrite(pin, HIGH);
}

void loop()
{
    if (bleKeyboard.isConnected())
    {
        checkPriorityKeys();
        for (int i = 0; i < rowsCount; i++)
        {
            digitalWrite(rows[i], LOW);
            for (int j = 0; j < colCount; j++)
            {
                // Pressed too soon
                if (!layer1Pressed && !layer2Pressed && millis() - lastTime[keys[i][j]] < debounceTime)
                {
                    continue;
                }
                else if (!layer1Pressed && !layer2Pressed && millis() - lastTime[keys[i][j]] > debounceTime)
                {
                    lastTime.erase(keys[i][j]);
                }
                if (layer1Pressed && !layer2Pressed && millis() - lastTime[layer1[i][j]] < debounceTime)
                {
                    continue;
                }
                else if (layer1Pressed && !layer2Pressed && millis() - lastTime[layer1[i][j]] > debounceTime)
                {
                    lastTime.erase(layer1[i][j]);
                }
                if (!layer1Pressed && layer2Pressed && millis() - lastTime[layer2[i][j]] < debounceTime)
                {
                    continue;
                }
                else if (!layer1Pressed && layer2Pressed && millis() - lastTime[layer2[i][j]] > debounceTime)
                {
                    lastTime.erase(layer2[i][j]);
                }

                // Already checked
                if (!layer1Pressed && !layer2Pressed && specialKeys.count(keys[i][j]))
                    continue;
                if (layer1Pressed && !layer2Pressed && specialKeys.count(layer1[i][j]))
                    continue;
                if (!layer1Pressed && layer2Pressed && specialKeys.count(layer2[i][j]))
                    continue;

                // Pressed
                if (!digitalRead(columns[j]))
                {
                    if (layer1Pressed && !layer2Pressed)
                    {
                        bleKeyboard.press(layer1[i][j]);
                        bleKeyboard.release(layer1[i][j]);
                        // Reset Time
                        lastTime.erase(layer1[i][j]);
                        lastTime[layer1[i][j]] = millis();
                    }
                    else if (!layer1Pressed && layer2Pressed)
                    {
                        bleKeyboard.press(layer2[i][j]);
                        bleKeyboard.release(layer2[i][j]);
                        // Reset Time
                        lastTime.erase(layer2[i][j]);
                        lastTime[layer2[i][j]] = millis();
                    }
                    else
                    {
                        bleKeyboard.press(keys[i][j]);
                        bleKeyboard.release(keys[i][j]);
                        // Reset Time
                        lastTime.erase(keys[i][j]);
                        lastTime[keys[i][j]] = millis();
                    }
                }
            }
            digitalWrite(rows[i], HIGH);
        }
    }
}
