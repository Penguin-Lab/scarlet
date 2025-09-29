#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>

#define TOTAL_PONTOS 66
#define METADE_PONTOS TOTAL_PONTOS / 2

int OFFSET_ESQF = 0;
int OFFSET_DIRT = METADE_PONTOS;
int OFFSET_ESQM = METADE_PONTOS;
int OFFSET_DIRF = METADE_PONTOS;
int OFFSET_ESQT = 0;
int OFFSET_DIRM = 0;

Adafruit_PWMServoDriver pwmE = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwmD = Adafruit_PWMServoDriver(0x41);

struct int3 {
    int ombro, femur, tibia;
};

struct float3 {
    float ombro, femur, tibia;
};

int3 radToDegree(float3 angles_rad){
    return {int(angles_rad.ombro * 180.0 / M_PI),int(angles_rad.femur * 180.0 / M_PI),int(angles_rad.tibia * 180.0 / M_PI)};
}

float3 degreeToRad(int3 angles){
    return {float(angles.ombro) * M_PI / 180.0,float(angles.femur) * M_PI / 180.0,float(angles.tibia) * M_PI / 180.0};
}

struct floatxyz {
    float x, y, z;
};

struct Pata {
  Adafruit_PWMServoDriver* pwm;
  int3 pins;
  int3 angles;
  floatxyz xyz_ini;
  int3 LIMMIN;
  int3 LIMMAX;
  float L1, L2, L3;
  float P0[2], P1[2], P2[2], P3[2];

  Pata(Adafruit_PWMServoDriver* pwm, int pinOmbro, int pinFemur, int pinTibia, int OMIN, int OMAX, int FMIN, int FMAX, int TMIN, int TMAX) {
    this->pwm = pwm;
    this->pins = {pinOmbro,pinFemur,pinTibia};
    this->angles = {-1,-1,-1};
    this->xyz_ini = {0.0f,0.0f,0.0f};
    this->LIMMIN = {OMIN,FMIN,TMIN};
    this->LIMMAX = {OMAX,FMAX,TMAX};
    this->L1 = 5.28;
    this->L2 = 10.4;
    this->L3 = 14.62;
    this->P0[0] = 0.0;
    this->P0[1] = 0.0;
    this->P1[0] = 0.0;
    this->P1[1] = 0.0;
    this->P2[0] = 0.0;
    this->P2[1] = 0.0;
    this->P3[0] = 0.0;
    this->P3[1] = 0.0;
  }
  
  void moverPata(int angOmbro, int angFemur, int angTibia) {
    if (this->angles.ombro != angOmbro) {
      this->angles.ombro = angOmbro;
      int pulso = map(this->angles.ombro, 0, 180, LIMMIN.ombro, LIMMAX.ombro);
      pwm->setPWM(this->pins.ombro, 0, pulso);
    }
    if (this->angles.femur != angFemur) {
      this->angles.femur = angFemur;
      int pulso = map(this->angles.femur, 0, 180, LIMMIN.femur, LIMMAX.femur);
      pwm->setPWM(this->pins.femur, 0, pulso);
    }
    if (this->angles.tibia != angTibia) {
      this->angles.tibia = angTibia;
      int pulso = map(this->angles.tibia, 0, 180, LIMMIN.tibia, LIMMAX.tibia);
      pwm->setPWM(this->pins.tibia, 0, pulso);
    }
  }

  floatxyz cinematicaDireta(int3 angles){
    float3 angles_rad = degreeToRad(angles);
    floatxyz xyz;
    xyz.x = -this->L3*(cos(angles_rad.femur)*cos(angles_rad.tibia)*sin(angles_rad.ombro) - sin(angles_rad.ombro)*sin(angles_rad.femur)*sin(angles_rad.tibia)) - this->L1*sin(angles_rad.ombro) - this->L2*cos(angles_rad.femur)*sin(angles_rad.ombro);
    xyz.y = this->L1*cos(angles_rad.ombro) - this->L3*(cos(angles_rad.ombro)*sin(angles_rad.femur)*sin(angles_rad.tibia) - cos(angles_rad.ombro)*cos(angles_rad.femur)*cos(angles_rad.tibia)) + this->L2*cos(angles_rad.ombro)*cos(angles_rad.femur);
    xyz.z = this->L3*sin(angles_rad.femur + angles_rad.tibia) + this->L2*sin(angles_rad.femur);
    return xyz;
  }

