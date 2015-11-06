#include "ebGEOM.h"


#ifndef ebMAT2hpp
#define ebMAT2hpp
                              
#include <math.h>

#include "ebMATH.h"
#include "ebTBOX.h"
#include "ebVEC.h"      //it is sometimes really easier to use these classes
   
   
namespace eb
{

class MAT
  {
  protected:
    int rows;
    int cols;
    double* mat;

    void Free();                                  //release memory

  public:
    MAT(int, int);
    MAT(MAT&);
    ~MAT();
    MAT& operator =(MAT&);
    MAT& operator * (MAT&);
    MAT& operator - (MAT&);
    MAT& operator + (MAT&);
     void Zero();

                    
     void Sethorz(int, double*);             //set a row
     void Setvert(int, double*);             //set a column
     void Setelement(int,int, double);
     void Set(int a, int b, double d)     {Setelement(a,b,d);}

                           //set or get by a linear array assumed to be
                           //the proper size. If it isn't you will be sorry.
                           //the last get gets an array element
       void Set(double*);
       void Get(double*);
       double Get(int a)               {return mat[a];} //assumes valid a
                  //add subtract, multiply. The overloaded operators
                  //supersede these, but these are left in for compatibility
    void Add(MAT*, MAT*);
    void Add(MAT*);
    void Sub(MAT*, MAT*);
    void Sub(MAT*);
    void Mult(MAT*, MAT*);
    void Mult(MAT*);

              //miscellaneous
    void Transpose();
    void Transpose(MAT);
    unsigned char Invert(double*);         //returnval is if valid; double* is det

      //Transform n doubles.
   void Transform(int, double*);
    void Transform(int, float*);


             //debugging;
    void Show();
    int Cols()                      {return cols;}
      int Rows()                                 {return rows;}
                        
  };

 /*__declspec(dllexport)*/class MATX: public MAT
  {
  public:
   MATX();
   MATX(MATX&);
   void Identity();
   MATX& operator =(MATX&);
   MATX& operator * (MATX&);
   MATX& operator - (MATX&);
   MATX& operator + (MATX&);
                  //matrices to scale, translate, rotate, shear. Rotate 3d
                  //rotates around a given axis in degrees (don't chain these).
                  //Coordinate_Rotation provides a full rotation matrix.
                  //Genrot also supplies a full rotation matrix, but in the
                  //form of axis rotation through an angle phi about a line
                  //with base b and direction d.
   void Translate3d(double, double, double);
   void Scale3d(double, double, double);
   void Rotate3d(int, double);
   void Rotate3dradians(int, double);
   void Coordinate_Rotation(VEC, VEC, VEC);
   void Shear(int, double, double);
   void Genrot(double,PNT,VEC);


                 //transformation of 3D vector or 4D point
   void Transform(VEC*);
   void Transform(PNT*);

   //save or load from file. 0 on failure.
   unsigned char Save(char*, char*);
   unsigned char Load(char*);
  };     
#endif 

                 //constructors and destructor and related functions
MAT::MAT(int i, int j)
  {
  rows = i;
  cols = j;
  mat = (double*)calloc(i*j, sizeof(double));
  if (!mat) 
      std::cerr<<"Could not create matrix\n";
  }

MAT::MAT(MAT& m)
  {
  rows = m.rows;
  cols = m.cols;
  mat = (double*)calloc(rows*cols, sizeof(double));
  if (!mat) std::cerr<<"could not create matrix\n";
  for (int i = 0; i< rows; i++)
    for (int j= 0; j<cols; j++)
       mat[i*cols + j] = m.mat[i*cols + j];
  }

MAT::~MAT()
  {if (mat) free(mat);}

void MAT::Free()
  {if (mat) free(mat); mat = NULL;}

void MAT::Zero()
  {
  for (int i = 0; i< rows; i++)
    for (int j= 0; j<cols; j++)
       mat[i*cols + j] = 0;
  }

     //*************************************
     //Attempt at some operator overloading
MAT& MAT::operator = (MAT& m)
  {
  if (rows != m.rows || cols !=m.cols) Free();
  rows = m.rows;
  cols = m.cols;
  mat = (double*)calloc(rows*cols, sizeof(double));
  for (int i = 0; i< rows; i++)
    for (int j= 0; j<cols; j++)
       mat[i*cols + j] = m.mat[i*cols + j];
  return *this;
  }

         //multiplies this by the second matrix m (*this times m) 
MAT& MAT::operator * (MAT& m)
  {
  int flag = 0;

  if (cols != m.rows) flag = 1;
  
  if (!flag) {
  MAT temp(rows,cols);
  for (int i = 0; i< rows; i++)
    {
    for (int j = 0; j< cols; j++)
      {
      for (int k = 0; k<cols; k++)
         mat[i*cols +j] += temp.mat[i*cols + k] * m.mat[k*m.cols +j];
      }
    }
  }
  else std::cerr<<"Matrix multiplication error\n";
  return *this;
  }
  
                //********************************************
                // Set row, col, element
                //********************************************
void MAT::Sethorz(int rownum, double* d)
  {
  int ptroffset = rownum * cols;

  for (int i = 0; i <cols; i++)
     mat[ptroffset + i] = *(d + i);
  }


void MAT::Setvert(int colnum, double* d)
  {
  for (int i = 0; i< rows; i++)
     mat[i*cols + colnum] = *(d+i);
  }


void MAT::Setelement(int r, int c, double d)
  {
  mat[r *cols + c] = d;
  }

