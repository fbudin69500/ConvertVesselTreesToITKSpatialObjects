#include "ebINTCURVE.h"

namespace eb
{

INTCURVE::INTCURVE():OBJECTIO()
   {
   limitpointdims = 3;

   numused = 0; index = -1; 
   startpoint = -1; lastknown = -1; rad = 0;
   Type("Tube");
   strcpy_s(error, 100, "");
   }
//----------------------------------------
INTCURVE::INTCURVE(short n):OBJECTIO()
  {
  limitpointdims = 3;
  pointnum = n;
  Realloc(pointnum, 3);
  numused = 0; index = -1;
  startpoint = -1; lastknown = -1;
  rad = 0; index = -1;
  Type("Tube");
  strcpy_s(error, 100, "");
   }
//------------------------------------------------------------
INTCURVE& INTCURVE::operator = (const INTCURVE& r)
  {
  if (this == &r) return *this;

  Copy(r);

  numused = r.numused;
  index = r.index;
  startpoint = r.startpoint; 
  lastknown = r.lastknown;
  rad = r.rad;
  ribparents = r.ribparents;
  limitpointdims = 3;
  return *this;
  }
//------------------------------------------------------
INTCURVE::~INTCURVE()
   {
   OBJECTIO::Releasemem();
   ribparents.Releasemem();
   }
//---------------------------------------------------------
void INTCURVE::Setmem(short a)
   {
   Realloc(a, 3);
   numused = 0; index = -1;
   startpoint = -1; lastknown = -1;
   rad = 0; index = -1;
   Type("Tube");
   strcpy_s(error, 100, "");
   }


         //*******************************
         //Get and set data. All return 0 on failure
unsigned char INTCURVE::Addpoint(short x, short y)
  {
  if (numused >= pointnum) {strcpy_s(error, 100, "You are at point maximum");
      return 0;}     

  pts[numused * 3] = (float)x;
  pts[numused * 3 + 1] = (float) y;

   ++numused;
  return 1;
  }
//----------------------------------------------------
unsigned char INTCURVE::Getpoint(short n, short* x, short* y)
   {
   if (n >= numused) return 0;
   *x = (short)pts[n * 3];
   *y = (short)pts[n*3 + 1];
   return 1;
   }

   //*******************************************
   //functions used to set startpoint and lastknown values used during 3D curve reconstruction
void INTCURVE::Setstart(short a)
  {
  if (a < numused) startpoint = a;
  else 
    {
   startpoint = 0;
    }
  }
//---------------------------------------------------
void INTCURVE::Setlastknown(short a)
  {
  if (a < numused) lastknown = a;
  else {strcpy_s(error, 100, "invalid setlastknown call in INTCURVE");}
  }


unsigned char INTCURVE::Islegal(short a)
  {
  unsigned char returnval = 1;
  if (a < 0 || a >=numused) returnval = 0;
  return returnval;
  }



   //******************************
         //used in 3D curve reconstruction
short INTCURVE::Gap()
  {
  return (short(numused - lastknown));
  }

unsigned char INTCURVE::Topless(short TOPLESS)
  {
  if (numused - lastknown <= TOPLESS)
    return 0;
  else return 1;
  }
   //***********************************
   //Multiply the x, y and rad values by some constant. Used
   //when displaying a precreated curve at another display size. Rad
   //is not rounded because it seems to get too large.
 void INTCURVE::Multby(double a)
  {
 
  if (a > 0)
    {
    for (int i = 0; i<numused; i++)
      {
      pts[3 * i] = (short)(a * pts[3 * i] + 0.5);
      pts[3 * i + 1] = (short)(a * pts[3 * i + 1] + 0.5);
      }
    rad = (short)(rad * a);
    }
  }
 //******************************************
 int INTCURVE::Load(std::ifstream* ifile)
   {
   if(!ifile) return 0;
   this->Releasemem();

   if(!OBJECTIO::LoadHeader(ifile))
      return(0);

               //you should have loaded all mandatory and optional fields
   return LoadPoints(ifile);
   }
//--------------------------------------------------
 unsigned char INTCURVE::SaveFileHeader(std::ofstream* ofile, int a, int b, int num)
   {
   if (!(*ofile))
      {
      strcpy_s(error, 100, "Cannot write header to output file");
      return 0;
      }
    *ofile<<"NDims: 2";
   *ofile<<"Dimensions:  "<<a<<"  "<<b<<"  "<<1<<std::endl;
   *ofile<<"VoxelSize:  "<<"1.0"<<"  "<<"1.0"<<"  "<<"1.0"<<std::endl;
   *ofile <<"NObjects:  "<<num<<std::endl;
   *ofile <<"End Header:"<<std::endl<<std::endl<<std::endl;
   return 1;
   }
//--------------------------------------------------
//--------------------------------------------------
 unsigned char INTCURVE::Save(std::ofstream* ofile)
   {
   int i;

   *ofile<<"Type:  Tube"<<std::endl;
   *ofile<<"NPoints:  "<<pointnum<<std::endl;
   *ofile<<"PointDim:  "<<"3  x  y  rad"<<std::endl;
   *ofile<<"Points:"<<std::endl;

   for(i = 0; i < pointnum; i++)
      *ofile<<pts[3 * i]<<"  "<<pts[3 * i + 1]<<"  "<<rad<<std::endl;

   *ofile<<std::endl<<std::endl;

   return 1;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PROTECTED LOAD FUNCTIONS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 int INTCURVE::LoadPoints(std::ifstream* ifile)
   {
   int rval =  OBJECTIO::LoadPoints(ifile);
   numused = pointnum; index = -1; 
   startpoint = -1; lastknown = -1; rad = (short)pts[2];
   Type("Tube");
   strcpy_s(error, 100, "");
   return rval;
   }

 }//end of namespace