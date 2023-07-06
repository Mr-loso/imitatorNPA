#include <iostream>
#include <math.h>
#include "Headers/position.h"
#include "Headers/deviation.h"

using namespace std;



//расчет координат на первом шаге решения задачи
int firstStepPosition (double* position, double deltaFi0, double deltaW0) {
    
    
    //большая полуось земного эллипса
    double a = 6378245;
    
    //квадрат эксцентриситета
    double ex2 = 0.00669342162;
    
    //радиус кривизны первого вертикала:
    double Rlambda = a/(sqrt(1-ex2 * pow (sin(position[2]),2)));
    //радиус кривизны меридианного сечения:
    double Rfi = (a*(1-ex2))/  sqrt(pow((1-ex2*pow(sin(position[2]),2)),3));
    
    position[2] = position[2] + deltaFi0 / Rfi;
    position[3] = position[3] + deltaW0 / (Rlambda * cos(position[2]));
    
    
    return 0;
}




//расчет координат по данным БИНС
int positionINS (double* position, double* speeds, double deltaT) {
    
    //большая полуось земного эллипса
    double a = 6378245;
    
    //квадрат эксцентриситета
    double ex2 = 0.00669342162;
    
    position[0] = position[2];
    position[1] = position[3];
    
    //радиус кривизны первого вертикала:
    double R1 = a/(sqrt(1-ex2 * pow (sin(position[2]),2)));
    //радиус кривизны меридианного сечения:
    double R2 = (a*(1-ex2))/  sqrt(pow((1-ex2*pow(sin(position[2]),2)),3));
    
    double deltaFi = speeds[2] * 1/R1 * deltaT * 180 / M_PI;
    double deltaW = speeds[3] * 1/R2 * deltaT * 180 / M_PI;

    
    position[2] = position[0] + deltaFi;
    position[3] = position[1] + deltaW / cos(position[0]);
    
    //вывод в консоль координат объекта
    //cout << "      fi: " << position[2] << "  lambda: " << position[3];
    
    return 0;
}

//коррекция координат по полученным данным от СНС
int positionSNS(double* position, double * truePosition) {
    
    position[0] = position[2];
    position[1] = position[3];

    double dev = (redNoise(30)+redNoise(30))/(111321*cos(position[3])*2);
    position[2] = truePosition[0] + dev;
    dev = (redNoise(30)+redNoise(30))/(111321*cos(position[3])*2);
    position[3] = truePosition[1] + dev;

    return 0;
}
