/*
 * ====================================================================
 * ROBÔ DE COMBATE "RAMPAGE" - ESP32 CONTROLLER
 * ====================================================================
 * UNIVERSIDADE SÃO FRANCISCO
 * Engenharia de Computação - 8º Semestre
 * Prática Profissional: Sistemas Autônomos e Robótica
 *
 * Equipe:
 * - Matheus Abrahão Martins Alvares - 202204581
 * - Cauê Becker Rossi - 202204081
 * - Bruno Ricci Zanoni - 202213443
 * - Giulio Passetti Figueiredo - 202201837
 *
 * ====================================================================
 * ESPECIFICAÇÕES DO ROBÔ:
 * ====================================================================
 * Classe: Beetleweight (1,36 kg, 30x30 cm)
 * Bateria: Li-Ion 11,1V (3S 2P) 4900mAh
 *
 * COMPONENTES:
 * - Microcontrolador: ESP32 Doit DevKit
 * - Motores: 2x DC 12V com redução (170 RPM)
 * - Drivers: 2x BTS7960 (43A cada)
 * - Servo: 1x MG995 (alto torque)
 * - Reguladores: 2x LM2596 Step-Down (5V e 6V)
 *
 * COMANDOS SERIAIS/BLUETOOTH:
 * F    = Frente
 * B    = Ré
 * L    = Girar Esquerda
 * R    = Girar Direita
 * S    = Parar
 * Axx  = Mover servo para ângulo xx (0-180)
 * V0-9 = Ajustar velocidade (0=mínimo, 9=máximo)
 * ====================================================================
 */

#include <ESP32Servo.h>

// ====================================================================
// CONFIGURAÇÃO DE PINOS - BTS7960 (2 PWM POR MOTOR)
// ====================================================================

// BTS7960 - Motor ESQUERDO (Driver #1)
#define L_EN_L 26 // Enable L (sempre HIGH)
#define R_EN_L 25 // Enable R (sempre HIGH)
#define LPWM_L 27 // PWM Ré (motor esquerdo)
#define RPWM_L 14 // PWM Frente (motor esquerdo)

// BTS7960 - Motor DIREITO (Driver #2)
#define L_EN_R 33 // Enable L (sempre HIGH)
#define R_EN_R 32 // Enable R (sempre HIGH)
#define LPWM_R 18 // PWM Ré (motor direito)
#define RPWM_R 19 // PWM Frente (motor direito)

// Servo da Arma
#define SERVO_PIN 4 // GPIO4 para controle do servo MG995

// ====================================================================
// CONFIGURAÇÕES PWM (LEDC)
// ====================================================================

// Configurações PWM
#define PWM_FREQ 20000  // 20 kHz para reduzir ruído audível
#define PWM_RESOLUCAO 8 // Resolução de 8 bits (0-255)

// Canais PWM do ESP32 (0-15 disponíveis)
#define CH_L_A 0 // LPWM_L (motor esquerdo ré)
#define CH_L_B 1 // RPWM_L (motor esquerdo frente)
#define CH_R_A 2 // LPWM_R (motor direito ré)
#define CH_R_B 3 // RPWM_R (motor direito frente)

// ====================================================================
// PROTÓTIPOS DAS FUNÇÕES
// ====================================================================

// Funções de controle dos motores
void inicializarMotores();
void pararMotores();
void moverFrente();
void moverRe();
void girarEsquerda();
void girarDireita();

// Funções de controle do servo
void inicializarServo();
void moverServo(int angulo);

// Funções auxiliares
void ajustarVelocidade(int nivel);
void processarComando(String cmd);
void verificarFailSafe();
void testeMotores();

// ====================================================================
// VARIÁVEIS GLOBAIS
// ====================================================================

Servo servoArma;           // Objeto para controlar o servo
int anguloServo = 90;      // Ângulo inicial do servo (posição neutra)
int velocidadeAtual = 255; // Velocidade atual dos motores (0-255)
int nivelVelocidade = 9;   // Nível de velocidade (0-9)

// Variáveis para fail-safe
unsigned long ultimoComando = 0;
const unsigned long TIMEOUT_FAILSAFE = 2000; // 2 segundos sem comando = parar

