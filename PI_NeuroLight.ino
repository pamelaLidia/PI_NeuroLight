/*
** Programa para controle da fita de LED para o PI 2
** Alunos: Jessica Louise, Joao Gabriel e Pamela Lidia
*/

// Definição dos pinos de controle da fita de LED
const int RED_PIN = 9;
const int GREEN_PIN = 10;
const int BLUE_PIN = 11;
const int CW_PIN = 6;  // Cold White (Branco Frio)
const int WW_PIN = 5;  // Warm White (Branco Quente)

// Variáveis para armazenar a hora atual e o modo de operação
int horaAtual = 12; 
int minutoAtual = 0;
char modoAtual = 'W'; // 'W' para Branco, 'R' para RGB

void setup() {
  // Configura todos os pinos como saídas
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(CW_PIN, OUTPUT);
  pinMode(WW_PIN, OUTPUT);

  // Inicia a comunicação serial
  Serial.begin(9600);
  delay(1000); 

  // Mensagem de boas-vindas com instruções
  Serial.println("==============================================");
  Serial.println("### Controlador de Iluminacao Circadiana v2.0 ###");
  Serial.println("Instrucoes:");
  Serial.println(" - Definir hora: T<HH:MM> (Ex: T08:30)");
  Serial.println(" - Mudar modo: MW (Branco) ou MR (RGB)");
  Serial.println("==============================================");
  Serial.print("Modo inicial: Branco. Hora inicial: ");
  Serial.print(horaAtual);
  Serial.println(":00");

  // Ajusta a iluminação inicial
  ajustarIluminacao();
}

void loop() {
  // Verifica se há dados disponíveis na porta serial
  if (Serial.available() > 0) {
    processarComandoSerial();
  }
}

/*
 * Função para ler e processar os comandos enviados pela Serial.
 */
void processarComandoSerial() {
  String comando = Serial.readStringUntil('\n');
  comando.trim(); 

  char tipoComando = comando.charAt(0);
  
  if (tipoComando == 'T' || tipoComando == 't') {
    int novaHora = comando.substring(1, 3).toInt();
    int novoMinuto = comando.substring(4, 6).toInt();

    if (novaHora >= 0 && novaHora <= 23 && novoMinuto >= 0 && novoMinuto <= 59) {
      horaAtual = novaHora;
      minutoAtual = novoMinuto;
      Serial.print("OK. Nova hora definida para: ");
      Serial.print(horaAtual);
      Serial.print(":");
      if(minutoAtual < 10) Serial.print("0");
      Serial.println(minutoAtual);
      ajustarIluminacao();
    } else {
      Serial.println("ERRO: Formato de hora invalido. Use T<HH:MM>.");
    }
  } 
  else if (tipoComando == 'M' || tipoComando == 'm') {
    char novoModo = comando.charAt(1);
    if (novoModo == 'W' || novoModo == 'w') {
      modoAtual = 'W';
      Serial.println("OK. Modo alterado para BRANCO (Suave).");
      ajustarIluminacao();
    } else if (novoModo == 'R' || novoModo == 'r') {
      modoAtual = 'R';
      Serial.println("OK. Modo alterado para RGB (Artistico).");
      ajustarIluminacao();
    } else {
      Serial.println("ERRO: Modo invalido. Use MW ou MR.");
    }
  } else {
    Serial.println("ERRO: Comando desconhecido.");
  }
}

/*
 * Função que decide a cor com base na hora e no modo.
 */
void ajustarIluminacao() {
  Serial.print("Ajustando luz para ");
  Serial.print(horaAtual);
  Serial.print("h - Modo: ");
  Serial.println(modoAtual == 'W' ? "Branco" : "RGB");

  if (modoAtual == 'W') {
    // --- LÓGICA DO MODO BRANCO  ---
    int cwValue, wwValue;

    if (horaAtual < 6) { // Madrugada
      // Desligado
      cwValue = 0;
      wwValue = 0;
    } else if (horaAtual < 12) { // 06:00 - 11:59 (Amanhecer -> Meio-dia)
      // Transição de 2000K para 6500K
      wwValue = map(horaAtual, 6, 12, 255, 80);  // WW diminui
      cwValue = map(horaAtual, 6, 12, 20, 255);   // CW aumenta
    } else if (horaAtual < 18) { // 12:00 - 17:59 (Meio-dia -> Entardecer)
      // Transição de 6500K para 2700K
      wwValue = map(horaAtual, 12, 18, 80, 255);  // WW aumenta
      cwValue = map(horaAtual, 12, 18, 255, 50);   // CW diminui
    } else if (horaAtual < 22) { // 18:00 - 21:59 (Entardecer -> Noite)
      // Mantém a cor quente, mas diminui a intensidade 
      wwValue = map(horaAtual, 18, 22, 255, 80);
      cwValue = map(horaAtual, 18, 22, 50, 0);
    } else { // 22:00 - 23:59 (Noite Profunda)
      // Luz mais fraca até desligar
      wwValue = map(horaAtual, 22, 24, 80, 0);
      cwValue = 0;
    }
    definirCor(0, 0, 0, cwValue, wwValue);

  } else {
    // --- LÓGICA DO MODO RGB ---
    if (horaAtual < 6) { // Madrugada
      definirCor(0, 0, 15, 0, 0);       // Azul muito escuro, quase apagado
    } else if (horaAtual < 8) { // Amanhecer
      definirCor(255, 100, 0, 0, 0);   // Laranja/Âmbar
    } else if (horaAtual < 12) { // Manhã
      definirCor(200, 220, 255, 0, 0);  // Branco amarelado claro, céu da manhã
    } else if (horaAtual < 17) { // Meio-dia e Tarde
      definirCor(120, 180, 255, 0, 0);  // Azul céu claro
    } else if (horaAtual < 19) { // Entardecer
      definirCor(255, 90, 100, 0, 0);   // Pôr do sol púrpura/rosa
    } else if (horaAtual < 22) { // Início da Noite
      definirCor(100, 0, 200, 0, 0);    // Roxo/Índigo
    } else { // Noite Profunda
      definirCor(0, 0, 80, 0, 0);       // Azul escuro
    }
  }
}

/*
 * Função auxiliar para definir os valores em cada pino.
 */
void definirCor(int r, int g, int b, int cw, int ww) {
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
  analogWrite(CW_PIN, cw);
  analogWrite(WW_PIN, ww);
}