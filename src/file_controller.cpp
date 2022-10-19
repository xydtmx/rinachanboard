//
// Created by tamashi on 2022/10/19.
//

#include "file_controller.h"
#include "file_builder.h"

void file_controller(){
    if (!SD_MMC.begin())
    {
        Serial.print("card is not ready");
    }
    else{
        switch (data[1]){
            case 0:file_builder();
            break;
            case 1:file_deleter();
            break;
            case 2:file_reader();
            break;
        }
    }

}