                //************************************
                // Add, subtract, multiply
                //*************************************
void MAT::Add(MAT* A, MAT* B)
  {
   if ( A->cols != B->cols || A->cols != cols ||
         A->rows != B->rows || A->rows != rows)
             {
         std::cerr<<"Matrices are not the same size. Cannot add.\n";
             return;
             }

  for (int i= 0; i<rows; i++)
    for (int j = 0; j< cols; j++)
       mat[i*cols + j] = A->mat[i*cols + j] + B->mat[i*cols + j];
   }
void MAT::Add(MAT* A)
  {
  if (A->cols != cols || A->rows != rows)
      {
      std::cerr<<"Matrices are not the same size. Cannot add.\n";
      return;
      }

  for (int i = 0; i< rows; i++)
    for (int j = 0; j< cols; j++)
        mat[i*cols + j] += A->mat[i*cols + j];
   }

void MAT::Sub(MAT* A, MAT* B)
  {
   if ( A->cols != B->cols || A->cols != cols ||
         A->rows != B->rows || A->rows != rows)
       {
     std::cerr<<"Matrices are not the same size. Cannot sub.\n";
       return;
       }

  for (int i= 0; i<rows; i++)
    for (int j = 0; j< cols; j++)
       mat[i*cols + j] = A->mat[i*cols + j] - B->mat[i*cols + j];
   }
void MAT::Sub(MAT* A)
  {
  if (A->cols != cols || A->rows != rows)
      {
      std::cerr<<"Matrices are not the same size. Cannot sub.\n";
      return;
      }

  for (int i = 0; i< rows; i++)
    for (int j = 0; j< cols; j++)
        mat[i*cols + j] -= A->mat[i*cols + j];
   }

void MAT::Mult(MAT* A, MAT* B)
  {
  int flag = 0;

  if (A->cols != B->rows) flag = 1;
  if (rows != A->rows || cols != B->cols) flag +=2;

  if (! flag)
    {
    for (int i = 0; i< rows; i++)
      for (int j = 0; j< cols; j++)
        {
        mat[i*cols + j] = 0;
        for (int k = 0; k<A->cols; k++)
           mat[i*cols +j] += A->mat[i* A->cols + k] * B->mat[k*B->cols +j];
        }
     }
  else std::cerr<<"Matrix multiplication error\n";
  }
              //******************************
      //Miscellaneous
      //******************************
 void MAT::Transpose()
   {
    int i;
    MAT newmat(cols, rows);        //note inversion of row/col params

    for (i = 0; i<rows; i++)
      for (int j = 0; j<cols; j++)
         newmat.mat[j*rows + i] = mat[i*cols + j];

    for (i=0; i<rows*cols; i++)
      mat[i] = newmat.mat[i];
    rows = newmat.rows;
    cols = newmat.cols;
    newmat.Free();
   }

void MAT::Transpose(MAT a)
  {
  if (cols* rows != a.cols * a.rows) 
     {
     free (mat);
     mat = (double*)calloc(a.cols*a.rows, sizeof(double));
     }
  rows = a.cols; cols = a.rows;
  
  int i;
    for (i = 0; i<rows; i++)
      for (int j = 0; j<cols; j++)
         mat[j*rows + i] = a.mat[i*cols + j];
   }
  
               //**********************************************
               //Debugging group   
               //**************************************
void MAT::Show()
  {
  for (int i = 0; i<rows; i++)
    {
    for (int j = 0; j<cols; j++)
      std::cerr<< mat[i * cols + j]<<"  ";
    std::cerr<<std::endl;
    }  
  }

      //**************************************
      //Transform n doubles
 
void MAT::Transform(int n, double* dptr)
  {
  int i, j;
  double sum;

  double* answer;
  try {answer= new double[n];}
  catch(...)
      {
      std::cerr<<"No mem for answer in MAT::Transform\n";
      return;
      }

      //Make sure that you are dealing with a valid
      //subset of this matrix
  int validrows = rows;
  int validcols = cols;
  if (n < rows) validrows = n;

  for (i = 0; i < validcols; i++)  //for each safe element in dptr
    {
    sum = 0;
    for (j = 0; j <validrows; j++)
     sum += dptr[j] * mat[i*cols +j]; 
    answer[i] = sum;
    }

  for (i = 0; i<validcols; i++)
    dptr[i] = answer[i];

  delete [] answer;
  }
//-------------------------------------------
         //same as above but for floats
void MAT::Transform(int n, float* dptr)
  {
  int i, j;
  double sum;

  double* answer;
  try {answer= new double[n];}
  catch(...)
      {
      std::cerr<<"No mem for answer in MAT::Transform\n";
      return;
      }

      //Make sure that you are dealing with a valid
      //subset of this matrix
  int validrows = rows;
  int validcols = cols;
  if (n < rows) validrows = n;

  for (i = 0; i < validcols; i++)  //for each safe element in dptr
    {
    sum = 0;
    for (j = 0; j <validrows; j++)
     sum += dptr[j] * mat[i*cols +j]; 
    answer[i] = sum;
    }

  for (i = 0; i<validcols; i++)
    dptr[i] = (float)answer[i];

  delete [] answer;
  }
//---------------------------------------------
void MAT::Set(double* a)
   {
   for (int i = 0; i < rows * cols; i++)
      mat[i] = a[i];
   }

void MAT::Get(double* a)
   {
   for (int i = 0; i < rows * cols; i++)
      a[i] = mat[i];
   }

