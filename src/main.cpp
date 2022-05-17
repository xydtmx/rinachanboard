#include <Adafruit_NeoPixel.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include "Preferences.h"
//#include <BLEUtils.h>

#include <LEDcontrol_purecolor.h>

#include <atomic>
#include <bitset>

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

string data;

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
        data = value;
        Serial.println(value.length());
        Serial.println(value[1]);
        functionCommand = bitset<8>(value[0]);
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


if(data[0] == 10){
    LEDcontrol_purecolor();
}
//        if (functionCommand[7] == 0 && functionCommand[6] == 0) {//直接控制部分
//            if (functionCommand[5] == 0) {//上传模式
//                if (boardSelect.to_ulong() == 0) {
//                    Ledset1.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
//                    Serial.print("in1");
//                } else if (boardSelect.to_ulong() == 1) {
//                    Ledset2.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
//                    Serial.print("in2");
//
//                } else if (boardSelect.to_ulong() == 2) {
//                    Ledset3.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
//                    Serial.print("in3");
//
//                } else if (boardSelect.to_ulong() == 3) {
//                    Ledset4.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
//                    Serial.print("in4");
//
//                } else if (boardSelect.to_ulong() == 4) {
//                    Ledset5.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
//                    Serial.print("in5");
//
//                }
//            }
//            else if(functionCommand[5] == 1){//输出模式
//                    if (boardSelect.to_ulong() == 0) {
//                        Ledset1.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
//                        Serial.print("out1");
//
//                    } else if (boardSelect.to_ulong() == 1) {
//                        Ledset2.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
//                        Serial.print("out2");
//
//                    } else if (boardSelect.to_ulong() == 2) {
//                        Ledset3.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
//                        Serial.print("out3");
//
//                    } else if (boardSelect.to_ulong() == 3) {
//                        Ledset4.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
//                        Serial.print("out4");
//
//                    } else if (boardSelect.to_ulong() == 4) {
//                        Ledset5.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
//                        Serial.print("out5");
//
//                    }
//                Ledset1.show();
//                Ledset2.show();
//                Ledset3.show();
//                Ledset4.show();
//                Ledset5.show();
////                Serial.println("out");
//            }
//        }
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

};

