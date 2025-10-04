#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>

#define TOTAL_PONTOS 500//2000//66
#define METADE_PONTOS TOTAL_PONTOS / 2
#define TOTAL_PONTOS_CIRCULAR 500

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

    // Soma
    int3 operator+(const int3& rhs) const {
      return { ombro + rhs.ombro, femur + rhs.femur, tibia + rhs.tibia };
    }

    // Subtracao
    int3 operator-(const int3& rhs) const {
      return { ombro - rhs.ombro, femur - rhs.femur, tibia - rhs.tibia };
    }

    // Multiplicacao por escalar
    int3 operator*(int scalar) const {
      return { ombro * scalar, femur * scalar, tibia * scalar };
    }

    // Divisao por escalar
    int3 operator/(int scalar) const {
      return { ombro / scalar, femur / scalar, tibia / scalar };
    }
};

struct float3 {
    float ombro, femur, tibia;

    // Soma
    float3 operator+(const float3& rhs) const {
      return { ombro + rhs.ombro, femur + rhs.femur, tibia + rhs.tibia };
    }

    // Subtracao
    float3 operator-(const float3& rhs) const {
      return { ombro - rhs.ombro, femur - rhs.femur, tibia - rhs.tibia };
    }

    // Multiplicacao por escalar
    float3 operator*(float scalar) const {
      return { ombro * scalar, femur * scalar, tibia * scalar };
    }

    // Divisao por escalar
    float3 operator/(float scalar) const {
      return { ombro / scalar, femur / scalar, tibia / scalar };
    }
};

int3 radToDegree(float3 angles_rad){
    return {int(angles_rad.ombro * 180.0 / M_PI),int(angles_rad.femur * 180.0 / M_PI),int(angles_rad.tibia * 180.0 / M_PI)};
}

float3 degreeToRad(int3 angles){
    return {float(angles.ombro) * M_PI / 180.0,float(angles.femur) * M_PI / 180.0,float(angles.tibia) * M_PI / 180.0};
}

struct floatxyz {
    float x, y, z;

    // Soma
    floatxyz operator+(const floatxyz& rhs) const {
      return { x + rhs.x, y + rhs.y, z + rhs.z };
    }

    // Subtracao
    floatxyz operator-(const floatxyz& rhs) const {
      return { x - rhs.x, y - rhs.y, z - rhs.z };
    }

    // Multiplicacao por escalar
    floatxyz operator*(float scalar) const {
      return { x * scalar, y * scalar, z * scalar };
    }

    // Divisao por escalar
    floatxyz operator/(float scalar) const {
      return { x / scalar, y / scalar, z / scalar };
    }

    floatxyz operator*(const floatxyz& rhs) const {
      return { x * rhs.x, y * rhs.y, z * rhs.z };
    }
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

  bool moverPataSuave(int angOmbro, int angFemur, int angTibia, int step) {
    bool stoped = true;
    if (this->angles.ombro != angOmbro) {
      stoped = false;
      if (abs(this->angles.ombro - angOmbro) < step){
        this->angles.ombro = angOmbro;
      }
      else{
        if (this->angles.ombro < angOmbro) this->angles.ombro += step;
        else this->angles.ombro -= step;
      }
      int pulso = map(this->angles.ombro, 0, 180, LIMMIN.ombro, LIMMAX.ombro);
      pwm->setPWM(this->pins.ombro, 0, pulso);
    }
    if (this->angles.femur != angFemur) {
      stoped = false;
      if (abs(this->angles.femur - angFemur) < step){
        this->angles.femur = angFemur;
      }
      else{
        if (this->angles.femur < angFemur) this->angles.femur += step;
        else this->angles.femur -= step;
      }
      int pulso = map(this->angles.femur, 0, 180, LIMMIN.femur, LIMMAX.femur);
      pwm->setPWM(this->pins.femur, 0, pulso);
    }
    if (this->angles.tibia != angTibia) {
      stoped = false;
      if (abs(this->angles.tibia - angTibia) < step){
        this->angles.tibia = angTibia;
      }
      else{
        if (this->angles.tibia < angTibia) this->angles.tibia += step;
        else this->angles.tibia -= step;
      }
      int pulso = map(this->angles.tibia, 0, 180, LIMMIN.tibia, LIMMAX.tibia);
      pwm->setPWM(this->pins.tibia, 0, pulso);
    }
    return stoped;
  }

