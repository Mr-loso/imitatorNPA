#include <iostream>
#include <math.h>

#include "Headers/course.h"
#include "Utilities/Headers/radianControl.h"
#include "Headers/deviation.h"

using namespace std;

int course (double kurs, double kursIzm, double kursDeviation) {
    
    //формирование погрешности измерений
    kursDeviation += Wnoise(0.5);
    
    //расчет текущего курса
    kurs = kursIzm + kursDeviation;
    
    //проверка на соответствие диапозону значений 0 - 2pi
    kurs = radianControl(kurs);
    
    //вывод текущих показаний курса в консоль
    cout << "      K: " << kurs;
    
    return 0;
}
