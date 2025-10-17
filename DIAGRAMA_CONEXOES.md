# 🔌 Diagrama de Conexões - Robô Rampage

## Esquema Completo de Fiação

```
                        BATERIA Li-Ion 11,1V (3S)
                                 │
                    ┌────────────┴────────────┐
                    │                         │
              LM2596 #1 (5V)           LM2596 #2 (6V)
                    │                         │
        ┌───────────┴──────────┐              │
        │                      │              │
    ESP32 5V              BTS7960 VCC         │
                         (ambos)              │
                                              │
                                        SERVO MG995 VCC


══════════════════════════════════════════════════════════════════════

                            ESP32 DevKit
                    ┌───────────────────────┐
                    │                       │
                    │   GPIO 25 ────────────┼──► Motor ESQ EN_L
                    │   GPIO 26 ────────────┼──► Motor ESQ EN_R
                    │   GPIO 27 ────────────┼──► Motor ESQ PWM
                    │                       │
                    │   GPIO 14 ────────────┼──► Motor DIR EN_L
                    │   GPIO 12 ────────────┼──► Motor DIR EN_R
                    │   GPIO 13 ────────────┼──► Motor DIR PWM
                    │                       │
                    │   GPIO 4  ────────────┼──► SERVO Sinal
                    │                       │
                    │   5V ──────────────┬──┼──► BTS7960 VCC (ambos)
                    │                    │  │
                    │   GND ─────────────┼──┼──► GND COMUM
                    │                    │  │
                    └────────────────────┼──┘
                                         │
                                    GND COMUM
══════════════════════════════════════════════════════════════════════

                         BTS7960 MOTOR ESQUERDO
                    ┌───────────────────────┐
                    │                       │
ESP32 GPIO 25 ──────┤ L_EN                  │
ESP32 GPIO 26 ──────┤ R_EN                  │
ESP32 GPIO 27 ──────┤ LPWM/RPWM             │
ESP32 5V ───────────┤ VCC                   │
GND COMUM ──────────┤ GND                   │
                    │                       │
Bateria 11,1V+ ─────┤ B+                    │
Bateria GND ────────┤ B-                    │
                    │                       │
Motor ESQ + ────────┤ M+                    │
Motor ESQ - ────────┤ M-                    │
                    │                       │
                    └───────────────────────┘

══════════════════════════════════════════════════════════════════════

                         BTS7960 MOTOR DIREITO
                    ┌───────────────────────┐
                    │                       │
ESP32 GPIO 14 ──────┤ L_EN                  │
ESP32 GPIO 12 ──────┤ R_EN                  │
ESP32 GPIO 13 ──────┤ LPWM/RPWM             │
ESP32 5V ───────────┤ VCC                   │
GND COMUM ──────────┤ GND                   │
                    │                       │
Bateria 11,1V+ ─────┤ B+                    │
Bateria GND ────────┤ B-                    │
                    │                       │
Motor DIR + ────────┤ M+                    │
Motor DIR - ────────┤ M-                    │
                    │                       │
                    └───────────────────────┘

══════════════════════════════════════════════════════════════════════

                            SERVO MG995
                    ┌───────────────────────┐
                    │                       │
ESP32 GPIO 4 ───────┤ Sinal (PWM)           │
LM2596 #2 6V ───────┤ VCC (Vermelho)        │
GND COMUM ──────────┤ GND (Preto/Marrom)    │
                    │                       │
                    └───────────────────────┘

══════════════════════════════════════════════════════════════════════

                        REGULADORES DE TENSÃO

        LM2596 #1 (5V)              LM2596 #2 (6V)
    ┌─────────────────┐         ┌─────────────────┐
    │                 │         │                 │
    │ IN+ ← Bat 11,1V │         │ IN+ ← Bat 11,1V │
    │ IN- ← Bat GND   │         │ IN- ← Bat GND   │
    │                 │         │                 │
    │ OUT+ → 5V       │         │ OUT+ → 6V       │
    │ OUT- → GND      │         │ OUT- → GND      │
    │                 │         │                 │
    └─────────────────┘         └─────────────────┘
            │                           │
            ├── ESP32                   └── SERVO MG995
            └── BTS7960 (ambos)

══════════════════════════════════════════════════════════════════════
```

## 🔴 IMPORTANTE: GND COMUM

**TODOS os GNDs devem estar conectados juntos:**

```
    Bateria GND
         │
    ┌────┴────┬─────────┬─────────┬─────────┬──────────┐
    │         │         │         │         │          │
  ESP32    BTS7960   BTS7960   LM2596    LM2596     SERVO
  GND      ESQ GND   DIR GND   #1 GND    #2 GND     GND
```

