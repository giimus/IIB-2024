#define ANALOG_PIN A1     // Pino analógico para medir a tensão
#define OFFSET_BUTTON_PIN 2 // Pino digital para o botão de offset

float voltage = 0;        // Variável para armazenar a tensão
float analogValue = 0;    // Valor lido do pino analógico
float offset = 0;         // Valor de offset (zeragem)

// Configuração inicial
void setup() {
  pinMode(OFFSET_BUTTON_PIN, INPUT_PULLUP); // Configura o botão como entrada com pull-up
  Serial.begin(9600);                       // Inicializa a comunicação serial
}

// Loop principal
void loop() {
  // Lê o valor analógico (0-1023)
  analogValue = analogRead(ANALOG_PIN);
  // Converte o valor analógico para tensão (0-5V) e aplica o offset
  voltage = (analogValue * (5.0 / 1023.0) - offset);

  voltage = max(voltage, 0);
  // Exibe a tensão medida no monitor serial
  Serial.print("Tensão detectada (com offset): ");
  Serial.print(voltage);
  Serial.println(" V");

  
  // Verifica se o botão de offset foi pressionado
  if (digitalRead(OFFSET_BUTTON_PIN) == LOW) { // Botão pressionado (LOW devido ao pull-up)
    offset = analogValue * (5.0 / 1023.0);     // Salva o valor atual como offset
    Serial.println("Offset ajustado!");
    delay(1000);  // Evita múltiplas leituras ao pressionar o botão
  }

  delay(500);  // Aguarda 500 ms para a próxima leitura
}
