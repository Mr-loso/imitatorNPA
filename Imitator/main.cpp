#include <iostream>
#include <math.h>
#include <time.h>
#include <fstream>

#include "Headers/deviation.h"
#include "Headers/speed.h"
#include "Headers/position.h"
#include "Headers/course.h"
#include "Headers/psiTeta.h"
#include "Experiments/Headers/etalonNS.h"

using namespace std;

bool loadConfig();

//перевод из градусов в радианы
double gradToRad = M_PI / 180;

//перевод из радиан в градусы
double radToGrad = 180 / M_PI;

//--------параметры для чтения из файла--------//

//частота решения задачи
double deltaT;

//частота поступления данных от ГАЛ
double GAL_Frequency;

//географические координаты аппарата { fi[-1], lambda[-1], fi, lambda }
double Position[4];

//продольная и поперечная составляющая скорости в связанной с аппаратом системе координат ("+" на правый борт, "+" вперед), м/с.
double Vxynk[2];

//начальное значение курса
double kurs;

//СКО марковского процесса 1-го порядка для расчета курса
double kursDeviationSKO;

//начальные значения углов крена и дифферента
double psiTeta[2];

//режим моделирования. 1 - проведение эксперимента 1, 2 - проведение эксперимента 2
double mode;

//время проведения моделирования (только для mode 1)
double endTime;

//массивы для записи координат конечных координат галсов. GalsN - значения Fi, GalsE - значения Lambda (только для mode 2)
double GalsN[4], GalsE[4];

//максимально допустимое СКО по координатам,М (только для mode 2)
double MaxСoordinatesSkoM;

//максимально допустимое число всплытий для коррекции (только для mode 2)
double MaxNumberOfEmersion;


