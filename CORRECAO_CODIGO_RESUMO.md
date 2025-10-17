# ✅ Código Corrigido - Resumo Final

**Data:** 17 de Outubro de 2025
**Status:** ✅ CONCLUÍDO

---

## 🎯 O Que Foi Corrigido

Atualizei o código do robô Rampage para usar corretamente o driver **BTS7960** com o método de **2 PWM por motor** (LPWM e RPWM).

---

## 📝 Arquivos Modificados

### 1. `rampage_esp32.ino` ✅
**Local:** `robo-rampage/rampage_esp32.ino`
**Mudanças:**
- ✅ Configuração de pinos atualizada (2 PWM por motor)
- ✅ 4 canais PWM configurados (CH_L_A, CH_L_B, CH_R_A, CH_R_B)
- ✅ Frequência PWM aumentada para 20 kHz
- ✅ Funções de movimento reescritas (moverFrente, moverRe, girarEsquerda, girarDireita)
- ✅ Função de teste melhorada
- ✅ Helper `pwmWrite()` adicionada

### 2. `main.cpp` (PlatformIO) ✅
**Local:** `Rampage_ESP32/src/main.cpp`
**Mudanças:**
- ✅ Código idêntico ao `.ino`
- ✅ `#include <Arduino.h>` adicionado
- ✅ Sincronizado com a versão 2.0

### 3. `ATUALIZACAO_CODIGO_BTS7960.md` ⭐ NOVO
**Local:** `robo-rampage/ATUALIZACAO_CODIGO_BTS7960.md`
**Conteúdo:**
- Documentação completa das mudanças
- Comparação antes/depois
- Tabela de pinos
- Guia de uso
- Troubleshooting

---

## 🔌 Mapeamento de Pinos Atualizado

### Motor Esquerdo (BTS7960 #1)
```
GPIO 26 → L_EN_L  (Enable L - sempre HIGH)
GPIO 25 → R_EN_L  (Enable R - sempre HIGH)
GPIO 27 → LPWM_L  (PWM para ré)
GPIO 14 → RPWM_L  (PWM para frente)
```

### Motor Direito (BTS7960 #2)
```
GPIO 33 → L_EN_R  (Enable L - sempre HIGH)
GPIO 32 → R_EN_R  (Enable R - sempre HIGH)
GPIO 18 → LPWM_R  (PWM para ré)
GPIO 19 → RPWM_R  (PWM para frente)
```

### Servo (Arma)
```
GPIO 4 → SERVO_PIN
```

---

## 🚀 Como Usar

### 1. Upload do Código

#### Arduino IDE:
```
1. Abrir: robo-rampage/rampage_esp32.ino
2. Ferramentas → Placa → ESP32 Dev Module
3. Selecionar porta COM
4. Clicar em Upload (→)
```

#### PlatformIO:
```
1. Abrir projeto: Rampage_ESP32
2. Clicar em "PlatformIO: Upload" na barra inferior
   OU
3. Terminal: pio run -t upload
```

### 2. Testar no Monitor Serial

**Configuração:** 115200 baud

**Comandos de teste:**
```
S      → Parar (sempre comece com isso)
V3     → Velocidade baixa (teste inicial)
F      → Frente por 2 segundos
S      → Parar
B      → Ré por 2 segundos
S      → Parar
L      → Girar esquerda
S      → Parar
R      → Girar direita
S      → Parar
A0     → Arma 0°
A90    → Arma 90°
A180   → Arma 180°
V9     → Velocidade máxima
```

### 3. Teste Automático (Opcional)

Para executar teste automático, edite o arquivo e descomente:

```cpp
void setup() {
  // ... código de inicialização ...

  testeMotores();  // ← DESCOMENTAR ESTA LINHA

  // ... resto do setup ...
}
```

O teste automático executa:
1. Motor esquerdo frente (2s)
2. Motor esquerdo ré (2s)
3. Motor direito frente (2s)
4. Motor direito ré (2s)
5. Servo 0° → 180° (passos de 30°)

---

## ⚡ Diferenças Principais

### Método Antigo (Incorreto):
```cpp
// 1 PWM por motor + 2 Enable para direção
digitalWrite(MOTOR_ESQ_EN_L, HIGH);  // Define direção
digitalWrite(MOTOR_ESQ_EN_R, LOW);
ledcWrite(PWM_CANAL, velocidade);    // Define velocidade
```

