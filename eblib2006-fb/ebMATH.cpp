#include "ebMATH.h"

namespace eb
{

MATH::MATH()
  {
  oldrand = 1;
  }

double MATH::Radians(double angle)     //get back radians
  {
      return (angle * eb::Piover180);
  }

double MATH::Degrees(double angle)     //get back degrees
  {
      return (angle * eb::Piunder180);
  }

double MATH::SinD(double angle)
  {
  double temp = sin(Radians(angle));
  return temp;
  }

double MATH::CosD(double angle)
  {
  double temp = cos(Radians(angle));
  return temp;
  }
  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
  //Exponents and logs
  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
        //returns base to power of int exponent
double MATH::Power(double base, int exponent)
  {
  double answer = 1;
  int i;

  if (exponent == 0) return 1;

  for (i = 0; i < abs(exponent); i++)
    answer *= base;

  if (exponent < 0)
      answer = 1.0/answer;
  return answer;
  }
        //-------------------------------------------------
              //returns a log to base 10, using the fact that
              //log(x)=ln(x)/ln(10)
double MATH::Log10(double x)
{
if (x <0.001)    return 0;
return ((log(x)* eb::OneOverLn10));
}
          //-----------------------------------------------
                //returns 10 to power of exp, using the fact
              //that ln(10 to xpower)=x*ln(10). The value of
              //ln(10) is private data Ln10.
double MATH::Exp10(double x)
{return (exp(x*eb::Ln10));}



         //********************
         // Min and max functions. The Fabs series returns the maximum
         //size regardless of sign
double MATH::Max(double a, double b)
  { double temp = (a>b)? a:b; return temp;}

double MATH::Max( double a, double b, double c)
   {double temp = Max(a,b); temp = (temp > c)? temp:c; return temp;}

double MATH::Min (double a, double b)
  {double temp = (a<b)? a:b; return temp;}

double MATH::Min(double a, double b, double c)
  {double temp = Min(a,b); temp = (temp<c)? temp:c; return temp;}
 
double MATH::Fabsmax(double a, double b)
  { double temp = (fabs(a)>fabs(b))? a:b; return temp;}

double MATH::Fabsmax( double a, double b, double c)
   {double temp = Fabsmax(a,b); 
    temp = (fabs(temp) > fabs(c))? temp:c; return temp;}

double MATH::Fabsmin (double a, double b)
  {double temp = (fabs(a)<fabs(b))? a:b; return temp;}

double MATH::Fabsmin(double a, double b, double c)
  {double temp = Fabsmin(a,b); 
   temp = (fabs(temp)<fabs(c))? temp:c; return temp;}
 
        //****************
        //equation solving
        //*************************
        //Should return the non-zero real roots of the quadratic equation
        //reduced to t = (-B +/- sqrt(B*B - 4AC)/2A). The initial equation
        //is parametric and set up as 0 = A*t2 + B*t + C. There are 2
        //possible roots; double* answer should point to a double[2].
unsigned char MATH::Solvequadratic(double A, double B, double C, double* answer)
  {
  if (A==0)
    {
    if (B !=0)
      {answer[0] = -C/B; return 1;}
    else return 0;
    }

  unsigned char foundroot = 0;
  double t,t2;
  double sroot = B*B - 4.0*A*C;
  double answer1 = -1;
  double answer2 = -1;

        //avoid domain errors by looking for sqrt of negative number or 0
  if (sroot < 0)
    {
    if (sroot > -.0001)  sroot = 0;
    else  return 0;
    }

  if (sroot == 0)
    {
    foundroot = 1;
    answer1 = -B/(2.0 * A);
    }
  else if (sroot > 0)
    {
    sroot = sqrt(sroot);
    t = (-B - sroot)/(2.0*A);
    t2 = (-B + sroot)/(2.0*A);

                 //order answers by smallest one first
    if (t<t2)  {answer1 = t; answer2 = t2;}
    else       {answer2 = t; answer1 = t2;}
    foundroot = 2;
    }
  answer[0] = answer1;
  answer[1] = answer2;
  return foundroot;
  }