                    ////////////////////////////////////////////////////
                    //    MATX derived class
                    ///////////////////////////////////////////////////
      
MATX::MATX():MAT(4,4)
  {
  Identity();
  }

MATX::MATX(MATX& m):MAT(4,4)
  {
  for (int i = 0; i< rows; i++)
    for (int j= 0; j<cols; j++)
       mat[i*cols + j] = m.mat[i*cols + j];
  }

void MATX::Identity()
  {
  Zero();
  for (int i = 0; i<4; i++)
     mat[5*i] = 1;
  }

MATX& MATX::operator = (MATX& m)
  {  
  for (int i = 0; i< rows; i++)
    for (int j= 0; j<cols; j++)
       mat[i*cols + j] = m.mat[i*cols + j];
  return *this;
  }

         //These are repeated in the derived class since MATX doesn't
         //need to check for size requirements. 
MATX& MATX::operator * (MATX& m)
  {
  MATX temp = *this;
  Zero();
  for (int i = 0; i< rows; i++)
    {
    for (int j = 0; j< cols; j++)
      {
      for (int k = 0; k<cols; k++)
         mat[i*cols +j] += temp.mat[i*cols + k] * m.mat[k*m.cols +j];
      }
    }
  return *this;
  }

MATX& MATX::operator + (MATX& m)
  {
  for (int i = 0; i< rows; i++)
    {
    for (int j = 0; j< cols; j++)
      mat[i*cols + j] += m.mat[i*cols + j];
    }
  return *this;
  }

MATX& MATX::operator - (MATX& m)
  {
  for (int i = 0; i< rows; i++)
    {
    for (int j = 0; j< cols; j++)
      mat[i*cols + j] -= m.mat[i*cols + j];
    }
  return *this;
  }

            //**************************************
            // specific matrices to scale, translate or rotate.
 void  MATX::Translate3d(double x, double y, double z)
  {
  Identity();
  mat[3] = x;    //element [0][3]
  mat[7] = y;    //element [1][3]
  mat[11] = z;   //element [2][3]
  }

void MATX::Scale3d(double x, double y, double z)
  {
  Identity();
  mat[0] = x;        //element [0][0]
  mat[5] = y;        //element [1][1]
  mat[10] = z;       //element [2][2]
  }

        //rotation in either degrees or radians. For rotation around z,
        //x' = xcos(theta) - ysin(theta); y' = xsin(theta) + ycos(theta).
void MATX::Rotate3dradians(int axis, double angle)
  {
  int m1, m2;
  double c = cos(angle);
  double s = sin(angle);
  Identity();

  //get locations for sin, cosine
  m1 = (axis%3) +1;
  m2 = m1%3;
  m1 -= 1;

  mat[m1*cols + m1] = c;
  mat[m1*cols + m2] = -s;
  mat[m2*cols + m2] = c;
  mat[m2*cols + m1] = s;
  }



void MATX::Rotate3d(int axis, double angle)
  {
  int m1, m2;
  double c,s;
  MATH math;
  
  Identity();
                  //get locations for sin, cosine
  m1 = (axis%3) +1;
  m2 = m1%3;
  m1 -= 1;

  c = math.CosD(angle);
  s = math.SinD(angle);

  mat[m1*cols + m1] = c;
  mat[m1*cols + m2] = s;
  mat[m2*cols + m2] = c;
  mat[m2*cols + m1] = -s;
  }

void MATX::Coordinate_Rotation(VEC xrot, VEC yrot, VEC zrot)
  {
  Identity();
  double a,b,c;
  xrot.Get(&a, &b, &c);
  mat[0] = a; mat[1] = b; mat[2] = c;

  yrot.Get(&a, &b, &c);
  mat[4] = a; mat[5] = b; mat[6] = c;

  zrot.Get(&a, &b, &c);
  mat[8] = a; mat[9] = b; mat[10] = c;
  }

void MATX::Shear(int axis, double a, double b)
  {
  Identity();
  switch(axis)
    {
    case 1: mat[4] = a; mat[8] = b; break;
    case 2: mat[1] = a; mat[9] = b; break;
    case 3: mat[2] = a; mat[6] = b; break;
    }
  }
          //*********************
          // transform point or vector
          //************************
 void MATX::Transform(VEC* v)
  {
  double v0, v1, v2;
  double v0temp, v1temp, v2temp;
  v->Get(&v0, &v1, &v2);
       
      //multiply each vector value by appropriate matrix row. Note
      //that 4th value is not used, as the vector value is 0 for the 4th
      //element. The multiplication is performed as if the matrix were on
      //the left and a vertical, 4x1 (matrix)vector were on the right.
  v0temp = mat[0]*v0 + mat[1]*v1 + mat[2]*v2;
  v1temp = mat[4]*v0 + mat[5]*v1 + mat[6]*v2;
  v2temp = mat[8]*v0 + mat[9]*v1 + mat[10]*v2;

  *v = VEC(v0temp, v1temp, v2temp);
  }

 void MATX::Transform(PNT* pt)
  {
  double a,b,c,d;
  pt->Get(&a,&b,&c,&d);
  double x,y,z,w;

  x = mat[0]*a + mat[1]*b + mat[2]*c + mat[3]*d;
  y = mat[4]*a + mat[5]*b + mat[6]*c + mat[7]*d;
  z = mat[8]*a + mat[9]*b + mat[10]*c + mat[11]*d;
  w = mat[12]*a + mat[13]*b + mat[14]*c + mat[15]*d;

 pt->Set(x,y,z,w);
 }

