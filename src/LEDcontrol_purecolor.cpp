//
// Created by tamashi on 2022/5/17.
//

#include "LEDcontrol_purecolor.h"



void LEDcontrol_purecolor(){
    if(data[1] == 0) {
        for (int i = 0; i < data.length() - 4; i++) {
            Ledset1.setPixelColor(data[i+5],data[2],data[3],data[4]);
            Serial.println("purecolor_RGB_set1");
        }
    }
    else if(data[1] == 1) {
        for (int i = 0; i < data.length() - 4; i++) {
            Ledset2.setPixelColor(data[i+5],data[2],data[3],data[4]);
            Serial.println("purecolor_RGB_set2");
        }
    }
    else if (data[1] == 2) {
        for (int i = 0; i < data.length()- 4; i++) {
            Ledset3.setPixelColor(data[i+4],data[2],data[3],data[4]);
            Serial.println("purecolor_RGB_set3");
        }
    }
    else if (data[1] == 3) {
        for (int i = 0; i < data.length() - 4; i++) {
            Ledset4.setPixelColor(data[i+4],data[2],data[3],data[4]);
            Serial.println("purecolor_RGB_set4");
        }
    }
    else if (data[1] == 4) {
        for (int i = 0; i < data.length() - 4; i++){
            Ledset5.setPixelColor(data[i+4],data[2],data[3],data[4]);
            Serial.println("purecolor_RGB_set5");
        }
    }

    Ledset1.show();
    Ledset2.show();
    Ledset3.show();
    Ledset4.show();
    Ledset5.show();

    Serial.println("out_purecolor");
}