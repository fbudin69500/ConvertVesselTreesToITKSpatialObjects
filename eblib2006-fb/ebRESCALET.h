#ifndef _eb_RESCALET_h_
#define _eb_RESCALET_h_

/* Used for rescaling image intensities (generally to range 0-255)

  revised 6/02 to put into std namespace

  Elizabeth Bullitt
*/

#include "ebINCLUDES.h"      //define std namespace or not
//#include "MemoryLeak.h"
namespace eb
{

template <class T>
class RESCALET
   {
   public:

            //Get the min and a max of a buffer
      int MinMax(const T* buff, int size, float* min, float* max);

                  //convert a buffer of one type into one of unsigned char or
                  //unsigned short
            //this is nondestructive of original data. It is the caller's
            //responsibility to be sure the buffers are the same size
            //and to determine the minmax of the source buffer in advance.
      int Convert(const T* buff, int size, float oldmin, float oldmax,\
unsigned char* newbuff, int newmin, int newmax);
      int Convert(const T* buff, int size, float oldmin, float oldmax,\
unsigned short* newbuff, int newmin, int newmax);
      int Convert(const T* buff, int size, float oldmin, float oldmax, \
         float* newbuff, float newmin, float newmax);

               //Do both steps
      int Rescale(const T* buff, int size, unsigned char* newbuff, float newmin, float newmax);
      int Rescale(const T* buff, int size, unsigned short* newbuff, float newmin, float newmax);
      int Rescale(const T* buff, int size, float* newbuff, float newmin, float newmax);

               //Clamp upper level to some value (all above will be set to val)
               //or clamp lower (all below will be set to val). If you
               //want to act on the same buffer, send the same one as src
               //and target.
      int Clamp(const T* src, int size, T* target, float lthresh, float lval, float hthresh, float hval);


                     //--------------------------------
                           //Draw a frequency histogram into a second image. This will only
                  //evaluate one dimensional values and expects the second buffer
                  //to be 2 dimensional. The first buffer can be of any dimension,
                  //but there must be only a single dimensional value at each place.
                  //If you already know the min and max vals, the function will go 
                  //faster.
int DrawHistogram(int numin,  const T* input, int xnumout, int ynumout, T* output);
int DrawHistogram(int numin, T minin, T maxin, const T* input, int xnumout, int ynumout, T* output);


            //------------------------------------
            //Put a buffer of one size into a buffer of another size and of same type.
            //You may not shrink the target along one dimension and expand along
            //another--if you need to do this you will need to call this function
            //twice. 
                  
   int Resize
(const T* src, int sx, int sy, int sz, T* targ, int tx, int ty, int tz);

                        //used in interpolating between buffers of different size
         int Resize2D(const T* src, int sx, int sy,T* targ, int tx, int ty);
         int Resize3D(const T* src, int sx, int sy, int sz, T* targ, int tx, int ty, int tz);

   




   };

}//end of namespace

#include "ebRESCALET.cpp"

#endif

            