       //*********************
       //rotation of axes by angle (in degrees) phi around a line with base 
       //base, and direction dir.
 
void MATX::Genrot(double phi, PNT base, VEC dir)
  {
  double bx,by,bz, dx,dy,dz, theta, beta, v;
  MATX F,G,H,W,FI,GI,HI,A;
  MATH math;

  base.Get(&bx,&by,&bz);
  dir.Get(&dx,&dy,&dz);

       //translation and inverse translation matrices to base
  F.Translate3d(-bx,-by,-bz); FI.Translate3d(bx,by,bz);
 
       //x,y angle and inverse set into z coords (rotate axes around z axis)
  theta = math.Atan3(dx,dy);
  G.Rotate3dradians(3, theta); GI.Rotate3dradians(3,-theta);

  if (fabs(dx) < .0001 && fabs(dy) < .0001) v = 0;
  else v = sqrt(dx*dx + dy*dy);
  beta = math.Atan3(dz,v);

  H.Rotate3dradians(2,beta); HI.Rotate3dradians(2, -beta);
  W.Rotate3d(3,phi);

  A = FI * GI * HI * W * H * G * F;
  *this = A;
  }
   //************************************
        //Get caller into the inverse of another MATX. From GRATIS
   //lib3d. Originally from philip r bevington data reduction and
   //error analysis for the physical sciences. pp 302-303. Gotten
   //from Mitch Soltys and George Sherouse.

unsigned char MAT::Invert(double* det)
  {
  if (rows != cols) return 0;
  int order = rows;       
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
       std::cerr<<"No mem for calloc in MAT::Invert";
       return 0;
       }

  for (i = 0; i< order * order; i++)
   matx[i] = mat[i]; 

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
         {*det = 0.0; return (0);}
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
     mat[i] = matx[i];
  free (matx);
  free (jk);
  free (ik);  
  return(1); 
  }
    //*****************************************
   //File saves and loads. Allows a 1 line description
unsigned char MATX::Save(char* fname, char* description)
  {
  std::ofstream ofile;
  ofile.open(fname);
  if (!ofile)
    {std::cerr<<"Could not open matrix file for save"; return 0;}
  int o,i;

  ofile<<description<<std::endl;
  for (i = 0; i<4; i++)
    {
    o = i * 4;
    ofile<<mat[o]<<"  "<<mat[o+1]<<"  "<<mat[o+2]<<"  "<<mat[o+3]<<'\n';
    }
  ofile.close();
  return 1;
  }

unsigned char MATX::Load(char* fname)
  {
  std::ifstream ifile;
  TBOX tbox;
  double vals[4];
  unsigned char returnval = 1;
  int i, j;

  //ifile.open(fname, ios::nocreate | ios::in);   //nocreate not recognized by compiler
  ifile.open(fname, std::ios::in);
  if (!ifile)
  {std::cerr<<"Could not open matrix file"; return 0;}

  char* line;
  try {line= new char[200];}
  catch(...){std::cerr<<"No mem for line in matrix load"; return 0;}
  ifile.getline(line,199);   //throw out descriptive line
  
  for (i = 0; i<4; i++)
    {
    ifile.getline(line, 199);
    if (!strlen(line) && ifile) {--i; continue;}   //possible blank line
    if (!tbox.Stringtondoubles(line, 4, vals))
     {std::cerr<<"Failure in MATX::Load"; returnval = 0; break;}
    for (j = 0; j < 4; j++)
      mat[4*i + j] = vals[j];
    }
  ifile.close();
  delete [] line;
  return returnval;
  }
      //********************************


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
               //END MATRIX. BEGIN GEOM
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

   //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      //LINE EQUATIONS
   //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   //Get the distance and projection point of a point on a line. Returns
   //0 if the point lies on the line.
unsigned char GEOM::Pointtoline (PNT src, VEC dir, PNT pt, double* dist, PNT* proj)
  {
  unsigned char returnval = 1;
  double d;
  dir.Vnormalize();
  VEC distVEC;
      //Get the equation of the plane perpendicular to the line.
      //The plane's surface normal is dir and pt lies in the plane.
      //Then find the line intersection with the plane. This is the
      //projection point of point pt.
  d = PlaneD(pt, dir);
  Linethroughplane(src, dir, dir, d, proj);
  distVEC = pt - *proj;
  *dist = distVEC.Vlen();
  if (dist == 0) returnval = 0;
  return returnval;
  }

unsigned char GEOM::Pointtolinewithdir
(PNT src, VEC dir, PNT pt, double* dist, PNT* proj, VEC* newdir)
  {
  unsigned char returnval = 1;
  double d;
  dir.Vnormalize();
  VEC distVEC;
      //Get the equation of the plane perpendicular to the line.
      //The plane's surface normal is dir and pt lies in the plane.
      //Then find the line intersection with the plane. This is the
      //projection point of point pt.
  d = PlaneD(pt, dir);
  Linethroughplane(src, dir, dir, d, proj);
  distVEC = pt - *proj;
  *newdir = distVEC;
  *dist = distVEC.Vlen();
  if (dist == 0) returnval = 0;
  return returnval;
  }

   
   //Return 0 if lines do not intersect or superimpose. Inelegant.