String comandoRecebido = ""; // Buffer para comandos multi-caractere

// ====================================================================
// FUNÇÕES AUXILIARES
// ====================================================================

/*
 * Helper inline para escrever PWM com constrain
 */
inline void pwmWrite(int canal, int duty)
{
  ledcWrite(canal, constrain(duty, 0, 255));
}

/*
 * Ajusta a velocidade dos motores (0-9)
 */
void ajustarVelocidade(int nivel)
{
  nivelVelocidade = constrain(nivel, 0, 9);
  // Mapear nível 0-9 para PWM 0-255
  velocidadeAtual = map(nivelVelocidade, 0, 9, 0, 255);

  Serial.print("[VELOCIDADE] Nível: ");
  Serial.print(nivelVelocidade);
  Serial.print(" - PWM: ");
  Serial.println(velocidadeAtual);
}

// ====================================================================
// FUNÇÕES DE CONTROLE DOS MOTORES (BTS7960 - 2 PWM POR MOTOR)
// ====================================================================

/*
 * Inicializa os pinos dos motores e configura PWM
 * BTS7960 usa 2 PWM por motor: LPWM (ré) e RPWM (frente)
 */
void inicializarMotores()
{
  // Configurar pinos Enable (sempre HIGH para habilitar)
  pinMode(L_EN_L, OUTPUT);
  pinMode(R_EN_L, OUTPUT);
  pinMode(L_EN_R, OUTPUT);
  pinMode(R_EN_R, OUTPUT);

  digitalWrite(L_EN_L, HIGH); // Habilita canal L do motor esquerdo
  digitalWrite(R_EN_L, HIGH); // Habilita canal R do motor esquerdo
  digitalWrite(L_EN_R, HIGH); // Habilita canal L do motor direito
  digitalWrite(R_EN_R, HIGH); // Habilita canal R do motor direito

  // Configurar 4 canais PWM (2 por motor)
  ledcSetup(CH_L_A, PWM_FREQ, PWM_RESOLUCAO);
  ledcSetup(CH_L_B, PWM_FREQ, PWM_RESOLUCAO);
  ledcSetup(CH_R_A, PWM_FREQ, PWM_RESOLUCAO);
  ledcSetup(CH_R_B, PWM_FREQ, PWM_RESOLUCAO);

  // Vincular pinos aos canais PWM
  ledcAttachPin(LPWM_L, CH_L_A); // Motor esquerdo - ré
  ledcAttachPin(RPWM_L, CH_L_B); // Motor esquerdo - frente
  ledcAttachPin(LPWM_R, CH_R_A); // Motor direito - ré
  ledcAttachPin(RPWM_R, CH_R_B); // Motor direito - frente

  // Parar motores inicialmente
  pararMotores();

  Serial.println("[INIT] Motores inicializados (BTS7960 - 2 PWM por motor)");
}

/*
 * Para todos os motores
 */
void pararMotores()
{
  // Zerar todos os PWM
  pwmWrite(CH_L_A, 0); // Motor esquerdo - ré
  pwmWrite(CH_L_B, 0); // Motor esquerdo - frente
  pwmWrite(CH_R_A, 0); // Motor direito - ré
  pwmWrite(CH_R_B, 0); // Motor direito - frente

  Serial.println("[MOTOR] Parado");
}

/*
 * Move o robô para frente
 * RPWM ativo (frente), LPWM = 0
 */
void moverFrente()
{
  // Motor esquerdo: frente (RPWM_L ativo)
  pwmWrite(CH_L_A, 0);               // LPWM_L = 0 (não vai ré)
  pwmWrite(CH_L_B, velocidadeAtual); // RPWM_L = velocidade (vai frente)

  // Motor direito: frente (RPWM_R ativo)
  pwmWrite(CH_R_A, 0);               // LPWM_R = 0 (não vai ré)
  pwmWrite(CH_R_B, velocidadeAtual); // RPWM_R = velocidade (vai frente)

  Serial.print("[MOTOR] Frente - PWM: ");
  Serial.println(velocidadeAtual);
}

/*
 * Move o robô para trás
 * LPWM ativo (ré), RPWM = 0
 */