  int3 cinematicaInversa(floatxyz xyz){
    float3 angles_rad;
    angles_rad.ombro = -atan2(xyz.x, xyz.y);
    float y_linha = sqrt(xyz.x * xyz.x + xyz.y * xyz.y) - this->L1;
    float L = sqrt(xyz.z * xyz.z + y_linha * y_linha);
    float val = (this->L2 * this->L2 + this->L3 * this->L3 - L * L) / (2 * this->L2 * this->L3);
    val = constrain(val, -1.0f, 1.0f);
    angles_rad.tibia = -M_PI + acos(val);
    val = (L * L + this->L2 * this->L2 - this->L3 * this->L3) / (2 * L * this->L2);
    val = constrain(val, -1.0f, 1.0f);
    angles_rad.femur = acos(val) + atan2(xyz.z, y_linha);
    return radToDegree(angles_rad);
  }

  floatxyz trajetoriaLinear(floatxyz xyz_ini, int k, int offset, float angle_rad){
    floatxyz xyz;
    int kn = (k + offset) % TOTAL_PONTOS;
    if (kn < METADE_PONTOS){
      float t = float(kn)/(METADE_PONTOS-1);
      float u = 1 - t;
      xyz.x = xyz_ini.x + cos(angle_rad)*(-xyz_ini.x + u * u * u * this->P0[0] + 3 * u * u * t * this->P1[0] + 3 * u * t * t * this->P2[0] + t * t * t * this->P3[0]);
      xyz.y = xyz_ini.y + sin(angle_rad)*(-xyz_ini.x + u * u * u * this->P0[0] + 3 * u * u * t * this->P1[0] + 3 * u * t * t * this->P2[0] + t * t * t * this->P3[0]);
      xyz.z = u * u * u * this->P0[1] + 3 * u * u * t * this->P1[1] + 3 * u * t * t * this->P2[1] + t * t * t * this->P3[1];
    }
    else{
      xyz.x = xyz_ini.x + cos(angle_rad)*(-xyz_ini.x + this->P3[0] + (this->P0[0] - this->P3[0])*(float(kn - METADE_PONTOS) / (METADE_PONTOS - 1)));
      xyz.y = xyz_ini.y + sin(angle_rad)*(-xyz_ini.x + this->P3[0] + (this->P0[0] - this->P3[0])*(float(kn - METADE_PONTOS) / (METADE_PONTOS - 1)));
      xyz.z = xyz_ini.z;
    }
    return xyz;
  }

  floatxyz trajetoriaPataBezier(floatxyz xyz_ini, int k, int offset, float dx, float dy, float dz) {
    floatxyz xyz;
    int kn = (k + offset) % TOTAL_PONTOS;
    float dx1 = dx/4.0;
    float dx2 = dx/2.0;
    float Px[4] = {xyz_ini.x, xyz_ini.x+dx1, xyz_ini.x+dx2, xyz_ini.x+dx};
    float dy1 = dy/4.0;
    float dy2 = dy/2.0;
    float Py[4] = {xyz_ini.y, xyz_ini.y+dy1, xyz_ini.y+dy2, xyz_ini.y+dy};
    float dz1 = dz/4.0;
    float dz2 = dz/2.0;
    float Pz[4] = {xyz_ini.z, xyz_ini.z+dz1+6.0, xyz_ini.z+dz2+10.0, xyz_ini.z+dz};
    if (kn < METADE_PONTOS){
      float t = float(kn)/(METADE_PONTOS-1);
      float u = 1 - t;
      xyz.x = u * u * u * Px[0] + 3 * u * u * t * Px[1] + 3 * u * t * t * Px[2] + t * t * t * Px[3];
      xyz.y = u * u * u * Py[0] + 3 * u * u * t * Py[1] + 3 * u * t * t * Py[2] + t * t * t * Py[3];
      xyz.z = u * u * u * Pz[0] + 3 * u * u * t * Pz[1] + 3 * u * t * t * Pz[2] + t * t * t * Pz[3];
    }
    else{
      xyz.x = Px[3];
      xyz.y = Py[3];
      xyz.z = Pz[3];
    }
    return xyz;
  }

