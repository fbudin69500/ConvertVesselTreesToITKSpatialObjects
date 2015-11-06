/*Class that sets up a slice viewer for axial, sagittal, coronal. It uses a buffer that is
unsigned short, and so assumes the incoming data will either be unsigned short or unsigned char.
*/


#ifndef _ebSLICEWIN_h
#define _ebSLICEWIN_h


         //eblib
#include "ebRESCALET.h"         //rescale image intensity data
#include "ebINCLUDES.h"      //put this before fltk calls--you need windows.h

         //Fltk library
#include <Fl/Fl.h>
#include <Fl/Fl_Box.h>
#include <FL/Fl_Value_Slider.h>
#include <Fl/fl_file_chooser.h>
#include <Fl/Fl_Group.h>
#include <Fl/Fl_Round_Button.h>
#include <Fl/Fl_Repeat_Button.h>
#include <Fl/Fl_Button.h>
#include <Fl/Fl_Gl_Window.h>
#include <Fl/Gl.h>





         //Stephen library
#include <MetaImageLib.h>      //metaimage load
       //#include "MemoryLeak.h"        
namespace eb
{
      //things you might want to make accessible later. Sets slider thickness
const int sl_width = 20;

class SLICEWIN:public Fl_Gl_Window
   {

   public:            
      Fl_Value_Slider* vertslide;   //slider that increments slices      
      Fl_Group* group1;                  //group that contains axial/coronal/sagittal radiobs
      Fl_Round_Button* axialrb;      //radio button sets slicing to axial
      Fl_Round_Button* coronalrb;   //radio button sets slicing to coronal
      Fl_Round_Button* sagittalrb;//radio button sets slicing to sagittal
      Fl_Repeat_Button* plusbutton;         //increment slice by one
      Fl_Repeat_Button* minusbutton;         //decrement slice by one
      Fl_Value_Slider* intensityminslider;   //clamp lower value
      Fl_Value_Slider* intensitymaxslider;   //clamp higher value


                  //constructor, destructor, mem
   SLICEWIN(int x, int y, int w, int h, char* s = 0);
   ~SLICEWIN();
   virtual void Releasemem();
   virtual void Initialize(int winid = 0);


                     //Gets and sets
    
               //WARNING: SET THIS BEFORE ANY FILE LOAD. It corrects for errors in MR acquisition
               //as measured by phantoms.
   void SetFixFactors(float a, float b, float c)
       {if (a > 0) {steps[0] = steps[0]/corx * a;corx = a;}
    if (b > 0) {steps[1] = steps[1]/cory * b; cory = b;} 
    if (c > 0) {steps[2] = steps[2]/corz * c; corz = c;}}


    void SetID(int a)           {winid = a;}
    int GetID()                 {return winid;}
   int GetNDims()                  
      {if (!sbuff) return 0; if(dims[2]<= 1) return 2; return 3;}
   int* GetDims()               {return dims;}
   int GetTotalSize()            {return dims[0] * dims[1] * dims[2];}

   float* GetSteps()            {return steps;}
   float* GetPosition()         {return position;}
   unsigned short* GetSBuff()      {return sbuff;}
   unsigned char* GetCurrentSlice(int* x, int* y)
      {*x = slicedims[0], *y = slicedims[1]; return currentslice;}
   unsigned char* GetCurrentSlice(int* x, int* y, int* z)
      {*x = slicedims[0]; *y = slicedims[1]; *z = (int)vertslide->value(); return currentslice;}
    int GetSliceDescription(int* minx, int* maxx, int* miny, int* maxy, int* minz, int* maxz);
    int GetDisptype()            {return disptype;}
   float* GetSliceSteps()      {return slicesteps;}
   int* GetSliceDims()            {return slicedims;}
   unsigned char* GetCurrentSlice()      {return currentslice;}
   float GetZPos()                     {return (float)vertslide->value() * slicesteps[2];}
    float GetCorX()              {return corx;}
    float GetCorY()              {return cory;}
    float GetCorZ()              {return corz;}

               //These calls give the orignal intensities of the loaded file
   void GetIntensityRange(unsigned short* a, unsigned short* b)
      {*a = origintensitymin; *b = origintensitymax;}
   unsigned short GetIntensityMin()         {return origintensitymin;}
   unsigned short GetIntensityMax()         {return origintensitymax;}

               //These calls give the intensities of the windowed buffer
   unsigned short GetWindowedMin()      {return (unsigned short)intensityminslider->value();}
   unsigned short GetWindowedMax()      {return (unsigned short)intensitymaxslider->value();}

               //Get the loaded filename
   char* GetFilename()            {return loadedfilename;}

                     //FLTK mandatory draw and handle calls
    virtual void draw();
    virtual int handle (int event);


            //load a new file or make a copy of a passed buffer or save
   int LoadMeta();
   virtual int LoadMetaHeader(char* s);
   virtual int LoadMeta(char* s);
   int SaveMeta();
   int SaveMeta(char* s);

   int SaveSplitMeta();

    int SaveMetaUchar();
    int SaveMetaUcharNoRescale();
   virtual int CopyBuff(int* ddims, float* ssteps, float* srcbuff, float* ppos = 0);
   virtual int CopyBuff(int* ddims, float* ssteps, unsigned short* srcbuff, float* ppos = 0);
   virtual int CopyBuff(int* ddims, float* ssteps, unsigned char* srcbuff, float* ppos = 0);

               //Don't make a copy: use as a passed pointer
   virtual int CopyBuffPointer(int* ddims, float* ssteps, unsigned short* srcbuff, float* ppos = 0);

   
                 //Slice manipulation
   int Clamp();               //clamps short buffer to current intensity settings

            //change the slice within a preset cut orientation or change the cut
            //orientation
   virtual int SetSlice(int a);
   virtual void SetAxial();
   virtual void SetCoronal();
   virtual void SetSagittal();
                         //Adjust the slider widgets on a new file load
   void InitializeSliderRange(int maxslice);

            //show or hide the slice
    void ShowSlice()          {SetSlice((int)vertslide->value());}   
    void HideSlice()         
{memset(currentslice, 0, slicedims[0] * slicedims[1] * sizeof(unsigned char));
    redraw();}

                  //Get an x,y mouseclick position or the voxel
                  //associated with this position. A derived class 
                  //can use the base functions here.
    void GetMouseClickPosition(int* x, int* y);
    int GetVoxelFromMouseClick(int* x, int* y, int* z);
    int GetVoxelFromMouseClick();
    int GetVoxelFromWindowPosition(int px, int py, int* x, int* y, int* z);


               //Allowed a derived class to reset the initial callbacks
    virtual void RestoreCallbacks();

    protected:
       int mymem;      //whether or not to delete sbuff
       int winid;       //id of this window if several are up
       char* loadedfilename;

            //image for display
      unsigned short* sbuff;
      int dims[3];
      float steps[3];
      int slicesize;
      unsigned short origintensitymin, origintensitymax;
      int disptype;         //0 = axial, 1 = coronal, 2 = sagittal
      unsigned char* currentslice;
      int slicedims[2];
      float slicesteps[3];
      float corx, cory, corz;      //MR correction factors

      float position[3];         //position of first voxel in the MR coord system.

               //use for mouseclick interpretation
        float magx, magy;
        int transx, transy;
        int mousex, mousey;
   };

}//end of namespace

#endif