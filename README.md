# scarlet
Um robô hexapod controlado por uma ESP32 automático ou remotamente. Nossa viúva negra.
A Scarlet é uma hexapod desenvolvida para andar de forma autônoma ou para ser controlada pelo celular. Para se locomover, ela possui 6 patas de 3 graus de liberdade cada (3 motores) e uma ESP32 que comanda o conjunto! Ela é a nossa viúva negra, a irmã da Tiffany e a nova integrante robótica do nosso espaço inteligente!

# Estrutura física
A estrutura física do robô é composta por seis patas de três eixos cada (três motores). As patas são fixadas a duas placas de PLA para deixar o robô firme. As peças são todas projetadas em 3D e foram impressas utilizando PLA, podendo ser encontradas na pasta Pecas_3d deste projeto.

<img src="https://github.com/Penguin-Lab/scarlet/blob/main/images/scarlet0.png" height="350"> <img src="https://github.com/Penguin-Lab/scarlet/blob/main/images/scarlet3.png" height="350"> <img src="https://github.com/Penguin-Lab/scarlet/blob/main/images/scarlet2.png" height="385"> <img src="https://github.com/Penguin-Lab/scarlet/blob/main/images/scarlet1.png" height="385">

## Peças 3D em STL
As peças em 3D desenvolvidas para este projeto foram baseadas no projeto da hexapod Tiffany. Na pasta Pecas_3d, estão listadas essas peças em STL. Para ajudar, segue abaixo uma lista de todas as peças que devem ser impressas e qual o valor da sua maior dimensão (para ajudar na escolha da impressora):
* Corpo
  * 1x [SCARLET_BODY_DOWN.STL (230 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_BODY_DOWN.STL)
  * 1x [SCARLET_BODY_UP.STL (230 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_BODY_UP.STL)
  * 4x [SCARLET_SPACER.STL (68 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_SPACER.STL)
  * 2x [SCARLET_SPACER_FRONT.STL (68 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_SPACER_FRONT.STL)
  * 1x [SCARLET_SUPPORT_PCI.STL (120 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_SUPPORT_PCI.STL)
  * 1x [SCARLET_SUPPORT_SWITCH.STL (50 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_SUPPORT_SWITCH.STL)
  * 1x [SCARLET_SYMBOL.STL (50 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_SYMBOL.STL)
  * 1x [SCARLET_LOGO.STL (50 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_LOGO.STL)
* Patas
  * 6x [SCARLET_TIBIA_LEFT.STL (141 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_TIBIA_LEFT.STL)
  * 6x [SCARLET_TIBIA_LEFT.STL espelhado (141 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_TIBIA_LEFT.STL)
  * 6x [SCARLET_TIBIA_MIDDLE.STL (63 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_TIBIA_MIDDLE.STL)
  * 3x [SCARLET_SHIELD_A.STL (10 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_SHIELD_A.STL)
  * 3x [SCARLET_SHIELD_A.STL espelhado (10 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_SHIELD_A.STL)
  * 3x [SCARLET_SHIELD_B.STL (10 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_SHIELD_B.STL)
  * 3x [SCARLET_SHIELD_B.STL espelhado (10 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_SHIELD_B.STL)
  * 3x [SCARLET_FEMUR_FRONT.STL (110 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_FEMUR_FRONT.STL)
  * 3x [SCARLET_FEMUR_BACK.STL (118 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_FEMUR_BACK.STL)
  * 3x [SCARLET_FEMUR_FRONT.STL espelhado (110 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_FEMUR_FRONT.STL)
  * 3x [SCARLET_FEMUR_BACK.STL espelhado (118 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_FEMUR_BACK.STL)
  * 6x [SCARLET_FEMUR_MIDDLE.STL (51 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_FEMUR_MIDDLE.STL)
  * 6x [SCARLET_OMBRO_UP.STL (61 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_OMBRO_UP.STL)
  * 3x [SCARLET_OMBRO_DOWN_LEFT.STL (61 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_OMBRO_DOWN_LEFT.STL)
  * 3x [SCARLET_OMBRO_DOWN_LEFT.STL espelhado (61 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_OMBRO_DOWN_LEFT.STL)
* Motor
  * 12x [SCARLET_TAMPA_MOTOR_DS3230.STL (41 mm)](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_TAMPA_MOTOR_DS3230.STL)

Obs 1: a tampa de 12 motores deverão ser trocadas pelas peças [SCARLET_TAMPA_MOTOR_DS3230.STL](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_TAMPA_MOTOR_DS3230.STL) para que seja possível fixar os rolamentos nas patas. Serão utilizados 3 rolamentos por pata.

Obs 2: os motores ficam fixados nas peças por [discos metálicos pequenos padrão para eixos de servo](https://a.aliexpress.com/_mPHt3nf).

<img src="https://github.com/Penguin-Lab/lilla/blob/main/images/discoeixo.jpeg" height="100">

<img src="https://github.com/Penguin-Lab/tiffany/blob/main/images/pata0.png" height="300"> <img src="https://github.com/Penguin-Lab/tiffany/blob/main/images/pata1.png" height="300"> <img src="https://github.com/Penguin-Lab/tiffany/blob/main/images/corpo0.png" height="300"> <img src="https://github.com/Penguin-Lab/tiffany/blob/main/images/corpo1.png" height="300">

## Outros itens da estrutura
Alguns outros itens são necessários para finalizar a estrutura da Tiffany:
* 6x Parafusos m4 ou similar de cm, arruelas e porcas auto-travantes: prender as peças [SCARLET_BODY_UP.STL](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_BODY_UP.STL) e [SCARLET_BODY_DOWN.STL](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_BODY_DOWN.STL)
* 12x Parafusos m4 de 5 cm: prender as peças [SCARLET_OMBRO_DOWN_LEFT.STL](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_OMBRO_DOWN_LEFT.STL) e [SCARLET_OMBRO_UP.STL](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_OMBRO_UP.STL)
* 24x Parafusos m4 de 5,5 cm: prender as peças [SCARLET_OMBRO_DOWN_LEFT.STL](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_OMBRO_DOWN_LEFT.STL) e [SCARLET_OMBRO_UP.STL](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_OMBRO_UP.STL)
* Parafusos m3 de cm: prender todas as peças das patas e a [SCARLET_SUPORT_PCI.STL](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_SUPORT_PCI.STL) no corpo do robô e na placa
* 6x Tiras de EVA de 5mm: colar e prender com braçadeira na ponta das peças [SCARLET_SHIELD_A.STL](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_SHIELD_A.STL) e [SCARLET_SHIELD_B.STL](https://github.com/Penguin-Lab/scarlet/blob/main/Pecas_3d/SCARLET_SHIELD_B.STL) para evitar que o robô deslize

## Placa de circuito impresso
No projeto, foi desenvolvida uma placa de circuito para servir como shield para a ESP32 e conexão com as duas placas PCA9685 e com futuros sensores e displays I2C.

<img src="https://github.com/Penguin-Lab/scarlet/blob/main/images/shield_esp0.png" height="300"> <img src="https://github.com/Penguin-Lab/scarlet/blob/main/images/circuit2.png" height="300"> <img src="https://github.com/Penguin-Lab/scarlet/blob/main/images/shield_esp2.png" height="280"> <img src="https://github.com/Penguin-Lab/scarlet/blob/main/images/shield_esp_projeto.png" height="280"> <img src="https://github.com/Penguin-Lab/scarlet/blob/main/images/circuit1.png" height="280">


## Componentes eletrônicos
Os componentes eletrônicos embarcados no robô e equipamentos utilizados são:
* Controlador com Wi-fi e Bluetooth
  * 1x ESP32 (38 pinos)
* Motores
  * 18x Servo DS3230 PRO-180
* 2x Placas PCA9685 para comando dos 18 motores
* Chave (switch)
* Alimentação
  * Bateria Zippy LiFe 1100mAh 2s 6,6V (ESP32)
  * Bateria TM Hobbies LiFe 2100mAh 2s 6,6v (motores)

# Software
Na pasta de códigos, há um código de programação da ESP32 (38 pinos). Ele é um exemplo em C++ do comando de movimentação da Tiffany:
* Dabble: a ESP32 pode se comunicar com o aplicativo Dabble no celular e receber comandos de ação.
