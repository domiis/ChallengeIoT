# 🛵Monitoramento Inteligente de Pátio de Motos
O projeto consiste em um sistema de monitoramento inteligente para pátios de motos da Mottu, permitindo a localização precisa e gestão em tempo real das motos através de tecnologias IoT e visualização em dashboard.

# Integrantes
* Celso Canaveze Teixeira Pinto - RM556118

* Sofia Domingues Gonçalves - RM554920

* Thiago Moreno Matheus - RM554507

## 📲 Visão Geral — Projeto de Monitoramento de Motos via ESP32
* Cada moto possui um identificador único (ID), placa e modelo, enviados automaticamente via IoT (simulados no código).

* Um ESP32 simula a recepção dos dados da moto (placa, modelo, status e localização).

* O sistema cruza esses dados com os comandos recebidos via broker MQTT (tópico mottu/patio/comandos).

* A informação da moto é publicada periodicamente no tópico MQTT mottu/patio/motos.

* Um dashboard Node-RED ou outro serviço MQTT pode visualizar o status, localização e dados da moto em tempo real.

## 🧠 Lógica do Projeto
* O ESP32 não faz leitura física da placa ou presença — os dados da moto são simulados e recebidos via rede/MQTT.

* Ao receber comandos via MQTT (ex: PRONTA_PARA_ALUGAR, EM_MANUTENCAO, ALERTA), o ESP32:

    1. Atualiza o status interno da moto.

    2. Atualiza o LED RGB, indicando o status visualmente:

        * 🟢 Verde: PRONTA_PARA_ALUGAR

        * 🟡 Amarelo: PENDENTE_REGULARIZACAO

        * 🔴 Vermelho: EM_MANUTENCAO

3. Aciona o buzzer com sons distintos para cada status.

4. Publica os dados atualizados da moto (incluindo posição aleatória simulada) no tópico mottu/patio/motos.

## 🔘 Interação Local (Botão Físico)
Um botão conectado ao ESP32 permite ciclar entre os três estados da moto manualmente.

A cada clique:

* O status é alterado em sequência.

* O LED e o buzzer reagem ao novo estado.

* Os dados são publicados automaticamente no broker MQTT.

## 🏷️ Exemplo de Status Cadastrados
* PRONTA_PARA_ALUGAR – Moto autorizada a sair do pátio

* EM_MANUTENCAO – Moto em manutenção, bloqueada

* PENDENTE_REGULARIZACAO – Cadastro pendente ou precisa de análise

## 🔌 Componentes Usados
* ESP32 (simulado no Wokwi)

* LED RGB para indicar status da moto:

    * Verde (LED_GREEN): Pronta para alugar

    * Amarelo (LED_YELLOW): Pendente

    * Vermelho (LED_RED): Em manutenção

* Buzzer: Alerta sonoro conforme o status da moto

* Botão físico: Altera o status manualmente (ciclo entre os 3 status)

* Conexão Wi-Fi

* Broker MQTT público: broker.hivemq.com

* Node-RED: Dashboard para visualização interativa

 ## 📡 Tópico MQTT Utilizado
* Publicação: mottu/patio/motos → Moto envia status e localização

* Comando: mottu/patio/comandos → Recebe comandos para alterar o status

## 📤 Fluxo da Informação
1. Moto (simulada) → Envia placa e status para o ESP32

2. ESP32 → Processa e atualiza status, acionando LEDs e buzzer

3. Publica via MQTT → Envia informações para o Broker MQTT

4. Node-RED → Exibe o status no painel de controle

## 🧪 Simulação de Placas
* A leitura da placa é simulada com seleção aleatória no código

* Em um cenário real, as placas seriam enviadas via módulo RFID ou dispositivo IoT conectado à moto

* A cada 10 segundos, o ESP32 simula a chegada de uma nova moto, publicando a localização e status

## 🖥️ Visualização com Node-RED
* Tópico monitorado: mottu/patio/motos

* Dashboard exibe:

    * Lista por status (Pronta para alugar, Manutenção, Pendente)

    * Mapa interativo com localização das motos

    * Botão "Localizar" para centralizar o mapa na moto selecionada

    * Atualização em tempo real dos status e localizações

## 🚦 Comportamento dos LEDs e Buzzer
* LED verde (🟢): Moto PRONTA_PARA_ALUGAR

* LED amarelo (🟡): Moto PENDENTE_REGULARIZACAO

* LED vermelho (🔴): Moto EM_MANUTENCAO

* Buzzer: Emite sons diferentes conforme o status da moto

## 📋 Exemplo de Saída Serial

    Copiar
    Editar
    [INFO] Sistema iniciado - Moto IoT  
    Wi-Fi conectado!  
    Endereço IP: 192.168.0.105  
    Moto recebida: ID 102 - JLM3F45 - Modelo CG160  
    Status atual: EM_MANUTENCAO  
    [MQTT] Publicado em mottu/patio/motos  
    [ALERTA] LED vermelho ativado | Som de erro  

## 🛠️ Tecnologias Utilizadas
* Hardware (simulado):

    * ESP32

    * LED RGB para status (pinos 2, 4)

    *  Buzzer (pino 5)

    * Botão (pino 6) para alterar manualmente o status

* Software:

    * Wokwi Simulator para simulação do ESP32

    * MQTT para comunicação entre os dispositivos

    * MQTT para comunicação entre os dispositivos
    Node-RED para visualização no dashboard