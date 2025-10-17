# 🤖 Robô de Combate "Rampage" - Documentação ESP32

## 📋 Especificações Técnicas

- **Classe**: Beetleweight (1,36 kg)
- **Dimensões**: 30x30 cm
- **Bateria**: Li-Ion 11,1 V (3S)
- **Motores**: 2x DC 12V com redução (170 RPM)
- **Drivers**: 2x BTS7960 (43A cada)
- **Servo**: MG995
- **Microcontrolador**: ESP32 DevKit

## 🔌 Esquema de Conexões

### Bateria e Reguladores
```
Bateria 11,1V (3S)
├─ LM2596 #1 → 5V (ESP32 + BTS7960)
├─ LM2596 #2 → 6V (Servo MG995)
└─ GND comum conectado entre TODOS os módulos
```

### ESP32 → Motor Esquerdo (BTS7960)
| Pino ESP32 | Função BTS7960 | Descrição |
|------------|----------------|-----------|
| GPIO 25 | L_EN | Enable para ré |
| GPIO 26 | R_EN | Enable para frente |
| GPIO 27 | LPWM/RPWM | Controle PWM velocidade |
| 5V | VCC | Alimentação lógica |
| GND | GND | Terra comum |

### ESP32 → Motor Direito (BTS7960)
| Pino ESP32 | Função BTS7960 | Descrição |
|------------|----------------|-----------|
| GPIO 14 | L_EN | Enable para ré |
| GPIO 12 | R_EN | Enable para frente |
| GPIO 13 | LPWM/RPWM | Controle PWM velocidade |
| 5V | VCC | Alimentação lógica |
| GND | GND | Terra comum |

