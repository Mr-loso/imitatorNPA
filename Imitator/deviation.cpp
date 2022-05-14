#include "deviation.h"
#include <iostream>
#include "math.h"

using namespace std;

//выработка погрешностей ГАЛ
double YanusLagDeviation(double H) {
    
    //длина зондирующего сигнала
    double T;
    
    //угол излучения относительно нормали
    double psiIzl = 30 * 180 / M_PI;
    
    //скорость звука в водной среде, м/с
    double Czv = 1450;
    
    //частота зондирующего сигнала
    double f = 20000;
    
    T = H/(Czv * cos(psiIzl));
    
    //СКО
    double YanusSKO = Czv * (1/3.5 * T) / (2 * f * sin(psiIzl));
    
    //белый шум с заданным СКО
    return noise(YanusSKO);
    
}

//генератор красного шума (Марковский процесс 1-го порядка)
double redNoise(double SKO)
{
    double b0=0, b1=0, b2=0, b3=0, b4=0, b5=0, b6=0;
    double red;
    b0 = 0.99886 * b0 + (2 * ((rand()/(static_cast<double>(RAND_MAX))) - 0.5)) * 0.0555179;
    b1 = 0.99332 * b1 + (2 * ((rand()/(static_cast<double>(RAND_MAX))) - 0.5)) * 0.0750759;
    b2 = 0.96900 * b2 + (2 * ((rand()/(static_cast<double>(RAND_MAX))) - 0.5)) * 0.1538520;
    b3 = 0.86650 * b3 + (2 * ((rand()/(static_cast<double>(RAND_MAX))) - 0.5)) * 0.3104856;
    b4 = 0.55000 * b4 + (2 * ((rand()/(static_cast<double>(RAND_MAX))) - 0.5)) * 0.5329522;
    b5 = -0.7616 * b5 - (2 * ((rand()/(static_cast<double>(RAND_MAX))) - 0.5)) * 0.0168980;
    b6 = (2 * ((rand()/(static_cast<double>(RAND_MAX))) - 0.5)) * 0.115926;
    red = b0 + b1 + b2 + b3 + b4 + b5 + b6 + (2 * ((rand()/(static_cast<double>(RAND_MAX))) - 0.5)) * 1.66 * SKO;
    return red;
}

//генератор белого шума
double noise (double K) {
    double x;
    
    x = K - rand()/(double(RAND_MAX)) * 2 * K;
    
    return x;
}
