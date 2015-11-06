#ifndef _ebZBUFF_h
#define _ebZBUFF_h

/* Specialized miniclass to carry info about vessel projection points on a viewplane.
Vessid and vessptid are integer values indicating which vessel and which vessel points are
making the projection. Zpos is the z value of the rotated, translated point within
the fluoroscope's coordinate system. Rescaled_rad is a radius representing how much volume
will be raytraced; at the skeleton point rescaled_rad is the same as the vessel radius. On
the viewplane, rescaled_rad is magnified according to the z position of the 3D point.

  moved to namespace std 6/2002

  Elizabeth Bullitt

*/
//#include "MemoryLeak.h"
namespace eb
{

class ZBUFF
   {
   public:
      ZBUFF() {zpos = 0; vessid = -1;}

      ZBUFF(const ZBUFF& a)
         {vessid = a.vessid; vessptid = a.vessptid; zpos = a.zpos; rescaled_rad = a.rescaled_rad;}
      
      ZBUFF& operator = (const ZBUFF& a)
         {
         if (this == &a) return *this;
         vessid = a.vessid; vessptid = a.vessptid; zpos = a.zpos; rescaled_rad = a.rescaled_rad;
         return * this;
         }


      void Set(int vid, int vpt, float z, float r)
         {vessid = vid; vessptid = vpt; zpos = z; rescaled_rad = r;}

      void Set(int vid, int vpt, float z, float r, float* _col)
         {vessid = vid; vessptid = vpt; zpos = z; rescaled_rad = r; col = _col;}


      void SetVess(int a, int b)
         {vessid = a; vessptid = b;}

      void SetZ(float a)                     {zpos = a;}
      void SetRRad(float a)                  {rescaled_rad = a;}
      void SetCol(float* a)                  {col = a;}
 
      void Unset()   {zpos = 0; vessid = -1;}

      float Set()      {return zpos;}

      int GetVessID()                        {return vessid;}
      float GetZ()                           {return zpos;}
      int GetPointID()                       {return vessptid;}
      float GetRRad()                        {return rescaled_rad;}
      float* GetCol()                        {return col;}

   protected:
      int vessid;
      int vessptid;
      float zpos; 
      float rescaled_rad;
      float* col;
     
   };
}//end of namespace
#endif
