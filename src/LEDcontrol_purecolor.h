//
// Created by tamashi on 2022/5/17.
//

#ifndef RINACHANBOARD_LEDCONTROL_PURECOLOR_H
#define RINACHANBOARD_LEDCONTROL_PURECOLOR_H

#include "arduino.h"
#include <Adafruit_NeoPixel.h>;

extern std::string data;
extern Adafruit_NeoPixel Ledset1;
extern Adafruit_NeoPixel Ledset2;
extern Adafruit_NeoPixel Ledset3;
extern Adafruit_NeoPixel Ledset4;
extern Adafruit_NeoPixel Ledset5;

void LEDcontrol_purecolor();

#endif //RINACHANBOARD_LEDCONTROL_PURECOLOR_H