### ESP32 → Servo MG995
| Pino ESP32 | Função Servo | Descrição |
|------------|--------------|-----------|
| GPIO 4 | Sinal | Controle PWM |
| 6V | VCC | Alimentação (do LM2596 #2) |
| GND | GND | Terra comum |

### BTS7960 → Motores DC
| Driver | Terminal Motor |
|--------|----------------|
| M+ | Motor DC Positivo |
| M- | Motor DC Negativo |
| B+ | Bateria 11,1V |
| B- | Bateria GND |

## 💾 Instalação do Código

### 1. Instalar Arduino IDE
- Baixe em: https://www.arduino.cc/en/software

### 2. Configurar ESP32 no Arduino IDE
```
1. Abra Arduino IDE
2. Vá em: Arquivo → Preferências
3. Em "URLs Adicionais para Gerenciadores de Placas", adicione:
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
4. Vá em: Ferramentas → Placa → Gerenciador de Placas
5. Procure "esp32" e instale "esp32 by Espressif Systems"
```

### 3. Instalar Biblioteca ESP32Servo
```
1. Vá em: Sketch → Incluir Biblioteca → Gerenciar Bibliotecas
2. Procure "ESP32Servo"
3. Instale a biblioteca "ESP32Servo by Kevin Harrington"
```

### 4. Carregar o Código
```
1. Abra o arquivo rampage_esp32.ino
2. Conecte o ESP32 via USB
3. Selecione: Ferramentas → Placa → ESP32 Dev Module
4. Selecione a porta COM correta
5. Clique em Upload (→)
```

## 🎮 Comandos de Controle

### Comandos Básicos
| Comando | Função |
|---------|--------|
| `F` | Mover para frente |
| `B` | Mover para trás (ré) |
| `L` | Girar para esquerda |
| `R` | Girar para direita |
| `S` | Parar todos os motores |

### Controle de Velocidade
| Comando | Velocidade |
|---------|------------|
| `V0` | Mínima (0%) |
| `V5` | Média (55%) |
| `V9` | Máxima (100%) |

### Controle do Servo (Arma)
| Comando | Ângulo |
|---------|--------|
| `A0` | 0° |
| `A90` | 90° (neutro) |
| `A180` | 180° |
| `A45` | 45° |

## 🔍 Monitor Serial

### Abrir Monitor Serial
```
1. No Arduino IDE: Ferramentas → Monitor Serial
2. Configure para 115200 baud
3. Envie comandos pela caixa de texto superior
```

### Exemplo de Saída
```
====================================
  ROBÔ DE COMBATE RAMPAGE - ESP32
====================================
Universidade São Francisco
Classe: Beetleweight (1,36 kg)
====================================

[INIT] Motores inicializados
[INIT] Servo inicializado - Ângulo: 90
[VELOCIDADE] Nível: 9 - PWM: 255

[SISTEMA] Pronto para receber comandos!

[CMD] Comando recebido: F
[MOTOR] Frente - Velocidade: 255

[CMD] Comando recebido: S
[MOTOR] Parado
```

## 🛡️ Sistema Fail-Safe

O código inclui proteção automática:
- Se nenhum comando for recebido por **2 segundos**
- Os motores são **automaticamente parados**
- Previne acidentes em caso de perda de conexão

```
[FAIL-SAFE] Timeout! Parando motores...
[MOTOR] Parado
```

## 📡 Controle via Bluetooth (Opcional)

### Para adicionar controle Bluetooth:

1. **Adicionar módulo Bluetooth Classic HC-05/HC-06:**
   - TX → RX2 (GPIO 16)
   - RX → TX2 (GPIO 17)
   - VCC → 5V
   - GND → GND

2. **Modificar código (adicionar no setup):**
```cpp
Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17
```

3. **Modificar loop para ler também do Serial2:**
```cpp
while (Serial2.available() > 0) {
  char c = Serial2.read();
  // ... mesmo processamento
}
```

## 🔧 Diagnóstico e Testes

### Teste Individual dos Motores
Descomente a linha no `setup()`:
```cpp
void setup() {
  // ... código existente ...

  testeMotores(); // ← Descomentar esta linha

  Serial.println("\n[SISTEMA] Pronto!");
}
```

Isso executará uma sequência de teste automática.

### Verificar Conexões
1. **Sem movimento?**
   - Verificar se GND está comum
   - Verificar alimentação dos BTS7960
   - Testar com V9 (velocidade máxima)

2. **Motor girando ao contrário?**
   - Inverter fios M+ e M- do motor
   - OU inverter EN_L e EN_R no código

3. **Servo não responde?**
   - Verificar alimentação 6V
   - Testar com A0, A90, A180
   - Verificar cabo de sinal no GPIO 4

## ⚙️ Personalização

### Ajustar Velocidade Máxima
No código, linha ~144:
```cpp
int velocidadeAtual = 255;  // Mudar para 200 se muito rápido
```

### Alterar Pinos
No topo do código (linhas 22-35), modifique conforme necessário:
```cpp
#define MOTOR_ESQ_EN_L  25  // Alterar aqui
#define MOTOR_ESQ_EN_R  26
// ...
```

### Desabilitar Fail-Safe
Comentar a linha no `loop()`:
```cpp
void loop() {
  // ...
  // verificarFailSafe(); // ← Comentar esta linha
}
```

## 📊 Consumo Estimado

| Componente | Corrente | Tensão |
|------------|----------|--------|
| ESP32 | ~250mA | 5V |
| Servo MG995 | ~1A (pico 2A) | 6V |
| Motor DC (cada) | ~5-10A | 11,1V |
| **Total estimado** | **15-25A** | **11,1V** |

**Duração da bateria depende da capacidade (ex: 2200mAh = ~6-10 min uso intenso)**

## 🚀 Próximos Passos (Expansões Futuras)

- [ ] Adicionar controle via Wi-Fi (ESP-NOW ou WebSocket)
- [ ] Implementar leitura de sensores (giroscópio MPU6050)
- [ ] Adicionar telemetria (tensão da bateria, temperatura)
- [ ] Criar app mobile para controle
- [ ] Implementar modos pré-programados (autônomo)

## 📞 Suporte

Para dúvidas sobre o código:
1. Verifique os comentários no arquivo `.ino`
2. Teste com Monitor Serial em 115200 baud
3. Consulte a documentação do ESP32Servo

---

**Desenvolvido para o Projeto de Robô de Combate - Universidade São Francisco**