unsigned char GEOM::Lineintersect(PNT p1, VEC d1, PNT p2, VEC d2, PNT* isection)
  {
  unsigned char returnval = 0;
  unsigned char matinvert = 0;
  double det;
  double answer[2];
  d1.Vnormalize();
  d2.Vnormalize();
  if (d1.Vdot(d2) == 1) return 0;
  double p1x, p1y, p1z, d1x, d1y, d1z, p2x, p2y, p2z, d2x, d2y, d2z;
  p1.Get(&p1x, &p1y, &p1z);
  d1.Get(&d1x, &d1y, &d1z);
  p2.Get(&p2x, &p2y, &p2z);
  d2.Get(&d2x, &d2y, &d2z);

        //try for x,y solution. t for line 1 ends up in answer[0]
      //and s for line 2 ends up in answer[1]
  MAT mat(2,2);
  mat.Set(0,0, d1x);
  mat.Set(0,1, -d2x);
  mat.Set(1,0, d1y);
  mat.Set(1,1, -d2y);
  double lastval;

  if (mat.Invert(&det))
    {
    matinvert = 1;
    answer[0] = p2x - p1x;
    answer[1] = p2y - p1y;
    mat.Transform(2, answer);
    lastval = fabs(p1z + answer[0] * d1z - (p2z + answer[1] * d2z));
    if (lastval < .000001)
      {
      returnval = 1;
      isection->Set(p1x + answer[0] * d1x, p1y + answer[0] * d1y, p1z + answer[0] * d1z);
      }
    }

   //if you did not get a matrix inversion, try x z
  if (!matinvert)
    {
    mat.Set(1,0, d1z);
    mat.Set(1,1, -d2z);
    if (mat.Invert(&det))
      {
      matinvert = 1;
      answer[0] = p2x - p1x;
      answer[1] = p2z - p1z;
      mat.Transform(2, answer);
      if (fabs(p1y + answer[0] * d1y - (p2y + answer[1] * d2y)) < .000001)
   {
        returnval = 1;
   isection->Set(p1x + answer[0] * d1x, p1y + answer[0] * d1y, p1z + answer[0] * d1z);
   }
      }
    }

   //if you did not get a matrix inversion, try y z
  if (!matinvert)
    {
    mat.Set(0,0, d1y);
    mat.Set(0,1, -d2y);
    if (mat.Invert(&det))
      {
      answer[0] = p2y - p1y;
      answer[1] = p2z - p1z;
      mat.Transform(2, answer);
      if (fabs(p1x + answer[0] * d1x - (p2x + answer[1] * d2x)) < .000001)
   {
   isection->Set(p1x + answer[0] * d1x, p1y + answer[0] * d1y, p1z + answer[0] * d1z);
   returnval = 1;
   }
      }
    }
  return returnval;
  }
  

   //**********************************************
   //Get closest point between two lines. Will also give the closest
   //point on each of the two lines and the distance between the lines.
   //Returns 0 if lines parallel. The assumption here is that the
   //line formed be (apt, bpt) is orthogonal to each of the two input
   //lines; the dot product between this joining line and each of the
   //input lines will be 0. If dist is 0 the lines intersect.

unsigned char GEOM::Closestpoint
  (PNT pa, VEC d1, PNT pb, VEC d2, PNT* closept, PNT* apt, PNT* bpt, double* dist)
  {
  unsigned char returnval = 1;
  double d1d2, d1d2squared, oneminusd1d2s;
  VEC temp;
  double lambda1,lambda2;
  double firstdot, seconddot;

  d1.Vnormalize();
  d2.Vnormalize();
  d1d2 = d1.Vdot(d2);
  d1d2squared = d1d2 * d1d2;
  if (d1d2squared == 1) return 0;       //lines are parallel
  oneminusd1d2s = 1.0 - d1d2squared;


        //get (pb-pa).d1 and (pb - pa).d2
  temp = pb - pa;
  firstdot = temp.Vdot(d1);
  seconddot = temp.Vdot(d2);

      //solve for lambdas
  
  lambda1 = (firstdot - (d1d2 * seconddot))/oneminusd1d2s;
  lambda2 = ( (d1d2 * firstdot) - seconddot)/oneminusd1d2s;


     //now use the parametric values to find the closest points on each
   //line
  *apt = pa + (d1 * lambda1);
  *bpt = pb + (d2 * lambda2);
  temp = *bpt - *apt;
  *dist = temp.Vlen();
  temp = temp * 0.5;
  *closept = *apt + temp;

  return returnval;
  }
   //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   //PLANE AND LINE-PLANE EQUATIONS
   //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   //Get surface normal of 3 points
unsigned char GEOM::Getsrfn( PNT p0, PNT p1, PNT p2, VEC* v)
  {
  unsigned char returnval = 0;
  VEC firstVEC = p1 - p0;   //1st VEC from zeroth to 1st point
  VEC secondVEC = p2 - p1;  //2nd VEC from 1st to 2nd point
  v->Vcross(firstVEC,secondVEC);

  if (v->Vlen() )   
    {
    returnval = 1;
    v->Vnormalize();
    }
  return returnval;
  }

   //*********************************************
   //For the plane equation Ax + By + Cz = D, get D.
double GEOM::PlaneD(PNT p1, VEC srfn)
  {
  VEC to_origin(p1);
  return (float)(srfn.Vdot(to_origin));
  }
   //***************************************************

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   //MISCELLANEOUS
   //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

   //Get reflection of a line from a surface of surfacenormal srfn
