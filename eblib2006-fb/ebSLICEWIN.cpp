#include "ebSLICEWIN.h"

namespace eb
{

//**************************************************
      //CALLBACKS FOR THE WIDGETS
//***********************************************

               //Change the displayed slice
void VertSliderCallback(Fl_Widget* w, void* v)
   {
  double val;
   int ival;
   
   val = ((Fl_Slider*)w)->value();
   ival = (int)(val + 0.5);

   ((SLICEWIN*)v)->SetSlice(ival);
   }
//--------------------------------------
            //change the slice cut to axial
void AxialCallback(Fl_Widget* w, void* v)
   {((SLICEWIN*)v)->SetAxial();}
//------------------------------------------
            //change the slice cut to coronal
void CoronalCallback(Fl_Widget* w, void* v)
   {((SLICEWIN*)v)->SetCoronal(); }
//------------------------------------------
            //change the slice cut to sagittal
void SagittalCallback(Fl_Widget* w, void* v)
   {((SLICEWIN*)v)->SetSagittal(); }
//------------------------------------------
         //increment slice by one
void IncrementOneCallback(Fl_Widget* w, void* v)
   {
   int currentval = (int)((SLICEWIN*)v)->vertslide->value();
   ((SLICEWIN*)v)->SetSlice(currentval + 1);
   }
//--------------------------------------------
         //decrement slice by one
void DecrementOneCallback(Fl_Widget* w, void* v)
   {
   int currentval = (int)((SLICEWIN*)v)->vertslide->value();
   ((SLICEWIN*)v)->SetSlice(currentval - 1);
   }
//--------------------------------------------
      //Change intensity windowing of slice
void WindowSlice(Fl_Widget* w, void* v)
   {((SLICEWIN*)v)->SetSlice((int)((SLICEWIN*)v)->vertslide->value());}

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //SLICEWIN CLASS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

SLICEWIN::SLICEWIN(int x, int y, int w, int h, char* s):Fl_Gl_Window(x,y,w,h,s)
   {
               //Set pointers to 0 when relevant
   sbuff = 0;
   dims[0] = dims[1] = dims[2] = 0;
   loadedfilename = 0;

               
    try
       {
    

            //Adjust the height and width so that the display win will be
            //square. If the box is too small in one dimension, take the smallest
            //size 
      int extrawidth = sl_width;      //width of vertical slider
      int slicewidgetsheight = sl_width + 3; //height of box contaning slice widgets
      int intensityheight = 2 * sl_width + 3;      //height of 2 widgets controlling intens window
      int extraheight = slicewidgetsheight + intensityheight;




               //for now, make currentslice the size of winsize
     slicedims[0] = w;
     slicedims[1] = h;
     slicesize = slicedims[0] * slicedims[1];

     currentslice = new unsigned char[h * w];
     memset(currentslice, 0, w * h * sizeof(unsigned char));


            //create vertical slider that walks through slices
      vertslide = new Fl_Value_Slider(x + w, y, sl_width, h);
      vertslide->type(4);
      vertslide->slider_size(1);
      vertslide->value(0);
      vertslide->minimum(0);
      vertslide->maximum(0);
      vertslide->step(1);

               //create group that holds radiobuttons to set axial, coronal, sagittal,
               //and create those buttons
      int butlen = w/3 - 4;
      group1 = new Fl_Group(x, y + h + 1, w - 1, sl_width + 2);
      group1->box(FL_ENGRAVED_FRAME);
         {
         axialrb = new Fl_Round_Button(x + 1, y + h + 2, butlen, sl_width, "Axial");
          axialrb->type(102);
          axialrb->down_box(FL_ROUND_DOWN_BOX);
         axialrb->value(1);
         coronalrb = new Fl_Round_Button(x + 1 + butlen, y + h + 2, butlen, sl_width, "Coronal");
         coronalrb->type(102);
         coronalrb->down_box(FL_ROUND_DOWN_BOX);
            coronalrb->value(0);
         sagittalrb = new Fl_Round_Button(x + 4 + butlen + butlen, y + h + 2, butlen, sl_width, "Sagittal");
         sagittalrb->type(102);
         sagittalrb->down_box(FL_ROUND_DOWN_BOX);
            sagittalrb->value(0);
         }
      group1->end();

            //buttons that increment or decrement one slice at a time
      plusbutton = new Fl_Repeat_Button(x + w -1, y + h, sl_width/2, sl_width, "+");
      minusbutton = new Fl_Repeat_Button(x + w - 1 + sl_width/2, y + h, sl_width/2, sl_width, "-");


            //sliders that clamp intensity to a min or max
      intensityminslider = new Fl_Value_Slider(x, y + h + 20, w + sl_width, sl_width);
      intensityminslider->type(5);
      intensityminslider->step(1);
      intensityminslider->value(0);

      intensitymaxslider = new Fl_Value_Slider(x, y + h + 20 + sl_width + 1, w + sl_width, sl_width);
      intensitymaxslider->type(5);
      intensitymaxslider->step(1);
      intensitymaxslider->value(0);
       }
    catch(...)
       {std::cerr<<"Memory allocation error in Slicewin constructor\n";}


    corx = cory = corz = 1.0;
    mymem = 0;
    winid = 0;
    mousex = mousey = 0;
    magx = magy = 1;
    transx = transy = 0;

    position[0] = position[1] = position[2] = 0;
   }
//-----------------------------------------
SLICEWIN::~SLICEWIN()
   {
      Releasemem();

         //I believe the fltk child widgets delete themselves when
         //the main window deletes, so am not doing here. 
   }
//-----------------------------------------
void SLICEWIN::Releasemem()
   {
   if (mymem && sbuff) delete [] sbuff; sbuff = 0; mymem = 0;
   if (currentslice) delete []currentslice; currentslice = 0;
   dims[0] = dims[1] = dims[2] = slicedims[0] = slicedims[1] = 0;

   disptype = 0;

   if (loadedfilename) delete [] loadedfilename;
   loadedfilename = 0;
   }
//------------------------------------
void SLICEWIN::Initialize(int a)
   {
    winid = a;
    RestoreCallbacks();
   }
//----------------------------------------------------
void SLICEWIN::RestoreCallbacks()
   {

               //Set callbacks
                        //add the callbacks to the sliders
   vertslide->callback(VertSliderCallback, this);

                        //for the axial/coronal/sagittal views
   axialrb->callback(AxialCallback, this);
   coronalrb->callback(CoronalCallback, this);
   sagittalrb->callback(SagittalCallback, this);

                        //for the buttons that increment/decrement by a single slice
   plusbutton->callback(IncrementOneCallback, this);
   minusbutton->callback(DecrementOneCallback, this);

                        //for the sliders that control intensity windowing
   intensityminslider->callback(WindowSlice, this);
   intensitymaxslider->callback(WindowSlice, this);

   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //DRAW  AND HANDLE FUNCTIONS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
void SLICEWIN::draw()
   {


      if(!valid())
      {
      valid(1); 
      glClearColor((float)0.0, (float)0.0, (float)0.0, (float)0.0);               
      glShadeModel(GL_FLAT);

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //if you don't include this
                                                                //image size differences distort
      glPixelStorei(GL_PACK_ALIGNMENT, 1);

      ortho();      //fltk call
       }

   glClear(GL_COLOR_BUFFER_BIT);      //this clears and paints to black
   


    if(currentslice)
       {
      glRasterPos2i(transx, transy);
       glPixelZoom(magx, magy);
      glDrawPixels(slicedims[0], slicedims[1],GL_LUMINANCE, GL_UNSIGNED_BYTE, currentslice);
       }

    Fl::flush();
   }
//-----------------------------------------
int SLICEWIN::handle (int event)
   {
   switch(event)
      {
      case FL_PUSH:
         mousex = Fl::event_x();
         mousey = h() - 1 - Fl::event_y();  //put 0 y at bottom of window
         return 1;

      case FL_HIDE:               //don't show the widget
         return Fl_Gl_Window::handle(event);

      case FL_SHOW:            //do show the widget
         return Fl_Gl_Window::handle(event);
      }

   //return 0;
   return Fl_Gl_Window::handle(event);
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //LOADS AND/OR COPY INFO INTO WINDOW
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //The first function uses a passed pointer to srcbuff; dispwin should not delete.
         //The remaining functions make a hard copy.
int SLICEWIN::CopyBuffPointer(int* ddims, float* ssteps, unsigned short* srcbuff, float* ppos)
   {
   if (!srcbuff) return 0;
   Releasemem();
   int i, ts;

   for (i = 0; i < 3; i++)
      {
      dims[i] = ddims[i];
      steps[i] = ssteps[i];
      }


   if (dims[2] <= 0) dims[2] = 1;
   slicesize = dims[0] * dims[1];
   ts = slicesize * dims[2];

    sbuff = srcbuff;
    mymem = 0;

   RESCALET<unsigned short> rscale;
   float omin, omax;

   

   rscale.MinMax(srcbuff, ts, &omin, &omax);
   

   if (omin < 0 || omax > 4095)
      {
      rscale.Convert(srcbuff, ts, omin, omax, sbuff, 0, 4095);      //12 bit
      origintensitymin = 0;
      origintensitymax = 4095;
      }
   else
      {
      origintensitymin = (unsigned short)omin;
      origintensitymax = (unsigned short) omax;
      }

               //Give the info on image intensity to the intensity sliders
   intensityminslider->maximum(origintensitymax);
   intensityminslider->minimum(origintensitymin);
   intensityminslider->value(origintensitymin);
   intensitymaxslider->maximum(origintensitymax);
   intensitymaxslider->minimum(origintensitymin);
   intensitymaxslider->value(origintensitymax);


               //Set the slice orientation to axial, make sure
               //the radio widgets indicate this, adjust the vertical slider,
               //and draw the slice
   axialrb->value(1); coronalrb->value(0); sagittalrb->value(0);
   SetAxial();

   int x;
   if (ppos)
      {
      for (x = 0; x < 3; x++)
         position[x] = ppos[x];
      }
   else
      {
      for (x = 0; x< 3; x++)
         position[x] = 0;
      }

    return 1;
   }
//--------------------------------------------------------------------------
int SLICEWIN::CopyBuff(int* ddims, float* ssteps, float* srcbuff, float* ppos)
   {


   if (!srcbuff) return 0;
   Releasemem();
   int i, ts;

   for (i = 0; i < 3; i++)
      {
      dims[i] = ddims[i];
      steps[i] = ssteps[i];
      }


   if (dims[2] <= 0) dims[2] = 1;
   slicesize = dims[0] * dims[1];
   ts = slicesize * dims[2];

   try
      {
      sbuff = new unsigned short[ts];
      if (!sbuff){fl_alert("Out of mem"); std::cerr<<"Out of mem\n";return 0;}
      }
   catch(...)
      {
      fl_alert("Out of mem");
      sbuff = 0;
      return 0;
      }

   RESCALET<float> rscale;
   float omin, omax;

   

   rscale.MinMax(srcbuff, ts, &omin, &omax);
   

   if (omin < 0 || omax > 4095)
      {
      rscale.Convert(srcbuff, ts, omin, omax, sbuff, 0, 4095);      //12 bit
      origintensitymin = 0;
      origintensitymax = 4095;
      }
   else
      {
      for (i = 0; i < ts; i++)
         sbuff[i] = (unsigned short)srcbuff[i];
      origintensitymin = (unsigned short)omin;
      origintensitymax = (unsigned short) omax;
      }

               //Give the info on image intensity to the intensity sliders
   intensityminslider->maximum(origintensitymax);
   intensityminslider->minimum(origintensitymin);
   intensityminslider->value(origintensitymin);
   intensitymaxslider->maximum(origintensitymax);
   intensitymaxslider->minimum(origintensitymin);
   intensitymaxslider->value(origintensitymax);


               //Set the slice orientation to axial, make sure
               //the radio widgets indicate this, adjust the vertical slider,
               //and draw the slice
   axialrb->value(1); coronalrb->value(0); sagittalrb->value(0);
   SetAxial();

    mymem = 1;

    int x;
   if (ppos)
      {
      for (x = 0; x < 3; x++)
         position[x] = ppos[x];
      }
   else
      {
      for (x = 0; x< 3; x++)
         position[x] = 0;
      }
   return 1;
   }
//---------------------------------------
int SLICEWIN::CopyBuff(int* ddims, float* ssteps, unsigned short* srcbuff, float* ppos)
   {
   if (!srcbuff) return 0;
   Releasemem();
   int i, ts;

   for (i = 0; i < 3; i++)
      {
      dims[i] = ddims[i];
      steps[i] = ssteps[i];
      }

   if (dims[2] == 0) dims[2] = 1;
   slicesize = dims[0] * dims[1];
   ts = slicesize * dims[2];

   try
      {
      sbuff = new unsigned short[ts];
      if (!sbuff){fl_alert("Out of mem"); std::cerr<<"Out of mem\n"; return 0;}
      }
   catch(...)
      {
      fl_alert("Out of mem");
      sbuff = 0;
      return 0;
      }

   origintensitymin = origintensitymax = srcbuff[0];
   for (i = 0; i < ts; i++)
      {
      sbuff[i] = srcbuff[i];
      if (origintensitymax < sbuff[i]) origintensitymax = srcbuff[i];
      else if (origintensitymin > sbuff[i]) origintensitymin = srcbuff[i];
      }

               //Give the info on image intensity to the intensity sliders
   intensityminslider->maximum(origintensitymax);
   intensityminslider->minimum(origintensitymin);
   intensityminslider->value(origintensitymin);
   intensitymaxslider->maximum(origintensitymax);
   intensitymaxslider->minimum(origintensitymin);
   intensitymaxslider->value(origintensitymax);


               //Set the slice orientation to axial, make sure
               //the radio widgets indicate this, adjust the vertical slider,
               //and draw the slice
   axialrb->value(1); coronalrb->value(0); sagittalrb->value(0);
   SetAxial();

   int x;
   if (ppos)
      {
      for (x = 0; x < 3; x++)
         position[x] = ppos[x];
      }
   else
      {
      for (x = 0; x< 3; x++)
         position[x] = 0;
      }

    mymem = 1;
   return 1;
   }
//--------------------------------
int SLICEWIN::CopyBuff(int* ddims, float* ssteps, unsigned char* srcbuff, float* ppos)
   {
   if (!srcbuff) return 0;
   Releasemem();
   int i, ts;

   for (i = 0; i < 3; i++)
      {
      dims[i] = ddims[i];
      steps[i] = ssteps[i];
      }

   if (dims[2] == 0) dims[2] = 1;
   slicesize = dims[0] * dims[1];
   ts = slicesize * dims[2];

   try
      {
      sbuff = new unsigned short[ts];
      if (!sbuff){fl_alert("Out of mem"); std::cerr<<"Out of mem\n";return 0;}
      }
   catch(...)
      {
      fl_alert("Out of mem");
      sbuff = 0;
      return 0;
      }


   origintensitymin = origintensitymax = srcbuff[0];
   for (i = 0; i < ts; i++)
      {
      sbuff[i] = srcbuff[i];
      if (srcbuff[i] > origintensitymax) origintensitymax = srcbuff[i];
      else if (srcbuff[i] < origintensitymin) origintensitymin = srcbuff[i];
      }

            //Give the info on image intensity to the intensity sliders
   intensityminslider->maximum(origintensitymax);
   intensityminslider->minimum(origintensitymin);
   intensityminslider->value(origintensitymin);
   intensitymaxslider->maximum(origintensitymax);
   intensitymaxslider->minimum(origintensitymin);
   intensitymaxslider->value(origintensitymax);


               //Set the slice orientation to axial, make sure
               //the radio widgets indicate this, adjust the vertical slider,
               //and draw the slice
   axialrb->value(1); coronalrb->value(0); sagittalrb->value(0);
   SetAxial();
    mymem = 1;

   int x;

   if (ppos)
      {
      for (x = 0; x < 3; x++)
         position[x] = ppos[x];
      }
   else
      {
      for (x = 0; x< 3; x++)
         position[x] = 0;
      }
   
   return 1;
   }
//------------------------------
int SLICEWIN::LoadMeta()
   {
            //pick a filename
   char* s = fl_file_chooser("Open a meta image file", "*.mh?", NULL);
   return LoadMeta(s);
   }
//--------------------------------------

int SLICEWIN::LoadMetaHeader(char* s)
   {
   if(!s) return 0;

   if (loadedfilename) delete [] loadedfilename;
   loadedfilename = 0;

            //open the image

   meta::Image mi(s,false);//read only the header
   if (mi.Error())
      {fl_alert(mi.Error()); std::cerr<<mi.Error(); return 0;}


   Releasemem();

   
    loadedfilename = new char[strlen(s) + 1];
    strcpy_s(loadedfilename, strlen(s) + 1, s);

            //save the raw data in floatbuff and processed data in bytebuff

   dims[0] = mi.DimSize(0);
   dims[1] = mi.DimSize(1);
   dims[2] = mi.DimSize(2);
   if (dims[2] == 0) dims[2] = 1;
   slicesize = dims[0] * dims[1];

   steps[0] = (float)(mi.ElemSize(0)/10.0 * corx);
   steps[1] = (float)(mi.ElemSize(1)/10.0 * cory);
   steps[2] = (float)(mi.ElemSize(2)/10.0 * corz);

   if(steps[0] <= 0) steps[0] = 1;
   if(steps[1] <= 0) steps[1] = 1;
   if(steps[2] <= 0) steps[2] = 1;

    std::cerr<<"\nDims: "<<dims[0]<<"  "<<dims[1]<<"  "<<dims[2]<<std::endl;
    std::cerr<<"Steps: "<<mi.ElemSize(0)<<"  "<<mi.ElemSize(1)<<"  "<<mi.ElemSize(2)<<std::endl;
    std::cerr<<"Intensities: "<<origintensitymin<<"  "<<origintensitymax<<std::endl;
    std::cerr<<"Data type: "<<meta::MET_TypeToString(mi.ElemType())<<std::endl;
    if (mi.ElemByteOrderMSB()) std::cerr<<"Byte order is sun (MSB)"<<std::endl;
    else std::cerr<<"Byte order is pc (LSB)"<<std::endl;
    std::cerr<<"File modality: "<<meta::MET_ModToString(mi.Modality())<<std::endl;
    std::cerr<<"Position: "<<position[0]<<"  "<<position[1]<<"  "<<position[2]<<std::endl;
    
    mymem = 1;

    std::cerr<<"Loaded "<<s<<std::endl;


   return 1;
   }

int SLICEWIN::LoadMeta(char* s)
   {
   if(!s) return 0;

   if (loadedfilename) delete [] loadedfilename;
   loadedfilename = 0;

            //open the image
   meta::Image mi(s);
   if (mi.Error())
      {fl_alert(mi.Error()); std::cerr<<mi.Error(); return 0;}


   Releasemem();

   
    loadedfilename = new char[strlen(s) + 1];
    strcpy_s(loadedfilename, strlen(s) + 1, s);

            //save the raw data in floatbuff and processed data in bytebuff
   int x,y,z, indexz, indexy;

   dims[0] = mi.DimSize(0);
   dims[1] = mi.DimSize(1);
   dims[2] = mi.DimSize(2);
   if (dims[2] == 0) dims[2] = 1;
   slicesize = dims[0] * dims[1];

   steps[0] = (float)(mi.ElemSize(0)/10.0 * corx);
   steps[1] = (float)(mi.ElemSize(1)/10.0 * cory);
   steps[2] = (float)(mi.ElemSize(2)/10.0 * corz);

   if(steps[0] <= 0) steps[0] = 1;
   if(steps[1] <= 0) steps[1] = 1;
   if(steps[2] <= 0) steps[2] = 1;

   try
      {
      sbuff = new unsigned short[slicesize * dims[2]];
      if (!sbuff){fl_alert("Out of mem"); std::cerr<<"Out of mem\n";return 0;}
      }
   catch(...)
      {
      fl_alert("Out of mem");
      sbuff = 0;
      return 0;
      }

   origintensitymin = origintensitymax = (unsigned short)mi.Get(0,0,0);

   for (z = 0; z<dims[2]; z++)
      {
      indexz = slicesize * z;
      for (y = 0; y <dims[1]; y++)
         {
         indexy = indexz + (y * dims[0]);
         for (x = 0; x < dims[0]; x++)
            {
            sbuff[indexy + x] = (unsigned short)mi.Get(x,y,z);
            if (origintensitymax < sbuff[indexy + x])
               origintensitymax = sbuff[indexy + x];
            else if (origintensitymin > sbuff[indexy + x])
               origintensitymin = sbuff[indexy + x];
            }
         }
      }


   float* fptr = mi.Position();
   if (fptr)
      {
      for (x = 0; x < 3; x++)
         position[x] = fptr[x];
      }
   else
      {
      for (x = 0; x< 3; x++)
         position[x] = 0;
      }

               //Give the info on image intensity to the intensity sliders
   intensityminslider->maximum(origintensitymax);
   intensityminslider->minimum(origintensitymin);
   intensityminslider->value(origintensitymin);
   intensitymaxslider->maximum(origintensitymax);
   intensitymaxslider->minimum(origintensitymin);
   intensitymaxslider->value(origintensitymax);


               //Set the slice orientation to axial, make sure
               //the radio widgets indicate this, adjust the vertical slider,
   SetAxial();
    std::cerr<<"\nDims: "<<dims[0]<<"  "<<dims[1]<<"  "<<dims[2]<<std::endl;
    std::cerr<<"Steps: "<<mi.ElemSize(0)<<"  "<<mi.ElemSize(1)<<"  "<<mi.ElemSize(2)<<std::endl;
    std::cerr<<"Intensities: "<<origintensitymin<<"  "<<origintensitymax<<std::endl;
    std::cerr<<"Data type: "<<meta::MET_TypeToString(mi.ElemType())<<std::endl;
    if (mi.ElemByteOrderMSB()) std::cerr<<"Byte order is sun (MSB)"<<std::endl;
    else std::cerr<<"Byte order is pc (LSB)"<<std::endl;
    std::cerr<<"File modality: "<<meta::MET_ModToString(mi.Modality())<<std::endl;
    std::cerr<<"Position: "<<position[0]<<"  "<<position[1]<<"  "<<position[2]<<std::endl;
    
    mymem = 1;

    std::cerr<<"Loaded "<<s<<std::endl;


   return 1;
   }

//-------------------------------
int SLICEWIN::SaveMeta()
   {
   if(!sbuff) return 0;

               //pick a filename
   char* s = fl_file_chooser("Pick filename for save", "*.mh*", NULL);
   return SaveMeta(s);
   }
//------------------------------------------
int SLICEWIN::SaveMeta(char* s)
   {
   if(!s) return 0;

   char finalname[500];
   strcpy_s(finalname, 500,s);

         //check that extension is valid
   if (strcmp(s + strlen(s) - 4, ".mha") )
      strcat_s(finalname, 500,".mha");
   else strcpy_s(finalname, 500, s);


               //save as unsigned short. 
   int size = 1;
   for (int i = 0; i < GetNDims(); i++)
      size = size * dims[i];

   float m_steps[3];
   m_steps[0] = steps[0] * 10/corx;
   m_steps[1] = steps[1] * 10/cory;
   m_steps[2] = steps[2] * 10/corz;


   meta::Image mi(GetNDims(), dims, meta::MET_USHORT, m_steps, 12, MET_SYSTEM_BYTE_ORDER_MSB, sbuff);
   mi.Position(position);
   mi.Save(finalname);
   return 1;
   }
//---------------------------------------------
int SLICEWIN::SaveSplitMeta()
   {
   if (!sbuff) return 0;
   int rval = 1;

   char* s = fl_file_chooser("Pick name for split meta save", "*.mh?", NULL);
   if (!s) return 0;

         //make sure the extensions are OK and get a second filename with the extension mda
   char* first = new char[strlen(s) + 6];
   char* second = new char[strlen(s) + 6];
   strcpy_s(first, strlen(s) + 6, s);
   if (strcmp(first + (strlen(first) - 4), ".mhd"))
      strcat_s(first, strlen(s) + 6, ".mhd");
   strcpy_s(second, strlen(s) + 6, first);
   second[strlen(second) - 3] = '\0';      //strip extension
   strcat_s(second, strlen(s) + 6, "mda");


   float ss[3] = {steps[0] *10/corx, steps[1] * 10/cory, steps[2] * 10/corz};

   meta::Image mi(dims[0], dims[1], dims[2], meta::MET_USHORT, ss, 16, MET_SYSTEM_BYTE_ORDER_MSB, sbuff);
   if(!mi.Save(first, second))
      {
      std::cerr<<mi.Error();
      rval = 0;
      }

   delete [] first;
   delete [] second;

   return rval;
   }
//-------------------------------
int SLICEWIN::SaveMetaUchar()
   {
   if(!sbuff) return 0;


               //pick a filename
   char* s = fl_file_chooser("Pick filename for save", "*.mh*", NULL);
   if(!s) return 0;


            //check that extension is valid
   char finalname [500];
   if (strcmp(s + strlen(s) - 4, ".mha") )
      strcat_s(finalname, 500,".mha");
   else strcpy_s(finalname, 500,s);

               //save as unsigned short. 
   int size = 1;
   for (int i = 0; i < GetNDims(); i++)
      size = size * dims[i];

   float m_steps[3];
   m_steps[0] = steps[0] * 10/corx;
   m_steps[1] = steps[1] * 10/cory;
   m_steps[2] = steps[2] * 10/corz;

   unsigned char* tempbuff;

    
   try
      {
      tempbuff = new unsigned char[size];
      if(! tempbuff)
         {
         std::cerr<<"No mem for save\n";
         return 0;
         }
      }
   catch(...)
      {
      fl_alert("No mem for save");
      return 0;
      }

    RESCALET<unsigned short> rscale;
   rscale.Convert(sbuff, size, (float)(intensityminslider->value()), (float)(intensitymaxslider->value()), tempbuff, 0, 255);


   meta::Image mi(GetNDims(), dims, meta::MET_UCHAR, m_steps, 8, MET_SYSTEM_BYTE_ORDER_MSB, tempbuff);
   mi.Position(position);

   mi.Save(finalname);
    delete [] tempbuff;
   return 1;
   }
//-------------------------------------------------------------
int SLICEWIN::SaveMetaUcharNoRescale()
   {
   if(!sbuff) return 0;
    int i;

               //pick a filename
   char* s = fl_file_chooser("Pick filename for save", "*.mh*", NULL);
   if(!s) return 0;

            //check that extension is valid
   char finalname [500];
   strcpy_s(finalname,500, s);
   if (strcmp(s + strlen(s) - 4, ".mha") )
      strcat_s(finalname, 500,".mha");
   else strcpy_s(finalname, 500,s);


               //save as unsigned short. 
   int size = 1;
   for (i = 0; i < GetNDims(); i++)
      size = size * dims[i];

   float m_steps[3];
   m_steps[0] = steps[0] * 10/corx;
   m_steps[1] = steps[1] * 10/cory;
   m_steps[2] = steps[2] * 10/corz;

   unsigned char* tempbuff;

   try
      {
      tempbuff = new unsigned char[size];
      if(! tempbuff)
          {
         std::cerr<<"No mem for save\n";
         return 0;
         }
      }
   catch(...)
      {
      fl_alert("No mem for save");
      return 0;
      }

  for (i = 0; i < size; i++)
     tempbuff[i] = (unsigned char)sbuff[i];

  meta::Image mi(GetNDims(), dims, meta::MET_UCHAR, m_steps, 8, MET_SYSTEM_BYTE_ORDER_MSB, tempbuff);
   mi.Position(position);

   mi.Save(finalname);
   std::cerr<<"Saved "<<finalname<<std::endl;
    delete [] tempbuff;
   return 1;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //MANIPULATE INTENSITIES AND GET SLICE INFO
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //rescales floatbuff to current intensity settings. You cannot reexpand
      //range after this is done, but means that floatbuff is available for
      //further image processing without having to recalculate each time.
int SLICEWIN::Clamp()
   {
   if (!sbuff) return 0;
   RESCALET<unsigned short> rscale;
   rscale.Clamp(sbuff, dims[0] * dims[1] * dims[2],sbuff, (float)(intensityminslider->value()), (float)(intensityminslider->value()), (float)(intensitymaxslider->value()), (float)(intensitymaxslider->value()));

   intensityminslider->maximum(intensitymaxslider->value());
   intensityminslider->minimum(intensityminslider->value());
   intensitymaxslider->minimum(intensityminslider->value());
   intensitymaxslider->maximum(intensitymaxslider->value());
   intensitymaxslider->value(intensitymaxslider->maximum());
   intensityminslider->value(intensityminslider->minimum());

   Fl::flush();
   return 1;
   }
//----------------------------------------------------
int SLICEWIN::GetSliceDescription(int* minx, int* maxx, int* miny, int* maxy, int* minz, int* maxz)
   {
   if(!sbuff) return 0; 

   switch (disptype)
      {
      case 0:     //axial slice
         *minx = *miny = 0;
         *maxx = dims[0] - 1;
         *maxy = dims[1] - 1;
         *minz = *maxz = (int)vertslide->value();
         break;

      case 1:        //coronal
         *minx = *minz = 0;
         *maxx = dims[0] - 1;
         *maxz = dims[2] - 1;
         *miny = *maxy = (int)vertslide->value();
         break;

      case 2:
         *minx = *maxx = (int)vertslide->value();
         *miny = *minz = 0;
         *maxy = dims[1] - 1;
         *maxz = dims[2] - 1;
         break;

      default:
         std::cerr<<"Unknown slice direction\n";
         return 0;
         break;

      }
   return 1;
   }
//--------------------------------------------------------
      //cut orientation is axial
void SLICEWIN::SetAxial()
   {
   if (!sbuff) return;

            //size of slice in axial view
   if (currentslice) delete[] currentslice;
   try
      {
      currentslice = new unsigned char[dims[0] * dims[1]];
      if (!currentslice){std::cerr<<"Out of mem\n"; fl_alert("Out of mem"); return;}
      }
   catch(...)
      {
      currentslice = 0;
      fl_alert("Out of mem");
      return;
      }

   for (int i = 0; i < 3; i++)
      {
      if (steps[i] <= 0) steps[i] = 1;
      if (dims[i] <= 0) dims[i] = 1;
      }

   slicedims[0] = dims[0];
   slicedims[1] = dims[1];
   slicesteps[0] = steps[0];
   slicesteps[1] = steps[1];
   slicesteps[2] = steps[2];


   disptype = 0;
   
    axialrb->value(1);
    coronalrb->value(0);
    sagittalrb->value(0);

   InitializeSliderRange(dims[2]);   //there are dims[2] axial slices

   

 
            //center and scale the slice, and save the values
    if(slicesteps[0] * slicedims[0] > slicesteps[1] * slicedims[1]) //xdim big
       {
       magx = (float)w()/slicedims[0];
       magy = ((float)slicesteps[1]/slicesteps[0]) * magx;
       transx = 0;
       transy = (int)(h() - magy * slicedims[1])/2;
       }
    else 
       {
       magy = (float)h()/slicedims[1];
       magx = ((float)slicesteps[0]/slicesteps[1]) * magy;
       transy = 0;
       transx = (int)(w() - magx* slicedims[0])/2;
       }
    if (transx < 0) transx = 0;
    if(transy < 0) transy = 0;

            //draw it
   SetSlice(dims[2]/2);                  //set a slice in the center of the range
   }
//----------------------------------------------------------------
      //cut orientation is coronal
void SLICEWIN::SetCoronal()
   {
   if (!sbuff) return;

               //size of slice in coronal view
   if (currentslice) delete[] currentslice;

   try
      {
      currentslice = new unsigned char[dims[0] * dims[2]];
      if (!currentslice){fl_alert("Out of mem"); std::cerr<<"Out of mem\n"; return;}
      }
   catch(...) {fl_alert("Out of mem"); currentslice = 0; return;}

   slicedims[0] = dims[0];
   slicesteps[0] = steps[0];
   slicedims[1] = dims[2];
   slicesteps[1] = steps[2];
   slicesteps[2] = steps[1];


   disptype = 1;                        //coronal
 
    axialrb->value(0);
    coronalrb->value(1);
    sagittalrb->value(0);
   InitializeSliderRange(dims[1]); //there are dims[1] coronal slices

             //center and scale the slice, and save the values
    if(slicesteps[0] * slicedims[0] > slicesteps[1] * slicedims[1]) //xdim big
       {
       magx = (float)w()/slicedims[0];
       magy = ((float)slicesteps[1]/slicesteps[0]) * magx;
       transx = 0;
       transy = (int)(h() - magy * slicedims[1])/2;
       }
    else 
       {
       magy = (float)h()/slicedims[1];
       magx = ((float)slicesteps[0]/slicesteps[1]) * magy;
       transy = 0;
       transx = (int)(w() - magx* slicedims[0])/2;
       }
 
    if (transx < 0) transx = 0;
    if(transy < 0) transy = 0;
    
    SetSlice(dims[1]/2);                  //set a slice in center of the range
   }
//-----------------------------------------------------
      //cut orientation is sagittal
void SLICEWIN::SetSagittal()
   {
   if (!sbuff) return;
               //size of slice in sagittal view
   if (currentslice) delete[] currentslice;

   try
      {
      currentslice = new unsigned char[dims[1] * dims[2]];
      if (!currentslice){fl_alert("Out of mem"); std::cerr<<"Out of mem\n"; return;}
      }
   catch(...)
      {fl_alert("Out of mem"); currentslice = 0; return;}

   slicedims[0] = dims[1];
   slicesteps[0] = steps[1];
   slicedims[1] = dims[2];
   slicesteps[1] = steps[2];
   slicesteps[2] = steps[0];


    axialrb->value(0);
    coronalrb->value(0);
    sagittalrb->value(1);

   disptype = 2;    //sagittal
 
   InitializeSliderRange(dims[0]);      //there are dims[0] y-z slices


    if(slicesteps[0] * slicedims[0] > slicesteps[1] * slicedims[1]) //xdim big
       {
       magx = (float)w()/slicedims[0];
       magy = ((float)slicesteps[1]/slicesteps[0]) * magx;
       transx = 0;
       transy = (int)(h() - magy * slicedims[1])/2;
       }
    else 
       {
       magy = (float)h()/slicedims[1];
       magx = ((float)slicesteps[0]/slicesteps[1]) * magy;
       transy = 0;
       transx = (int)(w() - magx* slicedims[0])/2;
       }

    if (transx < 0) transx = 0;
    if(transy < 0) transy = 0;

   SetSlice(dims[0]/2);
   }
//-----------------------------------------------------
         //Display a new slice from a preset orientation
int SLICEWIN::SetSlice(int a)
   {

   if(!sbuff) return 0;
   if (a > vertslide->maximum() || a < 0) return 0;

            //put the slider in the right position
   vertslide->value(a);

            //make a new buffer of correct size and put the right float values in
            //it. It will then be windowed into currentslice as unsigned char
   int x, y, index1, index2, offset, i;
   int indexx, indexy, indexz;

   unsigned short* tempbuff;
   try
      {
      tempbuff = new unsigned short[slicedims[0]* slicedims[1]];
      if (!tempbuff){fl_alert("Out of mem"); std::cerr<<"Out of mem\n";return 0;}
      }
   catch(...)
      {
      fl_alert("Out of mem");
      return 0;
      }

   switch (disptype)
      {
      case 0:            //axial: must flip upside down
         {
         offset = slicesize * a;

         for (y = 0; y < dims[1]; y++)
            {
            index1 = offset + y * dims[0];
            index2 = (dims[1] - 1 - y) * dims[0];
            for (x = 0; x < dims[0]; x++)
               tempbuff[index2 + x] = sbuff[index1 + x];
            }
         break;
         }

      case 1:            //coronal: x-z
         indexx = -1;
         indexy = a * dims[0];
         indexz = 0;

         for (i = 0; i < dims[0] * dims[2]; i++)      //for an x-z slice
            {
            ++indexx;
            if(indexx == dims[0])
               {
               indexx = 0;
               indexz += slicesize;
               }
            tempbuff[i] = sbuff[indexz + indexy + indexx];
            }
         break;

      case 2:            //sagittal y-z
         indexx = a;
         indexy = -1;
         indexz = 0;
         for (i = 0; i < dims[1] * dims[2]; i++)      //for a y-z slice
            {
            ++indexy;
            if (indexy == dims[1])
               {
               indexy = 0;
               indexz += slicesize;
               }
            tempbuff[i] = sbuff[indexz  + indexy * dims[0] + indexx];
            }
         break;

      default:
         fl_alert("Invalid slice orientation request");
         std::cerr<<"Invalid slice orientation request\n";
         return 0;
      }

               //process the values in tempbuff according to intensity window
               //clamp and put into currentslice
   RESCALET <unsigned short>rscale;
   rscale.Convert(tempbuff, slicedims[0] * slicedims[1], (float)(intensityminslider->value()), (float)(intensitymaxslider->value()), currentslice, 0, 255);
   delete [] tempbuff;

            
   redraw();

   return 1;
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&7
      //MOUSECLICK
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
void SLICEWIN::GetMouseClickPosition(int* x, int* y)
   {
   *x = mousex;
   *y = mousey;
   }
//-------------------------------------------
int SLICEWIN::GetVoxelFromMouseClick(int* x, int* y, int* z)
   {
   if(!sbuff) return 0;
   if (magx == 0 || magy == 0) return 0;

         //reverse the translation and scale of the slice in 
         //the window
   int tempx = (int)((mousex - transx) * 1/magx + 0.5);
   int tempy = (int)((mousey - transy) * 1/magy + 0.5);

   switch(disptype)
      {
      case 0:        //axial
         *z = (int)vertslide->value();
         *x = tempx;
         *y = dims[1] - 1 - tempy;
         break;

      case 1:     //coronal: x is x, y and z are reversed
         *x = tempx;
         *y = (int)vertslide->value();
         *z = tempy;
         break;

      case 2:     //sagittal: x is y, y is z, z is x
         *y = tempx;
         *z = tempy;
         *x = (int)vertslide->value();
         break;
      }


   if(*x < 0 || *y < 0 || *z < 0)
      return 0;
   if (*x >= dims[0] || *y >= dims[1] || *z >= dims[2])
      return 0;
   return 1;
   }
//-----------------------------------------------------
      //py is assumed sent as if 0 y were at bottom of window
int SLICEWIN::GetVoxelFromWindowPosition(int px, int py, int* x, int* y, int* z)
   {
   if(!sbuff) return 0;
   if (magx == 0 || magy == 0) return 0;

         //reverse the translation and scale of the slice in 
         //the window
   int tempx = (int)((px - transx) * 1/magx + 0.5);
   int tempy = (int)((py - transy) * 1/magy + 0.5);

   switch(disptype)
      {
      case 0:        //axial
         *z = (int)vertslide->value();
         *x = tempx;
         *y = dims[1] - 1 - tempy;
         break;

      case 1:     //coronal: x is x, y and z are reversed
         *x = tempx;
         *y = (int)vertslide->value();
         *z = tempy;
         break;

      case 2:     //sagittal: x is y, y is z, z is x
         *y = tempx;
         *z = tempy;
         *x = (int)vertslide->value();
         break;
      }


   if(*x < 0 || *y < 0 || *z < 0)
      return 0;
   if (*x >= dims[0] || *y >= dims[1] || *z >= dims[2])
      return 0;
   return 1;
   }
//-------------------------------------------------------------
int SLICEWIN::GetVoxelFromMouseClick()
   {
   int x, y, z, rval;

   if(!GetVoxelFromMouseClick(&x, &y, &z)) return 0;

   rval = z * slicesize + y * dims[0] + x;
   return rval;
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PROTECTED: ADJUST WIDGETS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
void SLICEWIN::InitializeSliderRange(int maxslice)
   {
   
               //vertical slider that scrolls through slices
   vertslide->minimum(0);
   vertslide->maximum(maxslice - 1);
   vertslide->value(maxslice/2);
   vertslide->slider_size(1.0/maxslice);
   }

}//end of namespace