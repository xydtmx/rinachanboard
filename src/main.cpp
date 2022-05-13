#include <Adafruit_NeoPixel.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <Preferences.h>
//#include <BLEUtils.h>

#include <atomic>
#include <bitset>


#include<stdio.h>
#include<stdbool.h>

typedef unsigned short u16;

const u16 crc16_table[256] = {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

unsigned char char_invert(unsigned char ch)
{
    char i;
    unsigned char invert = 0;

    for (i = 0; i < 8; i++) {
        if (ch & 0x1)
            invert |= 0x01 << (7-i);
        ch >>= 1;
    }

    return invert;
}

u16 short_invert(u16 sh)
{
    char i;
    u16 invert = 0;

    for (i = 0; i < 16; i++) {
        if (sh & 0x1)
            invert |= 0x01 << (15-i);
        sh >>= 1;
    }

    return invert;
}

u16 crc16(u16 crc, u16 poly,  u16 output_xor, unsigned char *buf, u16 len)
{
    unsigned char i;

    while (len--) {

        for (i = 0; i < 8; i++) {
            if (crc & 0x8000)
                crc = (crc << 1) ^poly;
            else
                crc <<= 1;
        }

    }

    return crc^output_xor;
}


#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define PIN 2;//Ledset1
#define PIN 4;//ledset2
#define PIN 5;//ledset3
#define PIN 18;//ledset4
#define PIN 19;//ledset5

using namespace std;

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
volatile bool deviceConnected = false;
uint32_t value = 0;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer)override {
        deviceConnected = true;            //如果有设备连接
//        BLEDevice::startAdvertising();     //开始广播
        Serial.print("mmm");
    };

    void onDisconnect(BLEServer* pServer)override {    //如果无设备
        deviceConnected = false;
        Serial.print("nnn");
    }
};