void moverRe()
{
  // Motor esquerdo: ré (LPWM_L ativo)
  pwmWrite(CH_L_A, velocidadeAtual); // LPWM_L = velocidade (vai ré)
  pwmWrite(CH_L_B, 0);               // RPWM_L = 0 (não vai frente)

  // Motor direito: ré (LPWM_R ativo)
  pwmWrite(CH_R_A, velocidadeAtual); // LPWM_R = velocidade (vai ré)
  pwmWrite(CH_R_B, 0);               // RPWM_R = 0 (não vai frente)

  Serial.print("[MOTOR] Ré - PWM: ");
  Serial.println(velocidadeAtual);
}

/*
 * Gira o robô para a esquerda (motor direito frente, esquerdo ré)
 */
void girarEsquerda()
{
  // Motor esquerdo: ré
  pwmWrite(CH_L_A, velocidadeAtual); // LPWM_L = velocidade (ré)
  pwmWrite(CH_L_B, 0);               // RPWM_L = 0

  // Motor direito: frente
  pwmWrite(CH_R_A, 0);               // LPWM_R = 0
  pwmWrite(CH_R_B, velocidadeAtual); // RPWM_R = velocidade (frente)

  Serial.print("[MOTOR] Girando ESQUERDA - PWM: ");
  Serial.println(velocidadeAtual);
}

/*
 * Gira o robô para a direita (motor esquerdo frente, direito ré)
 */
void girarDireita()
{
  // Motor esquerdo: frente
  pwmWrite(CH_L_A, 0);               // LPWM_L = 0
  pwmWrite(CH_L_B, velocidadeAtual); // RPWM_L = velocidade (frente)

  // Motor direito: ré
  pwmWrite(CH_R_A, velocidadeAtual); // LPWM_R = velocidade (ré)
  pwmWrite(CH_R_B, 0);               // RPWM_R = 0

  Serial.print("[MOTOR] Girando DIREITA - PWM: ");
  Serial.println(velocidadeAtual);
}

// ====================================================================
// FUNÇÕES DE CONTROLE DO SERVO
// ====================================================================

/*
 * Inicializa o servo da arma
 */
void inicializarServo()
{
  servoArma.setPeriodHertz(50);           // Frequência padrão para servos: 50Hz
  servoArma.attach(SERVO_PIN, 500, 2400); // Attach com valores min/max para MG995
  servoArma.write(anguloServo);           // Posição inicial
  Serial.print("[INIT] Servo inicializado - Ângulo: ");
  Serial.println(anguloServo);
}

/*
 * Move o servo para um ângulo específico (0-180)
 */
void moverServo(int angulo)
{
  angulo = constrain(angulo, 0, 180);
  anguloServo = angulo;
  servoArma.write(anguloServo);

  Serial.print("[SERVO] Ângulo: ");
  Serial.print(anguloServo);
  Serial.println("°");
}

// ====================================================================
// PROCESSAMENTO DE COMANDOS
// ====================================================================

/*
 * Processa comandos recebidos pela serial
 */
void processarComando(String cmd)
{
  cmd.trim();        // Remove espaços em branco
  cmd.toUpperCase(); // Converte para maiúsculas

  if (cmd.length() == 0)
    return;

  // Atualizar timestamp do último comando (fail-safe)
  ultimoComando = millis();

  Serial.print("[CMD] ");
  Serial.println(cmd);

  // Comandos de movimento
  if (cmd == "F")
  {
    moverFrente();
  }
  else if (cmd == "B")
  {
    moverRe();
  }
  else if (cmd == "L")
  {
    girarEsquerda();
  }
  else if (cmd == "R")
  {
    girarDireita();
  }
  else if (cmd == "S")
  {
    pararMotores();
  }
  // Comando de velocidade (V0-V9)
  else if (cmd.startsWith("V") && cmd.length() == 2)
  {
    int nivel = cmd.substring(1).toInt();
    ajustarVelocidade(nivel);
  }
  // Comando do servo (A + ângulo, ex: A90, A180)
  else if (cmd.startsWith("A"))
  {
    int angulo = cmd.substring(1).toInt();
    moverServo(angulo);
  }
  else
  {
    Serial.println("[CMD] Inválido");
  }
}

