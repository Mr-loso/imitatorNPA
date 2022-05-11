#include "deviation.h"
#include <iostream>
#include "math.h"

using namespace std;


double YanusLagDeviation(double H) {
    // длина зондирующего сигнала
    double T;
    
    //угол излучения относительно нормали
    double psiIzl = 30 * 180 / M_PI;
    
    //скорость звука в водной среде, м/с
    double Czv = 1450;
    
    //частота зондирующего сигнала
    double f = 20000;
    
    double Yanus;
    
    T = H/(Czv * cos(psiIzl));
    
    Yanus = Czv * (1/3.5 * T) / (2 * f * sin(psiIzl));
    
    return Wnoise(Yanus);
}


double Wnoise (double K) {
    double x;
    
    x = K/2 - rand()/(double(RAND_MAX)) * K;
    
    return x;
}



class RedNumber
{
private:
  int max_key;
  int key;
  unsigned int white_values[5];
  unsigned int range;
public:
  RedNumber(unsigned int range = 128)
    {
      max_key = 0x1f; // Five bits set
      this->range = range;
      key = 0;
      for (int i = 0; i < 5; i++)
 white_values[i] = rand() % (range/5);
    }
  int GetNextValue()
    {
      int last_key = key;
      unsigned int sum;

      key++;
      if (key > max_key)
          key = 0;
      int diff = last_key ^ key;
      sum = 0;
      for (int i = 0; i < 5; i++)
 {
   if (diff & (1 << i))
     white_values[i] = rand() % (range/5);
   sum += white_values[i];
 }
      return sum;
    }
};

double redNoise()
{
  RedNumber pn;
    return pn.GetNextValue();
}
