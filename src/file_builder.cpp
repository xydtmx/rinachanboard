//
// Created by tamashi on 2022/10/19.
//

#include "file_builder.h"


void file_builder(){

    File file = SD_MMC.open(face_store_number,FILE_WRITE);
    if (file){
        for (int i = 0; i < data.length(); i++){
            file.write(data[i]);
        }
    }
    face_store_number[2]++;

}