  floatxyz cinematicaDireta(int3 angles){
    float3 angles_rad = degreeToRad(angles);
    floatxyz xyz;
																																																																																																  
    xyz.x = -sin(angles_rad.ombro)*(this->L1 + this->L3*cos(angles_rad.femur + angles_rad.tibia) + this->L2*cos(angles_rad.femur));
																																																																										 
    xyz.y = cos(angles_rad.ombro)*(this->L1 + this->L3*cos(angles_rad.femur + angles_rad.tibia) +this->L2*cos(angles_rad.femur));
    xyz.z = this->L3*sin(angles_rad.femur + angles_rad.tibia) + this->L2*sin(angles_rad.femur);
    return xyz;
  }

  int3 cinematicaInversa(floatxyz xyz){
    float3 angles_rad;
    float y_linha = sqrt(xyz.x * xyz.x + xyz.y * xyz.y) - this->L1;
    float L = sqrt(xyz.z * xyz.z + y_linha * y_linha);

    float alpha = acos(constrain((this->L2 * this->L2 + this->L3 * this->L3 - L * L) / (2 * this->L2 * this->L3), -1.0f, 1.0f));
    float beta = acos(constrain((L * L + this->L2 * this->L2 - this->L3 * this->L3) / (2 * L * this->L2), -1.0f, 1.0f));
    
    angles_rad.tibia = -M_PI + alpha;
    angles_rad.ombro = -atan2(xyz.x, xyz.y);
    angles_rad.femur = beta + atan2(xyz.z, y_linha);
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

  floatxyz trajetoriaPataBezier(floatxyz xyz_ini, int k, int offset, float dx, float dy, float dz, int totalPontos) {
    floatxyz xyz;
    int metadePontos = totalPontos/2;
    int kn = (k + offset) % totalPontos;
    float dx1 = dx/4.0;
    float dx2 = dx/2.0;
    float Px[4] = {xyz_ini.x, xyz_ini.x+dx1, xyz_ini.x+dx2, xyz_ini.x+dx};
    float dy1 = dy/4.0;
    float dy2 = dy/2.0;
    float Py[4] = {xyz_ini.y, xyz_ini.y+dy1, xyz_ini.y+dy2, xyz_ini.y+dy};
    float dz1 = dz/4.0;
    float dz2 = dz/2.0;
    float Pz[4] = {xyz_ini.z, xyz_ini.z+dz1+6.0, xyz_ini.z+dz2+10.0, xyz_ini.z+dz};
    if (kn < metadePontos){
      float t = float(kn)/(metadePontos-1);
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

  floatxyz iniciaPata(int3 anglesIni) {
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
    return this->xyz_ini;
  }

  void escolheXyzini(int3 anglesIni){
    // Atualiza xyz inicial
    this->xyz_ini = this->cinematicaDireta(anglesIni);
    // Atualiza os vetores de trajetoria
    this->P0[0] = this->xyz_ini.x-4.0;
    this->P0[1] = this->xyz_ini.z;
    this->P1[0] = this->P0[0] + 2.0;
    this->P1[1] = this->P0[1] + 6.0;
    this->P2[0] = this->P0[0] + 4.7;
    this->P2[1] = this->P0[1] + 6.0;
    this->P3[0] = this->P0[0] + 8.0;
    this->P3[1] = this->P0[1];
										   
																								
												  
				  
  }

  void moverPosIni() {
    int3 angles = cinematicaInversa(this->xyz_ini);
    // this->moverPata(angles.ombro, angles.femur, angles.tibia);
    this->moverPataSuave(angles.ombro, angles.femur, angles.tibia, 1);
  }
};

struct Hexapod {
  Pata &EsqF, &EsqM, &EsqT, &DirF, &DirM, &DirT;
  floatxyz OmbDirF, OmbDirM, OmbDirT, OmbEsqF, OmbEsqM, OmbEsqT; 

  Hexapod(Pata &EsqF, Pata &EsqM, Pata &EsqT, Pata &DirF, Pata &DirM, Pata &DirT):
    EsqF(EsqF), EsqM(EsqM), EsqT(EsqT), DirF(DirF), DirM(DirM), DirT(DirT) {
      OmbDirF = {8.30,-5.55,0.0};
      OmbDirM = {0.0,-7.50,0.0};
      OmbDirT = {-8.29,-5.50,0.0};
      OmbEsqF = {8.30,5.55,0.0};
      OmbEsqM = {0.0,7.50,0.0};
      OmbEsqT = {-8.29,5.50,0.0};
  }
  
  void ligarHexapod(){
    int totalPontos = 2000;
    int metadePontos = totalPontos/2;
    int3 anglesF = {45,26,-100};
    int3 anglesM = {0,26,-100};
    int3 anglesT = {-45,26,-100};
    int3 anglesStartF = {anglesF.ombro,90,-145};
    int3 anglesStartM = {anglesM.ombro,90,-145};
    int3 anglesStartT = {anglesT.ombro,90,-145};
    // Manda a pata para a posicao inicial de coccum e guarda o xyz
    floatxyz xyzStartEsqF = this->EsqF.iniciaPata(anglesStartF);
    floatxyz xyzStartEsqM = this->EsqM.iniciaPata(anglesStartM);
    floatxyz xyzStartEsqT = this->EsqT.iniciaPata(anglesStartT);
    floatxyz xyzStartDirF = this->DirF.iniciaPata(anglesStartF);
    floatxyz xyzStartDirM = this->DirM.iniciaPata(anglesStartM);
    floatxyz xyzStartDirT = this->DirT.iniciaPata(anglesStartT);
    // Inicializa xyz_ini sem mover as patas
    this->EsqF.escolheXyzini(anglesF);
    this->EsqM.escolheXyzini(anglesM);
    this->EsqT.escolheXyzini(anglesT);
    this->DirF.escolheXyzini(anglesF);
    this->DirM.escolheXyzini(anglesM);
    this->DirT.escolheXyzini(anglesT);
    delay(2000);
    // Move as patas para (x_ini,y_ini,zStart)
    int k = 0;
    while (k < metadePontos){
      floatxyz xyzEsqF = this->EsqF.trajetoriaPataBezier(xyzStartEsqF, k, 0, EsqF.xyz_ini.x - xyzStartEsqF.x, EsqF.xyz_ini.y - xyzStartEsqF.y, 0, totalPontos);
      floatxyz xyzDirM = this->DirM.trajetoriaPataBezier(xyzStartDirM, k, 0, DirM.xyz_ini.x - xyzStartDirM.x, DirM.xyz_ini.y - xyzStartDirM.y, 0, totalPontos);
      floatxyz xyzEsqT = this->EsqT.trajetoriaPataBezier(xyzStartEsqT, k, 0, EsqT.xyz_ini.x - xyzStartEsqT.x, EsqT.xyz_ini.y - xyzStartEsqT.y, 0, totalPontos);
      floatxyz xyzDirF = this->DirF.trajetoriaPataBezier(xyzStartDirF, k, 0, DirF.xyz_ini.x - xyzStartDirF.x, DirF.xyz_ini.y - xyzStartDirF.y, 0, totalPontos);
      floatxyz xyzEsqM = this->EsqM.trajetoriaPataBezier(xyzStartEsqM, k, 0, EsqM.xyz_ini.x - xyzStartEsqM.x, EsqM.xyz_ini.y - xyzStartEsqM.y, 0, totalPontos);
      floatxyz xyzDirT = this->DirT.trajetoriaPataBezier(xyzStartDirT, k, 0, DirT.xyz_ini.x - xyzStartDirT.x, DirT.xyz_ini.y - xyzStartDirT.y, 0, totalPontos);
      int3 anglesEsqF = this->EsqF.cinematicaInversa(xyzEsqF);
      int3 anglesDirM = this->DirM.cinematicaInversa(xyzEsqM);
      int3 anglesEsqT = this->EsqT.cinematicaInversa(xyzEsqT);
      int3 anglesDirF = this->DirF.cinematicaInversa(xyzDirF);
      int3 anglesEsqM = this->EsqM.cinematicaInversa(xyzDirM);
      int3 anglesDirT = this->DirT.cinematicaInversa(xyzDirT);
      this->EsqF.moverPata(anglesEsqF.ombro, anglesEsqF.femur, anglesEsqF.tibia);
      this->DirM.moverPata(anglesDirM.ombro, anglesDirM.femur, anglesDirM.tibia);
      this->EsqT.moverPata(anglesEsqT.ombro, anglesEsqT.femur, anglesEsqT.tibia);
      this->DirF.moverPata(anglesDirF.ombro, anglesDirF.femur, anglesDirF.tibia);
      this->EsqM.moverPata(anglesEsqM.ombro, anglesEsqM.femur, anglesEsqM.tibia);
      this->DirT.moverPata(anglesDirT.ombro, anglesDirT.femur, anglesDirT.tibia);
      k++;
      // delay(1);
    }
    delay(2000);
    // Atualiza a posicao inicial da pata para antes de descer para xyzini
    xyzStartEsqF = {this->EsqF.xyz_ini.x,this->EsqF.xyz_ini.y,xyzStartEsqF.z};
    xyzStartEsqM = {this->EsqM.xyz_ini.x,this->EsqM.xyz_ini.y,xyzStartEsqM.z};
    xyzStartEsqT = {this->EsqT.xyz_ini.x,this->EsqT.xyz_ini.y,xyzStartEsqT.z};
    xyzStartDirF = {this->DirF.xyz_ini.x,this->DirF.xyz_ini.y,xyzStartDirF.z};
    xyzStartDirM = {this->DirM.xyz_ini.x,this->DirM.xyz_ini.y,xyzStartDirM.z};
    xyzStartDirT = {this->DirT.xyz_ini.x,this->DirT.xyz_ini.y,xyzStartDirT.z};
    // Move as patas para (x_ini,y_ini,z_ini)
    k = 0;
    while (k < metadePontos){
      floatxyz xyzEsqF = this->EsqF.trajetoriaPataBezier(xyzStartEsqF, k, 0, 0, 0, EsqF.xyz_ini.z - xyzStartEsqF.z, totalPontos);
      floatxyz xyzDirM = this->DirM.trajetoriaPataBezier(xyzStartDirM, k, 0, 0, 0, DirM.xyz_ini.z - xyzStartDirM.z, totalPontos);
      floatxyz xyzEsqT = this->EsqT.trajetoriaPataBezier(xyzStartEsqT, k, 0, 0, 0, EsqT.xyz_ini.z - xyzStartEsqT.z, totalPontos);
      floatxyz xyzDirF = this->DirF.trajetoriaPataBezier(xyzStartDirF, k, 0, 0, 0, DirF.xyz_ini.z - xyzStartDirF.z, totalPontos);
      floatxyz xyzEsqM = this->EsqM.trajetoriaPataBezier(xyzStartEsqM, k, 0, 0, 0, EsqM.xyz_ini.z - xyzStartEsqM.z, totalPontos);
      floatxyz xyzDirT = this->DirT.trajetoriaPataBezier(xyzStartDirT, k, 0, 0, 0, DirT.xyz_ini.z - xyzStartDirT.z, totalPontos);
      int3 anglesEsqF = this->EsqF.cinematicaInversa(xyzEsqF);
      int3 anglesDirM = this->DirM.cinematicaInversa(xyzEsqM);
      int3 anglesEsqT = this->EsqT.cinematicaInversa(xyzEsqT);
      int3 anglesDirF = this->DirF.cinematicaInversa(xyzDirF);
      int3 anglesEsqM = this->EsqM.cinematicaInversa(xyzDirM);
      int3 anglesDirT = this->DirT.cinematicaInversa(xyzDirT);
      this->EsqF.moverPata(anglesEsqF.ombro, anglesEsqF.femur, anglesEsqF.tibia);
      this->DirM.moverPata(anglesDirM.ombro, anglesDirM.femur, anglesDirM.tibia);
      this->EsqT.moverPata(anglesEsqT.ombro, anglesEsqT.femur, anglesEsqT.tibia);
      this->DirF.moverPata(anglesDirF.ombro, anglesDirF.femur, anglesDirF.tibia);
      this->EsqM.moverPata(anglesEsqM.ombro, anglesEsqM.femur, anglesEsqM.tibia);
      this->DirT.moverPata(anglesDirT.ombro, anglesDirT.femur, anglesDirT.tibia);
      k++;
      // delay(1);
    }
  }

  void desligarHexapod(){
    int totalPontos = 2000;
    int metadePontos = totalPontos/2;
    int3 anglesF = {45,26,-100};
    int3 anglesM = {0,26,-100};
    int3 anglesT = {-45,26,-100};
    int3 anglesStartF = {anglesF.ombro,90,-145};
    int3 anglesStartM = {anglesM.ombro,90,-145};
    int3 anglesStartT = {anglesT.ombro,90,-145};
    // Gero os xyz desses angulos
    floatxyz xyzStartEsqF = this->EsqF.cinematicaDireta(anglesStartF);
    floatxyz xyzStartDirM = this->DirM.cinematicaDireta(anglesStartM);
    floatxyz xyzStartEsqT = this->EsqT.cinematicaDireta(anglesStartT);
    floatxyz xyzStartDirF = this->DirF.cinematicaDireta(anglesStartF);
    floatxyz xyzStartEsqM = this->EsqM.cinematicaDireta(anglesStartM);
    floatxyz xyzStartDirT = this->DirT.cinematicaDireta(anglesStartT);
    // Move as patas para (x_ini,y_ini,zStart)
    int k = 0;
    while (k < metadePontos){
      floatxyz xyzEsqF = this->EsqF.trajetoriaPataBezier(EsqF.xyz_ini, k, 0, 0, 0, xyzStartEsqF.z - EsqF.xyz_ini.z, totalPontos);
      floatxyz xyzDirM = this->DirM.trajetoriaPataBezier(DirM.xyz_ini, k, 0, 0, 0, xyzStartDirM.z - DirM.xyz_ini.z, totalPontos);
      floatxyz xyzEsqT = this->EsqT.trajetoriaPataBezier(EsqT.xyz_ini, k, 0, 0, 0, xyzStartEsqT.z - EsqT.xyz_ini.z, totalPontos);
      floatxyz xyzDirF = this->DirF.trajetoriaPataBezier(DirF.xyz_ini, k, 0, 0, 0, xyzStartDirF.z - DirF.xyz_ini.z, totalPontos);
      floatxyz xyzEsqM = this->EsqM.trajetoriaPataBezier(EsqM.xyz_ini, k, 0, 0, 0, xyzStartEsqM.z - EsqM.xyz_ini.z, totalPontos);
      floatxyz xyzDirT = this->DirT.trajetoriaPataBezier(DirT.xyz_ini, k, 0, 0, 0, xyzStartDirT.z - DirT.xyz_ini.z, totalPontos);
      int3 anglesEsqF = this->EsqF.cinematicaInversa(xyzEsqF);
      int3 anglesDirM = this->DirM.cinematicaInversa(xyzEsqM);
      int3 anglesEsqT = this->EsqT.cinematicaInversa(xyzEsqT);
      int3 anglesDirF = this->DirF.cinematicaInversa(xyzDirF);
      int3 anglesEsqM = this->EsqM.cinematicaInversa(xyzDirM);
      int3 anglesDirT = this->DirT.cinematicaInversa(xyzDirT);
      this->EsqF.moverPata(anglesEsqF.ombro, anglesEsqF.femur, anglesEsqF.tibia);
      this->DirM.moverPata(anglesDirM.ombro, anglesDirM.femur, anglesDirM.tibia);
      this->EsqT.moverPata(anglesEsqT.ombro, anglesEsqT.femur, anglesEsqT.tibia);
      this->DirF.moverPata(anglesDirF.ombro, anglesDirF.femur, anglesDirF.tibia);
      this->EsqM.moverPata(anglesEsqM.ombro, anglesEsqM.femur, anglesEsqM.tibia);
      this->DirT.moverPata(anglesDirT.ombro, anglesDirT.femur, anglesDirT.tibia);
      k++;
      delay(1);
    }
    delay(2000);
    // Gera o xyz da posicao aberta elevada
    floatxyz xyzCoccumEsqF = {this->EsqF.xyz_ini.x,this->EsqF.xyz_ini.y,xyzStartEsqF.z};
    floatxyz xyzCoccumEsqM = {this->EsqM.xyz_ini.x,this->EsqM.xyz_ini.y,xyzStartEsqM.z};
    floatxyz xyzCoccumEsqT = {this->EsqT.xyz_ini.x,this->EsqT.xyz_ini.y,xyzStartEsqT.z};
    floatxyz xyzCoccumDirF = {this->DirF.xyz_ini.x,this->DirF.xyz_ini.y,xyzStartDirF.z};
    floatxyz xyzCoccumDirM = {this->DirM.xyz_ini.x,this->DirM.xyz_ini.y,xyzStartDirM.z};
    floatxyz xyzCoccumDirT = {this->DirT.xyz_ini.x,this->DirT.xyz_ini.y,xyzStartDirT.z};
    // Move as patas para posicao de coccum
    k = 0;
    while (k < metadePontos){
      floatxyz xyzEsqF = this->EsqF.trajetoriaPataBezier(xyzCoccumEsqF, k, 0, xyzStartEsqF.x - EsqF.xyz_ini.x, xyzStartEsqF.y - EsqF.xyz_ini.y, 0, totalPontos);
      floatxyz xyzDirM = this->DirM.trajetoriaPataBezier(xyzCoccumDirM, k, 0, xyzStartDirM.x - DirM.xyz_ini.x, xyzStartDirM.y - DirM.xyz_ini.y, 0, totalPontos);
      floatxyz xyzEsqT = this->EsqT.trajetoriaPataBezier(xyzCoccumEsqT, k, 0, xyzStartEsqT.x - EsqT.xyz_ini.x, xyzStartEsqT.y - EsqT.xyz_ini.y, 0, totalPontos);
      floatxyz xyzDirF = this->DirF.trajetoriaPataBezier(xyzCoccumDirF, k, 0, xyzStartDirF.x - DirF.xyz_ini.x, xyzStartDirF.y - DirF.xyz_ini.y, 0, totalPontos);
      floatxyz xyzEsqM = this->EsqM.trajetoriaPataBezier(xyzCoccumEsqM, k, 0, xyzStartEsqM.x - EsqM.xyz_ini.x, xyzStartEsqM.y - EsqM.xyz_ini.y, 0, totalPontos);
      floatxyz xyzDirT = this->DirT.trajetoriaPataBezier(xyzCoccumDirT, k, 0, xyzStartDirT.x - DirT.xyz_ini.x, xyzStartDirT.y - DirT.xyz_ini.y, 0, totalPontos);
      int3 anglesEsqF = this->EsqF.cinematicaInversa(xyzEsqF);
      int3 anglesDirM = this->DirM.cinematicaInversa(xyzEsqM);
      int3 anglesEsqT = this->EsqT.cinematicaInversa(xyzEsqT);
      int3 anglesDirF = this->DirF.cinematicaInversa(xyzDirF);
      int3 anglesEsqM = this->EsqM.cinematicaInversa(xyzDirM);
      int3 anglesDirT = this->DirT.cinematicaInversa(xyzDirT);
      this->EsqF.moverPata(anglesEsqF.ombro, anglesEsqF.femur, anglesEsqF.tibia);
      this->DirM.moverPata(anglesDirM.ombro, anglesDirM.femur, anglesDirM.tibia);
      this->EsqT.moverPata(anglesEsqT.ombro, anglesEsqT.femur, anglesEsqT.tibia);
      this->DirF.moverPata(anglesDirF.ombro, anglesDirF.femur, anglesDirF.tibia);
      this->EsqM.moverPata(anglesEsqM.ombro, anglesEsqM.femur, anglesEsqM.tibia);
      this->DirT.moverPata(anglesDirT.ombro, anglesDirT.femur, anglesDirT.tibia);
      k++;
      delay(1);
    }
  }

  floatxyz rotacaoPata(floatxyz ponto, int3 angles){
    floatxyz rotPonto;
    float3 angles_rad = degreeToRad(angles);
    float roll = angles_rad.ombro;
    float pitch = angles_rad.femur;
    float yaw = angles_rad.tibia;
    rotPonto.x = ponto.x*cos(pitch)*cos(yaw) + ponto.y*(cos(yaw)*sin(pitch)*sin(roll) - cos(roll)*sin(yaw)) + ponto.z*(sin(roll)*sin(yaw) + cos(roll)*cos(yaw)*sin(pitch));
    rotPonto.y = ponto.x*cos(pitch)*sin(yaw) + ponto.y*(cos(roll)*cos(yaw) + sin(pitch)*sin(roll)*sin(yaw)) + ponto.z*(cos(roll)*sin(pitch)*sin(yaw) - cos(yaw)*sin(roll));
    rotPonto.z = -ponto.x*sin(pitch) + ponto.y*cos(pitch)*sin(roll) + ponto.z*cos(pitch)*cos(roll);
    return rotPonto;
  } 

  void cinematicaInversaCorpo(int3 angles){
    int3 anglesRot = angles*(-1);
    // Correcao de sinais nas pontas das patas
    floatxyz signalsDir = {-1.0,-1.0,1.0};
    floatxyz signalsEsq = {-1.0,1.0,1.0};
    // Aplicar a translacao do ombro corrigido pro centro do corpo
    floatxyz pontoEsqF = this->EsqF.xyz_ini*signalsEsq + OmbEsqF;
    floatxyz pontoDirM = this->DirM.xyz_ini*signalsDir + OmbDirM;
    floatxyz pontoEsqT = this->EsqT.xyz_ini*signalsEsq + OmbEsqT;
    floatxyz pontoDirF = this->DirF.xyz_ini*signalsDir + OmbDirF;
    floatxyz pontoEsqM = this->EsqM.xyz_ini*signalsEsq + OmbEsqM;
    floatxyz pontoDirT = this->DirT.xyz_ini*signalsDir + OmbDirT;
    // Aplicar a rotacao da pata
    floatxyz xyzEsqFrot = rotacaoPata(pontoEsqF,anglesRot);
    floatxyz xyzDirMrot = rotacaoPata(pontoDirM,anglesRot);
    floatxyz xyzEsqTrot = rotacaoPata(pontoEsqT,anglesRot);
    floatxyz xyzDirFrot = rotacaoPata(pontoDirF,anglesRot);
    floatxyz xyzEsqMrot = rotacaoPata(pontoEsqM,anglesRot);
    floatxyz xyzDirTrot = rotacaoPata(pontoDirT,anglesRot);
    // Aplicar a translacao de volta para o ombro e descorrige
    floatxyz xyzEsqF = (xyzEsqFrot - OmbEsqF)*signalsEsq;
    floatxyz xyzDirM = (xyzDirMrot - OmbDirM)*signalsDir;
    floatxyz xyzEsqT = (xyzEsqTrot - OmbEsqT)*signalsEsq;
    floatxyz xyzDirF = (xyzDirFrot - OmbDirF)*signalsDir;
    floatxyz xyzEsqM = (xyzEsqMrot - OmbEsqM)*signalsEsq;
    floatxyz xyzDirT = (xyzDirTrot - OmbDirT)*signalsDir;
    // Calcula angulos cinematica inversa
    int3 anglesEsqF = this->EsqF.cinematicaInversa(xyzEsqF);
    int3 anglesDirM = this->DirM.cinematicaInversa(xyzDirM);
    int3 anglesEsqT = this->EsqT.cinematicaInversa(xyzEsqT);
    int3 anglesDirF = this->DirF.cinematicaInversa(xyzDirF);
    int3 anglesEsqM = this->EsqM.cinematicaInversa(xyzEsqM);
    int3 anglesDirT = this->DirT.cinematicaInversa(xyzDirT);
    // Move as patas
    this->EsqF.moverPata(anglesEsqF.ombro, anglesEsqF.femur, anglesEsqF.tibia);
    this->DirM.moverPata(anglesDirM.ombro, anglesDirM.femur, anglesDirM.tibia);
    this->EsqT.moverPata(anglesEsqT.ombro, anglesEsqT.femur, anglesEsqT.tibia);
    this->DirF.moverPata(anglesDirF.ombro, anglesDirF.femur, anglesDirF.tibia);
    this->EsqM.moverPata(anglesEsqM.ombro, anglesEsqM.femur, anglesEsqM.tibia);
    this->DirT.moverPata(anglesDirT.ombro, anglesDirT.femur, anglesDirT.tibia);
  }

  int3 circularRollPitchYaw(int k, int angle_max){
    float3 angles_rad = {0.0,0.0,0.0};
    float angle_max_rad = float(angle_max)*M_PI/180.0;
    // Percentual em angulo do circulo (posicao do fasor)
    int kn = k % TOTAL_PONTOS_CIRCULAR;
    float t = float(kn)/TOTAL_PONTOS_CIRCULAR;
    float angle_rad = 2*M_PI*t;
    // Projecao em roll e pitch do fasor
    angles_rad.ombro = angle_max_rad*cos(angle_rad);
    angles_rad.femur = angle_max_rad*sin(angle_rad);
    return radToDegree(angles_rad);
  }

  void darPatinha(int k, int totalPontos){
    float dx = -10;
    float dy = 0;
    float dz = 10;
    floatxyz xyz = this->DirF.trajetoriaPataBezier(this->DirF.xyz_ini, k, 0, dx, dy, dz, totalPontos);
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
  int totalPontos = TOTAL_PONTOS;
  for (;;) {
    if(estado == 1){ // "Dar a patinha": move patinha para a posição X = [x_ini+10;y_ini;z_ini+10]
        totalPontos = 200;
        scarlet.darPatinha(k,totalPontos);
        if (k < (totalPontos/2)){
          k++;
        }
        vTaskDelay(pdMS_TO_TICKS(7));
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
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    else if(estado == 3){ // Rebola
        // int3 angles = {20,0,0};
        int3 angles = scarlet.circularRollPitchYaw(k, 10);
        scarlet.cinematicaInversaCorpo(angles);
        if (k == TOTAL_PONTOS_CIRCULAR - 1){
          k = 0;
        }
        else{
          k++;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    else if(estado == 10){ // Desarma
        scarlet.desligarHexapod();
        estado = 11;
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    else if(estado == 0){
      k = 0;
      scarlet.parar();
      vTaskDelay(pdMS_TO_TICKS(15));
    }
    else{
      vTaskDelay(pdMS_TO_TICKS(10));
    }
  }
}

void TaskComunicacao(void *pvParameters) {
  for (;;) {
    Dabble.processInput();
    if (estado != 11){
      if (GamePad.isCirclePressed()){       // "Dar a patinha"
        estado = 1;
      }
      else if(GamePad.getRadius() > 2){     // Andar omnidirecional
        angle_joystick = GamePad.getAngle();
        estado = 2;
      }
      else if(GamePad.isTrianglePressed()){ // Rebola
        estado = 3;
      }
      else if(GamePad.isSelectPressed()){   // Desarma
        estado = 10;
      }
      else{
        estado = 0;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(20));   
  }
}
