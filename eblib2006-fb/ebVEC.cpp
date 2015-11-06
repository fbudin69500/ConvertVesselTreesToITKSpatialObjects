#include "ebVEC.h"
#include <math.h>


namespace eb
{

      //define a "Round" function
int Round(double a)
   {
   if (a >=0) return ( (int)(a + 0.5));
   else return ( (int)(a - 0.5));
   }


//----------------------------------------------------   
    //extra constructor that PNT definition order wouldn't permit in header
VEC::VEC(PNT pt)
  {double a,b,c; pt.Get(&a,&b,&c); v[0]=a;v[1]=b; v[2]=c;}

    //extra set included here for same reason
void VEC::Set(PNT pt)
    {double a,b,c; pt.Get(&a,&b,&c); v[0]=a; v[1]=b; v[2]=c;}

       //overloaded operators
VEC VEC::operator + (VEC a)
  {return VEC (v[0] + a.v[0], v[1] + a.v[1], v[2] + a.v[2]);}

VEC VEC::operator - (VEC a)
  {return VEC (v[0] - a.v[0], v[1] - a.v[1], v[2] - a.v[2]);}

VEC VEC::operator * (double a)
  {return VEC(a * v[0], a * v[1], a * v[2]);}

VEC& VEC::operator = (const VEC & m)
   {v[0] = m.v[0]; v[1] = m.v[1]; v[2] = m.v[2]; return *this;}

     //****************************
     //various multiplications
double VEC::Vdot(VEC a)
  { return v[0] * a.v[0]  +  v[1] * a.v[1]  + v[2] * a.v[2];}

VEC VEC::Vmult (VEC a)
  {
  double x = v[0]* a.v[0];
  double y = v[1]* a.v[1];
  double z = v[2]* a.v[2];
  return VEC (x,y,z);
  }

VEC VEC::Vcross(VEC a)
  {
  double aa = v[1]*a.v[2] - v[2]*a.v[1];
  double b = v[2]*a.v[0] - v[0]*a.v[2];
  double c = v[0]*a.v[1] - v[1]*a.v[0];
  return VEC(aa,b,c);
  }

void VEC::Vcross(VEC a, VEC b)
  {
   v[0] = a.v[1]*b.v[2] - a.v[2]*b.v[1];
   v[1] = a.v[2]*b.v[0] - a.v[0]*b.v[2];
   v[2] = a.v[0]*b.v[1] - a.v[1]*b.v[0];
  }

void VEC::Vscalmult(double scale, VEC VEC)
  {
  v[0] = scale * VEC.v[0];
  v[1] = scale * VEC.v[1];
  v[2] = scale * VEC.v[2];
  }

void VEC::Velemmult(double scale, VEC v1, VEC v2)
  {
  v[0] = scale * v1.v[0] * v2.v[0];
  v[1] = scale * v1.v[1] * v2.v[1];
  v[2] = scale * v1.v[2] * v2.v[2];
  }
       //*******************
       //miscellaneous
double VEC::Vlen()
  { 
  double a = (v[0]*v[0] + v[1]*v[1] + v[2]*v[2]); 
  if (a > 0)
    a= (double)sqrt(a);
  else a = 0;
  return a;
  }

int VEC::Vnormalize()
  {
  double a = Vlen();

  if (a)
    {
    v[0] = v[0] / a;
    v[1] = v[1] /a;
    v[2] = v[2] /a;
    return 1;
    }
  else
     return 0;
  }

 void VEC::Vnegate()
   {
   v[0] = -v[0];
   v[1] = -v[1];
   v[2] = -v[2];
   }

void VEC::Vmin(VEC a, VEC b)
  {
  v[0] = (a.v[0] > b.v[0])? b.v[0]:a.v[0];
  v[1] = (a.v[1] > b.v[1])? b.v[1]:a.v[1];
  v[2] = (a.v[2] > b.v[2])? b.v[2]:a.v[2];
  }

void VEC::Vmax(VEC a, VEC b)
  {
  v[0] = (a.v[0] > b.v[0])? a.v[0]:b.v[0];
  v[1] = (a.v[1] > b.v[1])? a.v[1]:b.v[1];
  v[2] = (a.v[2] > b.v[2])? a.v[2]:b.v[2];
  }
unsigned char VEC::Isequal(const VEC& a)
  {
  unsigned char flag = 0;
 
  if (fabs(v[0]-a.v[0]) < .0001)
    {
    if (fabs(v[1] - a.v[1]) < .0001)
      {
      if (fabs(v[2]- a.v[2]) < .0001)
        flag = 1;
      }
    }
  return flag;
  }

void VEC::Show()
  {
  std::cerr<<v[0]<<", "<<v[1]<<", "<<v[2]<<std::endl;
  }
   //-------------------------

