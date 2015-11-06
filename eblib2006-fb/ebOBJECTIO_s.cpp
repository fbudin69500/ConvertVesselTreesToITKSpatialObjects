#include "ebOBJECTIO_s.h"


namespace eb
{

OBJECTIO_s::OBJECTIO_s()
   {
   id = -1;                   //no valid id
   //type = new char[5];        //default type is tube
      //strcpy_s(type, 5,"tube");   
   type = "tube" ;
   //anat = 0;                  //default no anatomical description
   //treetype = 0;              //default no treetype;

   vparent = -1;               //default is no parent
   attachpt = -1;               //no parent, so no parental attachment pt
   //name = 0;                  //requested field for name for Joe, comment for Stephen
   pointnum = 0;               //number of points; (x,y,z,r...) is one pt            
   pointdim = 4;               //number of floats per point = default
   pts = 0;                      //fixed field of pointdim points
   //dimdesc = 0;                   //description of fixed field points (x,y,z,r1,r2)

   col[0] = col[3] = (float)1; col[1] = (float)0.3; col[2] = (float)0.3;

   limitpointdims = 4;              //default is max 4D points
   }
//------------------------------------------------------------------------
OBJECTIO_s::OBJECTIO_s(const OBJECTIO_s& a)
   {
   int i;

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
         type = "tube" ;
         //type = new char[5];
         //strcpy_s(type, 5, "tube");
         }

      if ( !a.anat.empty() )
         {
         anat = a.anat ;
         //anat = new char[strlen(a.anat) + 1];
         //strcpy_s(anat, strlen(a.anat) + 1,a.anat);
         }
      else 
         {
         //anat = 0 ;
         anat = "" ;
         }

      if( !a.treetype.empty() )
         {
         treetype = a.treetype ;
         //treetype = new char[strlen(a.treetype) + 1];
         //strcpy_s(treetype, strlen(a.treetype) + 1,a.treetype);
         }
      else 
         {
         //treetype = 0;
         treetype = "" ;
         }

      if ( !a.name.empty() )
         {
         name = a.name ;
         //name = new char[strlen(a.name) + 1];
         //strcpy_s(name, strlen(a.name) + 1,a.name);
         }
      else
         {
         //name = 0;
         name = "" ;
         }

      pointnum = a.pointnum;
      pointdim = a.pointdim;

      if (pointnum)
         {
         pts = new float[pointnum * pointdim];
      memcpy(pts, a.pts, pointnum * pointdim * sizeof(float));
         }
      else pts = 0;


      //dimdesc = new char*[pointdim];
      dimdesc = a.dimdesc ;
      /*dimdesc.resize( pointdim ) ;
      for (i = 0; i < pointdim; i++)
         {
         dimdesc.at(i) = a.dimdesc.at(i) ;
         //dimdesc[i] = new char[strlen(a.dimdesc[i]) + 1];
         //strcpy_s(dimdesc[i], strlen(a.dimdesc[i]) + 1, a. dimdesc[i]);
         }*/
      }
   catch(...) {std::cout<<"Memory failure in OJECTIO\n"; pointnum = 0; pts = 0;}

   for (i = 0; i < 4; i++)
      col[i] = a.col[i];



   limitpointdims = a.limitpointdims;
   vparent = a.vparent;
   attachpt = a.attachpt;
   }
