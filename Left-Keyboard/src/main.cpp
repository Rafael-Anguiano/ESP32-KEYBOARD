#include <Arduino.h>
#include <BleKeyboard.h>
#include <map>
#include <set>
// Pin Definition
#define ROW_1 4
#define ROW_2 3
#define ROW_3 22
#define ROW_4 23
#define COL_1 16
#define COL_2 17
#define COL_3 5
#define COL_4 18
#define COL_5 19
#define COL_6 21
// Constants
const int debounceTime = 160;
const int columns[] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6};
const int rows[] = {ROW_1, ROW_2, ROW_3, ROW_4};
const int rowsCount = 4;
const int colCount = 6;
BleKeyboard bleKeyboard;

// SPECIAL KEYS
bool ctrlPressed = false;
bool shiftPressed = false;
bool layerPressed = false;
bool guiPressed = false;
std::set<uint8_t> specialKeys = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, 'L', '*', 'X', KEY_LEFT_GUI};

std::map<uint8_t, long> lastTime;

// Keyboard default layout
uint8_t keys[rowsCount][colCount] = {
    {KEY_ESC, 'q', 'w', 'e', 'r', 't'},
    {KEY_TAB, 'a', 's', 'd', 'f', 'g'},
    {KEY_LEFT_SHIFT, 'z', 'x', 'c', 'v', 'b'},
    {'*', '*', '*', KEY_LEFT_CTRL, 'L', KEY_RETURN}};

uint8_t layer1[rowsCount][colCount] = {
    {'1', '2', '3', '4', '5', '6'},
    {'`', '*', '*', '*', '*', '*'},
    {KEY_LEFT_SHIFT, '*', '*', '*', '*', '*'},
    {'*', '*', '*', KEY_LEFT_GUI, 'L', KEY_RETURN}};

void checkPriorityKeys()
{
    digitalWrite(ROW_4, LOW);

    // Check layer and GUI
    if (!digitalRead(COL_5) && !digitalRead(COL_4))
    {
        bleKeyboard.press(KEY_LEFT_GUI);
        guiPressed = true;
    }
    else
    {
        if ((digitalRead(COL_5) || digitalRead(COL_4)) && guiPressed)
            bleKeyboard.release(KEY_LEFT_GUI);
        guiPressed = false;
    }

    // Check Layer
    if (!digitalRead(COL_5) && !layerPressed)
    {
        layerPressed = true;
    }
    else
    {
        if (digitalRead(COL_5))
            layerPressed = false;
    }

    // Check Ctrl
    if (!digitalRead(COL_4) && !ctrlPressed && !guiPressed)
    {
        bleKeyboard.press(KEY_LEFT_CTRL);
        ctrlPressed = true;
    }
    else
    {
        if (digitalRead(COL_4) && ctrlPressed)
            bleKeyboard.release(KEY_LEFT_CTRL);
        ctrlPressed = false;
    }

    digitalWrite(ROW_4, HIGH);

    // Check Shift
    digitalWrite(ROW_3, LOW);
    if (!digitalRead(COL_1) && !shiftPressed)
    {
        bleKeyboard.press(KEY_LEFT_SHIFT);
        shiftPressed = true;
    }
    else
    {
        if (digitalRead(COL_1) && shiftPressed)
            bleKeyboard.release(KEY_LEFT_SHIFT);
        shiftPressed = false;
    }
    digitalWrite(ROW_3, HIGH);
}

void setup()
{
    // Bluetooth
    bleKeyboard.setName("Sumito");
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
                if (!layerPressed && millis() - lastTime[keys[i][j]] < debounceTime)
                {
                    continue;
                }
                else if (!layerPressed)
                {
                    lastTime.erase(keys[i][j]);
                }
                // Pressed too soon
                if (layerPressed && millis() - lastTime[layer1[i][j]] < debounceTime)
                {
                    continue;
                }
                else if (layerPressed)
                {
                    lastTime.erase(layer1[i][j]);
                }

                // Already checked
                if (!layerPressed && specialKeys.count(keys[i][j]))
                    continue;
                if (layerPressed && specialKeys.count(layer1[i][j]))
                    continue;

                // Pressed
                if (!digitalRead(columns[j]))
                {
                    if (layerPressed)
                    {
                        bleKeyboard.press(layer1[i][j]);
                        bleKeyboard.release(layer1[i][j]);
                        // Reset Time
                        lastTime.erase(layer1[i][j]);
                        lastTime[layer1[i][j]] = millis();
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
