//
// Created by 王晨 on 2022/5/17.
//

#include "LEDcontrol_RGBmode.h"

void LEDcontrol_RGBmode(){
    if(data[1] == 0){
        for(int i = 0; i < (data.size() - 2) / 4; i++){
            Ledset1.setPixelColor((4*i+2),(4*i+3),(4*i+4),(4*i+5));
        }
    }
    else if(data[1] == 1){
        for(int i = 0; i < (data.size() - 2) / 4; i++){
            Ledset2.setPixelColor((4*i+2),(4*i+3),(4*i+4),(4*i+5));
        }
    }
    else if(data[1] == 2){
        for(int i = 0; i < (data.size() - 2) / 4; i++){
            Ledset3.setPixelColor((4*i+2),(4*i+3),(4*i+4),(4*i+5));
        }
    }
    else if(data[1] == 3){
        for(int i = 0; i < (data.size() - 2) / 4; i++){
            Ledset4.setPixelColor((4*i+2),(4*i+3),(4*i+4),(4*i+5));
        }
    }
    else if(data[1] == 4){
        for(int i = 0; i < (data.size() - 2) / 4; i++){
            Ledset5.setPixelColor((4*i+2),(4*i+3),(4*i+4),(4*i+5));
        }
    }
    Ledset1.show;
    Ledset2.show;
    Ledset3.show;
    Ledset4.show;
    Ledset5.show;
}