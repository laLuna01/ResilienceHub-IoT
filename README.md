# ResilienceHub IOT

## 1. Visão Geral do Projeto

Este projeto visa desenvolver um sistema de alerta de desastres em tempo real, utilizando um microcontrolador ESP32 para coletar dados de sensores de ambiente (temperatura, umidade, vibração e nível de água) e transmiti-los via protocolo MQTT para um dashboard de monitoramento no Node-RED. A solução é projetada para simular cenários de risco e emergência, fornecendo alertas visuais e sonoros, e permitindo o monitoramento contínuo das condições ambientais. O protótipo foi testado e validado em um ambiente simulado (Wokwi), garantindo a eficácia na coleta e transmissão de dados.

### 1.1. Motivação

A crescente incidência de desastres naturais e a necessidade de sistemas de alerta precoce eficazes motivaram o desenvolvimento deste projeto. A capacidade de monitorar condições ambientais críticas e alertar a população em tempo hábil pode salvar vidas e minimizar danos. Este protótipo demonstra uma abordagem prática e de baixo custo para a implementação de um sistema de monitoramento e alerta, utilizando tecnologias amplamente acessíveis no contexto da Internet das Coisas (IoT).

### 1.2. Aplicação Prática e Benefícios

A aplicação prática deste projeto reside na sua capacidade de ser implementado em áreas de risco, como encostas, margens de rios ou regiões sujeitas a eventos climáticos extremos. Os benefícios incluem:

*   **Monitoramento Contínuo:** Acompanhamento em tempo real de parâmetros ambientais críticos.
*   **Alertas Imediatos:** Notificação rápida de condições que excedem os limites de segurança, permitindo ações preventivas ou de evacuação.
*   **Visualização Clara:** Dashboard intuitivo no Node-RED para fácil interpretação dos dados e status dos alertas.
*   **Custo-benefício:** Utilização de hardware e software de código aberto, tornando a solução acessível.
*   **Escalabilidade:** A arquitetura modular permite a adição de mais sensores e pontos de monitoramento.

Em cenários de risco, como chuvas intensas que podem levar a enchentes, ou atividades sísmicas que podem indicar terremotos, este sistema pode fornecer informações cruciais para a tomada de decisões e a proteção da comunidade.

## 2. Componentes da Solução

O projeto é composto por três pilares principais:

*   **Hardware (ESP32 e Sensores):** Responsável pela coleta de dados ambientais.
*   **Comunicação (MQTT):** Protocolo leve para a transmissão de dados entre o ESP32 e o Node-RED.
*   **Dashboard (Node-RED):** Interface visual para monitoramento e visualização dos alertas.

### 2.1. Hardware

O hardware central do projeto é um ESP32-DevKitC V4, que atua como o microcontrolador principal. Os seguintes sensores e atuadores estão conectados a ele:

*   **Sensor DHT22:** Mede temperatura e umidade do ar.
*   **Potenciômetro (simulando sensor sísmico):** Simula a intensidade de vibrações.
*   **Sensor Ultrassônico HC-SR04:** Mede a distância, simulando o nível da água.
*   **LEDs (Vermelho, Amarelo, Azul):** Indicadores visuais para diferentes tipos de alerta (terremoto, calor extremo, enchente).
*   **Buzzer:** Atuador sonoro para alertas audíveis.


### 2.2. Código-Fonte do ESP32

O código-fonte do ESP32, `resiliencehub-iot.ino`, é responsável por:

*   Conectar-se a uma rede Wi-Fi.
*   Conectar-se a um broker MQTT (broker.hivemq.com).
*   Ler dados dos sensores de temperatura/umidade (DHT22), vibração (potenciômetro) e nível de água (HC-SR04).
*   Publicar os dados dos sensores em tópicos MQTT específicos (`esp32/alertas/temperatura`, `esp32/alertas/vibracao`, `esp32/alertas/nivel_agua`).
*   Verificar os limiares de alerta para cada sensor e ativar LEDs e um buzzer em caso de detecção de risco (calor extremo, terremoto, enchente).
*   Publicar o status do último alerta no tópico `esp32/alertas/ultimo_alerta`.

### 2.3. Node-RED e Dashboard

O Node-RED é utilizado para criar o dashboard de monitoramento, recebendo os dados via MQTT e exibindo-os de forma visualmente atraente. O `flows.json` define a lógica de processamento dos dados e a estrutura do dashboard, incluindo:

*   **Nós MQTT In:** Recebem os dados dos tópicos publicados pelo ESP32.
*   **Nós Gauge e Chart:** Exibem os valores dos sensores em tempo real e gráficos históricos.
*   **Nós Function:** Processam os dados para determinar o status dos alertas (calor, terremoto, enchente) e formatam as mensagens.
*   **Nós UI LED:** Indicam visualmente a ativação de cada tipo de alerta.
*   **Nó Join:** Combina os estados dos alertas individuais para gerar um status geral.
*   **Nó UI Text:** Exibe o status geral dos alertas, com cores indicativas.

## 3. Funcionalidades Implementadas

O sistema de alerta de desastres implementa as seguintes funcionalidades:

*   **Monitoramento de Temperatura e Umidade:** Coleta contínua de dados de temperatura e umidade do ambiente.
*   **Detecção de Vibração:** Monitoramento de vibrações que podem indicar atividades sísmicas ou tremores.
*   **Monitoramento de Nível de Água:** Acompanhamento do nível da água para identificar riscos de enchente.
*   **Alertas Visuais e Sonoros:** Ativação de LEDs e um buzzer no ESP32 quando os limiares de alerta são excedidos.
*   **Publicação MQTT:** Envio dos dados dos sensores e status dos alertas para um broker MQTT.
*   **Dashboard Interativo:** Visualização em tempo real dos dados dos sensores e do status dos alertas no Node-RED, com gráficos e indicadores visuais.
*   **Status Geral de Alertas:** Consolidação de todos os alertas ativos em uma única mensagem no dashboard, facilitando a identificação rápida de múltiplos riscos.

## 4. Configuração, Execução e Testes

Para configurar, executar e testar a aplicação, siga os passos abaixo:

### 4.1. Pré-requisitos

*   **Hardware:**
    *   ESP32-DevKitC V4
    *   Sensor DHT22
    *   Potenciômetro
    *   Sensor Ultrassônico HC-SR04
    *   3 LEDs (cores diferentes, por exemplo, vermelho, amarelo, azul)
    *   Buzzer
    *   Resistores (para os LEDs)
    *   Protoboard e fios jumper
*   **Software:**
    *   PlatformIO IDE (extensão para VS Code)
    *   Node-RED (instalado localmente ou em um servidor)
    *   Nós do Dashboard do Node-RED (`node-red-dashboard`)
    *   Conexão com a internet para o ESP32 e o Node-RED (para acesso ao broker MQTT público).

### 4.3. Configuração do Código-Fonte (PlatformIO)

1.  Abra o VS Code com a extensão PlatformIO instalada.
2.  Crie um novo projeto PlatformIO para ESP32.
3.  Copie o conteúdo do `resiliencehub-iot.ino` fornecido neste README e cole-o no arquivo `src/main.cpp` (ou `src/resiliencehub-iot.ino`) do seu projeto PlatformIO.
4.  No arquivo `resiliencehub-iot.ino`, certifique-se de que as credenciais do Wi-Fi (`ssid` e `password`) e o endereço do broker MQTT (`mqtt_server`) estejam configurados corretamente. Para simulação no Wokwi, `ssid` deve ser "Wokwi-GUEST" e `password` vazio. O `mqtt_server` pode ser "broker.hivemq.com".
5.  Build o projeto e inicie a simulação no Wokwi.

### 4.4. Configuração do Node-RED

1.  Inicie o Node-RED (geralmente acessível via `http://localhost:1880`).
2.  Instale a paleta do dashboard, se ainda não tiver: Vá em `Menu` -> `Manage palette` -> `Install` e procure por `node-red-dashboard`.
3.  Importe o fluxo do dashboard: Vá em `Menu` -> `Import` -> `Clipboard`.
4.  Copie o conteúdo completo do `flows.json` fornecido neste README e cole-o na caixa de texto de importação.
5.  Clique em `Import`.
6.  Verifique se os nós MQTT In estão configurados para o mesmo broker MQTT que o ESP32 está usando (`broker.hivemq.com`).
7.  Clique em `Deploy` para ativar o fluxo.
8.  Acesse o dashboard em `http://localhost:1880/ui` (ou o endereço configurado para seu Node-RED).

### 4.5. Testes da Aplicação

#### Teste em Wokwi (Ambiente Simulado)

1.  No Wokwi, inicie a simulação do ESP32 com o código `resiliencehub-iot.ino` carregado.
2.  Abra o Serial Monitor no Wokwi para verificar a conexão Wi-Fi e MQTT, e a publicação dos dados dos sensores.
3.  No dashboard do Node-RED (`http://localhost:1880/ui`), observe os medidores e gráficos sendo atualizados com os dados simulados.
4.  **Simule Alertas:**
    *   **Calor Extremo:** No sensor DHT22 do Wokwi, aumente a temperatura para acima de 35°C. O LED de calor (LED2) deve acender, o buzzer deve soar e o alerta deve aparecer no dashboard.
    *   **Terremoto:** No potenciômetro do Wokwi, gire-o para simular um valor acima de 3000. O LED de terremoto (LED1) deve acender, o buzzer deve soar e o alerta deve aparecer no dashboard.
    *   **Enchente:** No sensor ultrassônico HC-SR04 do Wokwi, diminua a distância para abaixo de 20cm. O LED de enchente (LED3) deve acender, o buzzer deve soar e o alerta deve aparecer no dashboard.
5.  Verifique se o 


status geral de alertas no dashboard reflete corretamente os alertas ativos.

## 5. Conclusão

Este projeto demonstra a viabilidade de um sistema de alerta de desastres baseado em IoT, utilizando tecnologias acessíveis como ESP32, MQTT e Node-RED. A capacidade de monitorar condições ambientais e gerar alertas em tempo real é crucial para a prevenção e mitigação de desastres, protegendo vidas e propriedades. A flexibilidade da plataforma permite futuras expansões, como a integração com serviços de notificação (SMS, e-mail) ou a adição de mais tipos de sensores para cobrir uma gama mais ampla de riscos.