       //*****************************
       //Solve a cubic. From Practical Ray tracing in C (Lindley). The
       //coefficients are passed to the function in coef (4 coefficients).
       //The roots are returned in roots (3 possible roots). The number of
       //valid roots found is returned in unsigned char. The initial equation is in form:
       //coef[0]*x3 + coef[1]*x2 + coef[2]*x + coef[3] = 0.
unsigned char MATH::Solvecubic(double* coef, double* roots)
  {
  unsigned char numroots;
  double q,r,q3,r2,sq,d,an,theta;
  double a1a1, a0, a1, a2, a3;

          //if equation is really quadratic, solve as quadratic
  if (coef[0] == 0)  return Solvequadratic(coef[1], coef[2], coef[3], roots);

          //if not quadratic, divide everything by coef[0].
  a0 = coef[0];
  a1 = coef[1]/a0;
  a2 = coef[2]/a0;
  a3 = coef[3]/a0;

  a1a1 = a1*a1;
  q = (a1a1 - 3.0*a2)/9.0;
  r = (2.0*a1a1*a1 - 9.0*a1*a2 + 27*a3)/54.0;
  q3 = q*q*q;
  r2 = r*r;
  d = q3 - r2;
  an = a1/3.0;

  if (d>=0)        //three real roots
    {
      if (q3 <= 0) {std::cerr<<"square root of <= 0 in solvecubic"; return 0;}
     d = r/sqrt(q3);
     theta = acos(d) /3.0;
     sq = -2.0 * sqrt(q);
     roots[0] = sq*cos(theta) - an;
     roots[1] = sq*cos(theta + eb::Twopiover3) - an;
     roots[2] = sq * cos(theta + 2* eb::Twopiover3) - an;
     numroots = 3;
     }
  else          //one real, 2 imaginary roots
    {
    numroots = 1;
    sq = pow(sqrt(r2 - q3) + fabs(r), 1.0/3.0);
    if (r < 0)  roots[0] = (sq + q/sq) - an;
    else        roots[0] = -(sq + q/sq) - an;
    }
  return numroots;
  }
      //************************
      //solve parametric quartic equation in the form
      //coef[0]*t4 + coef[1]*t3 + coef[2]*t2 + coef[3]*t + coef[4] = 0.
      //An early step is to make the coefficient of t4 1 by dividing by coef[0].
      //Equation is taken from "Standard Mathematical Tables and Formulas"
      //(Beyer).
unsigned char MATH::Solvequartic(double* coef, double* answer)
  {
  double c0, c1, c2, c3, c4;
  double cubic[4], cubicroot[3];  
  cubicroot[0] = -1; cubicroot[1] = -1; cubicroot[2] = -1;
  unsigned char numroots;
  double firsterm, secondterm;
  double y, a2over4, R, D, E;
  double smallcorrection = .00001;

  if (coef[0] == 0) return Solvecubic(&coef[1], answer);
  else if (coef[4] == 0)
    {
    answer[0] = 0;
    return (unsigned char)(1 + Solvecubic(&coef[1], &answer[1]));
    }

  c0 =coef[0];
  c1 = coef[1]/c0;
  c2 = coef[2]/c0;
  c3 = coef[3]/c0;
  c4 = coef[4]/c0;

     //the quartic is now in form t4 + c1*t3 + c2*t2 + c3*t + c4 = 0. Find
     //a solution to the resolvent cubic, in the form 
     //y3 - c[2]y2 + (c[1]c[3] -4c[4])*y - (c[1]c[1]c[4] - 4c[2]c[4] +
     //c[3]c[3]).
  cubic[0] = 1.0;
  cubic[1] = -1.0 * c2;
  cubic[2] = c1*c3 - 4.0*c4;
  cubic[3] = 4.0*c2*c4 - c1*c1*c4 - c3*c3;
  numroots = Solvecubic(cubic,cubicroot);

   //if there is a solution to the cubic, take one. Then solve the quartic.
   //The specific equations are on page 12 of Beyer. I THINK you are better
   //off with the largest y value; otherwise you may return 0 when a larger
   //root might give a valid answer.
  if (numroots == 3) y = Max(cubicroot[0], cubicroot[1], cubicroot[2]);
  else if (numroots == 0) return 0;
  else y = cubicroot[0];

  a2over4 = c1*c1/4.0;
  R = a2over4 - c2 + y;

  if (R < 0)  return 0;
  if (R==0)
     {
     firsterm = 3.0 * a2over4 - 2.0*c2;
     secondterm = y*y - 4.0*c4;
     if (secondterm < 0 ) return 0;
     else if (secondterm >0)  secondterm = 2.0 * sqrt(secondterm);
     }
   else       //R > 0
     {
     firsterm = 3.0 * a2over4 - R - 2.0*c2;
     R = sqrt(R);
     secondterm = (4.0*c1*c2 - 8.0*c3 - c1*c1*c1)/(4.0*R);
     }

        //solve quartic
  numroots = 0;
  double minusc1over4 = -c1/4.0;

  D = firsterm + secondterm;
  if (D > 0) D=sqrt(D);
  else if (D<0) {
                if (D + smallcorrection >0) D = 0;
                }

  if (D >= 0)
    {
    answer[0] = minusc1over4 + (R+D)/2.0;
    answer[1] = minusc1over4 + (R-D)/2.0;
    numroots = 2;
    }

  E = firsterm - secondterm;
  if (E>0) E =sqrt(E);
  else if (E<0) {if (E + smallcorrection > 0) E = 0;}

  if (E >=0)
   {
   answer[numroots++] = minusc1over4 - (R+E)/2.0;
   answer[numroots++] = minusc1over4 - (R-E)/2.0;
   }
  return numroots;
  }
   
