/*
 * ====================================================================
 * ROBÔ DE COMBATE "RAMPAGE" - ESP32 CONTROLLER
 * ====================================================================
 * Projeto: Engenharia de Computação - Universidade São Francisco
 * Classe: Beetleweight (1,36 kg, 30x30 cm)
 * Bateria: Li-Ion 11,1 V (3S)
 *
 * COMPONENTES:
 * - 2x Motores DC 12V com redução (170 RPM)
 * - 2x Drivers BTS7960 (43A cada)
 * - 1x Servo MG995 (arma)
 * - 2x Reguladores LM2596 (5V e 6V)
 * - ESP32 DevKit
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
// CONFIGURAÇÃO DE PINOS
// ====================================================================

// Motor Esquerdo (BTS7960)
#define MOTOR_ESQ_EN_L 25 // Enable L (controla direção reversa)
#define MOTOR_ESQ_EN_R 26 // Enable R (controla direção frente)
#define MOTOR_ESQ_PWM 27  // PWM (controla velocidade)

// Motor Direito (BTS7960)
#define MOTOR_DIR_EN_L 14 // Enable L (controla direção reversa)
#define MOTOR_DIR_EN_R 12 // Enable R (controla direção frente)
#define MOTOR_DIR_PWM 13  // PWM (controla velocidade)

// Servo da Arma
#define SERVO_PIN 4 // GPIO4 para controle do servo MG995

// ====================================================================
// CONFIGURAÇÕES PWM
// ====================================================================

// Canais PWM do ESP32 (0-15 disponíveis)
#define PWM_CANAL_ESQ 0
#define PWM_CANAL_DIR 1

// Configurações PWM
#define PWM_FREQ 1000   // Frequência PWM em Hz
#define PWM_RESOLUCAO 8 // Resolução de 8 bits (0-255)

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
// FUNÇÕES DE CONTROLE DOS MOTORES
// ====================================================================

/*
 * Inicializa os pinos dos motores e configura PWM
 */
void inicializarMotores()
{
  // Configurar pinos do motor esquerdo
  pinMode(MOTOR_ESQ_EN_L, OUTPUT);
  pinMode(MOTOR_ESQ_EN_R, OUTPUT);
  pinMode(MOTOR_ESQ_PWM, OUTPUT);

  // Configurar pinos do motor direito
  pinMode(MOTOR_DIR_EN_L, OUTPUT);
  pinMode(MOTOR_DIR_EN_R, OUTPUT);
  pinMode(MOTOR_DIR_PWM, OUTPUT);

  // Configurar canais PWM
  ledcSetup(PWM_CANAL_ESQ, PWM_FREQ, PWM_RESOLUCAO);
  ledcSetup(PWM_CANAL_DIR, PWM_FREQ, PWM_RESOLUCAO);

  // Vincular pinos aos canais PWM
  ledcAttachPin(MOTOR_ESQ_PWM, PWM_CANAL_ESQ);
  ledcAttachPin(MOTOR_DIR_PWM, PWM_CANAL_DIR);

  // Parar motores inicialmente
  pararMotores();

  Serial.println("[INIT] Motores inicializados");
}

/*
 * Para todos os motores
 */
void pararMotores()
{
  // Desabilitar todas as direções
  digitalWrite(MOTOR_ESQ_EN_L, LOW);
  digitalWrite(MOTOR_ESQ_EN_R, LOW);
  digitalWrite(MOTOR_DIR_EN_L, LOW);
  digitalWrite(MOTOR_DIR_EN_R, LOW);

  // Zerar PWM
  ledcWrite(PWM_CANAL_ESQ, 0);
  ledcWrite(PWM_CANAL_DIR, 0);

  Serial.println("[MOTOR] Parado");
}

/*
 * Move o robô para frente
 */
void moverFrente()
{
  // Motor esquerdo: frente
  digitalWrite(MOTOR_ESQ_EN_L, LOW);
  digitalWrite(MOTOR_ESQ_EN_R, HIGH);
  ledcWrite(PWM_CANAL_ESQ, velocidadeAtual);

  // Motor direito: frente
  digitalWrite(MOTOR_DIR_EN_L, LOW);
  digitalWrite(MOTOR_DIR_EN_R, HIGH);
  ledcWrite(PWM_CANAL_DIR, velocidadeAtual);

  Serial.print("[MOTOR] Frente - Velocidade: ");
  Serial.println(velocidadeAtual);
}

/*
 * Move o robô para trás
 */
void moverRe()
{
  // Motor esquerdo: ré
  digitalWrite(MOTOR_ESQ_EN_L, HIGH);
  digitalWrite(MOTOR_ESQ_EN_R, LOW);
  ledcWrite(PWM_CANAL_ESQ, velocidadeAtual);

  // Motor direito: ré
  digitalWrite(MOTOR_DIR_EN_L, HIGH);
  digitalWrite(MOTOR_DIR_EN_R, LOW);
  ledcWrite(PWM_CANAL_DIR, velocidadeAtual);

  Serial.print("[MOTOR] Ré - Velocidade: ");
  Serial.println(velocidadeAtual);
}

/*
 * Gira o robô para a esquerda (motor direito frente, esquerdo ré)
 */
