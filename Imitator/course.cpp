#include <iostream>
#include <math.h>

#include "Headers/course.h"
#include "Utilities/Headers/radianControl.h"
#include "Headers/deviation.h"

using namespace std;

int course (double kurs, double kursIzm, double kursDeviation, double kursDeviationSKO ) {
    
    //формирование погрешности измерений
    kursDeviation = redNoise(kursDeviationSKO);
    
    //расчет текущего курса
    kurs = kursIzm + kursDeviation;
    
    //проверка на соответствие диапозону значений 0 - 2pi
    kurs = radianControl(kurs);
    
    //вывод текущих показаний курса в консоль
    //cout << "      K: " << kurs;
    
    return 0;
}
