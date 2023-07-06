#include "Headers/speed.h"
#include "Headers/deviation.h"
#include <iostream>
#include <math.h>

using namespace std;

int speedINS (double* V, double* Vizm, double* Vxynk, double* Vdeviation, double accDeviation, double kurs, double time, double deltaT) {
    
    //весовой коэффициент
    double alpha = 0.9;
    
    //формирование показаний измерителя
    Vizm[0] = Vxynk[0]*sin(kurs);
    Vizm[1] = Vxynk[1]*cos(kurs);
    
    //формирование погрешности измерений
    Vdeviation[0] = redNoise(0.514/sqrt(2)) + pow(time, 0.84) * accDeviation;
    Vdeviation[1] = redNoise(0.514/sqrt(2)) + pow(time, 0.84) * accDeviation;
    
//    Vdeviation[0] = redNoise(0.514/sqrt(2)) + time * accDeviation;
//    Vdeviation[1] = redNoise(0.514/sqrt(2)) + time * accDeviation;
    
    // текущие значения становятся предыдущими
    V[0] = V[2];
    V[1] = V[3];
    
    //перерасчет текущих значений скорости
    V[2] = V[2] + pow(alpha, 0.1)*(V[2]-V[0]) + (1-pow(alpha, 0.1))*Vdeviation[0];
    V[3] = V[3] + pow(alpha, 0.1)*(V[3]-V[1]) + (1-pow(alpha, 0.1))*Vdeviation[1];
    
    //вывод текущих значений скорости в консоль
    //cout << "      Vn: " <<V[2]<< "  Ve: "<< V[3];
    
    return 0;
}

int refreshGAL (double* V, double* Vizm, double* Vxynk, double* Vdeviation, double kurs) {
    
    //формирование погрешности измерений
    Vdeviation[0] = YanusLagDeviation(20);
    Vdeviation[1] = YanusLagDeviation(20);
    
    //формирование измерений
    Vizm[0] = Vxynk[0]*cos(kurs) - Vxynk[1]*sin(kurs);
    Vizm[1] = Vxynk[0]*sin(kurs) + Vxynk[1]*cos(kurs);
    
    //весовой коэффициент
    double alpha = 0.9;
    
    // текущие значения становятся предыдущими
    V[0] = V[2];
    V[1] = V[3];
    
    //перерасчет текущих значений скорости
    V[2] = (alpha*Vizm[0]) + (1-alpha)*(V[0]+Vdeviation[0]);
    V[3] = (alpha*Vizm[1]) + (1-alpha)*(V[1]+Vdeviation[1]);
    
    //вывод текущих значений скорости в консоль
    //cout << "      Vn: " <<V[2]<< "  Ve: "<< V[3];
    
    return 0;
}
