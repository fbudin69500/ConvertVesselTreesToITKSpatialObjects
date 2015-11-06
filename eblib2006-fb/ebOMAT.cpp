#include "ebOMAT.h"

#include <math.h>
#include "ebTBOX.h"

namespace eb
{

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //CONSTRUCTOR, ASSIGNMENT, ETC
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
OMAT::OMAT()
   {
   Identity();
   } 
//------------------------------------------
OMAT::OMAT(const OMAT & m)
   {
   for (int i = 0; i < 16; i++)
      p[i] = m.p[i];
   }
//-----------------------------------------
OMAT& OMAT::operator = (const OMAT& m)
   {
   for (int i = 0; i < 16; i++)
      p[i] = m.p[i];
   return *this;
   }
//-------------------------------
OMAT OMAT::operator*(const OMAT & m)  
   {
   OMAT temp; 
   temp.Zero();
   int e=0;
   // i specify a column
   // j specify a row
   for(int i=0; i<4; i++) {
      for(int j=0; j<4; j++) {
         double s = 0.0;
         for(int k=0; k<4; k++ ) {
            s += p[j+(k<<2)] * m.p[(i<<2)+k];
            }
         temp.p[e++] = (float)s;
         }
      }

   return temp;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //UTILITIES
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
double OMAT::RadiansToDegrees(double a) {return a/eb::Piover180;}
double OMAT::DegreesToRadians(double a) {return a * eb::Piover180;}
      //-------------------------------------


double OMAT::Atan3(double x, double y)
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

//----------------------------------------------
double OMAT::Dot(const double* a, const double* v)
   {return v[0] * a[0]  +  v[1] * a[1]  + v[2] * a[2];}

double OMAT::Dot(const double* a, const double* v, int dims)
   {
   double dot = 0;
   for (int i = 0; i < dims; i++)
      dot += a[i] * v[i];
   return dot;
   }
//------------------------------------------------------
void OMAT::Cross(const double* v, const double* a, double* answer)
   {
   answer[0] = v[1]*a[2] - v[2]* a[1];
   answer[1] = v[2]*a[0] - v[0]* a[2];
   answer[2] = v[0]*a[1] - v[1]* a[0];
   }

//----------------------------------------------------
double OMAT::Len(const double* v)
   {return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);}

double OMAT::Len(const double* v, int dims)
   {
   double answer = 0;
   for (int i = 0; i < dims; i++)
      answer += v[i] * v[i];
   return sqrt(answer);
   }
//----------------------------------------------------
void OMAT::Normalize(double* v)
   {
   double len = Len(v);
   if(len)
      {
      v[0] = v[0]/len;
      v[1] = v[1]/len;
      v[2] = v[2]/len;
      }
   }

void OMAT::Normalize(double* v, int dims)
   {
   double len = Len(v, dims);
   int i;

   if (len)
      {
      for (i = 0; i < dims; i++)
         v[i] = v[i]/len;
      }
   }
 

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
   //MATRIX FUNCTIONS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//----------------------------------
void OMAT::Zero()
   {
   for (int i = 0; i < 16; i++)
      p[i] = 0;
   }

//---------------------------------------
void OMAT::Identity()
   {
    Zero();
  for (int i = 0; i<4; i++)
     p[5*i] = 1;
   }

//---------------------------------------------
      //sets with load in proper order
void OMAT::Set(const float* f)
   {
   for (int i = 0; i < 16; i++)
      p[i] = f[i];
   }
void OMAT::Set(const double * f)
   {
   for (int i = 0; i < 16; i++)
      p[i] = (float)f[i];
   }
//-------------------------------------------
void OMAT::Get(float* f)
   {
   for (int i = 0; i < 16; i++)
      f[i] = (float)p[i];
   }
//-------------------------------------------
void OMAT::Get(double* f)
   {
   for (int i = 0; i < 16; i++)
      f[i] = p[i];
   }

//------------------------------------------------
      //will homogenize. Will also treat a 3D point as if it were
      //4D. This function is taken from Luis Ibanez
void OMAT::TransformPt(int dims, float* fptr)
   {
   if (dims >= 3 && (fptr))
      {
      double a = p[ 0]* fptr[0] + p[ 4]* fptr[1] +p[ 8]* fptr[2] +p[12];
      double b = p[ 1]* fptr[0] + p[ 5]* fptr[1] +p[ 9]* fptr[2] +p[13];
      double c = p[ 2]* fptr[0] + p[ 6]* fptr[1] +p[10]* fptr[2] +p[14];
      double d = p[ 3]* fptr[0] + p[ 7]* fptr[1] +p[11]* fptr[2] +p[15];
      if (d)
         {
         a /= d;
         b /= d;
         c /= d;
         }
      fptr[0] = (float)a; fptr[1] = (float)b; fptr[2] = (float)c;
      }
   }
//------------------------------------------------
      //will homogenize. Will also treat a 3D point as if it were
      //4D. This function is based on one by Luis Ibanez
void OMAT::TransformPt(int dims, double* fptr)
   {
   if (dims >= 3 && (fptr))
      {
      double a = p[ 0]* fptr[0] + p[ 4]* fptr[1] +p[ 8]* fptr[2] +p[12];
      double b = p[ 1]* fptr[0] + p[ 5]* fptr[1] +p[ 9]* fptr[2] +p[13];
      double c = p[ 2]* fptr[0] + p[ 6]* fptr[1] +p[10]* fptr[2] +p[14];
      double d = p[ 3]* fptr[0] + p[ 7]* fptr[1] +p[11]* fptr[2] +p[15];
      if (d)
         {
         a /= d;
         b /= d;
         c /= d;
         }
      fptr[0] = a; fptr[1] = b; fptr[2] = c;
      }
   }
//-------------------------------------------------------
void OMAT::TransformPt(int dims, int* iptr)
   {
    if (dims >= 3 && (iptr))
      {
      double a = p[ 0]* iptr[0] + p[ 4]* iptr[1] +p[ 8]* iptr[2] +p[12];
      double b = p[ 1]* iptr[0] + p[ 5]* iptr[1] +p[ 9]* iptr[2] +p[13];
      double c = p[ 2]* iptr[0] + p[ 6]* iptr[1] +p[10]* iptr[2] +p[14];
      double d = p[ 3]* iptr[0] + p[ 7]* iptr[1] +p[11]* iptr[2] +p[15];
      if (d)
         {
         a /= d;
         b /= d;
         c /= d;
         }
      iptr[0] = Round(a); iptr[1] = Round(b); iptr[2] = Round(c);
      }
   }  

//---------------------------------------------------
   //As above but no homogenization
void OMAT::TransformPtNoH(int dims, float* fptr)
   {
   if (dims >= 3 && (fptr))
      {
      double a = p[ 0]* fptr[0] + p[ 4]* fptr[1] +p[ 8]* fptr[2] +p[12];
      double b = p[ 1]* fptr[0] + p[ 5]* fptr[1] +p[ 9]* fptr[2] +p[13];
      double c = p[ 2]* fptr[0] + p[ 6]* fptr[1] +p[10]* fptr[2] +p[14];

      fptr[0] = (float)a; fptr[1] = (float)b; fptr[2] = (float)c;
      }
   }
//------------------------------------------------
      //will not homogenize. Will also treat a 3D point as if it were
      //4D. This function is based on one by Luis Ibanez
void OMAT::TransformPtNoH(int dims, double* fptr)
   {
   if (dims >= 3 && (fptr))
      {
      double a = p[ 0]* fptr[0] + p[ 4]* fptr[1] +p[ 8]* fptr[2] +p[12];
      double b = p[ 1]* fptr[0] + p[ 5]* fptr[1] +p[ 9]* fptr[2] +p[13];
      double c = p[ 2]* fptr[0] + p[ 6]* fptr[1] +p[10]* fptr[2] +p[14];

      fptr[0] = a; fptr[1] = b; fptr[2] = c;
      }
   }

//-----------------------------------------------
      //will not homogenize. Will also treat a 3D point as if it were
      //4D. This function is based on one by Luis Ibanez
void OMAT::TransformPtNoH(int dims, int* iptr)
   {
   if (dims >= 3 && (iptr))
      {
      double a = p[ 0]* iptr[0] + p[ 4]* iptr[1] +p[ 8]* iptr[2] +p[12];
      double b = p[ 1]* iptr[0] + p[ 5]* iptr[1] +p[ 9]* iptr[2] +p[13];
      double c = p[ 2]* iptr[0] + p[ 6]* iptr[1] +p[10]* iptr[2] +p[14];

      iptr[0] = Round(a); iptr[1] = Round(b); iptr[2] = Round(c);
      }
   }

//-----------------------------------------------
         //will look at only the first 3 dims and treat as a vector
         //(no translation and no homogenization)
void OMAT::TransformVec(int dims, float* fptr)
   {
   if (dims>= 3 && (fptr))
      {
      double a = (double)p[ 0]* fptr[0] + p[ 4]* fptr[1] +p[ 8]* fptr[2];
      double b = (double)p[ 1]* fptr[0] + p[ 5]* fptr[1] +p[ 9]* fptr[2];
      double c = (double)p[ 2]* fptr[0] + p[ 6]* fptr[1] +p[10]* fptr[2];
      fptr[0] = (float)a; fptr[1] = (float)b; fptr[2] = (float)c;
      }
   }
//-----------------------------------------------
         //will look at only the first 3 dims and treat as a vector
         //(no translation and no homogenization)
void OMAT::TransformVec(int dims, double* fptr)
   {
   if (dims>= 3 && (fptr))
      {
      double a = (double)p[ 0]* fptr[0] + p[ 4]* fptr[1] +p[ 8]* fptr[2];
      double b = (double)p[ 1]* fptr[0] + p[ 5]* fptr[1] +p[ 9]* fptr[2];
      double c = (double)p[ 2]* fptr[0] + p[ 6]* fptr[1] +p[10]* fptr[2];
      fptr[0] = a; fptr[1] = b; fptr[2] = c;
      }
   }
//-----------------------------------------------
         //will look at only the first 3 dims and treat as a vector
         //(no translation and no homogenization)
void OMAT::TransformVec(int dims, int* iptr)
   {
   if (dims>= 3 && (iptr))
      {
      double a = (double)p[ 0]* iptr[0] + p[ 4]* iptr[1] +p[ 8]* iptr[2];
      double b = (double)p[ 1]* iptr[0] + p[ 5]* iptr[1] +p[ 9]* iptr[2];
      double c = (double)p[ 2]* iptr[0] + p[ 6]* iptr[1] +p[10]* iptr[2];
      iptr[0] = Round(a); iptr[1] = Round(b); iptr[2] = Round(c);
      }
   }
//-------------------------------------------------------------
         //The above Transform() functions are optimized for graphics
         //and avoid an extra multiplication by simply adding the translation term.
         //However, these matrices cannot be used for solving equations with
         //4 unknowns. The following version will multiply the final term.
void OMAT::Transform(int dims, float* fptr)
   {
   if (dims >= 4)
      {
      double a = p[ 0]* fptr[0] + p[ 4]* fptr[1] +p[ 8]* fptr[2] +p[12] * fptr[3];
      double b = p[ 1]* fptr[0] + p[ 5]* fptr[1] +p[ 9]* fptr[2] +p[13] * fptr[3];
      double c = p[ 2]* fptr[0] + p[ 6]* fptr[1] +p[10]* fptr[2] +p[14] * fptr[3];
      double d = p[ 3]* fptr[0] + p[ 7]* fptr[1] +p[11]* fptr[2] +p[15] * fptr[3];

      fptr[0] = (float)a; fptr[1] = (float)b; fptr[2] = (float)c; fptr[3] = (float)d;
      }
   }
      //------------------------------------------------------
void OMAT::Transform(int dims, double* fptr)
   {
   if (dims >= 4)
      {
      double a = p[ 0]* fptr[0] + p[ 4]* fptr[1] +p[ 8]* fptr[2] +p[12] * fptr[3];
      double b = p[ 1]* fptr[0] + p[ 5]* fptr[1] +p[ 9]* fptr[2] +p[13] * fptr[3];
      double c = p[ 2]* fptr[0] + p[ 6]* fptr[1] +p[10]* fptr[2] +p[14] * fptr[3];
      double d = p[ 3]* fptr[0] + p[ 7]* fptr[1] +p[11]* fptr[2] +p[15] * fptr[3];

      fptr[0] = a; fptr[1] = b; fptr[2] = c; fptr[3] = d;
      }
   }

//--------------------------------------------
void OMAT::Translate(double tx, double ty, double tz)
   {
   p[ 0]=1.0;p[ 4]=0.0;p[ 8]=0.0;p[12]=(float)tx;
   p[ 1]=0.0;p[ 5]=1.0;p[ 9]=0.0;p[13]=(float)ty;
   p[ 2]=0.0;p[ 6]=0.0;p[10]=1.0;p[14]=(float)tz;
   p[ 3]=0.0;p[ 7]=0.0;p[11]=0.0;p[15]=1.0;
   }

//-------------------------------------------