int main () {
    
    //запуск таймера выполнения имитационного моделирования
    clock_t start = clock();
    
    //проверка наличия файла конфигурации и чтение данных из него
    if (!loadConfig())
    {
        //значения по умолчанию при отсутствии файла конфигурации или если файл составлен некорректно
        GAL_Frequency = 1;
        deltaT = 0.1;
        Position[2] = 0;
        Position[3] = 0;
        Vxynk[0] = 1;
        Vxynk[1] = 0;
        kurs = 0;
        psiTeta[0] = 0;
        psiTeta[1] = 0;
        mode = 1;
        endTime = 60*10; //10 минут
        kursDeviationSKO = 12/60;
        MaxNumberOfEmersion = 3;
    }
    
    
    ofstream LogFile("Experiments/experimentLog.txt");
    
    //---------- параметры моделирования ----------//
    //таймер навигации, сек
    double time = 0;
    
    //составляюшие скорости { Vn[-1], Ve[-1], Vn, Ve }, м/с
    double V[4] = {0,0,0,0};
    
    //весовой коэффициент. 0,9 –  при демпфировании по абсолютной скорости, 0,5 – при демпфировании по относительной скорости, 0 – при отсутствии демпфирования
    
    //начальные погрешности определения места по широте и отшествию
    double deltaFi0 = 100, deltaW0 = 100;
    
    double factV[2];
    
    //СКО скорости
    double skoVNE[2];
    
    //СКО определения координат
    double skoFiLambda = 0;
    
    //---------- данные для расчета абсолютной скорости ----------//
    
    //данные, поступающие от измерителя скорости
    double Vizm[2];
    
    //погрешности скорости
    double Vdeviation[2];
    
    
    //---------- данные для расчета курса ----------//
    
    //данные, поступающие от измерителя курса
    double kursIzm = kurs;
    
    //погрешность измерения курса
    double kursDeviation;
    
    //фактический курс для расчета эталона
    double factKurs = kurs;
    
    //---------- данные для расчета боковой и килевой качки ----------//
    
    //данные, поступающие от измерителя бортовой и килевой качки
    double psiTetaIzm[2] = {0 * gradToRad,  0 * gradToRad};
    
    //погрешности измерения бортовой и килевой качки (будут значения Марковской последовательности)
    double psiTetaDeviation[2] = {0,0};
    
   
    
    
    
    
    //---------- выполнение задачи ----------//
    
    //установка точности вывода данных в консоль
    cout.setf(ios::fixed);
    cout.precision(6);
    
    //вычисление координат с начальными погрешностями определения места по широте и отшествию
    firstStepPosition(Position, deltaFi0, deltaW0);
    
    double factPosition[4] = { Position[2] , Position[3] , Position[2], 0};
    
    //Алгоритм применяется при проведении эксперимента 1
    if (mode == 1) {
        
        while (time < endTime)
        {
            time += deltaT;
            
            //вывести время в часах
            //cout << "hours: " << time/60/60;
            
            //вывести время в минутах
            cout << "min: " << time/60;
            
            //вывести время в секундах
            //cout << "sec: " << time;
            
            //вычисление курса по данным измерителя
            course(kurs, kursIzm, kursDeviation, kursDeviationSKO);
            
            //перерасчет скорости с полученными данными от ГАЛ
            refreshGAL(V, Vizm, Vxynk, Vdeviation, kurs);
            
            //вычисление координат по вычисленной скорости
            positionINS(Position, V, deltaT);
            
            //вычисление крена и дифферента данным измерителей
            psiTetaCalc(psiTeta, psiTetaIzm, psiTetaDeviation);
            
            etalonNS(Vxynk, factKurs, factPosition, factV, deltaT);
            
            skoVNE[0] = sqrt(pow(factV[0] - V[2], 2));
            skoVNE[1] = sqrt(pow(factV[1] - V[3], 2));
            
            LogFile << skoVNE[0] << " ; " << skoVNE[1];
            
            
            LogFile << " ; <---коррекция"<< endl;
            
            cout << endl;
            
            for (int i = 0; i < GAL_Frequency*10; i++)
            {
                time += deltaT;
                
                //вывести время в часах
                //cout << "hours: " << time/60/60;
                
                //вывести время в минутах
                cout << "min: " << time/60;
                
                //вывести время в секундах
                //cout << "sec: " << time;
                
                //вычисление курса по данным измерителя
                course(kurs, kursIzm, kursDeviation, kursDeviationSKO);
                
                //вычисление скорости по данным измерителя
                speedINS(V, Vizm, Vxynk, Vdeviation, kurs);
                
                //вычисление координат по вычисленной скорости
                positionINS(Position, V, deltaT);
                
                //вычисление крена и дифферента данным измерителей
                psiTetaCalc(psiTeta, psiTetaIzm, psiTetaDeviation);
                
                etalonNS(Vxynk, factKurs, factPosition, factV, deltaT);
                
                skoVNE[0] = sqrt(pow(factV[0] - V[2], 2));
                skoVNE[1] = sqrt(pow(factV[1] - V[3], 2));
                
                LogFile << skoVNE[0] << " ; " << skoVNE[1] << endl;
                
                cout  << endl;
            }
        }
    }
    //Алгоритм применяется при проведении эксперимента 2
    else if (mode == 2)
    {
        
        //вывести координаты галсов
        for (int i = 0; i < 4; i++)
        {
            cout.precision(4);
            
            cout << "point " << GalsE[i] << "  " << GalsN[i] << endl;
        }
        
        cout.setf(ios::fixed);
        cout.precision(6);
        
        //текущее число всплытий
        int numberOfEmersion = 0;
        
        //текущий галс
        int currentGals = 0;
        
        //признак достижения аппаратом точки назначения. false - не достигнута. true - достигнута
        bool destinationPointReached = false;
        
        //расстояние до пункта назначения, м
        double distanceToTarget = sqrt (pow(GalsN[currentGals]-Position[2], 2) - pow(GalsE[currentGals]-Position[3], 2) )* 111321*cos(Position[3]);
        
        //максимально допустимое СКО по координатам выражаемое в смещении координат
        double MaxСoordinatesSKO = MaxСoordinatesSkoM/111321*cos(Position[3]);
        
        //пока число всплытий меньше допустимого и пункт назначния не достигнут
        // (общий цикл навигации)
        while (numberOfEmersion <= MaxNumberOfEmersion && !destinationPointReached)
        {
            //пока текущее СКО не превышает максимальное СКО и дистанция до пункта назначения больше максимально допустимого СКО (выраженного в метрах)
            //(цикл штатной навигации)
            while (skoFiLambda < MaxСoordinatesSKO && distanceToTarget > MaxСoordinatesSkoM)
            {
                time += deltaT;
                
                //вывести время в часах
                cout << "hours: " << time/60/60;
                
                //вывести время в минутах
                //cout << "min: " << time/60;
                
                //вывести время в секундах
                //cout << "sec: " << time;
                
                //вычисление курса по данным измерителя
                course(kurs, kursIzm, kursDeviation, kursDeviationSKO);
                
                //перерасчет скорости с полученными данными от ГАЛ
                refreshGAL(V, Vizm, Vxynk, Vdeviation, kurs);
                
                //вычисление координат по вычисленной скорости
                positionINS(Position, V, deltaT);
                
                //вычисление крена и дифферента данным измерителей
                psiTetaCalc(psiTeta, psiTetaIzm, psiTetaDeviation);
                
                //расчет эталонных значний
                etalonNS(Vxynk, factKurs, factPosition, factV, deltaT);
                
                //расчет текещего СКО
                skoFiLambda = (sqrt(pow(factPosition[0] - Position[2], 2)) + sqrt(pow(factPosition[1] - Position[3], 2)));
                
                LogFile << endl << skoFiLambda;
                
                LogFile << " ; <---коррекция"<< endl;
                
                cout << endl;
                
                for (int j = 0; j < GAL_Frequency*10; j++)
                {
                    time += deltaT;
                    
                    //вывести время в часах
                    cout << "hours: " << time/60/60;
                    
                    //вывести время в минутах
                    //cout << "min: " << time/60;
                    
                    //вывести время в секундах
                    //cout << "sec: " << time;
                    
                    //вычисление курса по данным измерителя
                    course(kurs, kursIzm, kursDeviation, kursDeviationSKO);
                    
                    //вычисление скорости по данным измерителя
                    speedINS(V, Vizm, Vxynk, Vdeviation, kurs);
                    
                    //вычисление координат по вычисленной скорости
                    positionINS(Position, V, deltaT);
                    
                    //вычисление крена и дифферента данным измерителей
                    psiTetaCalc(psiTeta, psiTetaIzm, psiTetaDeviation);
                    
                    //расчет эталонных значний
                    etalonNS(Vxynk, factKurs, factPosition, factV, deltaT);
                    
                    //расчет текещего СКО
                    skoFiLambda = (sqrt(pow(factPosition[0] - Position[2], 2)) + sqrt(pow(factPosition[1] - Position[3], 2)));
                    
                    //расчет дистанции до пункта назначения
                    distanceToTarget = sqrt(pow(GalsN[currentGals]-Position[2], 2) - pow(GalsE[currentGals]-Position[3], 2))*111321*cos(Position[3]);
                    
                    //если дистанция до пункта назначения меньше максимально допустимого СКО -> выход из цикла штатной навигации
                    if (MaxСoordinatesSkoM > distanceToTarget) {
                        break;
                    }
                    //вывод выбранных параметров в log файл
                    LogFile << endl << time << "    skoFiLambda   " << skoFiLambda << "   dist:   "<< distanceToTarget;
                    
                    cout  << endl;
                    
                }
                //процесс навигации, расчет skoFiLambda, расчет distanceToTheTarget
            }
            
            time += deltaT;
            
            //вывести время в часах
            cout << "hours: " << time/60/60;
            
            //вывести время в минутах
            //cout << "min: " << time/60;
            
            //вывести время в секундах
            //cout << "sec: " << time;

            //если текущее СКО превышает максимальное СКО
            if (skoFiLambda > MaxСoordinatesSKO)
            {
                //число всплытий увеличивается на 1
                numberOfEmersion += 1;
                
                
                //если число всплытий больше 3
                if (numberOfEmersion > 3) {
                    // вывод конечных результатов
                    cout << endl << "погрешность превысила допустимую, возможности всплыть больше не осталось" << endl;
                    //выход из общего цикла навигации
                    break;
                }
                //если число всплытий меньше 3
                else
                {
                    cout << endl << "погрешность превысила допустимую, коррекция" << endl;
                    
                    //коррекция по СНС
                    positionSNS(Position, factPosition);
                    
                    //Перерасчет СКО координат
                    skoFiLambda = (sqrt(pow(factPosition[0] - Position[2], 2)) + sqrt(pow(factPosition[1] - Position[3], 2)));
                }
            }
            // если текущее СКО не превышает максимальное СКО, значит выход из цикла штатной навигации произошел по причине того, что дистанция до пункта назначения меньше максимально допустимого СКО (выраженного в метрах). Следовательно, аппарат прибыл в пункт назначения
            else
            {
                cout << endl << "прибытие в точку назначения" << endl;
                
                // прибытие в пункт назначения = true
                destinationPointReached = true;
                
                // вывод конечных результатов
                cout<< endl << "потребовалось " <<  numberOfEmersion  << " всплытий" << endl;
                
                //выход из общего цикла навигации
                break;
            }
        }
    }
    
    
    //остановка таймера и вывод затраченного времени в консоль
    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "completed in: " << seconds << " sec" << endl;
    
    //закрытие log-файла
    LogFile.close();
    
    return 0;
}

