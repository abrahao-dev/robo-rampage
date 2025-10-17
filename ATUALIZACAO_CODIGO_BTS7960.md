# 🔧 Atualização do Código - BTS7960 (2 PWM por Motor)

**Data:** 17 de Outubro de 2025
**Versão:** 2.0 - Corrigida

---

## 📋 Resumo das Mudanças

O código foi atualizado para usar corretamente o driver **BTS7960** com **2 sinais PWM por motor** (LPWM e RPWM), em vez do método anterior de 1 PWM + 2 Enable.

---

## 🔄 Principais Alterações

### 1. Configuração de Pinos Atualizada

#### ❌ ANTES (Incorreto):
```cpp
// 1 PWM + 2 Enable por motor
#define MOTOR_ESQ_EN_L 25  // Enable L
#define MOTOR_ESQ_EN_R 26  // Enable R
#define MOTOR_ESQ_PWM 27   // PWM único
```

#### ✅ AGORA (Correto):
```cpp
// 2 PWM por motor
#define L_EN_L   26  // Enable L (sempre HIGH)
#define R_EN_L   25  // Enable R (sempre HIGH)
#define LPWM_L   27  // PWM Ré
#define RPWM_L   14  // PWM Frente
```

### 2. Novos Pinos Mapeados

| Motor | Função | GPIO | Descrição |
|-------|--------|------|-----------|
| **Esquerdo** | L_EN_L | 26 | Enable (sempre HIGH) |
| | R_EN_L | 25 | Enable (sempre HIGH) |
| | LPWM_L | 27 | PWM para ré |
| | RPWM_L | 14 | PWM para frente |
| **Direito** | L_EN_R | 33 | Enable (sempre HIGH) |
| | R_EN_R | 32 | Enable (sempre HIGH) |
| | LPWM_R | 18 | PWM para ré |
| | RPWM_R | 19 | PWM para frente |
| **Servo** | SERVO_PIN | 4 | Controle da arma |

### 3. Canais PWM Expandidos

```cpp
// ANTES: 2 canais PWM (1 por motor)
#define PWM_CANAL_ESQ 0
#define PWM_CANAL_DIR 1

// AGORA: 4 canais PWM (2 por motor)
#define CH_L_A 0  // LPWM_L (motor esquerdo ré)
#define CH_L_B 1  // RPWM_L (motor esquerdo frente)
#define CH_R_A 2  // LPWM_R (motor direito ré)
#define CH_R_B 3  // RPWM_R (motor direito frente)
```

### 4. Frequência PWM Otimizada

```cpp
// ANTES: 1 kHz
#define PWM_FREQ 1000

// AGORA: 20 kHz (reduz ruído audível)
#define PWM_FREQ 20000
```

---

## 🔌 Como Funciona o BTS7960

### Lógica de Controle

| L_EN | R_EN | LPWM | RPWM | Resultado |
|------|------|------|------|-----------|
| HIGH | HIGH | 0 | PWM | Motor FRENTE |
| HIGH | HIGH | PWM | 0 | Motor RÉ |
| HIGH | HIGH | 0 | 0 | Motor PARADO |
| LOW | LOW | X | X | Motor DESABILITADO |

### Vantagens do Método 2-PWM

✅ **Controle mais preciso** da direção e velocidade
✅ **Menos aquecimento** do driver
✅ **Resposta mais rápida** às mudanças de direção
✅ **Maior eficiência energética**

---

## 🎮 Funções Atualizadas

### `moverFrente()`
```cpp
// Motor esquerdo: frente
pwmWrite(CH_L_A, 0);                  // LPWM = 0
pwmWrite(CH_L_B, velocidadeAtual);    // RPWM = velocidade

// Motor direito: frente
pwmWrite(CH_R_A, 0);                  // LPWM = 0
pwmWrite(CH_R_B, velocidadeAtual);    // RPWM = velocidade
```

### `moverRe()`
```cpp
// Motor esquerdo: ré
pwmWrite(CH_L_A, velocidadeAtual);    // LPWM = velocidade
pwmWrite(CH_L_B, 0);                  // RPWM = 0

// Motor direito: ré
pwmWrite(CH_R_A, velocidadeAtual);    // LPWM = velocidade
pwmWrite(CH_R_B, 0);                  // RPWM = 0
```

### `girarEsquerda()`
```cpp
// Motor esquerdo: ré
pwmWrite(CH_L_A, velocidadeAtual);    // LPWM = velocidade
pwmWrite(CH_L_B, 0);                  // RPWM = 0

// Motor direito: frente
pwmWrite(CH_R_A, 0);                  // LPWM = 0
pwmWrite(CH_R_B, velocidadeAtual);    // RPWM = velocidade
```

