#ifndef _eb_RESCALET_cpp
#define _eb_RESCALET_cpp

#include "ebRESCALET.h"

namespace eb
{

template <class T>
int RESCALET<T>::MinMax(const T* buff, int size, float* min, float* max)
   {
   if (!buff || size <= 0) return 0;

   register int i;
   *min = *max = (float)buff[0];

   for (i = 0; i < size; i++)
      {
      if (buff[i] > *max) *max = (float)buff[i];
      else if (buff[i] < *min) *min = (float)buff[i];
      }
   return 1;
   }
//-------------------------------------------------
template <class T>
int RESCALET<T>::Convert(const T* src, int size, float oldmin, float oldmax,\
unsigned char* target, int newmin, int newmax)
   {

   float oldrange = oldmax - oldmin;
   if (oldrange <= 0) return 0;

   float newrange = (float)(newmax - newmin);
   if (newrange <= 0) return 0;
   if (newmax > 255) return 0;
   if (newmin < 0) return 0;

   float finalrange = newrange/oldrange;
   register int i;
   float f;

   for (i = 0; i < size; i++)
      {
      f = (float)src[i];

            //This allows a clamp operation directly
      if (f > oldmax) f = oldmax;
      else if (f < oldmin) f = oldmin;

      f = ((f - oldmin)* finalrange) + newmin;
      if (f > newmax) f = (float)newmax;
      else if (f < newmin) f = (float)newmin;
      target[i] = (unsigned char)f;
      }

   return 1;
   }
//-------------------------------------------------
template <class T>
int RESCALET<T>::Convert(const T* src, int size, float oldmin, float oldmax,\
unsigned short* target, int newmin, int newmax)
   {

   float oldrange = oldmax - oldmin;
   if (oldrange <= 0) return 0;

   float newrange = (float)(newmax - newmin);
   if (newrange <= 0) return 0;
   
   float finalrange = newrange/oldrange;
   register int i;
   float f;

   for (i = 0; i < size; i++)
      {
      f = (float)src[i];

            //This allows a clamp operation directly
      if (f > oldmax) f = oldmax;
      else if (f < oldmin) f = (float)oldmin;

      f = ((f - oldmin)* finalrange) + newmin;
      if (f > newmax) f = (float)newmax;
      else if (f < newmin) f = (float)newmin;
      target[i] = (unsigned short)f;
      }

   return 1;
   }
//-------------------------------------------------
template <class T>
int RESCALET<T>::Convert(const T* src, int size, float oldmin, float oldmax,\
float* target, float newmin, float newmax)
   {

   float oldrange = oldmax - oldmin;
   if (oldrange <= 0) return 0;

   float newrange = (float)(newmax - newmin);
   if (newrange <= 0) return 0;
   
   float finalrange = newrange/oldrange;
   register int i;
   float f;

   for (i = 0; i < size; i++)
      {
      f = (float)src[i];

            //This allows a clamp operation directly
      if (f > oldmax) f = oldmax;
      else if (f < oldmin) f = (float)oldmin;

      f = ((f - oldmin)* finalrange) + newmin;
      if (f > newmax) f = newmax;
      else if (f < newmin) f = (float)newmin;
      target[i] = f;
      }

   return 1;
   }
   //------------------------------
template <class T>
int RESCALET<T>::Rescale(const T* buff, int size, unsigned char* newbuff, float newmin, float newmax)
   {
   float min, max;
   if (!MinMax(buff, size, &min, &max))
      return 0;

   return Convert(buff, size, min, max, newbuff, (int)newmin, (int)newmax);
   }
   //------------------------------
template <class T>
int RESCALET<T>::Rescale(const T* buff, int size, unsigned short* newbuff, float newmin, float newmax)
   {
   float min, max;
   if (!MinMax(buff, size, &min, &max))
      return 0;

   return Convert(buff, size, min, max, newbuff, (int)newmin, (int)newmax);
   }
   //------------------------------
template <class T>
int RESCALET<T>::Rescale(const T* buff, int size, float* newbuff, float newmin, float newmax)
   {
   float min, max;
   if (!MinMax(buff, size, &min, &max))
      return 0;

   return Convert(buff, size, min, max, newbuff, newmin, newmax);
   }
      //-----------------------------
template <class T>
int RESCALET<T>::Clamp(const T* src, int size, T* target, float lt, float lval, float ht, float hval)
   {
   register int i;
   for (i = 0; i < size; i++)
      {
      if (src[i] > ht)
         target[i] = (T)hval;
      else if (src[i] <lt)
         target[i] = (T)lval;
      else target[i] = src[i];
      }
   return 1;
   }
//---------------------------------------
         //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //      DRAW SIMPLE HISTOGRAM
         //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
int RESCALET<T>::DrawHistogram(int numin,  const T* input, int xnumout, int ynumout, T* output)
   {
   if(!input || !output) return 0;
   T min, max;
   min = max = input[0];
   register int i;
   for (i = 1; i < numin; i++)
      {
      if(input[i] > max) max = input[i];
      else if (input[i] < min) min = input[i];
      }
   return DrawHistogram(numin, min, max, input, xnumout, ynumout, output);
   }
         //-------------------------------------------------
template <class T>
int RESCALET<T>::DrawHistogram(int numin, T min, T max, const T* input, int xnumout, int ynumout, T* output)
   {
   if (xnumout<= 4 || ynumout <= 1 || numin <= 1) return 0;
   if(!input) return 0;
   if (min >= max) return 0;


            //clear the output image
   memset(output, 0, xnumout * ynumout * sizeof(T));

            //you need to scale the range to fit into the x axis of the second image
            //don't draw right to the ends of the image--too hard to see.
            //Go down to values of 0
   double range = (double)max;
   int nis = xnumout - 4;
   int* intsteps;
   try {intsteps = new int[nis];}
   catch(...) {std::cout<<"Out of mem in RESACLET::DrawHistogram\n"; return 0;}
   memset(intsteps, 0, nis * sizeof(int));

            //count the number of times a value from the first image falls
            //into any of these bins
   register int i;
   int j, ival;
   double val;
   for (i = 0; i < numin; i++)         //for each value in input image
      {
      val = ((double)input[i])/range;
      ival = (int)(val *(nis - 1) + 0.5);
      if (ival >= nis) ival = nis - 1;
      intsteps[ival] = intsteps[ival] + 1;
      }

            //You now need to scale the y axis. This gives the frequency
            //Assume the lowest frequency is 0
   range = intsteps[0];
   for (i = 1; i < nis; i++)
      if(range < intsteps[i]) range = intsteps[i];

            //Now draw lines in the ouput buffer. Max is a safe value to use.
            //Make the highest value only come 9/10 of the way up (easier to see).
            //Range is now the max number of steps. Want it to come close to top.
      for (i = 0; i <nis; i++)
         {
         val = (double)intsteps[i]/range;
         ival = (int)( val * (ynumout - 1) + 0.5);
         if (ival >= ynumout) ival = ynumout - 1;

         for (j = ynumout - 1; j >= ynumout -1 - ival; j--)
            output[j * xnumout + i+ 2] = max;
         }




   delete [] intsteps;
   return 1;
   }
                  //------------------------------------
                  //Put a buffer of one size into a buffer of another size.
                  //You may not shrink the target along one dimension and expand along
                  //another.
template <class T>
int RESCALET<T>::Resize
(const T* src, int sx, int sy, int sz, T* targ, int tx, int ty, int tz)
   {

      if (!src || !targ) return 0;

      if(sz <= 1) return Resize2D(src, sx, sy, targ, tx, ty);
      else return Resize3D(src, sx, sy, sz, targ, tx,ty,tz);
      }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PROTECTED
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


//----------------------------------------------
template <class T>
int RESCALET<T>::Resize2D(const T* src, int sx, int sy, T* targ, int tx, int ty)
   {
      //Get ratios of stepsizes. You can't divide by 0.....
   if (tx <= 0 || ty <= 0) return 0;

   float ratx = (float)sx/tx;      
   float raty = (float)sy/ty;
   
   register int x, y, ynewindexi;
   float xlocfp;
   float ylocfp;
   float leftup, rightup, leftdown, rightdown;
   int ylocupi, ylocdowni, xloclefti, xlocrighti;
   float xfrac, yfrac;
   float interpxup;
   float interpxdown;
   


            //First, find the floating point LOCATION of each voxel in the output image in terms
            //of its location in the source image. Then find the image INTENSITIES of the
            //integral bounding pixels in the source image. Interpolation gives the value of the
            //floating point location in the target image. This will only look at the 4 bounding
            //pixels. If you do a huge shrinkage you will lose tiny objects. 
   for (y = 0; y < ty; y++)      //for each target y
      {
      ynewindexi = y * tx;         //integer pixel LOCATION in target image
      ylocfp = y * raty;         //floating point LOCATION in source image
      ylocdowni = (int)(ylocfp);      //integer LOCATION of closest lower integer y in source image
      ylocupi = ylocdowni + 1;      //integer LOCATION of closest higher integer y in source image
      if(ylocupi >= sy) break;

      yfrac = ylocfp - ylocdowni;   //fractional position between integer y values in source image

      ylocdowni *= sx;               //location in a 1D array in source
      ylocupi *= sx;                  //location in a 1D array in source

      for (x = 0; x < tx; x++)      //for each target x
         {
         xlocfp = x * ratx;         //floating point LOCATION in source image
         xloclefti = (int)xlocfp;   //integer LOCATION of closest lower x in source image
         xlocrighti = xloclefti + 1; //integer LOCATION of cloest higher integer x in source image
         if (xlocrighti >= sx) break;

         xfrac = xlocfp - xloclefti;   //fractional position between integer x values

         leftdown = (float)src[ylocdowni + xloclefti];   //image INTENSITY in src lower left integer pixel
         rightdown = (float)src[ylocdowni + xlocrighti]; //image INTENSITY in src lower right integer pixel
         leftup = (float)src[ylocupi + xloclefti];         //image INTENSITY in src upper left integer pixel
         rightup = (float)src[ylocupi + xlocrighti];      //image INTENSITY in src upper right integer pixel.

         interpxdown = leftdown + xfrac * (rightdown - leftdown);   //interpolated intensity at botton of pixel
         interpxup = leftup + xfrac * (rightup - leftup);         //interpolated intensity at top of pixel

         targ[ynewindexi + x] = (T)(interpxdown + yfrac * (interpxup - interpxdown));
         }
      }


   return 1;
   }
//---------------------------------------------------
   //See comments in Resize2D for explanations....this just extends to 3D.
template <class T>
int RESCALET<T>::Resize3D(const T* src, int sx, int sy, int sz, T* targ, int tx, int ty, int tz)
   {

      //Get ratios of stepsizes. You can't divide by 0.....
   if (tx <= 0 || ty <= 0 || tz <= 0) return 0;

   float ratx = (float)sx/tx;      
   float raty = (float)sy/ty;
   float ratz = (float)sz/tz;
   
   register int x, y;
   int z;
   float xlocfp;
   float ylocfp;
   float zlocfp;
   int znewindexi, ynewindexi;
   int targetslicesize = tx * ty;
   int sourceslicesize = sx * sy;
   float xfrac, yfrac, zfrac;


   int zlocupi, zlocdowni, ylocupi, ylocdowni, xloclefti, xlocrighti;
   float leftup, rightup, leftdown, rightdown, front, back;
   float interpxup;
   float interpxdown;

   for (z = 0; z < tz; z++)
      {
      znewindexi = targetslicesize * z;      //integer 1D Z offset in target
      zlocfp = ratz * z;                     //fp offset in source
      zlocdowni = (int) zlocfp;               //smaller integer z offset in source
      zlocupi = zlocdowni + 1;               //larger integer z offset in source
      if (zlocupi >= sz) break;

      zfrac = zlocfp - zlocdowni;            //fractional position along z

      zlocdowni *= sourceslicesize;            //z offsets as 1D arrays
      zlocupi *= sourceslicesize;

      for (y = 0; y < ty; y++)      //for each target y
         {
         ynewindexi = znewindexi + y * tx;         //integer pixel LOCATION in target image
         ylocfp = y * raty;            //floating point LOCATION in source image
         ylocdowni = (int)(ylocfp);      //integer LOCATION of closest lower integer y in source image
         ylocupi = ylocdowni + 1;      //integer LOCATION of closest higher integer y in source image
         if(ylocupi >= sy) break;

         yfrac = ylocfp - ylocdowni;   //fractional position between integer y values in source image

         ylocdowni *= sx;               //location in a 1D array in source
         ylocupi *= sx;                  //location in a 1D array in source




         for (x = 0; x < tx; x++)      //for each target x
            {
            xlocfp = x * ratx;         //floating point LOCATION in source image
            xloclefti = (int)xlocfp;   //integer LOCATION of closest lower x in source image
            xlocrighti = xloclefti + 1; //integer LOCATION of cloest higher integer x in source image
            if (xlocrighti >= sx) break;

            xfrac = xlocfp - xloclefti;   //fractional position between integer x values

                        //Get the greyscale value for the center of the front face
            leftdown = (float)src[zlocdowni + ylocdowni + xloclefti];   //image INTENSITY in src lower left integer pixel
            rightdown = (float)src[zlocdowni + ylocdowni + xlocrighti]; //image INTENSITY in src lower right integer pixel
            leftup = (float)src[zlocdowni + ylocupi + xloclefti];         //image INTENSITY in src upper left integer pixel
            rightup = (float)src[zlocdowni + ylocupi + xlocrighti];      //image INTENSITY in src upper right integer pixel.

            interpxdown = leftdown + xfrac * (rightdown - leftdown);   //interpolated intensity at botton of pixel
            interpxup = leftup + xfrac * (rightup - leftup);         //interpolated intensity at top of pixel
            front = interpxdown + yfrac * (interpxup - interpxdown);

                           //Get the greyscale value for center of back face.
            leftdown = (float)src[zlocupi + ylocdowni + xloclefti];   //image INTENSITY in src lower left integer pixel
            rightdown = (float)src[zlocupi + ylocdowni + xlocrighti]; //image INTENSITY in src lower right integer pixel
            leftup = (float)src[zlocupi + ylocupi + xloclefti];         //image INTENSITY in src upper left integer pixel
            rightup = (float)src[zlocupi + ylocupi + xlocrighti];      //image INTENSITY in src upper right integer pixel.

            interpxdown = leftdown + xfrac * (rightdown - leftdown);   //interpolated intensity at botton of pixel
            interpxup = leftup + xfrac * (rightup - leftup);         //interpolated intensity at top of pixel
            back = interpxdown + yfrac * (interpxup - interpxdown);


            targ[ynewindexi + x] = (T)(front + zfrac * (back - front));
            }
         }
      }

   return 1;
   }

}//end of namespace

#endif