//--------------------------------------------------------------
OBJECTIO_s& OBJECTIO_s::operator = (const OBJECTIO_s& a)
   {
   Releasemem();
 int i;

   try
      {
      id = a.id;
      if ( !a.type.empty() )
         {
         type = a.type ;
         //type = new char[strlen(a.type) + 1]; 
         //strcpy_s(type, strlen(a.type) + 1, a.type);
         }
      else
         {
         type = "tube" ;
         //type = new char[5];
         //strcpy_s(type, 5, "tube");
         }

      if ( !a.anat.empty() )
         {
         anat = a.anat ;
         //anat = new char[strlen(a.anat) + 1];
         //strcpy_s(anat, strlen(a.anat) + 1, a.anat);
         }
      else 
         {
         //anat = 0;
         anat = "" ;
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
         treetype = "";
         }

      if ( !a.name.empty() )
         {
         name = a.name ;
         //name = new char[strlen(a.name) + 1];
         //strcpy_s(name, strlen(a.name) + 1, a.name);
         }
      else
         {
         name = "" ;
         //name = 0;
         }

      pointnum = a.pointnum;
      pointdim = a.pointdim;

      if (pointnum)
         {
         pts = new float[pointnum * pointdim];
         memcpy(pts, a.pts, pointnum * pointdim * sizeof(float));
         }
      else pts = 0;


      //dimdesc = new char*[pointdim];
      dimdesc = a.dimdesc ;
      /*for (i = 0; i < pointdim; i++)
       {
         dimdesc
         dimdesc[i] = new char[strlen(a.dimdesc[i]) + 1];
         strcpy_s(dimdesc[i], strlen(a.dimdesc[i]) + 1, a. dimdesc[i]);
         }*/
      }
   catch(...)
      {
      std::cout<<"Memory failure in OBJECTIO_s::assignment\n";
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
void OBJECTIO_s::Releasemem()
   {
   type.clear() ;
   anat.clear() ;
   treetype.clear() ;
   //if (type) {delete [] type; type = 0;}
   //if (anat) {delete [] anat; anat = 0;}
   //if (treetype) {delete [] treetype; treetype = 0;}

   vparent = -1;
   attachpt = -1;
   name.clear() ;
   //if (name) {delete [] name; name = 0;}

   if (pts) {delete [] pts; pts = 0;}
   pointnum = 0;
   dimdesc.clear() ;
   /*if (dimdesc)
      {
      for (int i = 0; i < pointdim; i++)
         delete [] dimdesc[i];
      delete [] dimdesc;
      dimdesc = 0;
      }*/
   }
   //---------------------------------------------------------------
OBJECTIO_s& OBJECTIO_s::Copy(const OBJECTIO_s& a)
   {
   if (this == &a) return *this;
   int i;
   Releasemem();

   try
      {
      id = a.id;
      if ( !a.type.empty() )
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


      if( !a.anat.empty() )
         {
         //anat = new char[strlen(a.anat) + 1]; 
         //strcpy_s(anat, strlen(a.anat) + 1, a.anat);
         anat = a.anat ;
         }
      else 
         {
         //anat = 0;
         anat = "" ;
         }

      vparent = a.vparent;
      attachpt = a.attachpt;
  
      if( !a.treetype.empty() )
         {
         //treetype = new char[strlen(a.treetype) + 1];
         //strcpy_s(treetype, strlen(a.treetype) + 1, a.treetype);
         treetype = a.treetype ;
         }
      else
         {
         //treetype = 0;
         treetype = "" ;
         }

   
      if ( !a.name.empty() )
         {
         name = a.name ;
         //name = new char[strlen(a.name) + 1];
         //strcpy_s(name, strlen(a.name) + 1, a.name);
         }
      else
         {
         name = "";
         //name = 0;
         }

      pointnum = a.pointnum;
      pointdim = a.pointdim;

      pts = new float[pointnum * pointdim];
      memcpy(pts, a.pts, pointnum * pointdim * sizeof(float));
      dimdesc = a.dimdesc ;
      /*dimdesc = new char*[pointdim];
      for (i = 0; i < pointdim; i++)
         {
         dimdesc[i] = new char[strlen(a.dimdesc[i]) + 1];
         strcpy_s(dimdesc[i], strlen(a.dimdesc[i]) + 1, a. dimdesc[i]);
         }*/

      for (i = 0; i < 4; i++)
         col[i] = a.col[i];

      limitpointdims = a.limitpointdims;
      }
   catch(...)
      {
      std::cerr<<"Memory error in OBJECTIO_s::Copy\n"; 
      pointnum = 0; pts = 0; 
      return * this;
      }

   return *this;
   }
//--------------------------------------------
OBJECTIO_s::~OBJECTIO_s()
   {Releasemem();}
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //DETERMINE SOME OF TEXT CHARACTERISTICS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int OBJECTIO_s::IsRoot()
   {
   //char s[100];
   std::string s;
   //TBOX tbox;
   TBOX_s tbox;
   if ( treetype.empty() ) return 0;

   //strcpy_s(s, 100, treetype);
   s = treetype;
   tbox.Lowercase(s);
   //if (!strcmp(s, "root")) return 1;
   if ( s == "root" ) return 1;
   return 0;
   }
//-----------------------------------
int OBJECTIO_s::IsOrphan()
   {
   //char s[100];
   std::string s ;
   //TBOX tbox;
   TBOX_s tbox;
   //if (!treetype) return 1;      //default is to be an orphan
   if ( !treetype.empty() ) return 1;      //default is to be an orphan

   //strcpy_s(s, 100, treetype);
   s = treetype ;
   tbox.Lowercase(s);
   //if (!strcmp(s, "orphan")) return 1;
   if ( s == "orphan" ) return 1;
   return 0;
   }
//----------------------------
int OBJECTIO_s::IsArtery()
   {
   //char s[100];
   std::string s;
   //TBOX tbox;
   TBOX_s tbox;
   //if (!anat) return 0;
   if ( anat.empty() ) return 0;

   //strcpy_s(s, 100, anat);
   s = anat ;
   tbox.Lowercase(s);
   //if (!strcmp(s, "artery")) return 1;
   if ( s == "artery" ) return 1;
   return 0;
   }
//------------------------------------------------
int OBJECTIO_s::IsVein()
   {
   //char s[100];
   std::string s;
   //TBOX tbox;
   TBOX_s tbox;
   //if (!anat) return 0;
   if ( anat.empty() ) return 0;

   //strcpy_s(s, 100, anat);
   s = anat;
   tbox.Lowercase(s);
   //if (!strcmp(s, "vein")) return 1;
   if( s == "vein" ) return 1;
   return 0;
   }
//---------------------------
               //set Anat
void OBJECTIO_s::Anat(std::string a)
   {
   //if (anat) delete [] anat;
   if( !a.empty() )
      {
      anat = a;
      //anat = new char[strlen(a) + 1];
      //strcpy_s(anat, strlen(a) + 1, a);
      }
   //else anat = NULL;
   else anat = "";
   }
//----------------------------
            //set treetype
void OBJECTIO_s::TreeType(std::string a)
   {
   //if (treetype) delete [] treetype;
   if( !a.empty() )
      {
      treetype = a ;
      //treetype = new char[strlen(a) + 1];
      //strcpy_s(treetype, strlen(a) + 1, a);
      }
   else treetype = "" ;
   }
//--------------------------------
            //set type
void OBJECTIO_s::Type(std::string a)
   {
   //if (type) delete [] type;
   if( !a.empty() )
      {
      type = a ;
      //type = new char[strlen(a) + 1];
      //strcpy_s(type, strlen(a) + 1, a);
      }
   else type = "" ;
   }
//--------------------------------
         //set name
void OBJECTIO_s::Name(std::string a)
   {
   //if (name) delete [] name;
   if ( !a.empty() )
      {
      name = a ;
      //name = new char[strlen(a) + 1];
      //strcpy_s(name, strlen(a) + 1, a);
      }
   else name = "";
   }

//--------------------------------------------------------
         //return a description of a fixed point field
std::string OBJECTIO_s::DimDesc(int a)
   {
   if (a < 0 || a >= pointdim) return "";
   return dimdesc.at(a);
   }
//---------------------------------------------------------
      //set a description of a fixed point field
int OBJECTIO_s::DimDesc(int a, std::string s)
   {
   if (a < 0 || a >= pointdim) return 0;
   if ( s.empty() ) return 0;

   dimdesc.at(a) = s ;

   //dimdesc[a] = new char[strlen(s) + 1];
   //strcpy_s(dimdesc[a], strlen(s) + 1, s);

   return 1;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PUBLIC FUNCTIONS TO GET AND SET POINT FIELDS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

         //allocate mem for a new point field
int OBJECTIO_s::Realloc(int pn, int pd)
   {
//   int i;
   if (pn <= 0 || pd <= 0) return 0;

   if (pts) delete [] pts;
   pts = NULL;

  //DeleteDimDesc();

   pointnum = pn;
   pointdim = pd;
    if (pd > limitpointdims) limitpointdims = pd;

    try
       {
      pts = new float[pn * pd];
      if(!pts) {std::cerr<<"No mem for reallocated points in OBJECTIO_s\n"; pointnum = 0; return 0;}

      //dimdesc = new char* [pd];
      dimdesc.clear() ;
      dimdesc.resize( pd ) ;
      /*for (i = 0; i < pointdim; i++)
         {
         dimdesc[i] = new char[2];            //prevent coredump if exit before set
         *(dimdesc[i]) = '\0';
         }*/
       }
    catch(...)
       {std::cerr<<"No mem for reallocated points in OBJECTIO_s\n"; pointnum = 0; return 0;}

   return 1;
   }
//------------------------------------------------

         //Set an individual fixed length point. It is assumed p has the proper dim
         //and that mem has been allocated for pts.
int OBJECTIO_s::Point(int pn, float* p)
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
int OBJECTIO_s::Points(int pn, int pd, float* p)
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
float* OBJECTIO_s::Point(int a)
   {
   if (a < 0 || a >= pointnum) return NULL;
   return &pts[a * pointdim];
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //CONVERT BETWEEN WORLD AND VOXEL COORDS. AFFECTS RAD ALSO
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //convert to world coords; no y inversion or offset
int OBJECTIO_s::VoxToWorld(double xs, double ys, double zs)
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
int OBJECTIO_s::WorldToVox(double xs, double ys, double zs)
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

void OBJECTIO_s::DeleteDimDesc()
   {
   dimdesc.clear();
   /*int i;
   if (dimdesc)
      {
      for (i = pointdim - 1; i >=0; i--)
         delete [] dimdesc[i];
      delete [] dimdesc;
      dimdesc = NULL;
      }*/
   }
//-------------------------------------------------------------------------
      //These load functions are separated so that a derived class can do some other things
      //between header load and Point load
int OBJECTIO_s::LoadHeader(std::istream* ifile)
   {

    //char readline[500];
    std::string readline;
   TBOX_s tbox;
   int i;
   unsigned int offset;
   //char* cptr;
   std::string cptr;
   if(!*ifile) { std::cerr<<"File seemed to end prematurely. Load failed.\n"; return 0;}
   Releasemem();


   while(*ifile)
      {
      std::getline(*ifile,readline);
      //ifile->getline(readline, 299);
      if (!tbox.Lowercase(readline)) continue;

   
            //the point field is not in the header. 
      if( !readline.compare(0 , 6 , "points" , 0 , 6 ))//!strncmp(readline, "points", 6))
         break;

                     //look for values in the object header
      if ( !readline.compare(0 , 3 , "id:" , 0 , 3 ))//!strncmp(readline, "id:", 3))
         {
         //cptr = readline + 3;
         cptr = readline.substr(3);
         std::istringstream stream(cptr);
         stream >> id ;  
         continue;
         }

      if( !readline.compare( 0 , 5 , "type:" , 0 , 5 ))//!strncmp(readline, "type:", 5))
         {
            offset = 0;
            cptr=readline.substr(6);
         //cptr = readline + 6;
         tbox.Strip(cptr, offset , cptr);
         //cptr += offset;
         tbox.GetWord(cptr, offset , type);
         continue;
         }

      if(!readline.compare(0 , 9 , "treetype:" , 0 , 9 ))//!strncmp(readline, "treetype:", 9))
         {
         offset = 0;
         cptr = readline.substr( 10 );
         
         tbox.Strip(cptr, offset , cptr ) ;
         //cptr += offset;

         tbox.GetWord(cptr, offset , treetype );
         continue;
         }

      if( !readline.compare( 0 , 4 , "anat:" , 0 , 4 ))//!strncmp(readline, "anat:", 4))
         {
            offset = 0;
            cptr = readline.substr( 5 );
         tbox.Strip(cptr, offset , cptr);
         //cptr += offset;
         tbox.GetWord(cptr , offset , anat );
         continue;
         }

   
      if( !readline.compare( 0 , 7 , "vparent:" ,0,7))//!strncmp(readline, "vparent:", 7))
         {
         cptr = readline.substr( 8 ) ;
         std::istringstream stream(cptr);
         stream >> vparent ;  
         //vparent = atoi(cptr);
         continue;
         }

     if(!readline.compare(0,8, "attachpt:",0,8) ) //!strncmp(readline, "attachpt:", 8))
         {
         cptr = readline.substr( 9 ) ;
         std::istringstream stream(cptr);
         stream >> attachpt ;  
         //attachpt = atoi(cptr);
         continue;
         }

     if(!readline.compare(0,6, "color:" , 0,6 )) //!strncmp(readline, "color:", 6))
         {
         cptr = readline.substr( 6 ) ;
         std::vector<float> vec ;
         tbox.Stringtonfloats(cptr, 4, vec);
         for(unsigned int k=0;k<vec.size() && k < 4 ;k++)
            { col[ k ] = vec.at( k ) ; }
         continue;
         }

     if( !readline.compare(0,4, "name:",0,4) )//!strncmp(readline, "name:", 4))
         {
         offset = 0;
         cptr = readline.substr( 5 ) ;
         tbox.Strip(cptr, offset , name ) ;
         //cptr += offset;
         //if(strlen(cptr))
         //   {
         //   if (name) delete [] name;
         //   name = new char[strlen(cptr) + 1];
         //   strcpy_s(name, strlen(cptr) + 1, cptr);
         //   }
         continue;
         }

               
            //pointdim line may contain optional description of point field
            //you must delete dimdesc before changing pointdim and reallocating
     if( !readline.compare(0,8, "pointdim:",0,8) )//!strncmp(readline, "pointdim:", 8))
         {
         //DeleteDimDesc();
         dimdesc.clear();
         cptr = readline.substr( 9 ) ;
         std::istringstream stream(cptr);
         stream >> pointdim ;  
         //pointdim = atoi(cptr);
         if (pointdim <= 0) continue;
            if(limitpointdims > 0 && limitpointdims < pointdim)
               pointdim = limitpointdims;
         //dimdesc = new char* [pointdim];
         dimdesc.resize(pointdim);
         /*for (i = 0; i < pointdim; i++)      //prevent coredump if deletion of **
            {
            dimdesc[i] = new char[2];
            *dimdesc[i] = '\0';
            }*/

                  //advance pointer beyond the int given by pointdim
         cptr = readline.substr( 9 ) ;
         offset = 0;

         tbox.Strip(cptr, offset , cptr);
         //cptr += offset;
         if(!tbox.GetWord(cptr, offset)) continue;
         cptr = cptr.substr(offset);      

                  //look for optional field of description of pointdim
         for (i = 0; i < pointdim; i++)
            {
            offset = 0;
            tbox.Strip(cptr, offset,cptr);
            //cptr += offset;
            tbox.GetWord(cptr, offset,dimdesc.at(i));
            }

         continue;
         }

     if(!readline.compare(0,8,"npoints:",0,8) )//!strncmp(readline, "npoints:", 8))
         {
         cptr = readline.substr( 9 );
         std::istringstream i(cptr);
         i>>pointnum;
         
         //pointnum = atoi(cptr);
         continue;
         }
      }      //end while ifile
      if(pointnum <= 0 || pointdim <= 0) {std::cerr<<"Invalid # points "<<pointnum<<"  or dims "<<pointdim<<"\n"; return 0;}
    return 1;
    }

//----------------------------------------------------------
int OBJECTIO_s::LoadPoints(std::istream* ifile)
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
   //char readline[500];
   std::string readline;
   TBOX_s tbox;

   while (*ifile)
      {
      if (i >= pointnum) break;
      std::getline( *ifile , readline ) ;
      //ifile->getline(readline, 499);
      if ( readline.empty() ) break;

               //get fixed field.
      for (counter = 0; counter < pointdim; counter++)
         {
         std::vector<float> vec;
         if (!tbox.Stringtonfloats(readline, pointdim, vec ) )
            break;
         for( unsigned k = 0 ; k < vec.size() && (signed)k < pointdim ; k++)
            { *(pts + ( pointdim * i ) + k) = vec.at(k) ;}
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
int OBJECTIO_s::SaveHeader(std::ofstream* ofile)
   {
   if (!ofile) return 0;
   int i, usedim;

   *ofile<<std::endl;
   if(id > -1)
      *ofile<<"ID: "<<id<<std::endl;

   if (!type.empty() )
   *ofile<<"Type: "<<type<<std::endl;

   if(!anat.empty() )
      *ofile<<"Anat: "<<anat<<std::endl;

   if (!treetype.empty())
      *ofile<<"TreeType: "<<treetype<<std::endl;

   if (vparent > - 1)
      *ofile<<"VParent: "<<vparent<<std::endl;

   if (attachpt > -1)
      *ofile<<"Attachpt: "<<attachpt<<std::endl;

   *ofile<<"Color: "<<col[0]<<"  "<<col[1]<<"  "<<col[2]<<"  "<<col[3]<<std::endl;

   if (!name.empty() )
      *ofile<<"Name: "<<name<<std::endl;

   usedim = pointdim;
   if (limitpointdims < usedim && limitpointdims > 0)
      usedim = limitpointdims;
   *ofile<<"PointDim: "<<usedim<<"  ";
   for(i = 0; i < usedim; i++)  *ofile<<dimdesc.at(i)<<"  ";
   *ofile<<std::endl;

   *ofile<<"NPoints: "<<pointnum<<std::endl;

   return 1;
   }

//-------------------------------------------------
int OBJECTIO_s::SavePoints(std::ofstream* ofile)
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

