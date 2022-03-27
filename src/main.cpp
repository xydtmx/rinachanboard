#include <Adafruit_NeoPixel.h>
#include <BLEDevice.h>
#include <BLEServer.h>
//#include <BLEUtils.h>

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

    BLEDevice::init("MyESP32");
    BLEServer *pServer = BLEDevice::createServer();

    BLEService *pService = pServer->createService(SERVICE_UUID);

    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );

    pCharacteristic->setCallbacks(new MyCallbacks());

    pCharacteristic->setValue("Hello World");
    pService->start();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();


    for (int i = 0; i <= 88; i++) {
        if (bitSelect.to_ulong() == i) {
            Ledset1.setPixelColor(i, Adafruit_NeoPixel::Color(
                    red.to_ulong(),
                    green.to_ulong(),
                    blue.to_ulong()))
                    ;} else
            Ledset1.setPixelColor(i, Adafruit_NeoPixel::Color(
                    15,
                    15,
                    15));;
        Ledset1.show();
    }
}

void loop() {
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

        if (functionCommand.to_ulong() << 7 == 0) {
            if (boardSelect.to_ulong() == 0) {
                Ledset1.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), blue.to_ulong());
            } else if (boardSelect.to_ulong() == 1) {
                Ledset2.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), red.to_ulong());
            } else if (boardSelect.to_ulong() == 2) {
                Ledset3.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), red.to_ulong());
            } else if (boardSelect.to_ulong() == 3) {
                Ledset4.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), red.to_ulong());
            } else if (boardSelect.to_ulong() == 4) {
                Ledset5.setPixelColor(bitSelect.to_ulong(), red.to_ulong(), green.to_ulong(), red.to_ulong());
            }
        }
        else if (functionCommand.to_ulong()<<7 ==1) {
            Ledset1.show();
            Ledset2.show();
            Ledset3.show();
            Ledset4.show();
            Ledset5.show();
        }
        updated = false;
    }
}