## 📐 Layout Físico Sugerido (Vista Superior)

```
┌──────────────────────────────────────────┐
│                                          │
│  ┌──────┐              ┌──────┐         │
│  │Motor │              │Motor │         │
│  │ ESQ  │              │ DIR  │         │
│  └───┬──┘              └──┬───┘         │
│      │                    │             │
│  ┌───┴────┐          ┌────┴───┐        │
│  │BTS7960 │          │BTS7960 │        │
│  │  ESQ   │          │  DIR   │        │
│  └────────┘          └────────┘        │
│                                         │
│         ┌──────────┐                    │
│         │  ESP32   │                    │
│         │ DevKit   │    ┌──────┐        │
│         └──────────┘    │ ARMA │        │
│                         │      │        │
│  ┌────────┐             │Servo │        │
│  │LM2596  │             └──────┘        │
│  │  5V    │                             │
│  └────────┘  ┌────────┐                 │
│              │LM2596  │                 │
│              │  6V    │                 │
│              └────────┘                 │
│                                         │
│         ┌──────────────┐                │
│         │   BATERIA    │                │
│         │   11,1V 3S   │                │
│         └──────────────┘                │
│                                         │
└──────────────────────────────────────────┘
     30cm x 30cm (Beetleweight)
```

## 🔧 Checklist de Montagem

### Passo 1: Preparar Alimentação
- [ ] Conectar bateria 11,1V nos dois reguladores LM2596
- [ ] Ajustar LM2596 #1 para 5,0V (multímetro)
- [ ] Ajustar LM2596 #2 para 6,0V (multímetro)
- [ ] Verificar polaridade (+/-)

### Passo 2: Conectar ESP32
- [ ] 5V do LM2596 #1 → ESP32 VIN ou 5V
- [ ] GND comum → ESP32 GND
- [ ] Verificar LED de alimentação do ESP32

### Passo 3: Conectar BTS7960 Motor Esquerdo
- [ ] VCC → 5V (LM2596 #1)
- [ ] GND → GND comum
- [ ] L_EN → GPIO 25
- [ ] R_EN → GPIO 26
- [ ] LPWM/RPWM → GPIO 27
- [ ] B+ → Bateria 11,1V+
- [ ] B- → Bateria GND
- [ ] M+ e M- → Motor esquerdo

### Passo 4: Conectar BTS7960 Motor Direito
- [ ] VCC → 5V (LM2596 #1)
- [ ] GND → GND comum
- [ ] L_EN → GPIO 14
- [ ] R_EN → GPIO 12
- [ ] LPWM/RPWM → GPIO 13
- [ ] B+ → Bateria 11,1V+
- [ ] B- → Bateria GND
- [ ] M+ e M- → Motor direito

### Passo 5: Conectar Servo MG995
- [ ] Fio Vermelho → 6V (LM2596 #2)
- [ ] Fio Preto/Marrom → GND comum
- [ ] Fio Laranja/Amarelo → GPIO 4

### Passo 6: Verificações Finais
- [ ] TODOS os GNDs conectados juntos
- [ ] Polaridades corretas (+/-)
- [ ] Fios bem fixados (solda ou conectores)
- [ ] Sem curto-circuitos
- [ ] Bateria desconectada durante montagem

## ⚠️ Avisos de Segurança

1. **SEMPRE desconectar bateria ao fazer alterações**
2. **Verificar polaridade antes de ligar**
3. **Não inverter VCC e GND**
4. **Usar fios adequados para corrente (mínimo 18 AWG para motores)**
5. **BTS7960 pode esquentar - prever ventilação/dissipador**
6. **Bateria Li-Ion requer cuidado - não descarregar abaixo de 9V**

## 🧪 Teste Passo a Passo

### Teste 1: Alimentação
```
1. Conectar apenas bateria e reguladores
2. Medir saída: LM2596 #1 = 5V? ✓
3. Medir saída: LM2596 #2 = 6V? ✓
```

### Teste 2: ESP32
```
1. Conectar ESP32 ao 5V
2. LED do ESP32 acende? ✓
3. Conectar USB e enviar código
```

### Teste 3: Motores
```
1. Com bateria conectada, enviar: V5 (vel. média)
2. Enviar: F (frente)
3. Ambos motores giram? ✓
4. Enviar: S (parar)
```

### Teste 4: Servo
```
1. Enviar: A0 → Servo vai para 0°? ✓
2. Enviar: A90 → Servo vai para 90°? ✓
3. Enviar: A180 → Servo vai para 180°? ✓
```

---

**Boa sorte no desenvolvimento do Rampage! 🤖⚔️**
