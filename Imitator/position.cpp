#include <iostream>
#include <math.h>
#include "position.h"

using namespace std;

//расчет координат по данным БИНС
int positionINS (double* position, double* speeds, double deltaT) {
    
    //большая полуось земного эллипса
    int a = 6378245;
    
    //квадрат эксцентриситета
    double ex2 = 0.00669342162;
    
    //радиус кривизны первого вертикала:
    double R1 = a/(sqrt(1-ex2 * pow (sin(position[0]),2)));
    //радиус кривизны меридианного сечения:
    double R2 = (a*(1-ex2))/  sqrt(pow((1-ex2*pow(sin(position[0]),2)),3));
    
    //погрешности определения координат
    double deviationFi = speeds[0] * 1/R1 * deltaT;
    double deviationLambda = speeds[1] * 1/R2 * deltaT;
    
    //расчет географических координат объекта
    position[0] = position[0] + deviationFi;
    position[1] = (position[1] + deviationLambda)/cos(position[0]);
    
    //вывод в консоль координат объекта
    cout << "      fi: " << position[0] << "  lambda: " << position[1];
    
    return 0;
}

//перерасчет скорости с полученными данными от ГАЛ
int positionSNS() {
    
    
    return 0;
}
