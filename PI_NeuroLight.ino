// --- CONTROLE DE ILUMINAÇÃO CIRCADIANA ---

// Definição dos pinos de controle da fita de LED
const int RED_PIN = 9;
const int GREEN_PIN = 10;
const int BLUE_PIN = 11;
const int CW_PIN = 6;  // Cold White (Branco Frio)
const int WW_PIN = 5;  // Warm White (Branco Quente)

// Variáveis para armazenar a hora atual e o modo de operação
int horaAtual = 12; // Começa ao meio-dia por padrão
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
  delay(1000); // Pequena pausa para a serial iniciar completamente

  // Mensagem de boas-vindas com instruções
  Serial.println("==============================================");
  Serial.println("### Controlador de Iluminacao Circadiana ###");
  Serial.println("Instrucoes:");
  Serial.println(" - Para definir a hora, envie: T<HH:MM>");
  Serial.println("   Exemplos: T08:30, T19:05, T23:15");
  Serial.println(" - Para mudar o modo de cor, envie: M<MODO>");
  Serial.println("   Exemplos: MW (Branco), MR (RGB)");
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

/**
 * Função para ler e processar os comandos enviados pela Serial.
 */
void processarComandoSerial() {
  String comando = Serial.readStringUntil('\n'); // Lê a linha inteira
  comando.trim(); // Remove espaços em branco

  // Pega o primeiro caractere para identificar o comando (T ou M)
  char tipoComando = comando.charAt(0);
  
  if (tipoComando == 'T' || tipoComando == 't') {
    // Comando para definir o TEMPO (T<HH:MM>)
    // Pega a parte da hora (HH) e a parte do minuto (MM)
    int novaHora = comando.substring(1, 3).toInt();
    int novoMinuto = comando.substring(4, 6).toInt();

    // Valida se a hora e o minuto são válidos
    if (novaHora >= 0 && novaHora <= 23 && novoMinuto >= 0 && novoMinuto <= 59) {
      horaAtual = novaHora;
      minutoAtual = novoMinuto;
      Serial.print("OK. Nova hora definida para: ");
      Serial.print(horaAtual);
      Serial.print(":");
      Serial.println(minutoAtual);
      ajustarIluminacao(); // Atualiza a luz imediatamente
    } else {
      Serial.println("ERRO: Formato de hora invalido. Use T<HH:MM>.");
    }
  } 
  else if (tipoComando == 'M' || tipoComando == 'm') {
    // Comando para definir o MODO (MW ou MR)
    char novoModo = comando.charAt(1);
    if (novoModo == 'W' || novoModo == 'w') {
      modoAtual = 'W';
      Serial.println("OK. Modo alterado para BRANCO (Quente/Frio).");
      ajustarIluminacao(); // Atualiza a luz imediatamente
    } else if (novoModo == 'R' || novoModo == 'r') {
      modoAtual = 'R';
      Serial.println("OK. Modo alterado para RGB.");
      ajustarIluminacao(); // Atualiza a luz imediatamente
    } else {
      Serial.println("ERRO: Modo invalido. Use MW ou MR.");
    }
  } else {
    Serial.println("ERRO: Comando desconhecido.");
  }
}

/**
 * Função principal que decide a cor com base na hora e no modo.
 */
void ajustarIluminacao() {
  Serial.print("Ajustando luz para ");
  Serial.print(horaAtual);
  Serial.print("h - Modo: ");
  Serial.println(modoAtual);

  if (modoAtual == 'W') {
    // --- LÓGICA DO MODO BRANCO (QUENTE/FRIO) ---
    if (horaAtual >= 0 && horaAtual < 6) { // Madrugada
      definirCor(0, 0, 0, 0, 0); // Desligado
    } else if (horaAtual >= 6 && horaAtual < 9) { // Amanhecer
      definirCor(0, 0, 0, 50, 255); // Bem quente
    } else if (horaAtual >= 9 && horaAtual < 12) { // Manhã
      definirCor(0, 0, 0, 150, 200); // Branco neutro
    } else if (horaAtual >= 12 && horaAtual < 15) { // Meio-dia
      definirCor(0, 0, 0, 255, 100); // Branco bem frio (luz do sol a pino)
    } else if (horaAtual >= 15 && horaAtual < 18) { // Tarde
      definirCor(0, 0, 0, 150, 200); // Neutro novamente
    } else if (horaAtual >= 18 && horaAtual < 21) { // Entardecer/Noite
      definirCor(0, 0, 0, 20, 255); // Bem quente
    } else { // Noite profunda
      definirCor(0, 0, 0, 0, 150); // Apenas luz quente e fraca
    }
  } else {
    // --- LÓGICA DO MODO RGB ---
    if (horaAtual >= 0 && horaAtual < 6) { // Madrugada
      definirCor(0, 0, 20, 0, 0); // Azul bem escuro, quase apagado
    } else if (horaAtual >= 6 && horaAtual < 9) { // Amanhecer
      definirCor(255, 80, 0, 0, 0); // Laranja
    } else if (horaAtual >= 9 && horaAtual < 16) { // Dia
      definirCor(100, 150, 255, 0, 0); // Azul claro (céu)
    } else if (horaAtual >= 16 && horaAtual < 19) { // Entardecer
      definirCor(255, 60, 100, 0, 0); // Púrpura/Rosa
    } else { // Noite
      definirCor(0, 0, 100, 0, 0); // Azul escuro
    }
  }
}

/**
 * Função auxiliar para definir os valores em cada pino.
 * Os valores vão de 0 (desligado) a 255 (intensidade máxima).
 */
void definirCor(int r, int g, int b, int cw, int ww) {
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
  analogWrite(CW_PIN, cw);
  analogWrite(WW_PIN, ww);
}