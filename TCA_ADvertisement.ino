#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include "esp_bt.h"

BLEScan* pBLEScan;

// Mac címek és helyiségek párosításaa
String getRoomName(String mac) {
  if (mac.equalsIgnoreCase("38:1F:8D:D7:C3:67")) return "Fürdőszoba";
  if (mac.equalsIgnoreCase("34:EC:B6:66:02:2B")) return "Hálószoba";
  if (mac.equalsIgnoreCase("40:B7:FC:17:3E:11")) return "Nappali";
  if (mac.equalsIgnoreCase("34:EC:B6:66:01:78")) return "Konyha";

  
  return "Ismeretlen helyiség";
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    String macAddress = advertisedDevice.getAddress().toString();
    String room = getRoomName(macAddress);

    uint8_t* payload = advertisedDevice.getPayload();
    size_t payloadLength = advertisedDevice.getPayloadLength();

    size_t pos = 0;
    while (pos < payloadLength) {
      uint8_t length = payload[pos];
      if (length == 0) break;
      if (pos + length >= payloadLength) break;

      uint8_t adType = payload[pos + 1];
      if (adType == 0x16) {  // Service Data - 16 bit UUID
        uint16_t uuid16 = (uint8_t)payload[pos + 3] << 8 | (uint8_t)payload[pos + 2];
        if (uuid16 == 0xFCD2) {
          Serial.printf("Sensor MAC: %s (%s)\n", macAddress.c_str(), room.c_str());

          Serial.print("Service Data: ");
          for (size_t i = pos + 2; i < pos + 1 + length; i++) {
            Serial.printf("%02X ", payload[i]);
          }
          Serial.println();

          uint8_t protoVersion = payload[pos + 4];
          Serial.printf("Protocol Version: %d\n", protoVersion);

          size_t dataPos = pos + 5;
          size_t dataEnd = pos + 1 + length;

          bool foundTemp = false, foundHum = false, foundVolt = false;
          float temperature = 0.0f, humidity = 0.0f, voltage = 0.0f;

          while (dataPos < dataEnd) {
            uint8_t objId = payload[dataPos++];

            if (objId == 0x02 && dataPos + 1 < dataEnd) {  // Temperature
              int16_t rawVal = (int16_t)(payload[dataPos] | (payload[dataPos + 1] << 8));
              dataPos += 2;
              temperature = rawVal * 0.01f;
              foundTemp = true;
            } else if (objId == 0x03 && dataPos + 1 < dataEnd) {  // Humidity
              uint16_t rawVal = (uint16_t)(payload[dataPos] | (payload[dataPos + 1] << 8));
              dataPos += 2;
              humidity = rawVal * 0.01f;
              foundHum = true;
            } else if (objId == 0x0C && dataPos + 1 < dataEnd) {  // Voltage
              uint16_t rawVal = (uint16_t)(payload[dataPos] | (payload[dataPos + 1] << 8));
              dataPos += 2;
              voltage = rawVal * 0.001f;
              foundVolt = true;
            } else {
              if (dataPos < dataEnd) dataPos++;  // Ismeretlen objID
            }
          }

          if (foundTemp) Serial.printf("Temperature: %.2f °C\n", temperature);
          else Serial.println("Temperature: N/A");
          if (foundHum) Serial.printf("Humidity: %.2f %%\n", humidity);
          else Serial.println("Humidity: N/A");
          if (foundVolt) Serial.printf("Voltage: %.3f V\n", voltage);
          else Serial.println("Voltage: N/A");

          Serial.println("------------------------------");
        }
      }
      pos += length + 1;
    }
  }
};

MyAdvertisedDeviceCallbacks myCallbacks;  // Globális példány

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE scan...");

  BLEDevice::init("");
   // Állítsuk be a maximális teljesítményt
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN, ESP_PWR_LVL_P9); // +9 dBm a maximum
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);  // ha később hirdetsz is
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL0, ESP_PWR_LVL_P9); // kapcsolat alatt is

  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(&myCallbacks);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(100);
  pBLEScan->setActiveScan(false);
}

void loop() {
  pBLEScan->start(5, false);  // 5 másodpercig scannelünk
  pBLEScan->stop();
  delay(1000);               // 1 másodperc pihenő
}
