#ifndef _ebSTAT_cpp_
#define _ebSTAT_cpp_

#include "ebSTAT.h"

#include <math.h>       //sqrt

namespace eb
{

         //Constructor
template <class T>
STAT<T>::STAT()
   {
   ONEOVERSQRT2PI = 1.0/(sqrt(2 * eb::Pi));
   }

      //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
            //FUNCTIONS--UTILITIES
      //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //Get the factorial of a positive int
template <class T>
int STAT<T>::Factorial(int val)
   {
   if (val == 0) return 1;  
   for (int i = val - 1; i> 1; i--) 
      val = val * i;
   return val;
   }   
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //FUNCTIONS: SIMPLEST BASE TOOLS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //Get the min and max. Multiple dimensional points are supported.
         //Mins and maxes returned in mins, maxs
template <class T>
int STAT<T>::MinMax(int num, int dims, T* input, double* mins, double* maxs)
   {
   if (!input) return 0;
   int i, j, index;
   T val;

   for(i = 0; i < dims; i++)
      mins[i] = maxs[i] = input[i];

   for (i = 1; i < num; i++)
      {
      index = i * dims;
      for (j = 0; j < dims; j++)
         {
         val = input[index + j];

         if (val > maxs[j])
            maxs[j] = val;
         else if (val < mins[j])
            mins[j] = val;
         }
      }
   return 1;
   }
//----------------------------------------------------
         //Get the mean. Multiple dimensional points are supported. Mean for each
         //axis are returned in m. The function returns 0 on failure.
template <class T>
int STAT<T>::Mean(int num, int dims, T* input, double* m)
   {
   if(num <= 0 || !input || dims <= 0) return 0;

   int i, d, index;

   for (i = 0; i < dims; i++)
      m[i] = 0;

         //Add each x,y,z value to their respective bins
   for (i = 0; i < num; i++)
      {
      index = i * dims;


      for (d = 0; d< dims; d++)
           {
         m[d] += (double)(input[index + d]);
           }
      }
  
  
         //divide each x,y,z value by the number of items
   for (i= 0; i < dims; i++)
      m[i] = m[i]/num;

 

   return 1;
   }
//---------------------------------------
         //Get the median. Multiple dimensional points are supported. Mean for each
         //axis are returned in m. The function returns 0 on failure.
template <class T>
int STAT<T>::Median(int num, int dims, T* input, double* m)
   {
   double* min;
   double* max;
   try
      {
      min = new double[dims];
      max = new double[dims];
      }
   catch
      {
      std::cerr<<"Memory allocation failure in STAT::Median\n";
      return 0;
      }


   if(!MinMax(num, dims, input, min, max))
      {delete [] min; delete[] max;return 0;}

   register int i , j;
   int ival, counter;
   int boxnum = 256;
   int btop = boxnum - 1;
   double* binbox;
   try {binbox = new float[boxnum];   }   //arbitrarily divide up into 256 bins
   catch {std::cout<<"Out of mem in STAT\n"; return 0;}
   memset(binbox, 0, boxnum * sizeof(double));

   double range, val;

   for (i = 0; i < dims; i++)      //for each dimension
      {
      range = max[i] - min[i];
      memset(binbox, 0, boxnum * sizeof(double));

      for (j = 0; j < num; j++)
         {
         val = input[j* dims + i];
         val = (val - min[i])/range;

         ival = (int)(val * btop + 0.5);
         if (ival > btop) ival = btop;
         binbox[ival] = binbox[ival] + 1;
         }

               //now figure out where the median is
      counter = 0;
      for (j = 0; j < boxnum; j++)
         {
         counter += binbox[j];
         if(counter >= num/2)
            {
            m[i] = min[i] + (range * j/(double)btop);
            break;
            }
         }      
      }
   delete [] min;
   delete [] max;
   delete [] binbox;
   return 1;
   }

   //----------------------------------------
         //Get the mean and the variance in each dimension
template <class T>
int STAT<T>::Variance(int num, int dims, T* input, double* mean, double* var)
   {
   if (num - 1 <= 0) return 0;
   if (!Mean(num, dims, input, mean)) return 0;

   int i, j, index;

   for (i = 0; i < dims; i++)
      var[i] = 0;

   for (i = 0; i < num; i++)
      {
      index = i * dims;
      for (j = 0; j< dims; j++)
         var[j] += (input[index + j] - mean[j]) * (input[index + j] - mean[j]);
      }
      
   for (i = 0; i < dims; i++)
      var[i] = var[i]/(num - 1);
   

         //return degree of freedom
   return num - 1;
   }

//----------------------------------------
         //Get the mean and standard deviation along each axis
template <class T>
int STAT<T>::StdDev(int num, int dims, T* input, double* mean, double* var)
   {
   if (!Variance(num, dims, input, mean, var)) {*mean = 0; *var = 0; return 0;}

   for (int i = 0; i < dims; i++)
       {
      var[i] = sqrt(var[i]);
       }

   return num - 1;
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PROBABILITY DISTRIBUTIONS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //probability of x successes in n trials with prob chance of success
         //each trial (n) q ^(n - x) p^ x
         //                  x
template <class T>
double STAT<T>::Binomial(int n, int x, double prob)
   {
   int combinations = Combination(n, x);         //number of ways x can be in n trials
   double chance = Power(1 - prob, n - x) * Power(prob, x);
   return combinations * chance;
   }
//------------------------------------------
      //probability that x random events will occur in an interval when
      //the average number of such events in an interval is lambda
      //f(x) = (e ^ (-l) * l ^ x)/x!
template <class T>
double STAT<T>::Poisson(int lambda, int x)
   {
   double exp(-lambda) * pow(lambda, x);
   double denominator = (double) Factorial(x);
   return numerator/denominator;
   }


   //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //PROTECTED
   //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
}//end of namespace

#endif