  void iniciaPata(int3 anglesIni) {
    // Inicializa xyz inicial
    this->xyz_ini = this->cinematicaDireta(anglesIni);
    // Inicializa os vetores de trajetoria
    this->P0[0] = this->xyz_ini.x-4.0;
    this->P0[1] = this->xyz_ini.z;
    this->P1[0] = this->P0[0] + 2.0;
    this->P1[1] = this->P0[1] + 6.0;
    this->P2[0] = this->P0[0] + 4.7;
    this->P2[1] = this->P0[1] + 6.0;
    this->P3[0] = this->P0[0] + 8.0;
    this->P3[1] = this->P0[1];
    // Move a pata para os angulos iniciais
    this->moverPata(anglesIni.ombro, anglesIni.femur, anglesIni.tibia);
    Serial.println("Perna iniciada!");
  }

  void moverPosIni() {
    int3 angles = cinematicaInversa(this->xyz_ini);
    this->moverPata(angles.ombro, angles.femur, angles.tibia);
  }
};

struct Hexapod {
  Pata &EsqF, &EsqM, &EsqT, &DirF, &DirM, &DirT;

  Hexapod(Pata &EsqF, Pata &EsqM, Pata &EsqT, Pata &DirF, Pata &DirM, Pata &DirT):
    EsqF(EsqF), EsqM(EsqM), EsqT(EsqT), DirF(DirF), DirM(DirM), DirT(DirT) {
  }
  
  void ligarHexapod(){
    int3 anglesF = {35,26,-100};
    int3 anglesM = {0,26,-100};
    int3 anglesT = {-35,26,-100};
    this->EsqF.iniciaPata(anglesF);
    this->EsqM.iniciaPata(anglesM);
    this->EsqT.iniciaPata(anglesT);
    this->DirF.iniciaPata(anglesF);
    this->DirM.iniciaPata(anglesM);
    this->DirT.iniciaPata(anglesT);
  }

  void darPatinha(int k){
    float dx = -10;
    float dy = 0;
    float dz = 10;
    floatxyz xyz = this->DirF.trajetoriaPataBezier(this->DirF.xyz_ini, k, 0, dx, dy, dz);
    int3 angles = this->DirF.cinematicaInversa(xyz);
    this->DirF.moverPata(angles.ombro, angles.femur, angles.tibia);
  }

  void andar(int k, int angle_rad){
    floatxyz xyzEsqF = this->EsqF.trajetoriaLinear(this->EsqF.xyz_ini, k, OFFSET_ESQF, -angle_rad);
    floatxyz xyzDirM = this->DirM.trajetoriaLinear(this->DirM.xyz_ini, k, OFFSET_DIRM, angle_rad);
    floatxyz xyzEsqT = this->EsqT.trajetoriaLinear(this->EsqT.xyz_ini, k, OFFSET_ESQT, -angle_rad);
    floatxyz xyzDirF = this->DirF.trajetoriaLinear(this->DirF.xyz_ini, k, OFFSET_DIRF, angle_rad);
    floatxyz xyzEsqM = this->EsqM.trajetoriaLinear(this->EsqM.xyz_ini, k, OFFSET_ESQM, -angle_rad);
    floatxyz xyzDirT = this->DirT.trajetoriaLinear(this->DirT.xyz_ini, k, OFFSET_DIRT, angle_rad);
    int3 anglesEsqF = this->EsqF.cinematicaInversa(xyzEsqF);
    int3 anglesDirM = this->DirM.cinematicaInversa(xyzDirM);
    int3 anglesEsqT = this->EsqT.cinematicaInversa(xyzEsqT);
    int3 anglesDirF = this->DirF.cinematicaInversa(xyzDirF);
    int3 anglesEsqM = this->EsqM.cinematicaInversa(xyzEsqM);
    int3 anglesDirT = this->DirT.cinematicaInversa(xyzDirT);
    this->EsqF.moverPata(anglesEsqF.ombro, anglesEsqF.femur, anglesEsqF.tibia);
    this->DirM.moverPata(anglesDirM.ombro, anglesDirM.femur, anglesDirM.tibia);
    this->EsqT.moverPata(anglesEsqT.ombro, anglesEsqT.femur, anglesEsqT.tibia);
    this->DirF.moverPata(anglesDirF.ombro, anglesDirF.femur, anglesDirF.tibia);
    this->EsqM.moverPata(anglesEsqM.ombro, anglesEsqM.femur, anglesEsqM.tibia);
    this->DirT.moverPata(anglesDirT.ombro, anglesDirT.femur, anglesDirT.tibia);
  }
  
