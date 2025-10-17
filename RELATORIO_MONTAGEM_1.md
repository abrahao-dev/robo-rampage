# 🔧 Robô Rampage - Relatório de Montagem 1

**UNIVERSIDADE SÃO FRANCISCO**
Engenharia de Computação - 8º Semestre
Prática Profissional: Sistemas Autônomos e Robótica

---

## 👥 Equipe

- **Matheus Abrahão Martins Alvares** - 202204581
- **Cauê Becker Rossi** - 202204081
- **Bruno Ricci Zanoni** - 202213443
- **Giulio Passetti Figueiredo** - 202201837

---

## 1. Introdução

Este relatório descreve a primeira fase do projeto "Robô Rampage": a montagem inicial dos componentes eletrônicos e a preparação para a integração mecânica. O objetivo desta etapa foi realizar as conexões elétricas essenciais, soldagens necessárias e a montagem provisória do circuito principal que controla o robô, garantindo que os componentes principais se comuniquem e funcionem como esperado. As tarefas foram distribuídas entre os membros da equipe para otimizar o processo.

---

## 2. Componentes Utilizados e Custos

### 2.1 Componentes Eletrônicos Principais

| Componente | Quantidade | Preço Unitário | Total |
|------------|------------|----------------|-------|
| **Microcontrolador** ESP32 Doit DevKit | 1x | R$ 38,00 | R$ 38,00 |
| **Motores de Tração** DC 12V com redução (170 RPM) | 2x | R$ 65,00 | R$ 130,00 |
| **Driver de Motor** Ponte H BTS7960 (43A) | 2x | R$ 50,00 | R$ 100,00 |
| **Fonte de Energia** Bateria Li-Ion 11,1V 3S 2P 4900mAh | 1x | R$ 110,00 | R$ 110,00 |
| **Regulador de Tensão** Módulo LM2596 Step-Down com display | 2x | R$ 23,00 | R$ 46,00 |
| **Servo Motor** MG995 (alto torque) | 1x | R$ 40,00 | R$ 40,00 |

### 2.2 Materiais Auxiliares

| Material | Preço |
|----------|-------|
| Kit de espaguete termo retrátil | R$ 30,00 |
| Fios flexíveis coloridos | R$ 22,00 |

### **💵 Custo Total Aproximado: R$ 516,00**

---

## 3. Etapas da Montagem

### Etapa 1: Soldagem dos Motores e da Bateria

A primeira tarefa consistiu na preparação dos componentes que exigiam solda. Foram soldados os fios de alimentação diretamente nos terminais dos dois motores DC de 12V, utilizando fios de cores distintas (vermelho e preto) para padronizar as polaridades.

**Objetivos:**
- Garantir conexões sólidas e duráveis nos motores
- Facilitar a identificação das polaridades

**Resultados:**
- ✅ Soldagem dos fios nos terminais dos motores DC (2x)
- ✅ Utilização de termo retrátil para isolamento
- ✅ Padronização de cores (vermelho = positivo, preto = negativo)

Adicionalmente, foi realizada uma adaptação na bateria, soldando um conector para facilitar o processo de recarga sem a necessidade de desmontar as conexões do projeto, além de garantir uma conexão mais segura e robusta com o resto do circuito.

**Vantagens da adaptação:**
- Facilita recarga da bateria
- Evita desmontagem do circuito
- Conexão mais robusta e segura

> 📸 *(Foto referente à soldagem dos motores)*
> 📸 *(Foto referente à soldagem dos fios da bateria)*

---

### Etapa 2: Conexão dos Motores à Ponte H

Com os motores preparados, a etapa seguinte foi conectar um deles ao módulo driver Ponte H BTS7960. Este módulo é responsável por controlar a direção e a velocidade do motor a partir dos sinais enviados pelo ESP32.

**Conexões Realizadas:**
- Terminal M+ e M- do BTS7960 → Motor DC
- Verificação de polaridade correta
- Teste de continuidade

**Especificações do BTS7960:**
- Corrente máxima: 43A
- Tensão de operação: 5,5V - 27V
- Controle via PWM para velocidade
- Enable pins para direção (L_EN e R_EN)

> 📸 *(Foto da conexão entre o motor e a Ponte H)*

---

### Etapa 3: Preparação do Sistema de Alimentação e Regulagem de Tensão

