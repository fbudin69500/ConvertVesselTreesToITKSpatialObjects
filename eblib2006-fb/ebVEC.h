/*
ebVEC.h
12/7/93

revised for double 3/99 and to remove ebmath

  Revised 6/02 to bring into std namespace
Elizabeth Bullitt

*/
  #ifndef ebebVECh
  #define ebebVECh

#include "ebINCLUDES.h"      //define std namespace or not
//#include "MemoryLeak.h"
namespace eb
{

  class PNT;
  
  class VEC
    {
    protected:
      double v[3];   

    public:
      VEC () {v[0] = 0; v[1]=0; v[2]= 0;}
      VEC(double a, double b, double c) {v[0]=a;v[1]=b;v[2]=c;}
      VEC(VEC& a) {v[0]=a.v[0], v[1]=a.v[1], v[2]=a.v[2];}
      VEC(double* a) {v[0] = a[0]; v[1] = a[1]; v[2] = a[2];}
      VEC(float* a) {v[0] = a[0]; v[1] = a[1]; v[2] = a[2];}
      VEC(PNT pt);
      ~VEC () {}
         VEC& operator = (const VEC & m);
         unsigned char operator == (const VEC& m)      {return Isequal(m);}
         unsigned char operator != (const VEC& m)      {return !Isequal(m);}

      void Null() {v[0]=0; v[1]=0; v[2]=0;}

      void Set(double a, double b, double c) {v[0]=a;v[1]=b;v[2]=c;}
      void Set(PNT pt);
      void Set(int a, double val) {v[a] = val;}

      void Get(double* a, double* b, double* c)
           {*a=v[0]; *b=v[1]; *c=v[2];}
      void Get(double* a, double* b)
          {*a = v[0]; *b = v[1];}
      double Get(int a) {return v[a];}
      VEC GetVec()
         {
            VEC temp(v[0],v[1],v[2]) ;
            return temp ;
//         return VEC(v[0],v[1],v[2]);
         }
         double* Get() {return &v[0];}


                     //included for backwards compatability only.
                     //These use option base 1 and are dangerous.
      void Axis(int a, double val)  {v[a - 1] = val;}
      double Axis(int a) {return v[a - 1];}

      void Show(); 

      VEC operator + (VEC);
      VEC operator - (VEC);
      VEC operator * (double);

                //various multiplication choices
      VEC Vcross (VEC);
      void Vcross(VEC,VEC);
      double Vdot (VEC);
      VEC Vmult(VEC);
      void Vscalmult(double, VEC);
      void Velemmult(double, VEC,VEC);

           //miscellaneous
      double Vlen();
      int Vnormalize();
      void Vnegate();
      void Vmin(VEC, VEC);
      void Vmax(VEC, VEC);
      unsigned char Isequal(const VEC&);
  };

   class PNT
   {
   protected:
    double p[4];
  
   public:
              //multiple constructors
     PNT() {p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 1;}
     PNT(double a, double b, double c) 
           {p[0] = a; p[1] = b; p[2] = c; p[3] = 1;}
     PNT(double a, double b, double c, double d)
           {p[0] = a; p[1] = b; p[2] = c; p[3] = d;}
     PNT (VEC v)
           {double x,y,z; v.Get(&x,&y,&z); p[0]=x; p[1]=y; p[2]=z; p[3]=1;}
     PNT(PNT& a) {p[0]=a.p[0], p[1]=a.p[1], p[2]=a.p[2], p[3]=a.p[3];}
       PNT(double* a) {p[0] = a[0]; p[1] = a[1]; p[2] = a[2]; p[3] = a[3];}
       PNT(float* a) {p[0] = a[0]; p[1] = a[1]; p[2] = a[2]; p[3] = a[3];}
       PNT& operator = (const PNT & m);
       unsigned char operator == (const PNT& m) {return Isequal(m);}
       unsigned char operator != (const PNT&m)   {return !Isequal(m);}

           //operator overloaded functions
     PNT operator + (VEC);       //point + VECtor = point
     VEC operator - (PNT);       //point - point = VECtor
     PNT operator - (VEC);       //point - VECtor = point                  

             //sets and gets
     void Set(double a, double b, double c, double d)
         {p[0] = a; p[1] = b; p[2] = c; p[3] = d;}
     void Set(double a, double b, double c)
         {p[0] = a; p[1] = b; p[2] = c; p[3] = 1;}
     void Set(VEC);
     void Set(int axis, double val)
         {p[axis] = val;}

     void Get(double* a, double* b, double* c, double* d)
         {*a = p[0]; *b = p[1]; *c = p[2]; *d = p[3];}
     void Get(double* a, double* b, double*c)
         {*a = p[0]; *b = p[1]; *c = p[2];}
     void Get(double* a, double* b)
          {*a = p[0]; *b = p[1];}

       double Get(int a) {return p[a];}
       double* Get() {return &p[0];}

                  //included for backward compatability only. Option base 1
     double Axis(int a)
         {return p[a - 1];}       

             // checks for equality. Does not include w. The 2 extension
             //checks only in x and y planes.
     unsigned char Isequal(const PNT& m);
     unsigned char Isequal2(const PNT& m);
     unsigned char Isequalint2(const PNT& m);
          
            //Zero a point; find a point with min and max values of 2 points.
            //Used in clipping routines
     void Null() {p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 1;}
     void Min(PNT, PNT);
     void Max(PNT, PNT);

           //miscellaneous
     void Translate(double a, double b, double c) {p[0]+=a; p[1]+=b; p[2]+=c;}
     void Homogenize();
     double Pointdot(PNT); 
     void Show();
   };

}//end of namespace

#endif
