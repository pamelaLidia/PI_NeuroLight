// Definição dos pinos de controle da fita de LED
// Conectados ao GATE de cada MOSFET
const int RED_PIN = 9;
const int GREEN_PIN = 10;
const int BLUE_PIN = 11;
const int CW_PIN = 6;  // Cold White (Branco Frio)
const int WW_PIN = 5;  // Warm White (Branco Quente, que você chamou de W)

void setup() {
  // Configura todos os pinos como saídas (OUTPUT)
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(CW_PIN, OUTPUT);
  pinMode(WW_PIN, OUTPUT);

  // Inicia a comunicação serial para debug (opcional, mas recomendado)
  Serial.begin(9600);
  Serial.println("Controlador de Fita LED RGBCW iniciado.");

  // Garante que todos os LEDs comecem desligados
  definirCor(0, 0, 0, 0, 0);
}

/**
 * Função principal para definir a cor e intensidade da fita.
 * Os valores vão de 0 (desligado) a 255 (intensidade máxima).
 */
void definirCor(int redValue, int greenValue, int blueValue, int cwValue, int wwValue) {
  // Escreve o valor PWM para cada pino
  analogWrite(RED_PIN, redValue);
  analogWrite(GREEN_PIN, greenValue);
  analogWrite(BLUE_PIN, blueValue);
  analogWrite(CW_PIN, cwValue);
  analogWrite(WW_PIN, wwValue);
}

void loop() {
  // --- Exemplo 1: Ciclo de Cores RGB ---
  Serial.println("Iniciando ciclo RGB...");

  // Fade de Vermelho para Verde
  for (int i = 0; i < 255; i++) {
    definirCor(255 - i, i, 0, 0, 0);
    delay(10);
  }

  // Fade de Verde para Azul
  for (int i = 0; i < 255; i++) {
    definirCor(0, 255 - i, i, 0, 0);
    delay(10);
  }

  // Fade de Azul para Vermelho
  for (int i = 0; i < 255; i++) {
    definirCor(i, 0, 255 - i, 0, 0);
    delay(10);
  }

  delay(1000); // Pausa por 1 segundo

  // --- Exemplo 2: Ciclo de Brancos ---
  Serial.println("Iniciando ciclo de Brancos...");

  // Acende e apaga o Branco Frio (CW)
  for (int i = 0; i < 255; i++) {
    definirCor(0, 0, 0, i, 0);
    delay(10);
  }
  for (int i = 255; i >= 0; i--) {
    definirCor(0, 0, 0, i, 0);
    delay(10);
  }

  // Acende e apaga o Branco Quente (WW)
  for (int i = 0; i < 255; i++) {
    definirCor(0, 0, 0, 0, i);
    delay(10);
  }
  for (int i = 255; i >= 0; i--) {
    definirCor(0, 0, 0, 0, i);
    delay(10);
  }

  delay(1000); // Pausa por 1 segundo
}