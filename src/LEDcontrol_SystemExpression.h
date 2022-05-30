//
// Created by tamashi on 2022/5/18.
//

#ifndef RINACHANBOARD_LEDCONTROL_SYSTEMEXPRESSION_H
#define RINACHANBOARD_LEDCONTROL_SYSTEMEXPRESSION_H
#include <arduino.h>;
#include <Adafruit_NeoPixel.h>;


extern std::string data;
extern Adafruit_NeoPixel Ledset1;
extern Adafruit_NeoPixel Ledset2;
extern Adafruit_NeoPixel Ledset3;
extern Adafruit_NeoPixel Ledset4;
extern Adafruit_NeoPixel Ledset5;

void SystemExpression();

#endif //RINACHANBOARD_LEDCONTROL_SYSTEMEXPRESSION_H