  void parar(){
    this->EsqF.moverPosIni();
    this->DirM.moverPosIni();
    this->EsqT.moverPosIni();
    this->DirF.moverPosIni();
    this->EsqM.moverPosIni();
    this->DirT.moverPosIni();
  }
};

// Criacao das patas e do hexapod
Pata EsqF = {&pwmE, 12, 13, 14, 303, 739, 318, 750, 146, -292};     // Messi
Pata EsqM = {&pwmE, 4, 5, 6, 302, 723, 315, 740, 140, -290};        // Junior e Ricardo
Pata EsqT = {&pwmE, 0, 1, 2, 350, 786, 327, 756, 149, -283};        // Zioto e Jones

Pata DirF = {&pwmD, 0, 1, 2, 326, -59, 330, -72, 456, 849};         // Gustavo e Duda
Pata DirM = {&pwmD, 8, 9, 10, 303, -96, 305, -89, 478, 870};        // Allefe
Pata DirT = {&pwmD, 12, 13, 14, 330, -64, 318, -75, 475, 872};      // Jobs e Rafas

Hexapod scarlet = {EsqF, EsqM, EsqT, DirF, DirM, DirT};

void TaskHexapod(void *pvParameters);
void TaskComunicacao(void *pvParameters);

void setup() {
  Serial.begin(115200);
  Dabble.begin("Scarlet");
  Serial.println("Scarlet ligada!");

  pwmE.begin();
  pwmE.setPWMFreq(50);
  
  pwmD.begin();
  pwmD.setPWMFreq(50);

  delay(2000);

  scarlet.ligarHexapod();
  delay(2000);

  xTaskCreate(TaskHexapod, "hexapod", 4096, NULL, 1, NULL);
  xTaskCreate(TaskComunicacao, "comunicacao", 4096, NULL, 1, NULL);

}

void loop(){
}

int estado = 0;
int angle_joystick = 90;

void TaskHexapod(void *pvParameters) {
  int k = 0;
  for (;;) {
    if(estado == 1){ // "Dar a patinha": move patinha para a posição X = [x_ini+10;y_ini;z_ini+10]
        scarlet.darPatinha(k);
        if (k < METADE_PONTOS){
          k++;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    else if(estado == 2){ // Andar omnidirecional
        float angle_rad = (float(angle_joystick)-90)*M_PI/180.0;
        scarlet.andar(k,angle_rad);
        if (k == 0){
          k = TOTAL_PONTOS - 1;
        }
        else{
          k--;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    else{
      k = 0;
      scarlet.parar();
      vTaskDelay(pdMS_TO_TICKS(10));
    }
  }
}

void TaskComunicacao(void *pvParameters) {
  for (;;) {
    Dabble.processInput();
    if (GamePad.isCirclePressed()){
      estado = 1;
    }
    else if(GamePad.getRadius() > 2){
      angle_joystick = GamePad.getAngle();
      estado = 2;
    }
    else{
      estado = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(20));   
  }
}
