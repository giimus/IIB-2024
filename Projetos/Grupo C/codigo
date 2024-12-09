#include <HX711.h>

// Definição dos pinos do sensor de carga (HX711)
#define DOUT  15  // DT
#define CLK   18  // SCK
HX711 balanca;

// Definição dos pinos do driver do motor de passo (ULN2003)
#define IN1   2
#define IN2   4
#define IN3   5
#define IN4   19

// Variáveis para controlar o tempo
unsigned long lastWeightCheck = 0;
unsigned long weightCheckInterval = 1000; // Intervalo para verificar o peso (em milissegundos)
unsigned long lastTime = 0;
unsigned long interval = 5 * 60 * 100; // 1 minuto em milissegundos
float pesoLimite = 200.0; // Limite de peso em gramas

// Variáveis do motor de passo
int motorDelay = 10; // Atraso entre passos do motor (em milissegundos)
bool motorRunning = false; // Estado do motor (se está rodando ou não)

void setup() {
  Serial.begin(115200);

  // Inicializando o HX711 (sensor de carga)
  balanca.begin(DOUT, CLK);
  balanca.set_scale(-1600); // Ajuste de escala (pode ser necessário calibrar)
  balanca.tare(); // Tarar a balança (ajustar para zero)

  // Configuração dos pinos do motor de passo
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Inicializando o tempo
  lastTime = millis();
}

void loop() {
  float peso = lerPeso();
  // Verifica o tempo para dar os passos do motor
  if (millis() - lastTime >= interval) {
    if (peso < pesoLimite) {
      if (!motorRunning) {
        Serial.println("Ligando motor...");
        motorRunning = true; // Inicia o motor
      }
    }
    lastTime = millis(); // Atualiza o tempo
  }

  // Verifica o peso periodicamente enquanto o motor está em movimento
  if (millis() - lastWeightCheck >= weightCheckInterval) {
    //float peso = lerPeso();
    if (peso >= pesoLimite) {
      Serial.println("Peso >= 200g, parando o motor.");
      motorRunning = false; // Interrompe o motor
      Serial.println("Motor desligado.");
    }
    lastWeightCheck = millis(); // Atualiza o tempo da última verificação de peso
  }

  // Se o motor estiver rodando, continue acionando-o
  if (motorRunning) {
    acionarMotor();
  }
}

void acionarMotor() {
  // Movimento do motor de passo: 8 passos de cada vez (sentido anti-horário)
  for (int i = 0; i < 100; i++) { // 8 passos de uma vez
    stepMotor(4);
    delay(motorDelay);
    stepMotor(3);
    delay(motorDelay);
    stepMotor(2);
    delay(motorDelay);
    stepMotor(1);
    delay(motorDelay);
  }
}

void stepMotor(int step) {
  // Função para controlar o motor de passo no sentido anti-horário
  if (step == 1) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  } else if (step == 2) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  } else if (step == 3) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else if (step == 4) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
}

float lerPeso() {
  long leitura = balanca.get_units(10); // Média de 10 leituras
  float peso = (float)leitura;
  Serial.print("Peso atual: ");
  Serial.print(peso);
  Serial.println(" g");
  return peso;
}
