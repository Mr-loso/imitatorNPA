#include <iostream>
#include <math.h>
#include <time.h>
#include <fstream>

#include "Headers/deviation.h"
#include "Headers/speed.h"
#include "Headers/position.h"
#include "Headers/course.h"
#include "Headers/psiTeta.h"

using namespace std;

bool loadConfig();

//перевод из градусов в радианы
double gradToRad = M_PI / 180;

//перевод из радиан в градусы
double radToGrad = 180 / M_PI;

//параметры для чтения из файла
double deltaT, GAL_Frequency;
double Position[2];
double Vxynk[2];
double kurs;
double psiTeta[2];

int main () {
    
    //запуск таймера выполнения имитационного моделирования
    clock_t start = clock();
    
    //проверка наличия файла конфигурации и чтение данных из него
    if (!loadConfig())
    {
        //значения по умолчанию при отсутствии файла конфигурации или если файл составлен некорректно
        GAL_Frequency = 1;
        deltaT = 0.1;
        Position[0] = 0;
        Position[1] = 0;
        Vxynk[0] = 1;
        Vxynk[1] = 0;
        kurs = 0;
        psiTeta[0] = 0;
        psiTeta[1] = 0;
    };
    
    //---------- параметры моделирования ----------//
    //таймер навигации, сек
    double time = 0;
    
    //признак работы средства БИНС (0 – работа БИНС с коррекцией или демпфированием; 1 – работа БИНС без коррекции или демпфирования)
    //bool priznakBinsNoKorr = 1;
    
    //составляюшие скорости [ Vn[-1], Ve[-1], Vn, Ve ] м/с
    double V[4] = {0,0,0,0};
    
    
    //---------- данные для расчета абсолютной скорости ----------//
    
    //тип измерителя скорости: gal=1 / ins=2 / kns=3 / vsg=4
    //int Vtype = 1;
    
    //продольная и поперечная составляющая скорости в связанной с аппаратом системе координат ("+" на правый борт, "+" вперед), м/с.
   
    
    //данные, поступающие от измерителя скорости
    double Vizm[2] = {2,0};
    
    //погрешности скорости:
    double Vdeviation[2] = {0,0};

    //весовой коэффициент. 0,9 –  при демпфировании по абсолютной скорости, 0,5 – при демпфировании по относительной скорости, 0 – при отсутствии демпфирования
    double alpha = 0.9;
    
    
    //---------- данные для расчета курса ----------//
    
    //данные, поступающие от измерителя курса
    double kursIzm = 180 * gradToRad;
    
    //погрешность измерения курса (будут значения Марковской последовательности)
    double kursDeviation=0;
    
    //---------- данные для расчета боковой и килевой качки ----------//
    
    //данные, поступающие от измерителя бортовой и килевой качки
    double psiTetaIzm[2] = {0 * gradToRad,  0 * gradToRad};
    
    //погрешности измерения бортовой и килевой качки (будут значения Марковской последовательности)
    double psiTetaDeviation[2] = {0,0};
    
    
    //---------- выполнение задачи ----------//
    
    //установка точности вывода данных в консоль
    cout.setf(ios::fixed);
    cout.precision(6);
    
    //моделирование останавливается по достижению определенного времени. Критерий меняется в зависимости от решаемой задачи
    while (time < 60*60)
    {
        time += deltaT;
        
        //вывести время в часах
        //cout << "hours: " << time/60/60;
        
        //вывести время в минутах
        cout << "min: " << time/60;
        
        //вывести время в секундах
        //cout << "sec: " << time;
        
        //вычисление курса по данным измерителя
        course(kurs, kursIzm, kursDeviation);
        
        //перерасчет скорости с полученными данными от ГАЛ
        refreshGAL(V, Vizm, Vxynk, Vdeviation, kurs);
        
        //вычисление координат по вычисленной скорости
        positionINS(Position, V, deltaT);
        
        //вычисление крена и дифферента данным измерителей
        psiTetaCalc(psiTeta, psiTetaIzm, psiTetaDeviation);
        
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
            course(kurs, kursIzm, kursDeviation);
            
            //вычисление скорости по данным измерителя
            speedINS(V, Vizm, Vxynk, Vdeviation, kurs);
            
            //вычисление координат по вычисленной скорости
            positionINS(Position, V, deltaT);
            
            //вычисление крена и дифферента данным измерителей
            psiTetaCalc(psiTeta, psiTetaIzm, psiTetaDeviation);
                
            cout << endl;
        }
    }
    
    //остановка таймера и вывод затраченного времени в консоль
    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "completed in: " << seconds << " sec" << endl;
    
    //вывод в консоль версии компилятора
    //std::cout << endl << __cplusplus << std::endl;
    
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
        if (key == "delta_T(hz)")
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
            //
            Position[0] = value;
        }
        else if (key == "PositionLambda")
        {
            //
            Position[1] = value;
        }
        else if (key == "SkorostNK_forvard(m/s)")
        {
            //
            Vxynk[0] = value;
        }
        else if (key == "SkorostNK_starboard(m/s)")
        {
            //
            Vxynk[1] = value;
        }
        else if (key == "Kurs(grad)")
        {
            //
            kurs = value * gradToRad;
        }
        else if (key == "Psi(grad)")
        {
            //
            psiTeta[0] = value * gradToRad;
        }
        else if (key == "Teta(grad)")
        {
            //
            psiTeta[1] = value * gradToRad;
        }
    };
    
    //вывод в консоль об успешной загрузке параметров из файла
    cout << "Configuration file is uploaded."<< endl;
    configFile.close();
    return true;
}
