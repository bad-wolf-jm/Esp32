#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>

TFT_eSPI gDisplay;

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
}

void loop()
{
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

    // Serial.println("Hello!!");
}