Adafruit_NeoPixel Ledset1(88, 2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel Ledset2(84, 4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel Ledset3(78, 5, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel Ledset4(57, 18, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel Ledset5(49, 19, NEO_GRB + NEO_KHZ800);

atomic<bool> updated(false);
bitset<8> functionCommand, boardSelect, bitSelect, red, green, blue;

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) override {
        auto value = pCharacteristic->getValue();
        Serial.print("Raw data length: ");
        Serial.println(value.length());
        if (value.length() != 6) {
            return;
        }
        functionCommand = bitset<8>(value[0]);
        boardSelect = bitset<8>(value[1]);
        bitSelect = bitset<8>(value[2]);
        red = bitset<8>(value[3]);
        green = bitset<8>(value[4]);
        blue = bitset<8>(value[5]);
        updated = true;
    }
};

void setup() {
    Serial.begin(115200);
    Ledset1.begin();
    Ledset1.show(); // Initialize all pixels to 'off'

    BLEDevice::init("RinachanBoard");
    BLEServer *pServer = BLEDevice::createServer();    //创建Server
    pServer->setCallbacks(new MyServerCallbacks());    //设置匿名回调函数（实例化MyServerCallbacks）

    BLEService *pService = pServer->createService(SERVICE_UUID);

    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );

    pCharacteristic->setCallbacks(new MyCallbacks());

    pCharacteristic->setValue("Hello World");
    pService->start();//服务启动

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();//开始广播
    Serial.println("等待一个客户端连接至notify...");


    for (int i = 0; i <= 88; i++) {  //重置面板
            Ledset1.setPixelColor(i, Adafruit_NeoPixel::Color(0,0,0));;
            Ledset1.show();
    }
    for (int i = 0; i <= 84; i++) {
        Ledset2.setPixelColor(i, Adafruit_NeoPixel::Color(0,0,0));;
        Ledset2.show();
    }
    for (int i = 0; i <= 78; i++) {
        Ledset3.setPixelColor(i, Adafruit_NeoPixel::Color(0,0,0));;
        Ledset3.show();
    }
    for (int i = 0; i <= 57; i++) {
        Ledset4.setPixelColor(i, Adafruit_NeoPixel::Color(0,0,0));;
        Ledset4.show();
    }
    for (int i = 0; i <= 49; i++) {
        Ledset5.setPixelColor(i, Adafruit_NeoPixel::Color(0,0,0));;
        Ledset5.show();
    }
}

void loop() {


    if (!deviceConnected) {  //如果断联
        pServer->startAdvertising(); // 重新广播
        Serial.println("start advertising");
        delay(2000);
    }

    if (updated) {
        Serial.print("Function command: ");
        Serial.println(functionCommand.to_string().c_str());
        Serial.print("Board select: ");
        Serial.println(boardSelect.to_string().c_str());
        Serial.print("Bit select: ");
        Serial.println(bitSelect.to_string().c_str());

        Serial.print("RGB: (");
        Serial.print(red.to_ulong());
        Serial.print(", ");
        Serial.print(green.to_ulong());
        Serial.print(", ");
        Serial.print(blue.to_ulong());
        Serial.println(")");

        if (functionCommand[7] == 0 && functionCommand[6] == 0) {//直接控制部分
            if (functionCommand[5] == 0) {//上传模式
                if (boardSelect.to_ulong() == 0) {
                    Ledset1.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
                    Serial.print("in1");
                } else if (boardSelect.to_ulong() == 1) {
                    Ledset2.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
                    Serial.print("in2");

                } else if (boardSelect.to_ulong() == 2) {
                    Ledset3.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
                    Serial.print("in3");

                } else if (boardSelect.to_ulong() == 3) {
                    Ledset4.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
                    Serial.print("in4");

                } else if (boardSelect.to_ulong() == 4) {
                    Ledset5.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
                    Serial.print("in5");

                }
            }
            else if(functionCommand[5] == 1){//输出模式
                    if (boardSelect.to_ulong() == 0) {
                        Ledset1.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
                        Serial.print("out1");

                    } else if (boardSelect.to_ulong() == 1) {
                        Ledset2.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
                        Serial.print("out2");

                    } else if (boardSelect.to_ulong() == 2) {
                        Ledset3.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
                        Serial.print("out3");

                    } else if (boardSelect.to_ulong() == 3) {
                        Ledset4.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
                        Serial.print("out4");

                    } else if (boardSelect.to_ulong() == 4) {
                        Ledset5.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
                        Serial.print("out5");

                    }
                Ledset1.show();
                Ledset2.show();
                Ledset3.show();
                Ledset4.show();
                Ledset5.show();
//                Serial.println("out");
            }
        }
        else if (functionCommand[7] == 1 && functionCommand[6] == 1) {//系统表情模式
                if (functionCommand[5]==0&&functionCommand[4]==0&&functionCommand[3]==0){
                    for (int i = 0; i <88;i++){
                        Ledset1.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <84;i++){
                        Ledset2.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <78;i++){
                        Ledset3.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <57;i++){
                        Ledset4.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <49;i++){
                        Ledset5.setPixelColor(i,0,0,0);
                    }
                    Ledset1.setPixelColor(50,15,0,10);
                    Ledset1.setPixelColor(51,15,0,10);
                    Ledset1.setPixelColor(70,15,0,10);
                    Ledset1.setPixelColor(71,15,0,10);
                    Ledset1.setPixelColor(81,15,0,10);
                    Ledset1.setPixelColor(82,15,0,10);
                    Ledset2.setPixelColor(3,15,0,10);
                    Ledset2.setPixelColor(4,15,0,10);
                    Ledset2.setPixelColor(13,15,0,10);
                    Ledset2.setPixelColor(26,15,0,10);
                    Ledset2.setPixelColor(27,15,0,10);
                    Ledset2.setPixelColor(37,15,0,10);
                    Ledset2.setPixelColor(38,15,0,10);
                    Ledset2.setPixelColor(45,15,0,10);
                    Ledset2.setPixelColor(46,15,0,10);
                    Ledset2.setPixelColor(58,15,0,10);
                    Ledset2.setPixelColor(59,15,0,10);
                    Ledset3.setPixelColor(22,15,0,10);
                    Ledset3.setPixelColor(24,15,0,10);
                    Ledset3.setPixelColor(36,15,0,10);
                    Ledset3.setPixelColor(38,15,0,10);
                    Ledset4.setPixelColor(5,15,0,10);
                    Ledset4.setPixelColor(13,15,0,10);
                    Ledset4.setPixelColor(25,15,0,10);
                    Ledset4.setPixelColor(31,15,0,10);
                    Ledset4.setPixelColor(45,15,0,10);
                    Ledset4.setPixelColor(46,15,0,10);
                    Ledset4.setPixelColor(47,15,0,10);
                    Ledset4.setPixelColor(48,15,0,10);
                    Ledset4.setPixelColor(49,15,0,10);

                    Serial.println("default1");
                }
                else if (functionCommand[5]==0&&functionCommand[4]==0&&functionCommand[3]==1){
                    for (int i = 0; i <88;i++){
                        Ledset1.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <84;i++){
                        Ledset2.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <78;i++){
                        Ledset3.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <57;i++){
                        Ledset4.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <49;i++){
                        Ledset5.setPixelColor(i,0,0,0);
                    }
                    Ledset1.setPixelColor(70,15,0,10);
                    Ledset1.setPixelColor(71,15,0,10);
                    Ledset1.setPixelColor(83,15,0,10);
                    Ledset1.setPixelColor(82,15,0,10);
                    Ledset2.setPixelColor(6,15,0,10);
                    Ledset2.setPixelColor(5,15,0,10);
                    Ledset2.setPixelColor(16,15,0,10);
                    Ledset2.setPixelColor(15,15,0,10);
                    Ledset2.setPixelColor(26,15,0,10);
                    Ledset2.setPixelColor(25,15,0,10);
                    Ledset2.setPixelColor(34,15,0,10);
                    Ledset2.setPixelColor(48,15,0,10);
                    Ledset2.setPixelColor(47,15,0,10);
                    Ledset2.setPixelColor(58,15,0,10);
                    Ledset2.setPixelColor(57,15,0,10);
                    Ledset2.setPixelColor(68,15,0,10);
                    Ledset2.setPixelColor(67,15,0,10);
                    Ledset2.setPixelColor(80,15,0,10);
                    Ledset2.setPixelColor(79,15,0,10);
                    Ledset3.setPixelColor(21,15,0,10);
                    Ledset3.setPixelColor(23,15,0,10);
                    Ledset3.setPixelColor(37,15,0,10);
                    Ledset3.setPixelColor(39,15,0,10);
                    Ledset3.setPixelColor(65,15,0,10);
                    Ledset3.setPixelColor(66,15,0,10);
                    Ledset3.setPixelColor(67,15,0,10);
                    Ledset3.setPixelColor(68,15,0,10);
                    Ledset3.setPixelColor(69,15,0,10);
                    Ledset3.setPixelColor(70,15,0,10);
                    Ledset3.setPixelColor(71,15,0,10);
                    Ledset3.setPixelColor(72,15,0,10);
                    Ledset3.setPixelColor(64,15,0,10);
                    Ledset4.setPixelColor(5,15,0,10);
                    Ledset4.setPixelColor(13,15,0,10);
                    Ledset4.setPixelColor(25,15,0,10);
                    Ledset4.setPixelColor(31,15,0,10);
                    Ledset4.setPixelColor(45,15,0,10);
                    Ledset4.setPixelColor(49,15,0,10);
                    Ledset5.setPixelColor(9,15,0,10);
                    Ledset5.setPixelColor(10,15,0,10);
                    Ledset5.setPixelColor(8,15,0,10);

                    Serial.println("default2");

                }
                else if (functionCommand[5]==0&&functionCommand[4]==1&&functionCommand[3]==0){
                    for (int i = 0; i <88;i++){
                        Ledset1.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <84;i++){
                        Ledset2.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <78;i++){
                        Ledset3.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <57;i++){
                        Ledset4.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <49;i++){
                        Ledset5.setPixelColor(i,0,0,0);
                    }
                    Ledset1.setPixelColor(70,15,0,10);
                    Ledset1.setPixelColor(71,15,0,10);
                    Ledset1.setPixelColor(83,15,0,10);
                    Ledset1.setPixelColor(84,15,0,10);
                    Ledset2.setPixelColor(6,15,0,10);
                    Ledset2.setPixelColor(5,15,0,10);
                    Ledset2.setPixelColor(14,15,0,10);
                    Ledset2.setPixelColor(15,15,0,10);
                    Ledset2.setPixelColor(28,15,0,10);
                    Ledset2.setPixelColor(34,15,0,10);
                    Ledset2.setPixelColor(48,15,0,10);
                    Ledset2.setPixelColor(47,15,0,10);
                    Ledset2.setPixelColor(56,15,0,10);
                    Ledset2.setPixelColor(57,15,0,10);
                    Ledset2.setPixelColor(66,15,0,10);
                    Ledset2.setPixelColor(67,15,0,10);
                    Ledset2.setPixelColor(80,15,0,10);
                    Ledset2.setPixelColor(79,15,0,10);
                    Ledset3.setPixelColor(21,15,0,10);
                    Ledset3.setPixelColor(23,15,0,10);
                    Ledset3.setPixelColor(37,15,0,10);
                    Ledset3.setPixelColor(39,15,0,10);
                    Ledset3.setPixelColor(65,15,0,10);
                    Ledset3.setPixelColor(66,15,0,10);
                    Ledset3.setPixelColor(67,15,0,10);
                    Ledset3.setPixelColor(68,15,0,10);
                    Ledset3.setPixelColor(69,15,0,10);
                    Ledset3.setPixelColor(70,15,0,10);
                    Ledset3.setPixelColor(71,15,0,10);
                    Ledset3.setPixelColor(72,15,0,10);
                    Ledset3.setPixelColor(64,15,0,10);
                    Ledset4.setPixelColor(5,15,0,10);
                    Ledset4.setPixelColor(13,15,0,10);
                    Ledset4.setPixelColor(25,15,0,10);
                    Ledset4.setPixelColor(31,15,0,10);
                    Ledset4.setPixelColor(45,15,0,10);
                    Ledset4.setPixelColor(49,15,0,10);
                    Ledset5.setPixelColor(9,15,0,10);
                    Ledset5.setPixelColor(10,15,0,10);
                    Ledset5.setPixelColor(8,15,0,10);

                    Serial.println("default3");

                }
                else if (functionCommand[5]==0&&functionCommand[4]==1&&functionCommand[3]==1){
                    for (int i = 0; i <88;i++){
                        Ledset1.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <84;i++){
                        Ledset2.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <78;i++){
                        Ledset3.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <57;i++){
                        Ledset4.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <49;i++){
                        Ledset5.setPixelColor(i,0,0,0);
                    }
                    Ledset1.setPixelColor(72,15,0,10);
                    Ledset1.setPixelColor(71,15,0,10);
                    Ledset1.setPixelColor(83,15,0,10);
                    Ledset1.setPixelColor(82,15,0,10);
                    Ledset2.setPixelColor(4,15,0,10);
                    Ledset2.setPixelColor(5,15,0,10);
                    Ledset2.setPixelColor(16,15,0,10);
                    Ledset2.setPixelColor(15,15,0,10);
                    Ledset2.setPixelColor(25,15,0,10);
                    Ledset2.setPixelColor(26,15,0,10);
                    Ledset2.setPixelColor(36,15,0,10);
                    Ledset2.setPixelColor(37,15,0,10);
                    Ledset2.setPixelColor(46,15,0,10);
                    Ledset2.setPixelColor(47,15,0,10);
                    Ledset2.setPixelColor(58,15,0,10);
                    Ledset2.setPixelColor(57,15,0,10);
                    Ledset2.setPixelColor(68,15,0,10);
                    Ledset2.setPixelColor(67,15,0,10);
                    Ledset2.setPixelColor(78,15,0,10);
                    Ledset2.setPixelColor(79,15,0,10);
                    Ledset3.setPixelColor(21,15,0,10);
                    Ledset3.setPixelColor(23,15,0,10);
                    Ledset3.setPixelColor(37,15,0,10);
                    Ledset3.setPixelColor(39,15,0,10);
                    Ledset3.setPixelColor(65,15,0,10);
                    Ledset3.setPixelColor(66,15,0,10);
                    Ledset3.setPixelColor(67,15,0,10);
                    Ledset3.setPixelColor(68,15,0,10);
                    Ledset3.setPixelColor(69,15,0,10);
                    Ledset3.setPixelColor(70,15,0,10);
                    Ledset3.setPixelColor(71,15,0,10);
                    Ledset3.setPixelColor(72,15,0,10);
                    Ledset3.setPixelColor(64,15,0,10);
                    Ledset4.setPixelColor(5,15,0,10);
                    Ledset4.setPixelColor(13,15,0,10);
                    Ledset4.setPixelColor(25,15,0,10);
                    Ledset4.setPixelColor(31,15,0,10);
                    Ledset4.setPixelColor(45,15,0,10);
                    Ledset4.setPixelColor(49,15,0,10);
                    Ledset5.setPixelColor(9,15,0,10);
                    Ledset5.setPixelColor(10,15,0,10);
                    Ledset5.setPixelColor(8,15,0,10);

                    Serial.println("default4");

                }
                else if (functionCommand[5]==1&&functionCommand[4]==0&&functionCommand[3]==0){
                    for (int i = 0; i <88;i++){
                        Ledset1.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <84;i++){
                        Ledset2.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <78;i++){
                        Ledset3.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <57;i++){
                        Ledset4.setPixelColor(i,0,0,0);
                    }
                    for (int i = 0; i <49;i++){
                        Ledset5.setPixelColor(i,0,0,0);
                    }
                    Ledset1.setPixelColor(69,15,0,10);
                    Ledset1.setPixelColor(70,15,0,10);
                    Ledset1.setPixelColor(84,15,0,10);
                    Ledset1.setPixelColor(85,15,0,10);
                    Ledset2.setPixelColor(4,15,0,10);
                    Ledset2.setPixelColor(5,15,0,10);
                    Ledset2.setPixelColor(15,15,0,10);
                    Ledset2.setPixelColor(16,15,0,10);
                    Ledset2.setPixelColor(27,15,0,10);
                    Ledset2.setPixelColor(35,15,0,10);
                    Ledset2.setPixelColor(46,15,0,10);
                    Ledset2.setPixelColor(47,15,0,10);
                    Ledset2.setPixelColor(57,15,0,10);
                    Ledset2.setPixelColor(58,15,0,10);
                    Ledset2.setPixelColor(80,15,0,10);
                    Ledset2.setPixelColor(81,15,0,10);
                    Ledset2.setPixelColor(65,15,0,10);
                    Ledset2.setPixelColor(66,15,0,10);
                    Ledset3.setPixelColor(22,15,0,10);
                    Ledset3.setPixelColor(24,15,0,10);
                    Ledset3.setPixelColor(36,15,0,10);
                    Ledset3.setPixelColor(38,15,0,10);
                    Ledset3.setPixelColor(40,15,0,10);
                    Ledset3.setPixelColor(42,15,0,10);
                    Ledset3.setPixelColor(45,15,0,10);
                    Ledset3.setPixelColor(46,15,0,10);
                    Ledset3.setPixelColor(52,15,0,10);
                    Ledset3.setPixelColor(53,15,0,10);
                    Ledset3.setPixelColor(56,15,0,10);
                    Ledset3.setPixelColor(58,15,0,10);
                    Ledset3.setPixelColor(63,15,0,10);
                    Ledset3.setPixelColor(66,15,0,10);
                    Ledset3.setPixelColor(67,15,0,10);
                    Ledset3.setPixelColor(68,15,0,10);
                    Ledset3.setPixelColor(69,15,0,10);
                    Ledset3.setPixelColor(70,15,0,10);
                    Ledset3.setPixelColor(73,15,0,10);
                    Ledset4.setPixelColor(4,15,0,10);
                    Ledset4.setPixelColor(14,15,0,10);
                    Ledset4.setPixelColor(23,15,0,10);
                    Ledset4.setPixelColor(33,15,0,10);
                    Ledset4.setPixelColor(42,15,0,10);
                    Ledset4.setPixelColor(45,15,0,10);
                    Ledset4.setPixelColor(46,15,0,10);
                    Ledset4.setPixelColor(47,15,0,10);
                    Ledset4.setPixelColor(48,15,0,10);
                    Ledset4.setPixelColor(49,15,0,10);
                    Ledset4.setPixelColor(52,15,0,10);
                    Ledset5.setPixelColor(5,15,0,10);
                    Ledset5.setPixelColor(6,15,0,10);
                    Ledset5.setPixelColor(12,15,0,10);
                    Ledset5.setPixelColor(13,15,0,10);

                    Serial.println("default5");

                }

            Ledset1.show();
            Ledset2.show();
            Ledset3.show();
            Ledset4.show();
            Ledset5.show();
            Serial.println("out");

        }
        updated = false;
    };
}