//функция для загрузки параметров из файла конфигурации "ImitatorConfiguration.config"
bool loadConfig()
{
    //попытка открыть файл
    ifstream configFile("Config/imitatorConfiguration.config");
    
    //функция вернет false в случае, если файл не найден
    if (!configFile.is_open())
    {
        cout << "Configuration file is not found. Default parameters are set." << endl;
        return false;
    };
    
    //чтение данных из открытого файла. key-ключ , value-значение
    string key;
    double value;
    while (!configFile.eof())
    {
        configFile >> key;
        configFile >> value;
        if (key == "Delta_T(hz)")
        {
            //частота решения задачи
            deltaT = 1/value;
        }
        else if (key == "GAL_Frequency(sec)")
        {
            //частота поступления данных от ГАЛ
            GAL_Frequency = value;
        }
        else if (key == "PositionFi")
        {
            //начальное положение объекта - широта
            Position[2] = value;
        }
        else if (key == "PositionLambda")
        {
            //начальное положение объекта - долгота
            Position[3] = value;
        }
        else if (key == "SkorostNK_forvard(m/s)")
        {
            //начальные данные о продольной скорости в СК связанной с аппаратом
            Vxynk[0] = value;
        }
        else if (key == "SkorostNK_starboard(m/s)")
        {
            //начальные данные о поперечной скорости в СК связанной с аппаратом
            Vxynk[1] = value;
        }
        else if (key == "Kurs(grad)")
        {
            //начальный курс
            kurs = value * gradToRad;
        }
        else if (key == "Psi(grad)")
        {
            //начальный крен
            psiTeta[0] = value * gradToRad;
        }
        else if (key == "Teta(grad)")
        {
            //начальный дифферент
            psiTeta[1] = value * gradToRad;
        }
        
        else if (key == "kursDeviationSKO")
        {
            //СКО марковского процесса 1-го порядка для расчета курса
            kursDeviationSKO = value * gradToRad;
        }
        
        else if (key == "EndTime(min)")
        {
            //время моделирования
            endTime = value * 60;
        }
        else if (key == "Experiment")
        {
            //
            mode = value;
        }
        else if (key == "Gals1N")
        {
            //
            GalsN[0] = value;
        }
        else if (key == "Gals1E")
        {
            //
            GalsE[0] = value;
        }
        else if (key == "Gals2N")
        {
            //
            GalsN[1] = value;
        }
        else if (key == "Gals2E")
        {
            //
            GalsE[1] = value;
        }
        else if (key == "Gals3N")
        {
            //
            GalsN[2] = value;
        }
        else if (key == "Gals3E")
        {
            //
            GalsE[2] = value;
        }
        else if (key == "Gals4N")
        {
            //
            GalsN[3] = value;
        }
        else if (key == "Gals4E")
        {
            //
            GalsE[3] = value;
        }
        else if (key == "MaxСoordinatesSKO(M)")
        {
            //максимально допустимое СКО по координатам,М
            MaxСoordinatesSkoM = value;
        }
        else if (key == "MaxNumberOfEmersion")
        {
            //максимально допустимое число всплытий для коррекции
            MaxNumberOfEmersion = value;
        }
        
        else
        {
            //если найдены неизвестные параментры - возвращает false, будут установлены параметры по умолчанию
            cout << "Configuration file is incorrect. Default parameters are set." << endl;
            return false;
        }
    }
    
    //вывод в консоль об успешной загрузке параметров из файла
    cout << "Configuration file is uploaded."<< endl;
    configFile.close();
    return true;
}
