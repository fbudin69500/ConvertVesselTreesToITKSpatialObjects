#ifndef _ebFILE_HEADER_IO_h
#define _ebFILE_HEADER_IO_h

//Base class for reading/writing header info for entire file for tre objects etc
/*
Modified 6/02 to go into std namespace, exception handling

  Elizabeth Bullitt
*/

#include "ebINCLUDES.h"      //define std namespace or not
//#include "MemoryLeak.h"
namespace eb
{

class FILEHEADERIO
   {
   public:
      FILEHEADERIO();
      void Init();
      void Set(int nd, int* d, float* s, int vn);    //steps assumed corrected with a direct pass
      
               //The following values are used to transform vessel voxel coordinates
               //on load. Fixfactors applies to the stepsize, and is designed to correct
               //MR spacing errors.
               //It is much more efficient to change the fixfactors (if you need to)
               //before vessels are loaded, but you can change the steps later.
      float GetXStep()         {return steps[0];}
      float GetYStep()         {return steps[1];}
      float GetZStep()         {return steps[2];}
        float* GetSteps()           {return steps;}
        float GetSteps(int a)       {return steps[a];}

      int GetXNum()               {return dims[0];}
      int GetYNum()               {return dims[1];}
      int GetZNum()               {return dims[2];}
        int* GetDims()                  {return dims;}
        int GetDims(int a)              {return dims[a];}

      float GetFixX()            {return corfactor[0];}
      float GetFixY()            {return corfactor[1];}
      float GetFixZ()            {return corfactor[2];}
        float* GetCorfactor()       {return corfactor;}
        void SetCorfactor(float* a) {SetFixFactors(a[0], a[1], a[2]);}
        void SetFixFactors(float a, float b, float c)    //historical
           {corfactor[0] = a; corfactor[1] = b; corfactor[2] = c;}

        int GetVessnum()                  {return vessnum;}

        int LoadHeader(std::ifstream* ifile);
        int WriteHeader(std::ofstream* ofile);



   protected:
      float steps[3];                        //used during saves
        int ndims;
      int dims[3];                       //used during saves
        float corfactor[3];                 //used to correct MR spacing errors
        int vessnum;                        //number of objects
   };
}//end of namespace
#endif

                            