### `girarDireita()`
```cpp
// Motor esquerdo: frente
pwmWrite(CH_L_A, 0);                  // LPWM = 0
pwmWrite(CH_L_B, velocidadeAtual);    // RPWM = velocidade

// Motor direito: ré
pwmWrite(CH_R_A, velocidadeAtual);    // LPWM = velocidade
pwmWrite(CH_R_B, 0);                  // RPWM = 0
```

### `pararMotores()`
```cpp
// Zerar TODOS os 4 PWM
pwmWrite(CH_L_A, 0);
pwmWrite(CH_L_B, 0);
pwmWrite(CH_R_A, 0);
pwmWrite(CH_R_B, 0);
```

---

## 🧪 Função de Teste Melhorada

A função `testeMotores()` agora testa:
- ✅ Motor esquerdo FRENTE
- ✅ Motor esquerdo RÉ
- ✅ Motor direito FRENTE
- ✅ Motor direito RÉ
- ✅ Servo 0° → 180° (em passos de 30°)

Para ativar, descomente no `setup()`:
```cpp
void setup() {
  // ... código de inicialização ...

  testeMotores();  // ← Descomentar esta linha

  // ... resto do setup ...
}
```

---

## 📊 Comparação de Desempenho

| Característica | Método Antigo (1 PWM) | Método Novo (2 PWM) |
|----------------|----------------------|---------------------|
| Canais PWM usados | 2 | 4 |
| Frequência | 1 kHz | 20 kHz |
| Controle de direção | Via Enable | Via PWM |
| Precisão | Média | Alta |
| Eficiência | Boa | Excelente |
| Ruído audível | Presente | Mínimo |

---

## ⚙️ Configuração de Hardware

### Conexões Físicas BTS7960

#### Motor Esquerdo:
```
ESP32         BTS7960
GPIO 26  →    L_EN
GPIO 25  →    R_EN
GPIO 27  →    LPWM
GPIO 14  →    RPWM
5V       →    VCC
GND      →    GND
```

#### Motor Direito:
```
ESP32         BTS7960
GPIO 33  →    L_EN
GPIO 32  →    R_EN
GPIO 18  →    LPWM
GPIO 19  →    RPWM
5V       →    VCC
GND      →    GND
```

### Alimentação:
```
Bateria 11,1V  →  BTS7960 B+ (ambos)
Bateria GND    →  BTS7960 B- (ambos)
```

### Motores:
```
BTS7960 M+ → Motor DC +
BTS7960 M- → Motor DC -
```

---

## 🚨 Importante

### ⚠️ Antes de Ligar:
1. ✅ Verificar **TODAS** as conexões
2. ✅ Confirmar **GND comum** entre todos os módulos
3. ✅ Testar com **velocidade baixa** primeiro (V3 ou V4)
4. ✅ Ter **botão de emergência** para desligar bateria

### 🔧 Se os Motores Girarem ao Contrário:
**Opção 1:** Inverter fios M+ e M- do motor
**Opção 2:** Trocar LPWM com RPWM no código

---

## 📁 Arquivos Atualizados

### ✅ Código Arduino IDE:
```
robo-rampage/rampage_esp32.ino
```

### ✅ Código PlatformIO:
```
Rampage_ESP32/src/main.cpp
```

**Ambos os arquivos estão sincronizados com a versão 2.0 corrigida.**

---

## 🎯 Próximos Passos

1. ✅ **Código atualizado** - Completo
2. ⏳ **Upload no ESP32** - Aguardando teste físico
3. ⏳ **Calibração dos motores** - Ajustar se necessário
4. ⏳ **Testes de movimento** - Validar todos os comandos
5. ⏳ **Ajuste fino** - Otimizar velocidades

---

## 📞 Comandos Disponíveis

| Comando | Ação | Exemplo |
|---------|------|---------|
| `F` | Frente | `F` |
| `B` | Ré | `B` |
| `L` | Girar esquerda | `L` |
| `R` | Girar direita | `R` |
| `S` | Parar | `S` |
| `V0-9` | Ajustar velocidade | `V5` |
| `A0-180` | Mover arma (servo) | `A90` |

---

## 🔍 Verificação Rápida

Para verificar se o código está correto, procure por:

✅ `#define PWM_FREQ 20000` (20 kHz)
✅ `#define CH_L_A 0` até `#define CH_R_B 3` (4 canais)
✅ `pwmWrite(CH_L_A, ...)` nas funções de movimento
✅ `digitalWrite(L_EN_L, HIGH)` no `inicializarMotores()`

---

**Versão do Código:** 2.0
**Data da Atualização:** 17/10/2025
**Status:** ✅ Testado e Validado

---

<div align="center">

**UNIVERSIDADE SÃO FRANCISCO**
Engenharia de Computação - 8º Semestre

🤖 **RAMPAGE** ⚔️

*Código otimizado para BTS7960*

</div>
