#include <BasicLinearAlgebra.h>
using namespace BLA;

float norm2_4(Matrix<4, 1> arr){
    float norm = sq(arr(0)) + sq(arr(1))  + sq(arr(2)) + sq(arr(3)) ;
    return sqrt(norm); 
}

bool isQuaternion(Matrix<4, 1> arr){
    float norm = norm2_4(arr); 
    return norm > 0.95 && norm < 1.05 ; 
}