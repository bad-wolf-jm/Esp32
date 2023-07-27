#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>

TFT_eSPI gDisplay;

const uint32_t BUTTON_1_PIN = 35;
const uint32_t BUTTON_2_PIN = 0;

const uint32_t BUTTON_PRESS_THRESHOLD = 30;

volatile uint32_t gButton1LastPress;
void IRAM_ATTR Button1Pressed()
{
    if (millis() - gButton1LastPress > BUTTON_PRESS_THRESHOLD)
    {
        gButton1LastPress = millis();
        Serial.println("Button 1 Pressed");
    }
}

volatile int gIndexToDisplay = -1;
volatile uint32_t gButton2LastPress;
void IRAM_ATTR Button2Pressed()
{
    if (millis() - gButton2LastPress > BUTTON_PRESS_THRESHOLD)
    {
        gButton2LastPress = millis();
        gIndexToDisplay = random(0, 16);
    }
}

const char *gPuzzleElements[] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16"};

void setup()
{
    Serial.begin(9600);
    Serial.println("");
    Serial.println("");
    Serial.println("==========================================");
    Serial.println("|                TEST                    |");
    Serial.println("==========================================");

    gDisplay.init();
    gDisplay.setRotation(1);
    gDisplay.fillScreen(TFT_BLUE);

    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);

    gButton1LastPress = gButton2LastPress = 0;
    attachInterrupt(BUTTON_1_PIN, Button1Pressed, FALLING);
    attachInterrupt(BUTTON_2_PIN, Button2Pressed, FALLING);
#if 0
    // gDisplay.setTextSize(3);

    // gDisplay.setCursor(10, 30, 2);
    // gDisplay.setTextColor(TFT_SKYBLUE);
    // gDisplay.println("Allo");

    // gDisplay.setCursor(20, 60, 2);
    // gDisplay.setTextColor(TFT_PINK);
    // gDisplay.println("LUCIE");

    WiFi.mode(WIFI_STA); // Set as station
    // WiFi.mode(WIFI_AP);
    // WiFi.mode(WIFI=_AP_STA);
    WiFi.disconnect(); // disconnect from network
    delay(100);

#endif
}

void loop()
{
    gDisplay.fillScreen(TFT_BLUE);
    gDisplay.setCursor(20, 40, 2);
    gDisplay.setTextColor(TFT_PINK);
    gDisplay.setTextSize(5);

    if (gIndexToDisplay != -1)
    {
        gDisplay.println(gPuzzleElements[gIndexToDisplay]);
    }

    delay(100);

#if 0
int n = WiFi.scanNetworks();

    if (n > 0)
    {
        char buffer[256];
        gDisplay.fillScreen(TFT_BLUE);
        sprintf(buffer, "There are %d networks", n);
        gDisplay.setCursor(20, 10, 2);
        gDisplay.setTextColor(TFT_PINK);
        gDisplay.println(buffer);

        for (uint32_t i = 0; i < n; i++)
        {
            char buffer[256];
            sprintf(buffer, "SSID: %s - Strength %d", WiFi.SSID(i), WiFi.RSSI(i));
            gDisplay.setCursor(20, 15 * (i + 2), 2);
            gDisplay.setTextColor(TFT_PINK);
            gDisplay.println(buffer);
        }
    }
#endif

    // Serial.println("Hello!!");
}
