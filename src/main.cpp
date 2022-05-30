#include <Adafruit_NeoPixel.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <Preferences.h>
//#include <BLEUtils.h>

#include "LEDcontrol_purecolor.h"
#include "LEDcontrol_RGBmode.h"
#include "LEDcontrol_SystemExpression.h"

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

std::string data;

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

    if (deviceConnected) {
        if (data[0] == 0) {
            LEDcontrol_purecolor();
        } else if (data[0] == 1) {
            LEDcontrol_RGBmode();
        } else if (data[0] == 2) {
            SystemExpression();
        }
    }



};

