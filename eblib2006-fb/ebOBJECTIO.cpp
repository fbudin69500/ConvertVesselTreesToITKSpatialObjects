#include "ebOBJECTIO.h"


#include "ebTBOX.h"            //string handling tools; module from eblib

namespace eb
{

OBJECTIO::OBJECTIO()
   {
   id = -1;                   //no valid id
   type = new char[5];        //default type is tube
      strcpy(type, "tube");   
   anat = 0;                  //default no anatomical description
   treetype = 0;              //default no treetype;

   vparent = -1;               //default is no parent
   attachpt = -1;               //no parent, so no parental attachment pt
   name = 0;                  //requested field for name for Joe, comment for Stephen
   pointnum = 0;               //number of points; (x,y,z,r...) is one pt            
   pointdim = 4;               //number of floats per point = default
   pts = 0;                      //fixed field of pointdim points
   dimdesc = 0;                   //description of fixed field points (x,y,z,r1,r2)

   col[0] = col[3] = (float)1; col[1] = (float)0.3; col[2] = (float)0.3;

   limitpointdims = 4;              //default is max 4D points
   }
//------------------------------------------------------------------------
OBJECTIO::OBJECTIO(const OBJECTIO& a)
   {
   int i;

   try
      {
      id = a.id;
      if (a.type)
         {
         type = new char[strlen(a.type) + 1]; 
         strcpy(type, a.type);
         }
      else
         {type = new char[5]; strcpy(type, "tube");}

      if (a.anat)
         {
         anat = new char[strlen(a.anat) + 1];
         strcpy(anat, a.anat);
         }
      else anat = 0;

      if(a.treetype)
         {
         treetype = new char[strlen(a.treetype) + 1];
         strcpy(treetype, a.treetype );
         }
      else treetype = 0;

      if (a.name)
         {
         name = new char[strlen(a.name) + 1];
         strcpy(name, a.name);
         }
      else
         name = 0;

      pointnum = a.pointnum;
      pointdim = a.pointdim;

      if (pointnum)
         {
         pts = new float[pointnum * pointdim];
      memcpy(pts, a.pts, pointnum * pointdim * sizeof(float));
         }
      else pts = 0;


      dimdesc = new char*[pointdim];
      for (i = 0; i < pointdim; i++)
         {
         dimdesc[i] = new char[strlen(a.dimdesc[i]) + 1];
         strcpy(dimdesc[i], a. dimdesc[i]);
         }
      }
   catch(...) {std::cout<<"Memory failure in OJECTIO\n"; pointnum = 0; pts = 0;}

   for (i = 0; i < 4; i++)
      col[i] = a.col[i];



   limitpointdims = a.limitpointdims;
   vparent = a.vparent;
   attachpt = a.attachpt;
   }
//--------------------------------------------------------------
OBJECTIO& OBJECTIO::operator = (const OBJECTIO& a)
   {
   Releasemem();
 int i;

   try
      {
      id = a.id;
      if (a.type)
         {
         type = new char[strlen(a.type) + 1]; 
         strcpy(type, a.type);
         }
      else
         {type = new char[5]; strcpy(type,"tube");}

      if (a.anat)
         {
         anat = new char[strlen(a.anat) + 1];
         strcpy(anat, a.anat);
         }
      else anat = 0;

      if(a.treetype)
         {
         treetype = new char[strlen(a.treetype) + 1];
         strcpy(treetype, a.treetype);
         }
      else treetype = 0;

         if (a.name)
         {
         name = new char[strlen(a.name) + 1];
         strcpy(name, a.name);
         }
      else
         name = 0;

      pointnum = a.pointnum;
      pointdim = a.pointdim;

      if (pointnum)
         {
         pts = new float[pointnum * pointdim];
         memcpy(pts, a.pts, pointnum * pointdim * sizeof(float));
         }
      else pts = 0;


      dimdesc = new char*[pointdim];
      for (i = 0; i < pointdim; i++)
       {
         dimdesc[i] = new char[strlen(a.dimdesc[i]) + 1];
         strcpy(dimdesc[i], a. dimdesc[i]);
         }
      }
   catch(...)
      {
      std::cout<<"Memory failure in objectio::assignment\n";
      pointnum = 0; pts = 0;
      return *this;
      }

   for (i = 0; i < 4; i++)
      col[i] = a.col[i];



   limitpointdims = a.limitpointdims;
   vparent = a.vparent;
   attachpt = a.attachpt;
   return *this;
   }
//--------------------------------------------------------------
void OBJECTIO::Releasemem()
   {
   if (type) {delete [] type; type = 0;}
   if (anat) {delete [] anat; anat = 0;}
   if (treetype) {delete [] treetype; treetype = 0;}

   vparent = -1;
   attachpt = -1;

   if (name) {delete [] name; name = 0;}

   if (pts) {delete [] pts; pts = 0;}
   pointnum = 0;

   if (dimdesc)
      {
      for (int i = 0; i < pointdim; i++)
         delete [] dimdesc[i];
      delete [] dimdesc;
      dimdesc = 0;
      }
   }
   //---------------------------------------------------------------
OBJECTIO& OBJECTIO::Copy(const OBJECTIO& a)
   {
   if (this == &a) return *this;
   int i;
   Releasemem();

   try
      {
      id = a.id;
      if (a.type)
         {
         type = new char[strlen(a.type) + 1]; 
         strcpy(type, a.type);
         }
      else
         {type = new char[5]; strcpy(type, "tube");}


      if(a.anat)
         {
         anat = new char[strlen(a.anat) + 1]; 
         strcpy(anat, a.anat);
         }
      else anat = 0;

      vparent = a.vparent;
      attachpt = a.attachpt;
  
      if(a.treetype)
         {
         treetype = new char[strlen(a.treetype) + 1];
         strcpy(treetype, a.treetype);
         }
      else treetype = 0;

   
      if (a.name)
         {
         name = new char[strlen(a.name) + 1];
         strcpy(name, a.name);
         }
      else
         name = 0;

      pointnum = a.pointnum;
      pointdim = a.pointdim;

      pts = new float[pointnum * pointdim];
      memcpy(pts, a.pts, pointnum * pointdim * sizeof(float));

      dimdesc = new char*[pointdim];
      for (i = 0; i < pointdim; i++)
         {
         dimdesc[i] = new char[strlen(a.dimdesc[i]) + 1];
         strcpy(dimdesc[i], a. dimdesc[i]);
         }

      for (i = 0; i < 4; i++)
         col[i] = a.col[i];

      limitpointdims = a.limitpointdims;
      }
   catch(...)
      {
      std::cerr<<"Memory error in objectio::Copy\n"; 
      pointnum = 0; pts = 0; 
      return * this;
      }

   return *this;
   }
//--------------------------------------------
OBJECTIO::~OBJECTIO()
   {Releasemem();}
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //DETERMINE SOME OF TEXT CHARACTERISTICS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int OBJECTIO::IsRoot()
   {
   char s[100];
   TBOX tbox;
   if (!treetype) return 0;

   strcpy(s, treetype);
   tbox.Lowercase(s);
   if (!strcmp(s, "root")) return 1;
   return 0;
   }
//-----------------------------------
int OBJECTIO::IsOrphan()
   {
   char s[100];
   TBOX tbox;
   if (!treetype) return 1;      //default is to be an orphan

   strcpy(s, treetype);
   tbox.Lowercase(s);
   if (!strcmp(s, "orphan")) return 1;
   return 0;
   }
//----------------------------
int OBJECTIO::IsArtery()
   {
   char s[100];
   TBOX tbox;
   if (!anat) return 0;

   strcpy(s, anat);
   tbox.Lowercase(s);
   if (!strcmp(s, "artery")) return 1;
   return 0;
   }
//------------------------------------------------
int OBJECTIO::IsVein()
   {
   char s[100];
   TBOX tbox;
   if (!anat) return 0;

   strcpy(s, anat);
   tbox.Lowercase(s);
   if (!strcmp(s, "vein")) return 1;
   return 0;
   }
//---------------------------
               //set Anat
void OBJECTIO::Anat(char* a)
   {
   if (anat) delete [] anat;
   if(a)
      {
      anat = new char[strlen(a) + 1];
      strcpy(anat, a);
      }
   else anat = NULL;
   }
//----------------------------
            //set treetype
void OBJECTIO::TreeType(char* a)
   {
   if (treetype) delete [] treetype;
   if(a)
      {
      treetype = new char[strlen(a) + 1];
      strcpy(treetype, a);
      }
   else treetype = NULL;
   }
//--------------------------------
            //set type
void OBJECTIO::Type(char* a)
   {
   if (type) delete [] type;
   if(a)
      {
      type = new char[strlen(a) + 1];
      strcpy(type, a);
      }
   else type = NULL;
   }
//--------------------------------
         //set name
void OBJECTIO::Name(char* a)
   {
   if (name) delete [] name;
   if (a)
      {
      name = new char[strlen(a) + 1];
      strcpy(name, a);
      }
   else name = NULL;
   }

//--------------------------------------------------------
         //return a description of a fixed point field
char* OBJECTIO::DimDesc(int a)
   {
   if (a < 0 || a >= pointdim) return NULL;
   return dimdesc[a];
   }
//---------------------------------------------------------
      //set a description of a fixed point field
int OBJECTIO::DimDesc(int a, char* s)
   {
   if (a < 0 || a >= pointdim) return 0;
   if (!s) return 0;
   if (!strlen(s)) return 0;

   if (dimdesc && dimdesc[a])
      delete [] dimdesc[a];

   dimdesc[a] = new char[strlen(s) + 1];
   strcpy(dimdesc[a], s);

   return 1;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PUBLIC FUNCTIONS TO GET AND SET POINT FIELDS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

         //allocate mem for a new point field
int OBJECTIO::Realloc(int pn, int pd)
   {
   int i;
   if (pn <= 0 || pd <= 0) return 0;

   if (pts) delete [] pts;
   pts = NULL;

  DeleteDimDesc();

   pointnum = pn;
   pointdim = pd;
    if (pd > limitpointdims) limitpointdims = pd;

    try
       {
      pts = new float[pn * pd];
      if(!pts) {std::cerr<<"No mem for reallocated points in OBJECTIO\n"; pointnum = 0; return 0;}

      dimdesc = new char* [pd];

      for (i = 0; i < pointdim; i++)
         {
         dimdesc[i] = new char[2];            //prevent coredump if exit before set
         *(dimdesc[i]) = '\0';
         }
       }
    catch(...)
       {std::cerr<<"No mem for reallocated points in OBJECTIO\n"; pointnum = 0; return 0;}

   return 1;
   }
//------------------------------------------------

         //Set an individual fixed length point. It is assumed p has the proper dim
         //and that mem has been allocated for pts.
int OBJECTIO::Point(int pn, float* p)
   {
   if (pn < 0 || pn >= pointnum || !p) return 0;

   float* f;
   int i;

   for (i = 0; i < pointdim; i++)
      {
      f = pts + (pointdim * pn) + i;      
      *f = p[i];  
      }
   return 1;
   }
//-----------------------------------------------------------
      //Realloc and set new points
int OBJECTIO::Points(int pn, int pd, float* p)
   {
   if (!p) return 0;

   if(!Realloc(pn, pd)) return 0;

   memcpy(pts, p, pn * pd * sizeof(float));
   return 1;
   }
//--------------------------------------------------------
      //put a pointer on the beginning of a fixed point field for get.
      //"a' refers to the line number or point number, with each point
      //consisting of a fixed number of floats. 
float* OBJECTIO::Point(int a)
   {
   if (a < 0 || a >= pointnum) return NULL;
   return &pts[a * pointdim];
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //CONVERT BETWEEN WORLD AND VOXEL COORDS. AFFECTS RAD ALSO
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //convert to world coords; no y inversion or offset
int OBJECTIO::VoxToWorld(double xs, double ys, double zs)
   {
   if (pointdim < 4)
      {std::cerr<<"No conversion for pointdims < 4\n"; return 0;}
   int i, index;

   
   for (i = 0; i < pointnum; i++)
      {
      index = pointdim * i;
      pts[index] = (float)(pts[index] * xs);                  //x val
      pts[index + 1] = (float)(pts[index + 1] * ys);               //y val
      pts[index + 2] = (float)(pts[index + 2] * zs);              //z val
      pts[index + 3] = (float)(pts[index + 3] * xs);            //rad
      }

   return 1;
   }
//=-----------------------------------------------------
            //convert from world to vox
int OBJECTIO::WorldToVox(double xs, double ys, double zs)
   {
   if (pointdim < 4)
      {std::cerr<<"No conversion for pointdims < 4\n"; return 0;}
   int i, index;

   
   if (xs <= 0 || ys <= 0 || zs <= 0) return 0; //divide error if 0

   for (i = 0; i < pointnum; i++)
      {
      index = pointdim * i;
      pts[index] = (float)(pts[index]/xs);                  //x val
      pts[index + 1] = (float)(pts[index + 1]/ys);   //y val
      pts[index + 2] = (float)(pts[index + 2]/zs);      //z val
      pts[index + 3] = (float)(pts[index + 3]/ xs);            //rad
      }

   return 1;
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PROTECTED: GENERAL FUNCTIONS USED DURING LOADS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

void OBJECTIO::DeleteDimDesc()
   {
   int i;
   if (dimdesc)
      {
      for (i = pointdim - 1; i >=0; i--)
         delete [] dimdesc[i];
      delete [] dimdesc;
      dimdesc = NULL;
      }
   }
//-------------------------------------------------------------------------
      //These load functions are separated so that a derived class can do some other things
      //between header load and Point load
int OBJECTIO::LoadHeader(std::ifstream* ifile)
   {

    char readline[500];
   TBOX tbox;
   int i;
   unsigned int offset;
   char* cptr;

   if(!*ifile) { std::cerr<<"File seemed to end prematurely. Load failed.\n"; return 0;}
   Releasemem();


   while(*ifile)
      {
      ifile->getline(readline, 299);
      if (!tbox.Lowercase(readline)) continue;

   
            //the point field is not in the header. 
     if(!strncmp(readline, "points", 6))
         break;

                     //look for values in the object header
      if (!strncmp(readline, "id:", 3))
         {
         cptr = readline + 3;
         id = atoi(cptr);  
         continue;
         }

      if(!strncmp(readline, "type:", 5))
         {
            offset = 0;
         cptr = readline + 6;
         tbox.Strip(cptr, &offset);
         cptr += offset;
         if (tbox.GetWord(cptr, &offset))
            {
            if (type) delete [] type;
            type = new char[offset + 1];
            for (i = 0; i < (int)offset; i++)
                type[i] = cptr[i];
            type[offset] = '\0';
            }
         continue;
         }

        if(!strncmp(readline, "treetype:", 9))
         {
            offset = 0;
         cptr = readline + 10;
         tbox.Strip(cptr, &offset);
         cptr += offset;

         if (tbox.GetWord(cptr, &offset))
            {
            if (treetype) delete [] treetype;
            treetype = new char[offset + 1];
            for (i = 0; i < (int)offset; i++)
                treetype[i] = cptr[i];
            treetype[offset] = '\0';
            }
            
         continue;
         }

      if(!strncmp(readline, "anat:", 4))
         {
            offset = 0;
         cptr = readline + 5;
         tbox.Strip(cptr, &offset);
         cptr += offset;
         if (tbox.GetWord(cptr, &offset))
            {
            if (anat) delete [] anat;
            anat= new char[offset + 1];
            for (i = 0; i < (int)offset; i++)
                anat[i] = cptr[i];
            anat[offset] = '\0';
            }
         continue;
         }

   
      if(!strncmp(readline, "vparent:", 7))
         {
         cptr = readline + 8;
         vparent = atoi(cptr);
         continue;
         }

      if(!strncmp(readline, "attachpt:", 8))
         {
         cptr = readline + 9;
         attachpt = atoi(cptr);
         continue;
         }

      if(!strncmp(readline, "color:", 6))
         {
         cptr = readline + 6;
            tbox.Stringtonfloats(cptr, 4, col);   ;
         continue;
         }

      if(!strncmp(readline, "name:", 4))
         {
            offset = 0;
         cptr = readline + 5;
         tbox.Strip(cptr, &offset);
         cptr += offset;
         if(strlen(cptr))
            {
            if (name) delete [] name;
            name = new char[strlen(cptr) + 1];
            strcpy(name, cptr);
            }
         continue;
         }

               
            //pointdim line may contain optional description of point field
            //you must delete dimdesc before changing pointdim and reallocating
      if(!strncmp(readline, "pointdim:", 8))
         {
         DeleteDimDesc();
         cptr = readline + 9;
         pointdim = atoi(cptr);
         if (pointdim <= 0) continue;
         if(limitpointdims > 0 && limitpointdims < pointdim)
               pointdim = limitpointdims;
         dimdesc = new char* [pointdim];
         for (i = 0; i < pointdim; i++)      //prevent coredump if deletion of **
            {
            dimdesc[i]=0;
            //dimdesc[i] = new char[2];
            //*dimdesc[i] = '\0';
            }

                  //advance pointer beyond the int given by pointdim
         cptr = readline + 9;
         offset = 0;

         tbox.Strip(cptr, &offset);
         cptr += offset;
         if(!tbox.GetWord(cptr, &offset)) continue;
         cptr +=offset;      

                  //look for optional field of description of pointdim
         for (i = 0; i < pointdim; i++)
            {
            offset = 0;
            tbox.Strip(cptr, &offset);
            cptr += offset;
            if (tbox.GetWord(cptr, &offset))
               { 
               delete [] dimdesc[i];
               dimdesc[i] = new char[offset + 1];
               for(int j = 0; j < (int)offset; j++)
                  *(dimdesc[i] + j) = *(cptr + j);
               cptr += offset + 1;
               *(dimdesc[i] + offset) = '\0';
               }
            else
               {
               dimdesc[i] = new char[2];
               *(dimdesc[i]) = '\0';
               }
            }

         continue;
         }

      if(!strncmp(readline, "npoints:", 8))
         {
         cptr = readline + 9;
         pointnum = atoi(cptr);
         continue;
         }
      }      //end while ifile
      if(pointnum <= 0 || pointdim <= 0) {std::cerr<<"Invalid # points "<<pointnum<<"  or dims "<<pointdim<<"\n"; return 0;}
    return 1;
    }

//----------------------------------------------------------
int OBJECTIO::LoadPoints(std::ifstream* ifile)
   {
   if(!ifile)
      {
      std::cerr<<"Load points failed--file is not valid\n";
      return 0;
      }

   if(pointdim > limitpointdims && limitpointdims > 0)
      pointdim = limitpointdims;

   try
      {
      pts = new float[pointnum * pointdim];
      if (!pts)
         {
         std::cerr<<"Out of mem for vessel points\n";
         pointnum = 0;
         return 0;
         }
      }
   catch(...)
      {
      std::cerr<<"Out of mem for vessel points\n";
      pointnum = 0;
      pts = 0;
      return 0;
      }

   int i = 0;
   int counter;
   char readline[500];
   TBOX tbox;

   while (*ifile)
      {
      if (i >= pointnum) break;
      ifile->getline(readline, 499);
      if (!strlen(readline)) break;

               //get fixed field.
      for (counter = 0; counter < pointdim; counter++)
         {
         if (!tbox.Stringtonfloats(readline, pointdim, pts + (pointdim * i)))
            break;
         }
      ++i;
      }

   if (i != pointnum)
      {
      std::cerr<<"\n\nExpected number of lines "<<pointnum<<"  lines read "<<i<<std::endl;
      return 0;
      }

   return 1;
   }
//--------------------------------------------------
int OBJECTIO::SaveHeader(std::ofstream* ofile)
   {
   if (!ofile) return 0;
   int i, usedim;

   *ofile<<std::endl;
   if(id > -1)
      *ofile<<"ID: "<<id<<std::endl;

   if (type)
   *ofile<<"Type: "<<type<<std::endl;

   if(anat)
      *ofile<<"Anat: "<<anat<<std::endl;

   if (treetype)
      *ofile<<"TreeType: "<<treetype<<std::endl;

   if (vparent > - 1)
      *ofile<<"VParent: "<<vparent<<std::endl;

   if (attachpt > -1)
      *ofile<<"Attachpt: "<<attachpt<<std::endl;

   *ofile<<"Color: "<<col[0]<<"  "<<col[1]<<"  "<<col[2]<<"  "<<col[3]<<std::endl;

   if (name)
      *ofile<<"Name: "<<name<<std::endl;

   usedim = pointdim;
   if (limitpointdims < usedim && limitpointdims > 0)
      usedim = limitpointdims;
   *ofile<<"PointDim: "<<usedim<<"  ";
   for(i = 0; i < usedim; i++)  *ofile<<dimdesc[i]<<"  ";
   *ofile<<std::endl;

   *ofile<<"NPoints: "<<pointnum<<std::endl;

   return 1;
   }

//-------------------------------------------------
int OBJECTIO::SavePoints(std::ofstream* ofile)
   {
   if (!ofile) return 0;
   int i, j;
   float* fptr;

   *ofile<<"Points: "<<std::endl;

   int usedims = pointdim;
   if (limitpointdims < usedims && limitpointdims > 0)
      usedims = limitpointdims;

    for (i = 0; i < pointnum; i++)
      {
      fptr = Point(i);
      for (j = 0; j < pointdim; j++)
         *ofile<<fptr[j]<<"  ";
      *ofile<<std::endl;
      }
   return 1;
   }





}//end of namespace