unsigned char GEOM::Refldir(VEC dir, VEC srfn, VEC* reflectedray)
  {
  double c =  -2.0 * srfn.Vdot(dir);
  *reflectedray = srfn * c + dir;
  return 1;
  }

   //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
   //Line/triangle/plane intersections
   //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
   //Does a line intersect a plane? Return 0 if line parallel to plane,
   //1 if line intersects plane, 2 if line is in plane.
   //Also returns 0 if something went wrong.
unsigned char GEOM::Linethroughplane(PNT src, VEC dir, PNT p0, PNT p1, PNT p2, PNT* answer)
  {
  unsigned char returnval;
  double d=0, dotdenom=0, dotnumer=0, t;
  VEC srfn;
  double xsrc,ysrc,zsrc,xdir,ydir,zdir;
  dir.Vnormalize();
  VEC srcVEC(src);


      //Get plane equation ax + by + cz = d;
  returnval = Getsrfn(p0, p1, p2, &srfn);
  if (returnval) d = PlaneD(p0, srfn);
  
      //find out if line is parallel to plane and if so if in it
  if (returnval)
    {
    dotdenom = dir.Vdot(srfn);
    dotnumer = d - srcVEC.Vdot(srfn);
    if (fabs(dotdenom) < .000001)
      {
      returnval = 0;      //line parallel to plane
      if (fabs(dotnumer) < .000001)
   returnval = 2;      //line is in plane
      }
    }

      //if line is not parallel, find intersection
      //a(px + t*dx) + b(py + t*dy) + c(pz + t*dz) = d; solve for t
  if (returnval == 1)
    {
    t = dotnumer/dotdenom;   
    src.Get(&xsrc, &ysrc, &zsrc);
    dir.Get(&xdir, &ydir, &zdir);
    answer->Set(xsrc + t*xdir, ysrc + t*ydir, zsrc + t*zdir);
    }
  return returnval;
  }
//------------------------------------------
      //This version uses a plane characterized by a point and srfn,
      //and only gives the value for t. Use for queries of directed line
      //intersection.
unsigned char GEOM::Linethroughplane      
     (PNT src, VEC dir, PNT pt, VEC srfn, double* tt)
   {
  unsigned char returnval = 1;
  double d=0, dotdenom=0, dotnumer=0;
  dir.Vnormalize();
  VEC srcVEC(src);

        //Get plane equation ax + by + cz = d;
  d = PlaneD(pt, srfn);

  dotdenom = dir.Vdot(srfn);
  if (fabs(dotdenom) < .0000001)
     {
      returnval = 0;      //line parallel to plane or in it
      if (fabs(dotnumer) < .0000001)
         {
         returnval = 2;     //line is in plane. Take the first point of line
         *tt = 0;
         }
     }

  if (returnval == 1)
     {
     dotnumer = d - srcVEC.Vdot(srfn);
     *tt = dotnumer/dotdenom;
     }

  return returnval;
  }
   //*************************************
   //Same as above but plane has already been characterized
unsigned char GEOM::Linethroughplane(PNT src, VEC dir, VEC srfn, double d, PNT* answer)
  {
  unsigned char returnval = 1;
  double dotdenom, dotnumer, t;
  double xsrc,ysrc,zsrc,xdir,ydir,zdir;
  dir.Vnormalize();
  VEC srcVEC(src);

      //find out if line is parallel to plane and if so if in it
  dotdenom = dir.Vdot(srfn);
  dotnumer = d - srcVEC.Vdot(srfn);
  if (fabs(dotdenom) < .000001)
    {
    returnval = 0;      //line parallel to plane
    if (fabs(dotnumer) < .000001)
    returnval = 2;      //line is in plane
    }

      //if line is not parallel, find intersection
      //a(px + t*dx) + b(py + t*dy) + c(pz + t*dz) = d; solve for t
  if (returnval == 1)
    {
    t = dotnumer/dotdenom;   
    src.Get(&xsrc, &ysrc, &zsrc);
    dir.Get(&xdir, &ydir, &zdir);
    answer->Set(xsrc + t*xdir, ysrc + t*ydir, zsrc + t*zdir);
    }
  return returnval;
  }

   //*************************************
   //Is a point inside an arbitrarily defined triangle?
   //The idea is to make a parametric equation out of the base triangle
   //line and draw a VECtor between the triangle tip and the test point.
   //If an extension of that VECtor crosses the base within defined
   //limits, the point is inside the triangle.
   //The unnormalized parametric equation of the base is 
   // t0 + t(t1 - t0) with valid values of t between 0 and 1.
   //The parametric equation of the VECtor is
   //t2 + s(point - t2); s must be greater or equal to 1.
