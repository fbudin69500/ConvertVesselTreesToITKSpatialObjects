#include "ebVESS_s.h"

namespace eb
{

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
   //CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT, RELEASEMEM
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

VESS_s::VESS_s(): OBJECTIO_s()
   {      
   //anat = new char[7];         //default anat is artery
      //strcpy_s(anat, 7, "artery");
   anat = "artery" ;
   //treetype = new char[9];    //default treetype is orphan
   //   strcpy_s(treetype, 9, "orphan");
   treetype = "orphan" ;
 
   proxoffset = 0;                      //set proximal cut
   distoffset = 0;                 //set distal cut
   sample = 0;                 //sample of skeleton points + rad
   numsamplepoints = 0;           //number of sample points
   childlist.SetDims(2);         //prepare for childID, attachpt pairs
   samplestep = 1;
   }
//----------------------------------------------------
VESS_s::VESS_s(const VESS_s& a):OBJECTIO_s(a)
   {
   try
      {
      if( anat.empty() )
         {
         anat = a.anat ;
         //anat = new char[strlen(a.anat) + 1]; 
         //strcpy_s(anat, strlen(a.anat) + 1, a.anat);
         }


      if(vparent != -1)
         {
         treetype = "child" ;
         //treetype = new char[6];
         //strcpy_s(treetype, 6, "child");
         }


      if (treetype.empty() )
         {
         //treetype = new char[9];
         //strcpy_s(treetype, 9, "orphan");
         treetype = "orphan" ;
         }

      proxoffset = a.proxoffset;
      distoffset = a.distoffset;

      sample = 0;
      numsamplepoints = 0;
      childlist.SetDims(2);
      childlist = a.childlist;
      }
   catch(...)
      {std::cout<<"Memory allocation failure in VESS_s copy constructor\n";}
   }
//-----------------------------------------------------------------------
VESS_s::~VESS_s()
   {this->Releasemem();}
//-------------------------------------------------------------------------
void VESS_s::Releasemem()
   {
   OBJECTIO_s::Releasemem();

   if (numsamplepoints)
      {
      delete [] sample;
      sample = 0;
      numsamplepoints = 0;
      }



   col[0] = col[3] = (float)1; col[1] = (float)0.3; col[2] = (float)0.3;
   }
//----------------------------------------------------------
VESS_s& VESS_s::operator = (VESS_s& a)
   {
   if (this == &a) return *this;

   Releasemem();
   try
      {
      OBJECTIO_s::Copy(a);

      if(anat.empty() )
         {
//         anat = new char[7];
//         strcpy_s(anat, 7, "artery");
         anat = "artery" ;
         }

      if(vparent > -1)
         {
         treetype = "child" ;
         //treetype = new char[6];
         //strcpy_s(treetype, 6, "child");
         }

 
      if(treetype.empty () )
         {
         treetype = "orphan" ;
         //treetype = new char[9];
         //strcpy_s(treetype, 9, "orphan");
         }


      proxoffset = a.proxoffset;
      distoffset = a.distoffset;

      sample = 0;
      numsamplepoints = 0;
      childlist.SetDims(2);
      childlist = a.childlist;
      }
   catch(...)
      {std::cout<<"Mem alloc error in VESS_s assignment operator\n";}

   return *this;
   }

//---------------------------------------------
VESS_s& VESS_s::ConcatenateVessels(VESS_s& a, VESS_s& b)
   {
   Releasemem();

   if (a.pointdim != b.pointdim)
      {
      std::cerr<<"Cannot concatenate vessels of different point dimensions\n";
      return *this;
      }

   int i;
   float* fptr;
   int twoi[2];

   try
      {
      id = a.id;
      if (!a.type.empty() )
         {
         type = a.type ;
         //type = new char[strlen(a.type) + 1]; 
         //strcpy_s(type, strlen(a.type) + 1, a.type);
         }
       else
         {
         //type = new char[5];
         //strcpy_s(type, 5, "tube");
         type = "tube" ;
         }

      if (!a.anat.empty() )
          {
          //anat = new char[strlen(a.anat) + 1];
          //strcpy_s(anat, strlen(a.anat) + 1, a.anat);
          anat = a.anat ;
          }
       else     
          {
          //anat = new char[7]; 
         //strcpy_s(anat, 7, "artery");
          anat = "artery" ;
         }

      if( !a.treetype.empty() )
          {
          treetype = a.treetype ;
          //treetype = new char[strlen(a.treetype) + 1];
         //strcpy_s(treetype, strlen(a.treetype) + 1, a.treetype);
         }
       else 
          {
          //treetype = 0;
          treetype = "" ;
          }
      if (!a.name.empty() )
        {
         //name = new char[strlen(a.name) + 1];
         //strcpy_s(name, strlen(a.name) + 1, a.name);
        name = a.name ;
         }
     else
        {
         //name = 0;
        name = "" ;
         }

     pointnum = a.pointnum + b.pointnum;
     pointdim = a.pointdim;

    pts = new float[pointnum * pointdim];
    if (!pts) {pointnum = 0;}
    else
       {
       memcpy(pts, a.pts, a.pointnum * pointdim * sizeof(float));
       fptr = pts + (a.pointnum * pointdim);
       memcpy(fptr, b.pts, b.pointnum * pointdim * sizeof(float));
       }

      dimdesc = a.dimdesc;
      //dimdesc = new char*[pointdim];
      /* for (i = 0; i < pointdim; i++)
        {
        dimdesc[i] = new char[strlen(a.dimdesc[i]) + 1];
        strcpy_s(dimdesc[i], strlen(a.dimdesc[i]) + 1, a. dimdesc[i]);
        }*/

      for (i = 0; i < 4; i++)
       col[i] = a.col[i];

    limitpointdims = a.limitpointdims;
    vparent = a.vparent;
      attachpt = a.attachpt;


    if(vparent > -1)
      {
      treetype = "child";
      //treetype = new char[6];
      //strcpy_s(treetype, 6, "child");
      }

 
    if(treetype.empty() )
      {
      treetype = "orphan";
      //treetype = new char[9];
      //strcpy_s(treetype, 9, "orphan");
       }


    proxoffset = a.proxoffset;
      distoffset = b.distoffset + a.NPoints();

      sample = 0;
      numsamplepoints = 0;
      childlist.SetDims(2);
      a.ResetChild();
      while (a.GetChild(twoi))
        AddChild(twoi[0], twoi[1]);

      b.ResetChild();
      while (b.GetChild(twoi))
        {
        twoi[1] += a.pointnum;
        AddChild(twoi[0], twoi[1]);
        }
      }

   catch(...)  {pointnum = 0;}

   return *this;
   }


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //DISTANCE FUNCTIONS AND  BOUNDING BOX
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //Get min and max for bounding box--skeleton only
int VESS_s::MinMaxBox(float* min, float* max)
   {
   int count = distoffset - proxoffset;

   if (pointdim < 3 || count <1) return 0;

   float* fptr = Point(proxoffset);

   float minx = *fptr;
   float miny = *(fptr + 1);
   float minz = *(fptr + 2);
   float maxx = *fptr;
   float maxy = *(fptr + 1);
   float maxz = *(fptr + 2);
   int i;
   
  count = 0;
   for (i = proxoffset; i < distoffset; i++)
      {
      fptr = Point(i);
      if (*fptr > maxx) maxx = *fptr;
      else if(*fptr < minx) minx = *fptr;


      ++fptr;
      if (*fptr > maxy) maxy = *fptr;
      else if(*fptr < miny) miny = *fptr;

      ++fptr;
      if (*fptr >maxz) maxz = *fptr;
      else if (*fptr < minz) minz = *fptr;

      ++count;
      }

   min[0] = minx;
   min[1] = miny;
   min[2] = minz;
   max[0] = maxx;
   max[1] = maxy;
   max[2] = maxz;

   return count;
   }
//-------------------------------------
         //Get min and max for bounding box. This is a dangerous
         //function that takes into account rad, but is blind
         //to actual box confines. It will give a larger bounding box
         //then is real, but should be complete. Be careful if rad has been multiplied.
int VESS_s::DangerousMinMaxBox(float* min, float* max)
   {
   int count = distoffset - proxoffset;

   if (pointdim < 4 || count <1) return 0;

   float* fptr = Point(proxoffset);
   float rad = fptr[3];
   int i;
   min[0] = fptr[0] - rad;
   min[1] = fptr[1] - rad;
   min[2] = fptr[2] - rad;
   max[0] = fptr[0] + rad;
   max[1] = fptr[1] + rad;
   max[2] = fptr[2] + rad;

   for (i = proxoffset + 1; i < distoffset; i++)
      {
      fptr = Point(i);
      rad = fptr[3];
      if (max[0] < fptr[0] + rad) max[0] = fptr[0] + rad;
      if (min[0] > fptr[0] - rad) min[0] = fptr[0] - rad;
      if (max[1] < fptr[1] + rad) max[1] = fptr[1] + rad;
      if (min[1] > fptr[1] - rad) min[1] = fptr[1] - rad;
      if (max[2] < fptr[2] + rad) max[2] = fptr[2] + rad;
      if (min[2] > fptr[2] - rad) min[2] = fptr[2] - rad;
      }
   return 1;
   }
//--------------------------------------------------------------
double VESS_s::MinDistSkel(float* fptr)
{
int i;
double dist;
double mindist;

if (!pointnum) return 100000000;      //a large number

            //get distance of first point
mindist = Distsqr(fptr, &pts[0]);

for (i = 1; i < pointnum; i++)
  {
  dist = Distsqr(fptr, &pts[i * pointdim]);             //will give square of distance
  if (dist < mindist)
    mindist = dist;
  }

        //Get the square root of the squared mindistance
if (mindist)
  mindist = sqrt(mindist);

return mindist;
}
//--------------------------------------------------------------------
double VESS_s::Distsqr(float* a, float* b)
   {
   return (((a[0] - b[0])*(a[0]-b[0])) + ((a[1]-b[1])*(a[1]-b[1])) + ((a[2]-b[2]) * (a[2]-b[2])));
   }
//----------------------------------------------------------------------
    //return minimum distance between a 3D point and the vessel skeleton
double VESS_s::MinDistSkel(float* src, int* loc)
{
int i;
double dist;
double mindist;

if (!pointnum) return 100000000;      //a large number

            //get distance of first point;
mindist = Distsqr(src, &pts[0]);
*loc = 0;

for (i = 1; i < pointnum; i++)
  {
  dist = Distsqr(src, &pts[i * pointdim]);             //will give square of distance
  if (dist < mindist)
    {
    mindist = dist;
    *loc = i;
    }
  }

        //Get the square root of the squared mindistance
if (mindist)
  mindist = sqrt(mindist);

return mindist;
}
//-------------------------------------------
      //Same as above, but only goes from proxoffset to distoffset
    //return minimum distance between a 3D point and the vessel skeleton
double VESS_s::MinDistAllowedSkel(float* src, int* loc)
{
int i;
double dist;
double mindist;

if (!pointnum) return 100000000;      //a large number

            //get distance of first point;
mindist = Distsqr(src, &pts[proxoffset]);
*loc = 0;

for (i = proxoffset; i < distoffset; i++)
  {
  dist = Distsqr(src, &pts[i * pointdim]);             //will give square of distance
  if (dist < mindist)
    {
    mindist = dist;
    *loc = i;
    }
  }

        //Get the square root of the squared mindistance
if (mindist)
  mindist = sqrt(mindist);

return mindist;
}
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //TRANSLATE 3D POINTS, MULTIPLY RAD, etc
   //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //Translate all points by some amount
void VESS_s::Translate(float a, float b, float c)
   {
   float* fptr = pts;
    int i;
   for (i = 0; i < pointnum; i++)
      {
      *fptr += a;
      *(fptr + 1) += b;
      *(fptr + 2) +=c;
      fptr += pointdim;
      }
    if(sample)
       {
       for (i = 0; i < numsamplepoints; i++)
          {
          sample[4 * i] +=a;
          sample[4 * i + 1] += b;
          sample[4 * i + 2] += c;
          }
       }
   }
   //----------------------------------
      //Multiply all points by some amount
void VESS_s::Mult(float a, float b, float c)
   {
   float* fptr = pts;
    int i;
   for (i = 0; i < pointnum; i++)
      {
      *fptr = (*fptr) * a;
      *(fptr + 1) = (*(fptr+ 1)) * b;
      *(fptr + 2) = (*(fptr+ 2)) * c;
      fptr += pointdim;
      }
    if (sample)
       {
       for (i = 0; i < numsamplepoints; i++)
          {
          sample[4 * i] *= a;
          sample[4 *i + 1] *= b;
          sample[4 * i + 2] *= c;
          }
       }
   }
//-----------------------------------
         //multiply radius by some amount
void VESS_s::Radmult(float rm)
   {
   float* fptr = pts;
    int i;

   for (i = 0; i < pointnum; i++)
      {
      *(fptr + 3) = *(fptr + 3) * rm;
      fptr += pointdim;
      }
    if(sample)
       {
       for (i = 0; i < numsamplepoints; i++)
          sample[4 * i + 3] *= rm;
       }

   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //DO A SAMPLE
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

float* VESS_s::DoSample(int nskip, int* num)
   {
    int i;
    samplestep = nskip;

   DeleteSample();

   if (pointnum == 0)
      {
      *num = 0;
        numsamplepoints = 0;
      return 0;
      }

   if(nskip < 1) nskip = 1;

         //get a count of the number of points to be used. Always include the first
         //and last
   int counter = 1;      //first point
   for (i = nskip; i < pointnum; i+=nskip)
      ++counter;

   if ((counter - 1) * nskip != pointnum - 1)      //last point
      ++counter;

   sample = new float[counter * 4];
   float* fptr = Point(0);
   sample[0] = *fptr;
   sample[1] = *(fptr + 1);
   sample[2] = *(fptr + 2);
   sample[3] = *(fptr + 3);

   int counter2 = 1;

   for (i = nskip; i < pointnum; i+=nskip)
      {
      fptr = Point(i);
      sample[counter2 * 4] = *fptr;
      sample[4 * counter2 + 1] = *(fptr + 1);
      sample[4 * counter2 + 2] = *(fptr + 2);
      sample[4 * counter2 + 3] = *(fptr + 3);
      ++counter2;
      }

   if (counter2 < counter)
      {
      fptr = Point(pointnum - 1);
      sample[4 * counter - 4] = *fptr;
      sample[4 * counter - 3] = *(fptr + 1);
      sample[4 * counter - 2] = *(fptr + 2);
      sample[4 * counter - 1] = *(fptr + 3);
      }

   *num = counter;
    numsamplepoints = counter;
   return sample;
   }
//--------------------------------------------
void VESS_s::DeleteSample()
   {
   if (sample) delete [] sample;
   sample = 0;
   numsamplepoints = 0;
   }
//--------------------------------------
      //Add the parental connection
float* VESS_s::DoConnectedSample(int nskip, int* num, float* parentpoint)
   {

   if(!parentpoint)
      return DoSample(nskip, num);
   samplestep = nskip;

    int i, index;

   DeleteSample();

   if (pointnum == 0)
      {
      *num = 0;
        numsamplepoints = 0;
      return 0;
      }

   if(nskip < 1) nskip = 1;

         //get a count of the number of points to be used. Always include the first
         //and last
   int counter = 1;      //first point
   for (i = nskip; i < pointnum; i+=nskip)
      ++counter;

   if ((counter - 1) * nskip != pointnum - 1)      //last point
      ++counter;

    ++counter;    //make room for the parental point

   float* fptr = Point(0);
    sample = new float[counter * 4];
    if(!sample)
       {numsamplepoints = 0; return 0;}


      //you need to process differently for arteries and veins. The
      //connection point goes on the end for a vein. It goes at the
      //start for an artery.
   if(!IsVein())
      {

         //parental point
      sample[0] = parentpoint[0];
      sample[1] = parentpoint[1];
      sample[2] = parentpoint[2];
      sample[3] = fptr[3];            //radius

         //first vessel point
     sample[4] = *fptr;
     sample[5] = *(fptr + 1);
     sample[6] = *(fptr + 2);
     sample[7] = *(fptr + 3);

     int counter2 = 2;

     for (i = nskip; i < pointnum; i+=nskip)
      {
      fptr = Point(i);
        index = 4 * counter2;
      sample[index] = *fptr;
      sample[index + 1] = *(fptr + 1);
      sample[index + 2] = *(fptr + 2);
      sample[index + 3] = *(fptr + 3);
      ++counter2;
      }

     if (counter2 < counter)
      {
      fptr = Point(pointnum - 1);
      sample[4 * counter - 4] = *fptr;
      sample[4 * counter - 3] = *(fptr + 1);
      sample[4 * counter - 2] = *(fptr + 2);
      sample[4 * counter - 1] = *(fptr + 3);
      }
      }

   else     //is a vein
      {
         //first vessel point
     sample[0] = *fptr;
     sample[1] = *(fptr + 1);
     sample[2] = *(fptr + 2);
     sample[3] = *(fptr + 3);

     int counter2 = 1;

     for (i = nskip; i < pointnum; i+=nskip)
      {
      fptr = Point(i);
        index = 4 * counter2;
      sample[index] = *fptr;
      sample[index + 1] = *(fptr + 1);
      sample[index + 2] = *(fptr + 2);
      sample[index + 3] = *(fptr + 3);
      ++counter2;
      }

     if (counter2 < counter - 1)
      {
        ++counter2;
      fptr = Point(pointnum - 1);
      sample[4 * counter2 - 4] = *fptr;
      sample[4 * counter2 - 3] = *(fptr + 1);
      sample[4 * counter2 - 2] = *(fptr + 2);
      sample[4 * counter2 - 1] = *(fptr + 3);
      }

            //add the connection point at the end
             //parental point
      sample[4 * counter - 4] = parentpoint[0];
      sample[4 * counter - 3] = parentpoint[1];
      sample[4 * counter - 2] = parentpoint[2];
      sample[4 * counter - 1] = fptr[3];            //radius
      }

   *num = counter;
    numsamplepoints = counter;

   return sample;
    }
//------------------------------------
      //Get a sample that goes only from proxoffset to distoffset
float* VESS_s::DoAllowedSample(int nskip, int* num)
   {
    int i;
    samplestep = nskip;


   if (proxoffset >= distoffset)
      {*num = 0;  numsamplepoints = 0; return 0;}

   if (pointnum == 0)
      {
      *num = 0;
        numsamplepoints = 0;
      return 0;
      }

   if (nskip < 1) nskip = 1;
    
    
    DeleteSample();



   //get a count of the number of points to be used. 
   int counter = 1;      //first point
   for (i = nskip + proxoffset; i < distoffset; i+=nskip)
      ++counter;

   if ( proxoffset + (counter - 1) * nskip != distoffset - 1)      //last point
      ++counter;

   sample = new float[counter * 4];
   float* fptr = Point(proxoffset);
   sample[0] = *fptr;
   sample[1] = *(fptr + 1);
   sample[2] = *(fptr + 2);
   sample[3] = *(fptr + 3);

   int counter2 = 1;

   for (i = nskip + proxoffset; i < distoffset; i+=nskip)
      {
      fptr = Point(i);
      sample[counter2 * 4] = *fptr;
      sample[4 * counter2 + 1] = *(fptr + 1);
      sample[4 * counter2 + 2] = *(fptr + 2);
      sample[4 * counter2 + 3] = *(fptr + 3);
      ++counter2;
      }

   if (counter2 < counter)
      {
      fptr = Point(distoffset - 1);
      sample[4 * counter - 4] = *fptr;
      sample[4 * counter - 3] = *(fptr + 1);
      sample[4 * counter - 2] = *(fptr + 2);
      sample[4 * counter - 1] = *(fptr + 3);
      }

   *num = counter;
    numsamplepoints = counter;


   return sample;
   }
//------------------------------------------
float* VESS_s::DoConnectedAllowedSample(int nskip, int* num, float* parentpoint)
   {
   if (!parentpoint || proxoffset > 0) return DoAllowedSample(nskip, num);
    int i, index;

   samplestep = nskip;

   if (proxoffset >= distoffset)
      {*num = 0;  numsamplepoints = 0; return 0;}

   if (pointnum == 0)
      {
      *num = 0;
        numsamplepoints = 0;
      return 0;
      }

   if (nskip < 1) nskip = 1;
    
    
    DeleteSample();


         //get a count of the number of points to be used. Always include the first
         //and last
   int counter = 1;      //first point
   for (i = nskip + proxoffset; i < distoffset; i+=nskip)
      ++counter;

   if ( proxoffset + (counter - 1) * nskip != distoffset - 1)      //last point
      ++counter;

    ++counter;    //Make room for connection point

   sample = new float[counter * 4];
   float* fptr = Point(proxoffset);

    if(!IsVein())
       {
       sample[0] = parentpoint[0];
       sample[1] = parentpoint[1];
       sample[2] = parentpoint[2];
       sample[3] = fptr[3];            //radius
      sample[4] = *fptr;
      sample[5] = *(fptr + 1);
      sample[6] = *(fptr + 2);
      sample[7] = *(fptr + 3);

      int counter2 = 2;

      for (i = nskip + proxoffset; i < distoffset; i+=nskip)
      {
      fptr = Point(i);
        index = counter2 * 4;
      sample[index] = *fptr;
      sample[index + 1] = *(fptr + 1);
      sample[index + 2] = *(fptr + 2);
      sample[index + 3] = *(fptr + 3);
      ++counter2;
      }

     if (counter2 < counter)
      {
      fptr = Point(distoffset - 1);
      sample[4 * counter - 4] = *fptr;
      sample[4 * counter - 3] = *(fptr + 1);
      sample[4 * counter - 2] = *(fptr + 2);
      sample[4 * counter - 1] = *(fptr + 3);
      }
       }

    else       //is a vein
       {
      sample[0] = *fptr;
      sample[1] = *(fptr + 1);
      sample[2] = *(fptr + 2);
      sample[3] = *(fptr + 3);

       int counter2 = 1;

       for (i = nskip + proxoffset; i < distoffset; i+=nskip)
      {
      fptr = Point(i);
        index = counter2 * 4;
      sample[index] = *fptr;
      sample[index + 1] = *(fptr + 1);
      sample[index + 2] = *(fptr + 2);
      sample[index + 3] = *(fptr + 3);
      ++counter2;
      }

     if (counter2 < counter - 1)
      {
        ++counter2;
      fptr = Point(distoffset - 1);
      sample[4 * counter2 - 4] = *fptr;
      sample[4 * counter2 - 3] = *(fptr + 1);
      sample[4 * counter2 - 2] = *(fptr + 2);
      sample[4 * counter2 - 1] = *(fptr + 3);
      }

                 //add the connection point at the end
             //parental point
      sample[4 * counter - 4] = parentpoint[0];
      sample[4 * counter - 3] = parentpoint[1];
      sample[4 * counter - 2] = parentpoint[2];
      sample[4 * counter - 1] = fptr[3];            //radius
      }



   *num = counter;
    numsamplepoints = counter;

   return sample;
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PUBLIC CHILD HANDLING
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //Add a child, inserting it into the proper order by pt
int VESS_s::AddChild(int child, int pos)
   {
   int i2[2];
   int vals[2];
   vals[0] = child;
   vals[1] = pos;

   int returnval = 0;

   if (pos < 0 || pos >= pointnum) return 0;
  if (child < 0) return 0;

        //make sure no previous reference to this child exists. This
        //gets rid of any previous position and order of the same child.
  childlist.DeleteByValue(0, child);

   childlist.Reset();
   if(!childlist.Peek(i2))      //list is empty
      {
      childlist.AddToHead(vals);
      returnval = 1;
      }


   else if(i2[1] >= pos)         //first child is farther along curve than this one
      {
      childlist.AddToHead(vals);
      returnval = 1;
      }
   else 
      {
      while (childlist.PeekNext(i2))
         {
         if (i2[1] >= pos)
            {
            childlist.Insert(vals);
            returnval = 1;
            break;
            }
         if(!childlist.Get(i2)) break;
         }
      }
   if(!returnval)
      childlist.AddToTail(vals);

   return 1;
   }
//-------------------------------------------------------
         //print childlist to stderr. Debugging purposes
void VESS_s::ShowChild()
   {
   int i[2];
   childlist.Reset();
   if(!childlist.Get(i))
      std::cerr<<"Empty list";
   else
      {
      std::cerr<<"["<<i[0]<<", "<<i[1]<<"]  ";
      while (childlist.Get(i))
         std::cerr<<"["<<i[0]<<", "<<i[1]<<"]  ";
      }
   std::cerr<<std::endl;
   }
   
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //LOADS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



int VESS_s::Load(std::istream* ifile)
   {
   if(!ifile) return 0;
   this->Releasemem();

   if(!OBJECTIO_s::LoadHeader(ifile))
      {
      std::cerr<<"\nAn individual vessel could not load its header\n";
      return(0);
      }

               //you should have loaded all mandatory and optional fields
   return LoadPoints(ifile);
   }
//------------------------------------------------------------------------------
int VESS_s::Save(std::ofstream* ofile)
   {

   if(!*ofile) return 0;

   int i, j;
   float* fptr;

   *ofile<<std::endl;
   *ofile<<"ID: "<<id<<std::endl;

   *ofile<<"Type: Tube\n";

   *ofile<<"Anat: ";
   if (!anat.empty() ) *ofile<<anat;
   else *ofile<<"Artery";
   *ofile<<std::endl;

   *ofile<<"TreeType: ";
   if (vparent > -1)
      *ofile<<"child";
   else if (!treetype.empty() )
      *ofile<<treetype;
   else
      *ofile<<"orphan";
   *ofile<<std::endl;

   if (vparent > - 1)
      *ofile<<"VParent: "<<vparent<<std::endl;

   if (attachpt > -1)
      *ofile<<"Attachpt: "<<attachpt<<std::endl;

   *ofile<<"Color: "<<col[0]<<"  "<<col[1]<<"  "<<col[2]<<"  "<<col[3]<<std::endl;

   if (!name.empty() )
      *ofile<<"Name: "<<name<<std::endl;

   *ofile<<"PointDim: "<<pointdim<<"  ";
   for(i = 0; i < pointdim; i++)  *ofile<<dimdesc.at(i)<<"  ";
   *ofile<<std::endl;


   *ofile<<"NPoints: "<<distoffset - proxoffset<<std::endl;

   *ofile<<"Points:"<<std::endl;
   for (i = proxoffset; i < distoffset; i++)
      {
      fptr = Point(i);
      for (j = 0; j < pointdim; j++)
         *ofile<<fptr[j]<<"  ";
      *ofile<<std::endl;
      }
   return 1;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //REVERSE A VESSEL AND ITS CHILDLIST
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //note that this only affects the current vessel
         //An external function must change the connectpt vals of each child
void VESS_s::Reverse()
   {
   float* floatptr = new float[pointdim];
   int i, j, valfar, valnear;

               //reverse the points
   for (i = 0; i < pointnum/2; i++)
      {
      valfar = pointdim * (pointnum - 1 - i);
      valnear = pointdim * i;

               //copy near points into floatptr and put far points in near
               //and copied floatptr pts into far
      for (j = 0; j < pointdim; j++)
         {
         floatptr[j] = pts[valnear + j];
         pts[valnear + j] = pts[valfar + j];
         pts[valfar + j] = floatptr[j];
         }
      }

            //reverse child order
   childlist.Reset();
   int twoi[2];
   ARLIST <int>templist;
   templist.SetDims(2);
   while (childlist.Get(twoi))
      templist.AddToHead(twoi);

   childlist.Releasemem();
   templist.Reset();
   while (templist.Get(twoi))
      {
      twoi[1] = pointnum - 1 - twoi[1];      //adjust loc for reverse
      childlist.AddToTail(twoi);
      }
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PROTECTED LOAD FUNCTIONS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int VESS_s::LoadPoints(std::istream* ifile)
   {
   int rval =  OBJECTIO_s::LoadPoints(ifile);
   
    proxoffset = 0;
    distoffset = pointnum;
    return rval;
   }

}//end of namespace