// ====================================================================
// FAIL-SAFE
// ====================================================================

/*
 * Verifica se passou muito tempo sem receber comandos
 * Se sim, para os motores por segurança
 */
void verificarFailSafe()
{
  unsigned long agora = millis();

  // Verificar overflow do millis() (acontece após ~50 dias)
  if (agora < ultimoComando)
  {
    ultimoComando = agora;
  }

  // Se passou mais de 2 segundos sem comando, parar motores
  if (agora - ultimoComando > TIMEOUT_FAILSAFE)
  {
    static bool fs = false;

    if (!fs)
    {
      Serial.println("[FAIL-SAFE] Timeout. Parando motores.");
      pararMotores();
      fs = true;
    }
  }
}

// ====================================================================
// SETUP E LOOP PRINCIPAL
// ====================================================================

void setup()
{
  // Inicializar comunicação serial
  Serial.begin(115200);
  delay(300);

  Serial.println("\n");
  Serial.println("====================================");
  Serial.println("  ROBÔ DE COMBATE RAMPAGE - ESP32");
  Serial.println("====================================");
  Serial.println("Universidade São Francisco");
  Serial.println("Classe: Beetleweight (1,36 kg)");
  Serial.println("BTS7960 - 2 PWM por motor");
  Serial.println("====================================\n");

  // Inicializar componentes
  inicializarMotores();
  inicializarServo();
  ajustarVelocidade(9); // Velocidade máxima inicial

  // Inicializar timestamp do fail-safe
  ultimoComando = millis();

  Serial.println("\n[SISTEMA] Pronto para receber comandos!");
  Serial.println("\nComandos disponíveis:");
  Serial.println("  F      = Frente");
  Serial.println("  B      = Ré");
  Serial.println("  L      = Girar Esquerda");
  Serial.println("  R      = Girar Direita");
  Serial.println("  S      = Parar");
  Serial.println("  V0..9  = Velocidade (0=mín, 9=máx)");
  Serial.println("  A0..180 = Servo ângulo");
  Serial.println("\nFail-safe: 2s sem comando => STOP\n");
}

void loop()
{
  // Verificar se há dados disponíveis na serial
  while (Serial.available())
  {
    char c = Serial.read();

    // Se recebeu nova linha ou carriage return, processar comando
    if (c == '\n' || c == '\r')
    {
      if (comandoRecebido.length())
      {
        processarComando(comandoRecebido);
        comandoRecebido = "";
      }
    }
    // Senão, adicionar caractere ao buffer
    else
    {
      comandoRecebido += c;
    }
  }

  // Verificar fail-safe periodicamente
  verificarFailSafe();

  // Pequeno delay para estabilidade
  delay(10);
}

// ====================================================================
// FUNÇÕES ADICIONAIS (OPCIONAL - PARA EXPANSÕES FUTURAS)
// ====================================================================

/*
 * Teste de motores (pode ser chamado no setup para diagnóstico)
 * Descomente a chamada no setup() para executar
 */
void testeMotores()
{
  Serial.println("\n[TESTE] Iniciando teste de motores...");

  Serial.println("[TESTE] Motor Esquerdo - Frente");
  pwmWrite(CH_L_A, 0);
  pwmWrite(CH_L_B, 150);
  delay(2000);
  pararMotores();
  delay(500);

  Serial.println("[TESTE] Motor Esquerdo - Ré");
  pwmWrite(CH_L_A, 150);
  pwmWrite(CH_L_B, 0);
  delay(2000);
  pararMotores();
  delay(500);

  Serial.println("[TESTE] Motor Direito - Frente");
  pwmWrite(CH_R_A, 0);
  pwmWrite(CH_R_B, 150);
  delay(2000);
  pararMotores();
  delay(500);

  Serial.println("[TESTE] Motor Direito - Ré");
  pwmWrite(CH_R_A, 150);
  pwmWrite(CH_R_B, 0);
  delay(2000);
  pararMotores();
  delay(500);

  Serial.println("[TESTE] Servo - 0° a 180°");
  for (int ang = 0; ang <= 180; ang += 30)
  {
    moverServo(ang);
    delay(500);
  }
  moverServo(90);

  Serial.println("[TESTE] Teste completo!\n");
}
