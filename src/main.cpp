#include <Arduino.h>
#include <TFT_eSPI.h> 

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
    gDisplay.setTextSize(3);

    gDisplay.setCursor(10, 30, 2);
    gDisplay.setTextColor(TFT_SKYBLUE);
    gDisplay.println("Allo");

    gDisplay.setCursor(20, 60, 2);
    gDisplay.setTextColor(TFT_PINK);
    gDisplay.println("LUCIE");
}

void loop()
{
    // Serial.println("Hello!!");
}