### Método Novo (Correto):
```cpp
// 2 PWM por motor (LPWM para ré, RPWM para frente)
// Enable sempre HIGH
pwmWrite(CH_L_A, 0);           // LPWM = 0 (não vai ré)
pwmWrite(CH_L_B, velocidade);  // RPWM = vel (vai frente)
```

---

## 🔧 Vantagens do Novo Método

| Aspecto | Melhoria |
|---------|----------|
| **Controle** | Mais preciso e responsivo |
| **Eficiência** | Menor aquecimento do driver |
| **Ruído** | 20 kHz = inaudível para humanos |
| **Torque** | Melhor distribuição de potência |
| **Código** | Mais limpo e organizado |

---

## ⚠️ Checklist de Segurança

Antes de ligar pela primeira vez:

- [ ] ✅ Todas as conexões verificadas
- [ ] ✅ GND comum conectado (ESP32 + BTS7960 + Bateria)
- [ ] ✅ Polaridade da bateria correta
- [ ] ✅ Reguladores ajustados (5V e 6V)
- [ ] ✅ Código uploadado com sucesso
- [ ] ✅ Motor Serial funcionando (115200 baud)
- [ ] ✅ Robô elevado (rodas sem tocar o chão)
- [ ] ✅ Começar com velocidade baixa (V3)
- [ ] ✅ Botão de emergência pronto (desconectar bateria)

---

## 🐛 Troubleshooting

### Motor não gira:
1. Verificar conexões dos pinos PWM
2. Verificar se Enable está em HIGH
3. Testar com velocidade V9
4. Verificar GND comum

### Motor gira ao contrário:
**Solução 1:** Inverter fios M+ e M- do motor
**Solução 2:** No código, trocar LPWM com RPWM

### Servo não responde:
1. Verificar GPIO 4 conectado
2. Verificar alimentação 6V
3. Testar comandos A0, A90, A180

### Erro ao compilar:
1. Verificar biblioteca ESP32Servo instalada
2. Verificar placa ESP32 selecionada
3. Verificar porta COM correta

---

## 📊 Consumo de Pinos

| Pino GPIO | Função | Componente |
|-----------|--------|------------|
| 4 | Servo PWM | MG995 |
| 14 | RPWM_L | BTS7960 #1 |
| 18 | LPWM_R | BTS7960 #2 |
| 19 | RPWM_R | BTS7960 #2 |
| 25 | R_EN_L | BTS7960 #1 |
| 26 | L_EN_L | BTS7960 #1 |
| 27 | LPWM_L | BTS7960 #1 |
| 32 | R_EN_R | BTS7960 #2 |
| 33 | L_EN_R | BTS7960 #2 |

**Total:** 9 pinos GPIO utilizados

---

## 📚 Documentação Relacionada

- `README.md` - Visão geral do projeto
- `INSTRUCOES.md` - Manual completo de instalação
- `DIAGRAMA_CONEXOES.md` - Esquemas de fiação
- `ATUALIZACAO_CODIGO_BTS7960.md` - Documentação técnica detalhada
- `RELATORIO_MONTAGEM_1.md` - Relatório de montagem

---

## ✨ Próximos Passos

### Imediato:
- [ ] Upload do código no ESP32
- [ ] Teste básico dos motores
- [ ] Teste do servo
- [ ] Calibração de velocidades

### Curto Prazo:
- [ ] Montagem mecânica completa
- [ ] Testes de tração com rodas
- [ ] Ajuste de PID (se necessário)
- [ ] Implementação de controle remoto

### Médio Prazo:
- [ ] Adicionar módulo Bluetooth
- [ ] Desenvolver app de controle
- [ ] Telemetria da bateria
- [ ] Sensores IMU

---

## 🎓 Equipe

- **Matheus Abrahão Martins Alvares** - 202204581
- **Cauê Becker Rossi** - 202204081
- **Bruno Ricci Zanoni** - 202213443
- **Giulio Passetti Figueiredo** - 202201837

**UNIVERSIDADE SÃO FRANCISCO**
Engenharia de Computação - 8º Semestre

---

## 📅 Histórico de Versões

| Versão | Data | Mudanças |
|--------|------|----------|
| 1.0 | Out 2025 | Versão inicial (1 PWM) |
| **2.0** | **17/10/2025** | **Corrigido para 2 PWM (BTS7960)** ✅ |

---

**Status:** ✅ Código corrigido e pronto para uso
**Última atualização:** 17 de Outubro de 2025

---

<div align="center">

🤖 **RAMPAGE** ⚔️

*"Beetleweight de combate - Código otimizado para BTS7960"*

</div>