A bateria de 11,1V fornece a energia principal para todo o sistema. No entanto, componentes como o ESP32 e o servo motor operam com tensões menores. Para isso, os reguladores de tensão LM2596 foram preparados para "descer" a tensão da bateria para níveis seguros.

**Configuração dos Reguladores:**

| Regulador | Tensão de Entrada | Tensão de Saída | Componentes Alimentados |
|-----------|-------------------|-----------------|-------------------------|
| LM2596 #1 | 11,1V (bateria) | 5,0V | ESP32 + BTS7960 (ambos) |
| LM2596 #2 | 11,1V (bateria) | 6,0V | Servo MG995 |

**Procedimento:**
1. Conexão dos fios de entrada (IN+ e IN-) à bateria
2. Ajuste do potenciômetro para tensão de saída correta
3. Verificação com multímetro
4. Preparação dos fios de saída (OUT+ e OUT-)

**⚠️ Importante:** Sempre ajustar a tensão SEM carga conectada e verificar com multímetro antes de conectar os componentes.

> 📸 *(Foto do regulador de tensão LM2596)*

---

### Etapa 4: Integração Geral dos Componentes

Nesta fase, os principais componentes foram interligados para formar o cérebro e o sistema de controle do robô.

**Conexões Realizadas:**

#### ESP32 → Motor Esquerdo (BTS7960)
- GPIO 25 → L_EN (Enable ré)
- GPIO 26 → R_EN (Enable frente)
- GPIO 27 → LPWM/RPWM (PWM velocidade)

#### ESP32 → Motor Direito (BTS7960)
- GPIO 14 → L_EN
- GPIO 12 → R_EN
- GPIO 13 → LPWM/RPWM

#### ESP32 → Servo MG995
- GPIO 4 → Sinal (PWM)

#### Sistema de Alimentação
- LM2596 #1 (5V) → ESP32 VIN + BTS7960 VCC (ambos)
- LM2596 #2 (6V) → Servo MG995 VCC
- **GND COMUM** unificado entre TODOS os módulos

**⚠️ CRÍTICO:** Foi feita a unificação dos terras (GND) dos diferentes módulos para garantir uma referência elétrica comum e evitar instabilidades. Este é um passo fundamental para o funcionamento correto do sistema.

**Atividades Paralelas:**
- Equipe de hardware: Conexões físicas e soldagens
- Equipe de software: Configuração do ambiente Arduino IDE e programação do ESP32

> 📸 *(Foto da montagem geral provisória)*

---

### Etapa 5: Planejamento e Design da Estrutura Mecânica

Paralelamente à montagem eletrônica, foi desenvolvido um modelo 3D da carcaça do robô utilizando o software **Blender**.

**Objetivos do Modelo 3D:**
- Servir como guia visual para a construção
- Definir dimensões precisas (30x30 cm)
- Planejar posicionamento dos componentes
- Criar blueprint para o protótipo

**Estratégia de Prototipagem:**
- Modelo 3D completo em Blender
- Protótipo inicial em papelão reforçado
- Estrutura final aguardando chegada das rodas
- Possibilidade de impressão 3D futura

**Especificações do Modelo:**
- Dimensões: 30cm x 30cm (Beetleweight)
- Espaço para bateria centralizado
- Compartimentos para drivers e ESP32
- Sistema de arma móvel com servo
- Suportes para motores nas laterais

> 📸 *(Imagem do modelo 3D do robô no Blender)*
> 📸 *(Imagem do modelo 3D do robô sem arma no Blender)*

---

## 4. Desafios e Soluções

### 4.1 Componentes Faltantes

**Desafio:** Alguns componentes ainda não foram entregues, impedindo a montagem final completa.

**Componentes Pendentes:**
- ⏳ Rodas (principal impedimento para testes de tração)
- ⏳ Segundo Módulo Ponte H BTS7960 (já entregue durante desenvolvimento)
- ⏳ Segundo Regulador de Tensão LM2596 (já entregue durante desenvolvimento)

**Solução Adotada:**
- Montagem provisória fora da estrutura
- Testes com um motor para validar conexões
- Documentação antecipada do processo
- Planejamento detalhado aguardando componentes

### 4.2 Organização da Fiação

**Desafio:** Quantidade grande de fios e conexões pode gerar confusão.

**Soluções Implementadas:**
- ✅ Padronização de cores (vermelho/preto para alimentação)
- ✅ Uso de termo retrátil para identificação
- ✅ Documentação fotográfica de cada etapa
- ✅ Criação de diagrama de conexões detalhado

