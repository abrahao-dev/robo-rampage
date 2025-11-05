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

#include <BluetoothSerial.h>
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
Servo servoPa;

// Motores - Ponte H 1 (Esquerda)
#define RPWM_ESQ 18
#define LPWM_ESQ 19

// Motores - Ponte H 2 (Direita)
#define RPWM_DIR 25
#define LPWM_DIR 26

// Servo
#define SERVO_PIN 27

// LED indicador
#define LED_PIN 2

int posServo = 90;      // posição inicial da pá

void setup() {
  Serial.begin(9600);
  SerialBT.begin("RoboCombate_ESP32");
  servoPa.attach(SERVO_PIN);

  pinMode(RPWM_ESQ, OUTPUT);
  pinMode(LPWM_ESQ, OUTPUT);
  pinMode(RPWM_DIR, OUTPUT);
  pinMode(LPWM_DIR, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, HIGH);
  servoPa.write(posServo);

  Serial.println("Robô de Combate pronto!");
}

void loop() {
  if (SerialBT.available()) {
    char comando = SerialBT.read();
    Serial.println(comando);

    switch (comando) {
      case 'F': moverFrente(); break;     // frente
      case 'B': moverRe(); break;         // ré
      case 'L': girarEsquerda(); break;   // esquerda
      case 'R': girarDireita(); break;    // direita
      case 'U': subirPa(); break;
      case 'D': descerPa(); break;
      case 'S': parar(); break;
    }
  }
}

// --- Funções de movimento ---
void moverFrente() {
  analogWrite(RPWM_ESQ, 255);
  analogWrite(LPWM_ESQ, 0);
  analogWrite(RPWM_DIR, 255);
  analogWrite(LPWM_DIR, 0);
}

void moverRe() {
  analogWrite(RPWM_ESQ, 0);
  analogWrite(LPWM_ESQ, 255);
  analogWrite(RPWM_DIR, 0);
  analogWrite(LPWM_DIR, 255);
}

void girarEsquerda() {
  analogWrite(RPWM_ESQ, 0);
  analogWrite(LPWM_ESQ, 255);
  analogWrite(RPWM_DIR, 255);
  analogWrite(LPWM_DIR, 0);
}

void girarDireita() {
  analogWrite(RPWM_ESQ, 255);
  analogWrite(LPWM_ESQ, 0);
  analogWrite(RPWM_DIR, 0);
  analogWrite(LPWM_DIR, 255);
}

void parar() {
  analogWrite(RPWM_ESQ, 0);
  analogWrite(LPWM_ESQ, 0);
  analogWrite(RPWM_DIR, 0);
  analogWrite(LPWM_DIR, 0);
}

// --- Controle da pá ---
void subirPa() {
  posServo = constrain(posServo + 135, 0, 135);
  servoPa.write(posServo);
}

void descerPa() {
  posServo = constrain(posServo - 135, 0, 135);
  servoPa.write(posServo);
}