#ifndef _ebSTAT_h
#define _ebSTAT_h

/* This class provides a very simple set of statistical tools. T is the 
datatype, and it may be of dims dimensions. As the data type is unknown,
numerical answers are generally returned in the form of doubles. 
When it makes sense to do so, returnval is the degree of freedom. A returnval
of 0 means failure. Gaussians are in CONVOLVE


  moved to std namespace 6/02
*/

#include "ebINCLUDES.h"      //define std namespace or not
#include "ebDEFINE.h"
//#include "MemoryLeak.h"
namespace eb
{

template <class T>
class STAT
   {
   public:
      STAT();   

                  //utilites used internally; might as well make available
        int Factorial(int val);         

                  //Get the mean, or the mean + variance, or the mean + stdev
                  //from a set of inpput data of dims dimensions and num points.
      int MinMax(int num, int dims, T* input, double* mins, double* maxs);
      int Mean(int num, int dims, T* input, double* m);
      int Median(int num, int dims, T* input, double* m);
      int Variance(int num, int dims, T* input, double* mean, double* var);
      int StdDev(int num, int dims, T* input, double* mean, double* stdev);


                  //Simple permutation, combination
      int Permutation(int numobjects, int atatime)      //order important (AB != BA)
         {return Factorial(numobjects)/Factorial(numobjects - atatime);}

      int Combination(int numobjects, int atatime)    //order irrelevant (AB == BA)
         {return Permutation(numobjects, atatime)/Factorial(atatime);}


                  //Simple discretized probability distributions.
                        //Binomial (Bernoulli): probability of achieving x successes
                        //in n trials, with the probability of success of any trial = p.
      double Binomial(int n, int x, double p);

                        //Poisson; probability that x events will occur in a
                        //given interval when lambda is the average number of
                        //occurrences per interval
                        //f(x) = e (power -lambda) * lambda (power x)/x!
      double Poisson(int lambda, int x);

      protected:
         double ONEOVERSQRT2PI;

         float* kernel;
      

   };

}//end of namespace

#include "ebSTAT.cpp"


#endif