### 4.3 Gestão de Alimentação

**Desafio:** Diferentes tensões necessárias (11,1V, 6V, 5V) a partir de uma única bateria.

**Solução:**
- Uso de 2 reguladores LM2596 step-down
- Regulador #1: 5V para ESP32 e drivers
- Regulador #2: 6V para servo de alto torque
- GND comum unificado para estabilidade

---

## 5. Próximos Passos

### 5.1 Curto Prazo
- [ ] Receber os componentes restantes (rodas principalmente)
- [ ] Finalizar conexões do segundo motor e driver
- [ ] Construir protótipo da carcaça baseado no modelo 3D
- [ ] Montar todos os componentes na estrutura física

### 5.2 Médio Prazo
- [ ] Programação e calibração do ESP32
- [ ] Testes de movimento (frente, ré, rotação)
- [ ] Teste de acionamento da arma (servo)
- [ ] Ajuste fino de velocidades e controle

### 5.3 Longo Prazo
- [ ] Implementar controle via Bluetooth (módulo HC-05)
- [ ] Desenvolver app de controle mobile
- [ ] Adicionar telemetria (tensão bateria, temperatura)
- [ ] Testes de combate e ajustes finais

---

## 6. Evidências do Desenvolvimento

Como evidência adicional do processo, foram produzidos vídeos demonstrando:

### 6.1 Vídeos Disponíveis

1. **Teste-Servo-Motor.mp4**
   - Demonstração do funcionamento do servo MG995
   - Teste de ângulos (0°, 90°, 180°)
   - Validação do controle via ESP32

2. **Esquema-Projeto.mp4**
   - Disposição dos componentes
   - Conexões realizadas
   - Visão geral do circuito

3. **Demonstração-Arma-Blender3D.mp4**
   - Modelo 3D da estrutura mecânica
   - Animação do sistema de arma
   - Visualização do design final

### 6.2 Documentação Fotográfica

Cada etapa foi fotografada para:
- Documentar o processo de montagem
- Facilitar replicação e manutenção
- Criar material de apresentação
- Servir de referência para ajustes futuros

---

## 7. Conhecimentos Aplicados

### 7.1 Eletrônica de Potência
- Uso de drivers de motor de alta corrente (BTS7960)
- Regulação de tensão com LM2596
- Gestão de alimentação de múltiplos componentes
- Importância do GND comum

### 7.2 Programação Embarcada
- Programação em Arduino/C++ para ESP32
- Controle PWM para motores e servo
- Comunicação serial (debugging e controle)
- Biblioteca ESP32Servo

### 7.3 Mecânica e Design
- Modelagem 3D com Blender
- Planejamento de espaço e fixação
- Distribuição de peso para estabilidade
- Design de sistema de arma

### 7.4 Gestão de Projeto
- Divisão de tarefas entre a equipe
- Documentação técnica detalhada
- Controle de custos e orçamento
- Cronograma e planejamento

---

## 8. Conclusão

A primeira fase de montagem do Robô Rampage foi concluída com **sucesso**. As principais soldagens foram realizadas, e os componentes eletrônicos centrais foram interligados de forma provisória, estabelecendo a base para o sistema de controle do robô.

### Conquistas da Fase 1:
✅ Soldagem profissional dos motores e bateria
✅ Conexão e teste de um motor com BTS7960
✅ Configuração dos reguladores de tensão
✅ Integração inicial do ESP32 com os componentes
✅ Modelo 3D completo da estrutura mecânica
✅ Documentação detalhada de todo o processo

### Status do Projeto:
Apesar do atraso na entrega de algumas peças (principalmente as rodas), a equipe conseguiu avançar significativamente na preparação do circuito e está **pronta para a integração final** assim que os itens restantes chegarem.

### Perspectivas:
O projeto está dentro do cronograma planejado, com todas as etapas críticas de validação de conceito concluídas. A próxima fase focará em:
1. Montagem mecânica completa
2. Programação final do sistema de controle
3. Testes extensivos de funcionamento
4. Preparação para competições

---

**Data do Relatório:** Outubro de 2025
**Revisão:** 1.0
**Status:** Fase 1 Concluída ✅

---

<div align="center">

**UNIVERSIDADE SÃO FRANCISCO**
Engenharia de Computação - 8º Semestre
Prática Profissional: Sistemas Autônomos e Robótica

🤖 **RAMPAGE** ⚔️

</div>
