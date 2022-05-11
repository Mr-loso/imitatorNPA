#include "radianControl.h"
#include "math.h"

double radianControl (double rad) {
    
    
    if (rad > M_PI*2) {
        rad -= M_PI*2;
    } else if (rad < 0) {
        rad += M_PI*2;
    }
    
    return rad;
}
