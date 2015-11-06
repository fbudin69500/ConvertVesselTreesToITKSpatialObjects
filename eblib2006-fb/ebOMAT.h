/*
4x4 matrix class for use with geometrical transformations for imaging. Matrices
can be passed to and from opengl. 

 
  Multiplication is done from right to left.

  Vectors do not translate (TransformVec); points do translate (TransformPt).

  The default is to homegenize for points. TransformPtNoH will not. 

  Calls to Get() and Set() will return/give pointers to matrix contents that can
  be used interchangeably with opengl matrices.

   modified 6/24/01 to include vector to vector rotations
   Modified 6/02 to go into std namespace
   Elizabeth Bullitt


*/

#ifndef _ebOMAT_h_
#define _ebOMAT_h_

#include "ebINCLUDES.h"      //define std namespace or not
#include "ebDEFINE.h"
//#include "MemoryLeak.h"
namespace eb
{

class OMAT
   {
   protected:
      double p[16];

      void Zero();

   public:
      OMAT();
      OMAT(const OMAT & m);
      OMAT& operator = (const OMAT & m);
      OMAT operator * (const OMAT & m) ;

                             //utilities, mostly taken from other eblib modules but included
                             //here so you don't have extra overhead
      double GetPi()                                     {return Pi;}
      double RadiansToDegrees(double a);
      double DegreesToRadians(double a);
      double Atan3(double x, double y);                  //360 degree atan with no divide/0 probs
      double Dot(const double* a, const double* v);                  //dot product of two 3D vectors
      double Dot(const double* a, const double* v, int dims);        //dot product of two n-d vectors
      void Cross(const double* v, const double* a, double* answer); //cross product two 3D vectors
      double Len(const double* v);                            //length of a 3D vector
      double Len(const double* v, int dims);                   //length of an n-d vector
      void Normalize(double* v);                         //normalize a 3D vector
      void Normalize(double* v, int dims);               //normalize an n-d vector
      int Round (float a)     {if (a < 0) return (int)(a - 0.5); return (int)(a + 0.5);}
      int Round(double a)  {if (a < 0) return (int)(a - 0.5); return (int)(a + 0.5);}   


                  //Get or set all points in a matrix. Order is same as opengls (element [1] is
                  //second in first column rather than second in first row)
      void Get(float* f);            
      void Get(double* f);      
      double* Get()   {return p;}   

      void Set(const float* f);            
      void Set(const double* d);
      
               //no error checks. Use at your own risk.
               //passed param i should be 0-15.
      void SetElement(int i, double val)      {p[i] = val;}
      double GetElement(int i)                        {return p[i];}

      void Identity();

      void TransformPt(int dims, float* pts); //works for 3 or 4d pts; homogenizes
      void TransformPt(int dims, double* pts);
        void TransformPt(int dims, int* pts);
      void TransformVec(int dims, float* pts);   //works as for 3D vector; no translations
      void TransformVec(int dims, double* pts);
        void TransformVec(int dims, int* pts);

      void TransformPtNoH(int dims, float* pts);      //will translate but not homog
      void TransformPtNoH(int dims, double* pts);
        void TransformPtNoH(int dims, int* pts);


               //The above Transform operations are optimized for graphics.
               //The function below will actually multiply the final term,
               //which you need if you are using for equation solving
        void Transform(int dims, float* pts);
        void Transform(int dims, double* pts);


                     //the following functions will identity() the
                     //matrix and turn it into a single matrix of the
                     //following types
      void Translate(double, double, double);
      void RotX(double a);      //take input angle in degrees
      void RotY(double a);      //take input angle in degrees
      void RotZ(double a);      //take input angle in degrees
      void RotXRadians(double a);      //take input angle in radians
      void RotYRadians(double a);      //take input angle in radians
      void RotZRadians(double a);      //take input angle in radians
      void Scale(double, double, double);
      void Coordinate_Rotation(const float*, const float*, const float*);
      void Coordinate_Rotation(const double*, const double*, const double*);
      void Shear(int axis, double a, double b);

                           //modify an existing matrix
      void Transpose();
      int Invert(double* det);
      void Show();
      int Load(const char* fname);
      int Save(const char* fname, const char* comment = 0);

            //The following functions will multiply the current matrix
            //by the passed params, making it easier to parallel opengl
            //functions. The value of the current matrix will change with
            //each call so you need to be careful about order. Like opengl,
            //the last line written will be the first matrix processed
            //during point transformations
      void MTranslate(double, double, double);
      void MRotX(double a);      //take input angle in degrees
      void MRotY(double a);      //take input angle in degrees
      void MRotZ(double a);      //take input angle in degrees
      void MRotXRadians(double a);      //take input angle in radians
      void MRotYRadians(double a);      //take input angle in radians
      void MRotZRadians(double a);      //take input angle in radians
      void MScale(double, double, double);

               //create special matrices for projection transformations
               //total transformation is toscreen * viewmode * (object position)
               //Viewmode will convert to unit cube and homogenize
      void Viewmode (double winwidth, double winheight, double angle);
      void ViewmodeDist(double winwidth, double winheight, double dist);
      void Toscreen(double winwidth, double winheight);


          //produce a rotation matrix that rotates first vec into second or that rotates
          //an angle theta around an arbitrary vector
        double VectorToVectorRotation(const float* pt1, const float* pt2);
        double VectorToVectorRotation(const double* pt1, const double* pt2);
      void Genrot(double theta, const float* dir);
        void Genrot(double theta, const double* dir)     {GenrotRadians(DegreesToRadians(theta), dir);}
        void GenrotRadians(double theta, const double* dir);
      };

}//end of namespace

#endif