        //*****************************
        //Random number functions
void MATH::Initrand(float a)
  {oldrand = a;}

int MATH::Randint(int range)
  {
  float sigma = (float)423.1966;
  float temp = sigma * oldrand;
  int integerval = (int) temp;
  oldrand = temp - integerval;      //oldrand is fraction between 0 and 1

  return (int) (oldrand * range);
  }

float MATH::Rand()
  {
  float sigma = (float)423.1966;
  float temp = sigma * oldrand;
  int integerval = (int) temp;
  oldrand = temp - integerval;      //oldrand is fraction between 0 and 1
  return oldrand;
  }

        //****************************
        //return angle whose tangent is y/x. This will give the full 360
        //range, not just the 180 in atan2.
double MATH::Atan3(double x, double y)
  {
  if (fabs(x) <.000001)
    {
      if (fabs(y) <.000001) return 0.0;
    else if (y > 0.0) return (eb::Pi * 0.5);
    else return (eb::Pi*1.5);
    }
  else if (x < 0) return (atan(y/x) + eb::Pi);
  else return atan(y/x);
  }

         //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //Miscellaneous functions
         //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
         //Round a double
int MATH::Round(double x)
  {
  if (x <0) x -= 0.5;
  else if (x > 0) x +=0.5;

  return (int)x;
  }
//----------------------------------------------------------
   //return -1 if <0, 0 if x == 0, 1 if > 0
int MATH::Sign(double x)
  {
  if (x<0) return -1;
  if (x>0) return 1;
  return 0;
  }

//--------------------------------------------------
   //swap two values
void MATH::Swap(double* a, double* b)
  {
  double temp;
  temp = *a;
  *a = *b;
  *b = temp;
  }
void MATH::Swap(int* a, int* b)
  {
  int temp;
  temp = *a;
  *a = *b;
  *b = temp;
  }


}//end of namespace