unsigned char GEOM::Pointintriangle(PNT point, PNT t0, PNT t1, PNT t2)
  {
  double pointx, pointy, pointz; 
  double t0x, t0y, t0z;
  double t1x, t1y, t1z;
  double t2x,t2y, t2z;
  unsigned char returnval = 0;
  unsigned char matinversion = 0;

  MAT mat(2,2);
  double* answer = new double[2];
  double det;

  point.Get(&pointx, &pointy, &pointz);
  t0.Get(&t0x, &t0y, &t0z);
  t1.Get(&t1x, &t1y, &t1z);
  t2.Get(&t2x, &t2y, &t2z);

      //try to find solution for s and t 
      // using x and y values. The value of t is put in the
      //zeroth position of answer. 
      // general form: t(t1x - t0x) - s(pointx - t2x) = t2x - t0x
      //       t(t1y - t0y) - s(pointy - t2y) = t2y - t0y
  mat.Set(0,0, t1x - t0x);
  mat.Set(0,1, -(pointx - t2x));
  mat.Set(1,0, t1y - t0y);
  mat.Set(1,1, -(pointy - t2y));
  if (mat.Invert(&det))
    {
    matinversion = 1;
    answer[0] = t2x - t0x;
    answer[1] = t2y - t0y;
    mat.Transform(2, answer);
 
      //if you are OK in 2 dimensions, check for the third. Allow
        //a little room for floating point error.
    if (answer[1] >= .999999 && answer[0] >= -.000001 && answer[0] <=1.000001) 
      {
      if (fabs(answer[0] * (t1z - t0z) - answer[1] *(pointz - t2z) - (t2z -t0z)) < .000001)
   returnval = 1;

      }
    }

      //if the first attempt provided the equivalent of dividing
      //by 0, you could not invert matrix. Try x and z values.
  if (!matinversion)
    {
     mat.Set(1,0, t1z - t0z);
     mat.Set(1,1, -(pointz - t2z));
     if (mat.Invert(&det))
      {
      matinversion = 1;
      answer[0] = t2x - t0x;
      answer[1] = t2z - t0z;
      mat.Transform(2, answer);
      if (answer[1] >= 1 && answer[0] >=0 && answer[0] <=1)
   {
        if (fabs(answer[0] * (t1y - t0y) - answer[1] *(pointy - t2y) - (t2y -t0y)) < .000001)
     returnval = 1;
        }
      }
    }
      //if neither of the above attempts worked, try
      //y and z values
  if (!matinversion)
    {
     mat.Set(0,0, t1y - t0y);
     mat.Set(0,1, -(pointy - t2y));
     if (mat.Invert(&det))
      {
      answer[0] = t2y - t0y;
      answer[1] = t2z - t0z;
      mat.Transform(2, answer);
      if (answer[1] >=1 && answer[0] >=0 && answer[0] <=1) 
   {
        if (fabs(answer[0] * (t1x - t0x) - answer[1] *(pointx - t2x) - (t2x -t0x)) < .000001)
     returnval = 1;
        }
      }
    }
  delete [] answer;
  return returnval;
  }
      //***************************************
      //Does a line pass through a triangle? Returns the number of
      //points (0, 1, or 2 intersection points if line coplanar)
unsigned char GEOM::Linethroughtriangle   //Does a 3D line pass through a triangle?
   (PNT src, VEC dir, PNT p0, PNT p1, PNT p2, PNT* answer1, PNT* answer2)
  {
  unsigned char returnval, tempreturnval;
  VEC ptdir;

      //Get intersection point with plane
  returnval = Linethroughplane(src, dir, p0, p1, p2, answer1);

        //If the line pierces the plane, see if it is in the triangle
  if (returnval == 1)
    returnval = Pointintriangle(*answer1, p0, p1, p2);

      //If the line is coplanar get both intersection points if present
  else if (returnval == 2)
    {
    ptdir = p1 - p0;
    returnval = Lineintersect(src, dir, p0, ptdir, answer1);
    if (returnval) returnval = Pointintriangle(*answer1, p0, p1, p2);

    ptdir = p2 - p1;
    if (returnval)
      {
      tempreturnval= Lineintersect(src, dir, p1, ptdir, answer2);
      if (tempreturnval)
   {
        tempreturnval = Pointintriangle(*answer2, p0, p1, p2);
        if (tempreturnval) returnval = 2;
   }
      }
    else 
      {
      tempreturnval = Lineintersect(src, dir, p1, ptdir, answer1);
      if (tempreturnval)
   {
        tempreturnval = Pointintriangle(*answer1, p0, p1, p2);
        if (tempreturnval) returnval = 1;
   }
      }

    ptdir = p0 - p2;
    if (returnval == 0)
      {
      tempreturnval = Lineintersect(src, dir, p2, ptdir, answer1);
      if (tempreturnval)
   {
        tempreturnval = Pointintriangle(*answer1, p0, p1, p2);
        if (tempreturnval) returnval =1;
   }
       }
    else if (returnval == 1)
      {
      tempreturnval = Lineintersect(src, dir, p2, ptdir, answer2);
      if (tempreturnval)
   {
        tempreturnval = Pointintriangle(*answer2, p0, p1, p2);
        if (tempreturnval) returnval = 2;
   }
      }
    }
          
  return returnval;
  }
      //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      //PYRAMIDS
      //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
         //Checks only sides, not base
