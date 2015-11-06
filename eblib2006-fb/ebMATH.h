#ifndef ebMATHH
#define ebMATHH

/*
ebMATH.h
10/20/93; revised for windows 8/97
purpose: personalized math class; radians to degrees, random number functions,
         useful constants, some polynomial solutions through quartics


4/99: removed ebCONSOLE refs

  6/02: put into std namespace
Elizabeth Bullitt

*/

#include <math.h>
#include "ebINCLUDES.h"      //define std namespace or not
#include "ebDEFINE.h"
//#include "MemoryLeak.h"
namespace eb
{





 /*__declspec(dllexport)*/ class MATH
  {
  protected:
   float oldrand;
  
  public:
    MATH();

    double Getpi()  {return eb::Pi;}
    double Getpiovertwo()  {return eb::Piovertwo;}
    double Getwopi()  {return eb::Twopi;}
    double Getoneoverpi()  {return eb::Oneoverpi;}
    double Getoneovertwopi() {return eb::Oneovertwopi;}
    double Getwopiover3()  {return eb::Twopiover3;}

                   //radians and degrees conversion
    double Radians(double);         //returns value in radians
    double Degrees(double);         //returns value in degrees
    double CosD(double);            //returns cos for angle in degrees
    double SinD(double);            //returns sin for angle in degrees

             //logs and exponents. The same functions are available
                //in the c library of some compilers. Here the functions
                //return 0 for errors other than overflow errors.
   double Power(double, int);     //return double to int power
   double Log10(double);          //return log to base 10
   double Exp10(double);        //return 10 to power of exp

                 //min and max; the fabs series returns the number whose
                 //absolute value is greatest or smallest
    double Max(double, double);
    double Max(double, double, double);
    double Min(double, double);
    double Min(double, double, double);
    double Fabsmax(double, double);
    double Fabsmax(double, double, double);
    double Fabsmin(double, double);
    double Fabsmin(double, double, double);
    
        //solve equations. The first param or group of params is coefficients;
        //the final pointer is to the returned roots. The number of valid
        //roots is returned in unsigned char.
    unsigned char Solvequadratic(double,double,double, double*);
    unsigned char Solvecubic(double*, double*);
    unsigned char Solvequartic(double*, double*); 

         //pseudorandom numbers
    void Initrand(float);
    int Randint(int);         //returns an integer within passed param range
    float Rand();

         //return "angle" whose tangent is y/x. Works for full 360 degrees
         //unlike atan2.
    double Atan3(double, double);
    
         //miscellaneous functions
    int Round(double);                //round a double
    int Sign(double);               //return 1 if >0, -1 if <0
    void Swap(double*, double*);
    void Swap(int*, int*);
  };

}//end of namespace

#endif