         //takes angle in degrees
void OMAT::RotX (double a)
   {
   Identity();
   p[ 5] = cos(a * eb::Piover180);
   p[10] = p[5];
   p[ 6] = sin(a * eb::Piover180);
   p[ 9] = -p[6];
   }
//------------------------------------------
void OMAT::RotY(double a) 
   {
   Identity();
   p[ 0] = cos(a * eb::Piover180);
   p[10] = p[0];
   p[ 8] = sin(a * eb::Piover180);
   p[ 2] = -p[8];
   }
//-----------------------------------------
void OMAT::RotZ(double a)
   {
   Identity();
   p[ 0] = cos(a * eb::Piover180);
   p[ 5] = p[0];
   p[ 1] = sin(a * eb::Piover180);
   p[ 4] = -p[1];
   }
//-------------------------------------------
void OMAT::RotXRadians (double a)
   {
   Identity();
   p[ 5] = cos(a);
   p[10] = p[5];
   p[ 6] = sin(a);
   p[ 9] = -p[6];
   }
//------------------------------------------
void OMAT::RotYRadians(double a) 
   {
   Identity();
   p[ 0] = cos(a);
   p[10] = p[0];
   p[ 8] = sin(a);
   p[ 2] = -p[8];
   }
//-----------------------------------------
void OMAT::RotZRadians(double a)
   {
   Identity();
   p[ 0] = cos(a);
   p[ 5] = p[0];
   p[ 1] = sin(a);
   p[ 4] = -p[1];
   }
//-------------------------------------------
void OMAT::Scale(double x, double y, double z)
   {
   p[ 0]= x ;p[ 4]= 0.0;p[ 8]= 0.0;p[12]=0.0;
   p[ 1]= 0.0;p[ 5]= y ;p[ 9]= 0.0;p[13]=0.0;
   p[ 2]= 0.0;p[ 6]= 0.0;p[10]= z ;p[14]=0.0;
   p[ 3]= 0.0;p[ 7]= 0.0;p[11]= 0.0;p[15]=1.0;
   }

//------------------------------------------
void OMAT::Transpose()
   {
   OMAT temp;

   int i, j;
   for (i = 0; i < 4; i++)
      {
      for (j = 0; j < 4; j++)
         temp.p[i * 4 + j] = p[j * 4 + i];
      }

 *this = temp;
   }
//---------------------------------------------
void OMAT::Coordinate_Rotation(const float* xrot, const float* yrot, const float* zrot)
   {
  Identity();
  
  p[0] = xrot[0]; p[4] = xrot[1]; p[8] = xrot[2];
  p[1] = yrot[0]; p[5] = yrot[1]; p[9] = yrot[2];
  p[2] = zrot[0]; p[6] = zrot[1]; p[10] = zrot[2];
  }

void OMAT::Coordinate_Rotation(const double* xrot, const double* yrot, const double* zrot)
   {
  Identity();
  
  p[0] = xrot[0]; p[4] = xrot[1]; p[8] = xrot[2];
  p[1] = yrot[0]; p[5] = yrot[1]; p[9] = yrot[2];
  p[2] = zrot[0]; p[6] = zrot[1]; p[10] = zrot[2];
  }
//------------------------------------------------
void OMAT::Shear(int axis, double a, double b)
  {
  Identity();
  switch(axis)
    {
    case 0: p[1] = (float)a; p[2] = (float)b; break;
    case 1: p[4] = (float)a; p[6] = (float)b; break;
    case 2: p[8] = (float)a; p[9] = (float)b; break;
    }
  }
//----------------------------------------------
 
//---------------------------------------------------------
   //General matrix inversion for an n x n matrix. From GRATIS
   //lib3d. Originally from philip r bevington data reduction and
   //error analysis for the physical sciences. pp 302-303. Gotten
   //from Mitch Soltys and George Sherouse.

int OMAT::Invert(double* det)
  {
         //for the general case, rows must = columns and this number
         //should be given as "order" below. Here, order is 4.
  int order = 4;       
  double amax, saveval;
  int i, j, k, l;
  int* ik;
  int* jk;
  double* matx;

   //memory allocations--temporary  
   matx = (double*)calloc(order * order, sizeof(double));
   ik = (int*)calloc(order, sizeof(int));       
   jk = (int*)calloc(order, sizeof(int));
   if (!jk || !ik || !matx)
       {
       std::cerr<<"No mem for MAT::Invert";
       return 0;
       }

  for (i = 0; i< order * order; i++)
   matx[i] = p[i]; 

   /////////////////////////
   //begin loops

  *det = 1.0;
  for (k = 0; k<order; k++)
    {
   //find largest element matrix(i,j) in rest of matrix
    amax = 0.0;
    do
      {
      do
        {
   for (i = k; i < order; i++)
     for (j = k; j<order; j++)
            {
       if (fabs(amax) <= fabs(matx[i * order + j]))
         {
         amax = matx[i * order + j];
         ik[k] = i;
         jk[k] = j;
         }
       }          //end both for loops

   //interchange rows and columns and put amax in matx[k,k]
        if (amax == 0.0)  
         {*det = 0.0; std::cerr<<"Cannot invert matrix\n"; return (0);}
        i = ik[k];
   }
        while (i<k);             //end inner do loop

      if (i > k)  
   for (j = 0; j< order; j++)
     {
     saveval = matx[k * order + j]; 
     matx[k * order + j] = matx[i * order +j];
     matx[i * order + j] = -saveval;
     }
      j = jk[k];
      }while (j<k);                 //end outer do loop

    if (j > k)
      for (i = 0; i < order; i++)
   {
   saveval = matx[i * order + k];
   matx[i * order + k] = matx[i * order + j];
   matx[i * order + j] = -saveval;
        }

   //accumulate elements of inverse matrix
    for (i = 0; i < order; i++)
      if (i != k)   
   matx[i * order + k] = -(matx[i * order + k]/amax);

    for(i = 0; i < order; i++)
      for (j = 0; j < order; j++)
   if (i != k)
     if (j != k)
       matx[i * order + j] = (matx[i * order + j] + 
      matx[i * order + k] * matx[k * order + j]);

    for ( j=0; j < order; j++)
      if (j != k)
   matx[k * order + j] = matx[k * order + j]/amax;

    matx[k * order + k] = 1.0/amax;
    *det *= amax;
    }                      //end for k = ....

     //restore ordering of matx
  for (l = 0; l < order; l++)
    {
    k = order - l - 1;
    j = ik[k];
    if (j >k)
      for (i = 0; i < order; i++)
        {
   saveval = matx[i * order + k];
   matx[i * order + k] = -matx[i * order + j];
   matx[i * order + j] = saveval;
   }
    i = jk[k];
    if (i > k)
      for (j = 0; j < order; j++)
   {
   saveval = matx[k * order + j];
   matx[k * order + j] = -matx[i * order + j];
   matx[i * order + j] = saveval;
   }
    }

  for (i = 0; i < order * order; i++)
     p[i] = (float)matx[i];
  free (matx);
  free (jk);
  free (ik);  
  return(1); 
  }
   //-----------------------------
      