unsigned char GEOM::Linethroughpyramid(PNT src, VEC dir, PNT tip, PNT tl, PNT tr, PNT br, PNT bl, PNT* answer1, PNT* answer2)
  {
  unsigned char howmany;
  unsigned char count = 0;
  PNT tempanswer1, tempanswer2;

   //look at top
  howmany = Linethroughtriangle(src, dir, tr, tl, tip, &tempanswer1, &tempanswer2);
  if (howmany == 1)
    { ++count; *answer1 = tempanswer1;}
  else if (howmany == 2)
    {*answer1 = tempanswer1; *answer2 = tempanswer2; return 2;}

   //look at bottom
  howmany = Linethroughtriangle(src, dir, bl, br, tip, &tempanswer1, &tempanswer2);
  if (howmany == 1)
    {
    ++count;
    if (count ==2) {*answer2 = tempanswer1; return 2;}
    else *answer1 = tempanswer1;
    }
  else if (howmany ==2)
    {
    if (count) {std::cout<<"point may be missed"<<std::endl; *answer2 = tempanswer1;}
    else {*answer1 = tempanswer1; *answer2 = tempanswer2;}
    return 2;
    }

   //look at left side
  howmany = Linethroughtriangle(src, dir, tl, bl, tip, &tempanswer1, &tempanswer2);
  if (howmany == 1)
    {
    ++count;
    if (count ==2) {*answer2 = tempanswer1; return 2;}
    else *answer1 = tempanswer1;
    }
  else if (howmany == 2)
    {
    if (count) {std::cout<<"point may be missed"<<std::endl; *answer2 = tempanswer1;}
    else {*answer1 = tempanswer1; *answer2 = tempanswer2;}
    return 2;
    }

   //look at right side
  howmany = Linethroughtriangle(src, dir, br, tr, tip, &tempanswer1, &tempanswer2);
  if (howmany == 1)
    {
    ++count;
    if (count ==2) {*answer2 = tempanswer1; return 2;}
    else *answer1 = tempanswer1;
    }
  else if (howmany == 2)
    {
    if (count) {std::cout<<"point may be missed"<<std::endl; *answer2 = tempanswer1;}
    else {*answer1 = tempanswer1; *answer2 = tempanswer2;}
    return 2;
    }

      //this is under test; look at base
    howmany = Linethroughtriangle(src, dir, bl, tr, br, &tempanswer1, &tempanswer2);
    if (howmany == 1)
      {
      ++count;
      if (count ==2) {*answer2 = tempanswer1; return 2;}
      else *answer1 = tempanswer1;
      }
    else if (howmany == 2)
      {
      if (count) {std::cout<<"point may be missed"<<std::endl; *answer2 = tempanswer1;}
      else {*answer1 = tempanswer1; *answer2 = tempanswer2;}
      return 2;
      }

   howmany = Linethroughtriangle(src, dir, bl, tl, tr, &tempanswer1, &tempanswer2);
    if (howmany == 1)
      {
      ++count;
      if (count ==2) {*answer2 = tempanswer1; return 2;}
      else *answer1 = tempanswer1;
      }
    else if (howmany == 2)
      {
      if (count) {std::cout<<"point may be missed"<<std::endl; *answer2 = tempanswer1;}
      else {*answer1 = tempanswer1; *answer2 = tempanswer2;}
      return 2;
      }

  return count;
  }   
   //****************************************
   //Faster version of same thing--returns yes or no only
      //Checks only sides, not base
unsigned char GEOM::Linethroughpyramid(PNT src, VEC dir, PNT tip, PNT tl, PNT tr, PNT br, PNT bl)
  {
  unsigned char howmany;
  PNT tempanswer1, tempanswer2;

   //look at top
  howmany = Linethroughtriangle(src, dir, tr, tl, tip, &tempanswer1, &tempanswer2);
  if (howmany) return howmany;
  
   //look at bottom
  howmany = Linethroughtriangle(src, dir, bl, br, tip, &tempanswer1, &tempanswer2);
  if (howmany) return howmany;

   //look at left side
  howmany = Linethroughtriangle(src, dir, tl, bl, tip, &tempanswer1, &tempanswer2);
  if (howmany) return howmany;

   //look at right side
  howmany = Linethroughtriangle(src, dir, br, tr, tip, &tempanswer1, &tempanswer2);
  if(howmany) return howmany;

  return 0;
  }   
 //*************************************************
      //FRENET FRAME AND 3D CURVES
//****************************************************
int GEOM::GetFrenetFrame(VEC velocity, VEC accel, VEC* tangent, VEC* normal, VEC* b)
   {
         //tangent vector is the normalized velocity
   *tangent = velocity;
   tangent->Vnormalize();

            //normal vector is derived by the crosses of VxAxV and normalized
   VEC temp;
   temp.Vcross(velocity, accel);
   normal->Vcross(temp, velocity);
   normal->Vnormalize();

            //the remaining vector is at right angles to the other two
   b->Vcross(*tangent, *normal);
   b->Vnormalize();

            //if one of them is a zero vector, b will be a 0 vector
   if (!b->Vlen()) return 0;
   return 1;
   }
//-----------------------------------------
double GEOM::GetTriangleArea(PNT a, PNT b, PNT c)
   {
         //Get line for base
   VEC dir = b - a;
   double baselen = dir.Vlen();

   PNT intersectpt;
   double height;
   Pointtoline(a, dir, c, &height, &intersectpt);

   return (0.5 * height * baselen);
   }
//-------------------------------------------
      //Get the radius of the oscultating circle given three points
      //along the curve. Radius = (length1) * (length2) * length3/(4 *triangle_area)
double GEOM::GetCircleRadius(PNT a, PNT b, PNT c)
   {
   VEC v1 = b - a;
   double dist1 = v1.Vlen();

   VEC v2 = c - a;
   double dist2 = v2.Vlen();

   VEC v3 = b - c;
   double dist3 = v3.Vlen();

   double triangle_area = GetTriangleArea(a,b,c);
   if (!triangle_area) return 0;                //pts are colinear or duplicated

   return (dist1 * dist2 * dist3)/(4 * triangle_area);
   }
    
}//end of namespace