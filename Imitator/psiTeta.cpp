#include <iostream>
#include "math.h"
#include "psiTeta.h"
#include "Utilities/Headers/radianControl.h"
#include "Headers/deviation.h"

using namespace std;

int psiTetaCalc (double* psiTeta, double* psiTetaIzm, double* psiTetaDeviation) {
    
    //формирование погрешности измерений
    psiTetaDeviation[0] = Wnoise(0.05);
    psiTetaDeviation[1] = Wnoise(0.05);
    
    //расчет текущих значений крена и дифферента
    psiTeta[0] = psiTetaIzm[0] + psiTetaDeviation[0];
    psiTeta[1] = psiTetaIzm[1] + psiTetaDeviation[1];
        
    //проверка на соответствие диапозону значений 0 - 2pi
    psiTeta[0] = radianControl(psiTeta[0]);
    psiTeta[1] = radianControl(psiTeta[1]);
    
    //вывод в консоль значений крена и дифферента
    cout << "    psi: " << psiTeta[0] << "  teta: " << psiTeta[1];
    
    return 0;
}
