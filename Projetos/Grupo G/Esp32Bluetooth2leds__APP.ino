#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

const int led1 = 2; // GPIO do LED 1
const int led2 = 15; // GPIO do LED 2

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_LED");
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  Serial.println("ESP32 Bluetooth Iniciado");
}

void loop() {
  if (SerialBT.available()) {
    char command = SerialBT.read();

    if (command == 'A') {
      digitalWrite(led1, HIGH); // Liga LED 1
      Serial.println("LED 1 Ligado");
    } else if (command == 'B') {
      digitalWrite(led1, LOW); // Desliga LED 1
      Serial.println("LED 1 Desligado");
    } else if (command == 'C') {
      digitalWrite(led2, HIGH); // Liga LED 2
      Serial.println("LED 2 Ligado");
    } else if (command == 'D') {
      digitalWrite(led2, LOW); // Desliga LED 2
      Serial.println("LED 2 Desligado");
    } else {
      Serial.println("Comando desconhecido");
    }
  }
}