   void OMAT::Show()
      {
      int index;
      std::cerr<<std::endl;
      for (int i = 0; i < 4; i++)
         {
         index = 4 * i;
         std::cerr<<p[index]<<"  "<<p[index + 1]<<"  "<<p[index + 2]<<"  "<<p[index + 3]<<std::endl;
         }
      }
   //----------------------------------------------
         
int OMAT::Load(const char* fname)
      {
      char readline[200];
      TBOX tbox;
      double d[16];

      std::ifstream ifile;
      ifile.open(fname);
      if(!ifile) { return 0;}

      ifile.getline(readline,99);      //text comment
      if(!ifile) { return 0;}

      for (int i = 0; i < 4; i++)
         {
         ifile.getline(readline, 199);
         if(!ifile) { return 0;}

      if (!strlen(readline) && ifile) {--i; continue;}   //possible blank line
         if (!tbox.Stringtondoubles(readline, 4, &d[i * 4] ))
            {ifile.close(); return 0;}
         }
      Set(d);
      ifile.close();
      return 1;
      }
//------------------------------------------------
int OMAT::Save(const char* fname, const char* comment)
   {
   std::ofstream ofile;
   ofile.open(fname);
   if (!ofile) return 0;

   if (!comment)
      ofile<<"Unknown source\n\n";
  else
      ofile<<comment<<"\n\n";
   for (int i = 0; i < 4; i++)
      ofile<<p[i * 4]<<"  "<<p[i * 4 + 1]<<"  "<<p[i * 4 + 2]<<"  "<<p[i * 4 + 3]<<std::endl;
   ofile.close();
   return 1;
   }
         
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
   //ATTEMPT TO CREATE PARALLEL OF OPENGL FUNCTIONS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
void OMAT::MTranslate(double a, double b, double c)
   {
   OMAT copy = *this;
   OMAT temp;
   temp.Translate(a,b,c);
   *this = copy * temp;
   }
//-----------------------------------------------------------
void OMAT::MRotX(double a)
   {
   OMAT copy = *this;
   OMAT temp;
   temp.RotX(a);
   *this = copy * temp;
   }
//-------------------------------------------------------------
void OMAT::MRotY(double a)
   {
   OMAT copy = *this;
   OMAT temp;
   temp.RotY(a);
   *this = copy * temp;
   }
//-----------------------------------------------------
void OMAT::MRotZ(double a)
   {
   OMAT copy = *this;
   OMAT temp;
   temp.RotZ(a);
   *this = copy * temp;
   }
//----------------------------------------------------------
void OMAT::MRotXRadians(double a)
   {
   OMAT copy = *this;
   OMAT temp;
   temp.RotXRadians(a);
   *this = copy * temp;
   }
//------------------------------------------------------
void OMAT::MRotYRadians(double a)
   {
   OMAT copy = *this;
   OMAT temp;
   temp.RotYRadians(a);
   *this = copy * temp;
   }
//---------------------------------------------------------
void OMAT::MRotZRadians(double a)
   {
   OMAT copy = *this;
   OMAT temp;
   temp.RotZRadians(a);
   *this = copy * temp;
   }
//-------------------------------------------------------
void OMAT::MScale(double a, double b , double c)
   {
   OMAT copy = *this;
   OMAT temp;
   temp.Scale(a, b, c);
   *this = copy * temp;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //Special projection stuff
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
            //This is set up for ray source on 0,0,0 looking along - z
void OMAT::Viewmode(double winwidth, double winheight, double field_of_view)
   {
   OMAT oscale, opar, otrans;
   double vph = winheight/2.0;
   double radangle = eb::Piover180 * field_of_view/2.0000;
  double dist = vph/tan(radangle);

   oscale.Scale(2.0/winwidth, 2.0/winheight, 1/dist);
  opar.SetElement(11,-1);    //puts -z vals in w            
  opar.SetElement(15,0);
   otrans.Translate(0,0,2.0);

  *this = otrans * opar * oscale;
   }
//----------------
      //same as above but dist is given rather than angle
void OMAT::ViewmodeDist(double winwidth, double winheight, double dist)
   {
   OMAT oscale, opar, otrans;
   oscale.Scale(2.0/winwidth, 2.0/winheight, 1/dist);
  opar.SetElement(11,-1);    //puts -z vals in w            
  opar.SetElement(15,0);
   otrans.Translate(0,0,2.0);

  *this = otrans * opar * oscale;
   }
//------------------------------------------------------
void OMAT::Toscreen(double winwidth, double winheight)
   {
   OMAT orot, otrans;

   orot.Scale(winwidth/2.0, winheight/2.0, 1);
  otrans.Translate(1.0, 1.0, 0);
  *this = orot * otrans;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //SPECIAL STUFF
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //Produce a rotation matrix that gives a rotation between one 3D vector and another.
      //Idea is to 1) get a dot product between vectors, thus giving an angle, 2) get the
      //cross product between vectors, thus giving an axis that is orthogonal to the
      //two vectors, and 3) use a formula that rotates an angle theta around an aribitrary
      //axis. 
      //The rotation formula is taken from Pique M, Rotation Tools:
      //Converting between Matrix and Axis-Amount Representations, in
      //Glassner AS (ed) Graphics Gems (1990) Academic Press, Chestnut Hill MA, pp 466-467.
      //Vectors as used below are pure vectors (do not translate; are 3D not 4D objects,
      //have one end at 0,0,0 and give a direction only).

double OMAT::VectorToVectorRotation(const float* a, const float* b)
   {
   double vec1[3];
   double vec2[3];
   for (int i = 0; i < 3; i++)
      {
      vec1[i] = (double)a[i];
      vec2[i] = (double)b[i];
      }
   return VectorToVectorRotation(vec1, vec2);
   }

double OMAT::VectorToVectorRotation(const double* vec1, const double* vec2)
   {
   double  dot, theta, len, temp;
   double crossvec[3];
   double t,c,s;

              //Get the angle between the two vectors
   dot = Dot(vec1, vec2);
   len = Len(vec1) * Len(vec2);
   if (!len) return 0;

            //you can get floating point error here. acos is only valid
         //between 1 and -1
   temp = dot/len;
   if (temp > 1) temp = 1;
   else if (temp < -1) temp = -1;
   theta = acos(temp);
   
            //Get normalized axis at right angles to the two input vectors. Reversing the
            //cross order will reverse the direction of rotation around that axis, The order 
            //below is for rotation that goes from vec1 to vec2. 
   Cross(vec2, vec1, crossvec);
   Normalize(crossvec);

            //define terms you will need for rotation matrix. From here down, code is a
            //paraphrase of Pique's chapter.
   s = sin(theta);
   c = cos(theta);

   //s = theta;
   //c = 1;

   t = 1 - c;

            //fill in matrix values for the 3x3 rotation grid. 
   Identity();
         //first column
   p[0] = t * crossvec[0] * crossvec[0] + c;
   p[1] = t * crossvec[0] * crossvec[1] - (s * crossvec[2]);
   p[2] = t * crossvec[0] * crossvec[2] + (s * crossvec[1]);

            //second column
   p[4] = t * crossvec[0] * crossvec[1] + (s * crossvec[2]);
   p[5] = t * crossvec[1] * crossvec[1] + c;
   p[6] = t * crossvec[1] * crossvec[2] - (s * crossvec[0]);

            //third column
   p[8] = t * crossvec[0] * crossvec[2] - (s * crossvec[1]);
   p[9] = t * crossvec[1] * crossvec[2] + (s * crossvec[0]);
   p[10] = t * crossvec[2] * crossvec[2] + c;

   return theta;
   }
//-----------------------------------------------
      //rotation of axes by angle (in degrees or radians) theta around a line with base 
       //base, and direction dir. See comments on VectorToVectorRotation()
      //for reference
void OMAT::Genrot(double theta, const float* dir)
   {
   theta = DegreesToRadians(theta);
   double d[3] = {dir[0], dir[1], dir[2]};
   GenrotRadians(theta, d);
   }


void OMAT::GenrotRadians(double theta, const double* dir)
  {
  double s,c,t;
  
            //you need to normalize. Caller might want dir later, so don't change it. 
  double vec[3] = {dir[0], dir[1], dir[2]};
  Normalize(vec);

           //define terms you will need for rotation matrix. 
   s = sin(theta);
   c = cos(theta);
   t = 1 - c;

   Identity();

       //first column
   p[0] = t * vec[0] * vec[0] + c;
   p[1] = t * vec[0] * vec[1] - (s * vec[2]);
   p[2] = t * vec[0] * vec[2] + (s * vec[1]);

            //second column
   p[4] = t * vec[0] * vec[1] + (s * vec[2]);
   p[5] = t * vec[1] * vec[1] + c;
   p[6] = t * vec[1] * vec[2] - (s * vec[0]);

            //third column
   p[8] = t * vec[0] * vec[2] - (s * vec[1]);
   p[9] = t * vec[1] * vec[2] + (s * vec[0]);
   p[10] = t * vec[2] * vec[2] + c;
  }




   
   
}//end of namespace