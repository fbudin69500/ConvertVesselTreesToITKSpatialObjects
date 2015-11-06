#include "ebFVEC.h"
#include <math.h>

namespace eb
{

      //define a "Round" function
int Round(float a)
   {
   if (a >=0) return ( (int)(a + 0.5));
   else return ( (int)(a - 0.5));
   }


//----------------------------------------------------   
       //overloaded operators
FVEC FVEC::operator + (FVEC a)
{    FVEC temp(v[0] + a.v[0], v[1] + a.v[1], v[2] + a.v[2]);
     return temp;}

FVEC FVEC::operator - (FVEC a)
  {  FVEC temp(v[0] - a.v[0], v[1] - a.v[1], v[2] - a.v[2]);
     return temp;}

FVEC FVEC::operator * (float a)
{FVEC temp(a * v[0], a * v[1], a * v[2]);return temp;}

FVEC& FVEC::operator = (const FVEC& m)
   {v[0] = m.v[0]; v[1] = m.v[1]; v[2] = m.v[2]; return *this;}

     //****************************
     //various multiplications
float FVEC::Vdot(FVEC a)
  { return v[0] * a.v[0]  +  v[1] * a.v[1]  + v[2] * a.v[2];}

FVEC FVEC::Vmult (FVEC a)
  {
  float x = v[0]* a.v[0];
  float y = v[1]* a.v[1];
  float z = v[2]* a.v[2];
  FVEC temp(x,y,z);
  return temp;
  }

FVEC FVEC::Vcross(FVEC a)
  {
  float aa = v[1]*a.v[2] - v[2]*a.v[1];
  float b = v[2]*a.v[0] - v[0]*a.v[2];
  float c = v[0]*a.v[1] - v[1]*a.v[0];
  FVEC temp(aa,b,c);
  return temp;
  }

void FVEC::Vcross(FVEC a, FVEC b)
  {
   v[0] = a.v[1]*b.v[2] - a.v[2]*b.v[1];
   v[1] = a.v[2]*b.v[0] - a.v[0]*b.v[2];
   v[2] = a.v[0]*b.v[1] - a.v[1]*b.v[0];
  }

void FVEC::Vscalmult(float scale, FVEC FVEC)
  {
  v[0] = scale * FVEC.v[0];
  v[1] = scale * FVEC.v[1];
  v[2] = scale * FVEC.v[2];
  }

void FVEC::Velemmult(float scale, FVEC v1, FVEC v2)
  {
  v[0] = scale * v1.v[0] * v2.v[0];
  v[1] = scale * v1.v[1] * v2.v[1];
  v[2] = scale * v1.v[2] * v2.v[2];
  }
       //*******************
       //miscellaneous
float FVEC::Vlen()
  { 
  float a = (v[0]*v[0] + v[1]*v[1] + v[2]*v[2]); 
  if (a > 0)
    a= (float)sqrt(a);
  else a = 0;
  return a;
  }

int FVEC::Vnormalize()
  {
  float a = Vlen();
  if (a)
    {
    v[0] = v[0] / a;
    v[1] = v[1] /a;
    v[2] = v[2] /a;
    return 1;
    }
  return 0;
  }

 void FVEC::Vnegate()
   {
   v[0] = -v[0];
   v[1] = -v[1];
   v[2] = -v[2];
   }

void FVEC::Vmin(FVEC a, FVEC b)
  {
  v[0] = (a.v[0] > b.v[0])? b.v[0]:a.v[0];
  v[1] = (a.v[1] > b.v[1])? b.v[1]:a.v[1];
  v[2] = (a.v[2] > b.v[2])? b.v[2]:a.v[2];
  }

void FVEC::Vmax(FVEC a, FVEC b)
  {
  v[0] = (a.v[0] > b.v[0])? a.v[0]:b.v[0];
  v[1] = (a.v[1] > b.v[1])? a.v[1]:b.v[1];
  v[2] = (a.v[2] > b.v[2])? a.v[2]:b.v[2];
  }
unsigned char FVEC::Isequal(const FVEC& a)
  {
  unsigned char flag = 0;
 
  if (fabs(v[0]-a.v[0]) < .00001)
    {
    if (fabs(v[1] - a.v[1]) < .00001)
      {
      if (fabs(v[2]- a.v[2]) < .00001)
        flag = 1;
      }
    }
  return flag;
  }

void FVEC::Show()
  {
  std::cout<<v[0]<<", "<<v[1]<<", "<<v[2]<<std::endl;
  }
   //-------------------------

}//end of namespace
