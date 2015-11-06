 /*
 Identical to VEC but used for floats.

  Modified 6/02 to go into std namespace

  Elizabeth Bullitt
*/

#ifndef ebFVEC_h
  #define ebFVEC_h

 #include "ebINCLUDES.h"      //define std namespace or not
 //#include "MemoryLeak.h"
namespace eb
{

  class FVEC
    {
    protected:
      float v[3];   

    public:
      FVEC () {v[0] = 0; v[1]=0; v[2]= 0;}
      FVEC(float a, float b, float c) {v[0]=a;v[1]=b;v[2]=c;}
      FVEC(FVEC& a) {v[0]=a.v[0], v[1]=a.v[1], v[2]=a.v[2];}
     FVEC(float* a) {v[0] = a[0]; v[1] = a[1]; v[2] = a[2];}
     FVEC(double* a) {v[0] = (float)a[0]; v[1] = (float)a[1]; v[2] = (float)a[2];}

     FVEC& operator = (const FVEC & m);
     unsigned char operator == (const FVEC& m)      {return Isequal(m);}
     unsigned char operator != (const FVEC& m)      {return !Isequal(m);}

      void Null() {v[0]=0; v[1]=0; v[2]=0;}

      void Set(float a, float b, float c) {v[0]=a;v[1]=b;v[2]=c;}
      void Set(int a, float val) {v[a] = val;}
      void Set(float* a)         {v[0] = a[0]; v[1] = a[1]; v[2] = a[2];}

      void Get(float* a, float* b, float* c)
           {*a=v[0]; *b=v[1]; *c=v[2];}
      void Get(float* a, float* b)
          {*a = v[0]; *b = v[1];}
      float Get(int a) {return v[a];}
      FVEC GetVec() { FVEC temp(v[0],v[1],v[2]); return temp;}
     float* Get() {return &v[0];}

      void Add(float a, float b, float c)
         {v[0] += a; v[1] += b; v[2] += c;}
      void Add(float* a)
         {v[0] += a[0]; v[1] += a[1]; v[2] += a[2];}




                     //included for backwards compatability only.
                     //These use option base 1 and are dangerous.
      void Axis(int a, float val)  {v[a - 1] = val;}
      float Axis(int a) {return v[a - 1];}

      void Show(); 

      FVEC operator + (FVEC);
      FVEC operator - (FVEC);
      FVEC operator * (float);

                //various multiplication choices
      FVEC Vcross (FVEC);
      void Vcross(FVEC,FVEC);
      float Vdot (FVEC);
      FVEC Vmult(FVEC);
      void Vscalmult(float, FVEC);
      void Velemmult(float, FVEC,FVEC);

           //miscellaneous
      float Vlen();
      int Vnormalize();
      void Vnegate();
      void Vmin(FVEC, FVEC);
      void Vmax(FVEC, FVEC);
      unsigned char Isequal(const FVEC&);
  };

}//end of namespace

#endif