void girarEsquerda()
{
  // Motor esquerdo: ré
  digitalWrite(MOTOR_ESQ_EN_L, HIGH);
  digitalWrite(MOTOR_ESQ_EN_R, LOW);
  ledcWrite(PWM_CANAL_ESQ, velocidadeAtual);

  // Motor direito: frente
  digitalWrite(MOTOR_DIR_EN_L, LOW);
  digitalWrite(MOTOR_DIR_EN_R, HIGH);
  ledcWrite(PWM_CANAL_DIR, velocidadeAtual);

  Serial.print("[MOTOR] Girando ESQUERDA - Velocidade: ");
  Serial.println(velocidadeAtual);
}

/*
 * Gira o robô para a direita (motor esquerdo frente, direito ré)
 */
void girarDireita()
{
  // Motor esquerdo: frente
  digitalWrite(MOTOR_ESQ_EN_L, LOW);
  digitalWrite(MOTOR_ESQ_EN_R, HIGH);
  ledcWrite(PWM_CANAL_ESQ, velocidadeAtual);

  // Motor direito: ré
  digitalWrite(MOTOR_DIR_EN_L, HIGH);
  digitalWrite(MOTOR_DIR_EN_R, LOW);
  ledcWrite(PWM_CANAL_DIR, velocidadeAtual);

  Serial.print("[MOTOR] Girando DIREITA - Velocidade: ");
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
  if (angulo < 0)
    angulo = 0;
  if (angulo > 180)
    angulo = 180;

  anguloServo = angulo;
  servoArma.write(anguloServo);

  Serial.print("[SERVO] Ângulo ajustado para: ");
  Serial.println(anguloServo);
}

// ====================================================================
// FUNÇÕES DE CONTROLE DE VELOCIDADE
// ====================================================================

/*
 * Ajusta a velocidade dos motores (0-9)
 */
void ajustarVelocidade(int nivel)
{
  if (nivel < 0)
    nivel = 0;
  if (nivel > 9)
    nivel = 9;

  nivelVelocidade = nivel;
  // Mapear nível 0-9 para PWM 0-255
  velocidadeAtual = map(nivel, 0, 9, 0, 255);

  Serial.print("[VELOCIDADE] Nível: ");
  Serial.print(nivelVelocidade);
  Serial.print(" - PWM: ");
  Serial.println(velocidadeAtual);
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

  Serial.print("[CMD] Comando recebido: ");
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
  else if (cmd.startsWith("A") && cmd.length() >= 2)
  {
    int angulo = cmd.substring(1).toInt();
    moverServo(angulo);
  }
  else
  {
    Serial.println("[CMD] Comando não reconhecido!");
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
  unsigned long tempoAtual = millis();

  // Verificar overflow do millis() (acontece após ~50 dias)
  if (tempoAtual < ultimoComando)
  {
    ultimoComando = tempoAtual;
  }

  // Se passou mais de 2 segundos sem comando, parar motores
  if ((tempoAtual - ultimoComando) > TIMEOUT_FAILSAFE)
  {
    static bool failsafeAtivado = false;

    if (!failsafeAtivado)
    {
      Serial.println("[FAIL-SAFE] Timeout! Parando motores...");
      pararMotores();
      failsafeAtivado = true;
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
  delay(500);

  Serial.println("\n\n");
  Serial.println("====================================");
  Serial.println("  ROBÔ DE COMBATE RAMPAGE - ESP32");
  Serial.println("====================================");
  Serial.println("Universidade São Francisco");
  Serial.println("Classe: Beetleweight (1,36 kg)");
  Serial.println("====================================\n");

  // Inicializar componentes
  inicializarMotores();
  inicializarServo();
  ajustarVelocidade(9); // Velocidade máxima inicial

  // Inicializar timestamp do fail-safe
  ultimoComando = millis();

  Serial.println("\n[SISTEMA] Pronto para receber comandos!");
  Serial.println("\nComandos disponíveis:");
  Serial.println("  F    = Frente");
  Serial.println("  B    = Ré");
  Serial.println("  L    = Girar Esquerda");
  Serial.println("  R    = Girar Direita");
  Serial.println("  S    = Parar");
  Serial.println("  V0-9 = Velocidade (0=mín, 9=máx)");
  Serial.println("  Axx  = Servo ângulo (0-180)");
  Serial.println("\nFail-safe: 2s sem comando = parar\n");
}

void loop()
{
  // Verificar se há dados disponíveis na serial
  while (Serial.available() > 0)
  {
    char c = Serial.read();

    // Se recebeu nova linha ou carriage return, processar comando
    if (c == '\n' || c == '\r')
    {
      if (comandoRecebido.length() > 0)
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
 */
void testeMotores()
{
  Serial.println("\n[TESTE] Iniciando teste de motores...");

  Serial.println("[TESTE] Motor Esquerdo - Frente");
  digitalWrite(MOTOR_ESQ_EN_L, LOW);
  digitalWrite(MOTOR_ESQ_EN_R, HIGH);
  ledcWrite(PWM_CANAL_ESQ, 150);
  delay(2000);
  pararMotores();
  delay(500);

  Serial.println("[TESTE] Motor Direito - Frente");
  digitalWrite(MOTOR_DIR_EN_L, LOW);
  digitalWrite(MOTOR_DIR_EN_R, HIGH);
  ledcWrite(PWM_CANAL_DIR, 150);
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
