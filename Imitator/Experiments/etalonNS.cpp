#include "Headers/etalonNS.h"
#include <iostream>
#include "math.h"

using namespace std;

int etalonNS (double* Vxynk, double factKurs, double* factPosition, double* factV, double deltaT) {
    
    //большая полуось земного эллипса
    int a = 6378245;
    
    //квадрат эксцентриситета
    double ex2 = 0.00669342162;
    
    //фактические составляющие скорости
    factV[0] = Vxynk[0]*cos(factKurs) - Vxynk[1]*sin(factKurs);
    factV[1] = Vxynk[0]*sin(factKurs) + Vxynk[1]*cos(factKurs);
    
    //радиус кривизны первого вертикала:
    double R1 = a/(sqrt(1-ex2 * pow (sin(factPosition[0]),2)));
    //радиус кривизны меридианного сечения:
    double R2 = (a*(1-ex2))/  sqrt(pow((1-ex2*pow(sin(factPosition[0]),2)),3));
    
    //фактическая прибавка к координатам
    double factDeltaFi = factV[0] * 1/R1 * deltaT * 180 / M_PI;
    double factDeltaLambda = factV[1] * 1/R2 * deltaT * 180 / M_PI;
    
    //фактитческие координаты аппарата
    factPosition[0] = factPosition[0] + factDeltaFi;
    factPosition[1] = factPosition[1] + factDeltaLambda/ cos(factPosition[0]);
    
    //вывод параметров в консоль
    //cout << "    factFi " << factPosition[0] << " factLambda " << factPosition [1];
    
    return 0;
}
