#include "speed.h"
#include "deviation.h"
#include <iostream>
#include <math.h>

using namespace std;

int speedINS (double* V, double* Vizm, double* Vnk, double* Vdeviation, double kurs) {
    
    //весовой коэффициент
    double alpha = 0.9;
    
    //формирование показаний измерителя
    Vizm[0] = Vnk[0]*sin(kurs*180 / M_PI);
    Vizm[1] = Vnk[1]*cos(kurs*180 / M_PI);
    
    //формирование погрешности измерений
    Vdeviation[0] = Wnoise(0.5);
    Vdeviation[1] = Wnoise(0.5);
    
    //составляюшие скорости Vn[-1], Ve[-1], Vn, Ve, м/с
    //текущие значения становятся предыдущими
    V[0] = V[2];
    V[1] = V[3];

    //перерасчет текущих значений скорости
    V[2] = V[2] + pow(alpha, 0.1)*(V[2]-V[0]) + (1-pow(alpha, 0.1))*Vdeviation[0];
    V[3] = V[3] + pow(alpha, 0.1)*(V[3]-V[1]) + (1-pow(alpha, 0.1))*Vdeviation[1];
    
    //вывод текущих значений скорости в консоль
    cout << "      Vn: " <<V[2]<< "  Ve: "<< V[3];
    
    return 0;
}

int refreshGAL (double* V, double* Vizm, double* Vxynk, double* Vdeviation, double kurs) {
    
    //формирование погрешности измерений
    Vdeviation[0] = Wnoise(0.5);
    Vdeviation[1] = Wnoise(0.5);

    //формирование измерений
    Vizm[0] = Vxynk[0]*cos(kurs) - Vxynk[1]*sin(kurs);
    Vizm[1] = Vxynk[0]*sin(kurs) + Vxynk[1]*cos(kurs);
    
    //весовой коэффициент
    double alpha = 0.9;
    
    V[0] = V[2];
    V[1] = V[3];
    
    //перерасчет текущих значений скорости
    V[2] = (alpha*Vizm[0]) + (1-alpha)*(V[0]+Vdeviation[0]);
    V[3] = (alpha*Vizm[1]) + (1-alpha)*(V[1]+Vdeviation[1]);
    
    //вывод текущих значений скорости в консоль
    cout << "      Vn: " <<V[2]<< "  Ve: "<< V[3];
    
    return 0;
}