     //############################################
     //      PNT functions
     //############################################
PNT& PNT::operator = (const PNT & m)
   {p[0] = m.p[0]; p[1] = m.p[1]; p[2] = m.p[2]; p[3] = m.p[3];return *this;}
//------------------------------------------
     //overloaded operators
     // point + VECtor = point
PNT PNT:: operator + (VEC v)
  {
  double a,b,c;
  v.Get(&a,&b,&c);
  return PNT(a+p[0], b+ p[1], c+p[2], p[3]);
  }

     //point - point = VECtor
VEC PNT:: operator - (PNT point)
  {
  return VEC(p[0] - point.p[0], p[1] - point.p[1], p[2] - point.p[2]);
  }

     //point - VECtor = point
PNT PNT::operator - (VEC v)
  {
  double a,b,c;
  v.Get(&a,&b,&c);
  return PNT(p[0]-a, p[1]-b, p[2]-c, p[3]);
  }
       //****************************************
       //sets and gets not included in header
void PNT::Set(VEC v)
  {
  double a,b,c;
  v.Get(&a,&b,&c);
  p[0] = a; p[1] = b; p[2] = c;
  }

      //*****************************************
      //isequal functions
unsigned char PNT::Isequal(const PNT& a)
  {
  unsigned char flag = 0;
 
  if (fabs(p[0]-a.p[0]) < .000001)
    {
    if (fabs(p[1] - a.p[1]) < .000001)
      {
      if (fabs(p[2]- a.p[2]) < .000001) 
        flag = 1;
      }
    }
  return flag;
  }

unsigned char PNT::Isequal2(const PNT& a)
  {
  unsigned char flag = 0;
  if (fabs(p[0]-a.p[0]) < .000001)
    {
    if (fabs(p[1] - a.p[1]) < .000001)
      flag = 1;
    }
  return flag;
  }

unsigned char PNT::Isequalint2(const PNT& a)
  {
  unsigned char flag = 0;
  int x, ax, y, ay;

  x = Round(p[0]);
  ax = Round(a.p[0]);
  
  if (x == ax)
     {
     y =  Round(p[1]);
     ay = Round(a.p[1]);
     if (y == ay) flag = 1;
     }
  return flag;
  }
     //*****************************************
     // return a point with min and max x,y,z vals
void PNT::Min(PNT a, PNT b)
  {
  p[0] = (a.p[0]>b.p[0])? b.p[0]:a.p[0];
  p[1] = (a.p[1]>b.p[1])? b.p[1]:a.p[1];
  p[2] = (a.p[2]>b.p[2])? b.p[2]:a.p[2];
  }

void PNT::Max(PNT a, PNT b)
  {
  p[0] = (a.p[0]>b.p[0])? a.p[0]:b.p[0];
  p[1] = (a.p[1]>b.p[1])? a.p[1]:b.p[1];
  p[2] = (a.p[2]>b.p[2])? a.p[2]:b.p[2];
  }

      //***********************************************
      //Miscellaneous. I forget why I needed the weird Pointdot function
      //but it's included in case its needed again.

void PNT::Homogenize()
  {
  if (p[3] == 0)
    {
    p[3] = 1;
    }
  if (p[3] == 1) return;
  p[0] = p[0]/p[3]; p[1] = p[1]/p[3]; p[2] = p[2]/p[3]; p[3] = 1;
  }

double PNT::Pointdot(PNT pt)
  {
  return (p[0] * pt.p[0] + p[1] * pt.p[1] + p[2] * pt.p[2] +p[3] * pt.p[3]);
  }

     
void PNT::Show()
  {
  std::cerr<<p[0]<<", "<<p[1]<<", "<<p[2]<<", "<<p[3]<<std::endl;
  }


}//end of namespace