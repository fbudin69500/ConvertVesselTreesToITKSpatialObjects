#include "ebTBOX_s.h"


namespace eb
{

   //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   // PUBLIC FUNCTIONS
   //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

TBOX_s::TBOX_s()
{
    err=&std::cerr;
}

void TBOX_s::SetErrorStream(std::ostream &_err)
{
    err=&_err;
}



   //*******************************
   //returns first instance of target STRING. Nondestructive of
    //original data.
int TBOX_s::Instring(std::string originalsource, std::string ctarget)
{  
   return originalsource.find(ctarget,0);                        
}

        //********************************
        //returns last instance of target CHAR. Use for things like finding
        //a filename after a directory listing separated by '/'. Nondestructive
        //of original data.
int TBOX_s::Instringlastchar(std::string source, char target)
{
    return source.find_last_of(target,source.size()-1);
}

        //************************
        //Similar to strncpy except that the target is guaranteed null-
        //terminated. Does not affect source. Note that a length of 5 will
        //copy only 4 chars, with the last space reserved for '\0'.  
std::string TBOX_s::Strncopy(std::string &dest, std::string src, int len)
{
    dest=src;
    dest.resize(len);
    return dest;
}
       //*********************************
       //Convert a string to lower or upper case. Will not affect numbers.
       //The original string is modified here so send a copy if you want
       //to keep the original.

bool TBOX_s::Lowercase(std::string &s)
{
  if(s.empty()) return false;
 
  for (unsigned int x = 0; x<s.size(); x++)
    {
    if (s.at(x) >='A' && s.at(x)<='Z')
       s.at(x) = (char)(s.at(x)+32);
    }
  return true;
}

bool TBOX_s::Uppercase(std::string &s)
{
    if(s.empty()) return false;

    for (unsigned int x = 0; x<s.size(); x++)
    {
        if(s.at(x) >='a' && s.at(x)<='z')
            s.at(x) = (char) (s.at(x) -32);
    }
    return true;
}
   //***************************
   //Find numerical info separated by spaces
int TBOX_s::Stringto2ints(std::string stringline, int &cubex, int &cubey)
{
    try
    {
        int count=0;
        int count2=0;

        while (isspace(stringline.at(count))) ++count;
        count2 = count;
        while(!isspace(stringline.at(count2))) ++count2;
        std::string temp;
        temp.assign(stringline,count,count2-count);
        std::istringstream i(temp);
        i.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
        i>>cubex;

        count = count2+1;
        while (isspace(stringline.at(count))) ++count;
        count2 = count;
        while(count2!=stringline.size() && !isspace(stringline.at(count2)))
        {++count2;}
        temp="";
        temp.assign(stringline,count,count2-count);
        i.clear(std::stringstream::goodbit);
        i.str(temp);
        i>>cubey;
        return 1;
    }
    catch(...)
    {
        (*err)<<"error; impossible to extract 2 integers from the given string"<<std::endl;
        return 0;
    }
}

int TBOX_s::Stringto3ints
(std::string stringline, int &cubex, int &cubey,int &cubez)
{
    try
    {
        int count=0;
        int count2=0;

        while (isspace(stringline.at(count))) ++count;
        count2 = count;
        while(!isspace(stringline.at(count2))) ++count2;
        std::string temp;
        temp.assign(stringline,count,count2-count);
        std::istringstream i(temp);
        i.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
        i>>cubex;
        count = count2+1;
        while (isspace(stringline.at(count))) ++count;
        count2 = count;
        while(!isspace(stringline.at(count2))) ++count2;
        temp="";
        temp.assign(stringline,count,count2-count);
        i.clear(std::stringstream::goodbit);
        i.str(temp);
        i>>cubey;
        count = count2+1;
        while (isspace(stringline.at(count))) ++count;
        count2 = count;
        while(count2!=stringline.size() && !isspace(stringline.at(count2)))
        {++count2;}
        temp="";
        temp.assign(stringline,count,count2-count);
        i.clear(std::stringstream::goodbit);
        i.str(temp);
        i>>cubez;
        return 1;
    }
    catch(...)
    {
        (*err)<<"error; impossible to extract 3 integers from the given string"<<std::endl;
        return 0;
    }
}

      //***************************
      //Here is a more general function
template<class T>
bool TBOX_s::StringtonTs(std::string s, int n, std::vector<T> &d)
{
    try
    {
        d.clear();
        bool returnval = true;
        unsigned int count=0;
        unsigned int count2=0;
        std::string temp;
        T value;
        std::istringstream stream;
        stream.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
        for(int i=0;i<n;i++)
        {
            while(s.at(count)!='\n' && isspace(s.at(count)))
            {
                count++;
            }
            if(s.at(count)=='\n')
            {throw "error";}
            count2=count;
            while(count2<s.size() && !isspace(s.at(count2)))
            {
                count2++;
            }
            if(count2==s.size())
            {
                if(i<n-1) returnval=false;
            }
            temp="";
            temp.assign(s,count,count2-count);
            stream.clear(std::stringstream::goodbit);
            stream.str(temp);
            stream>>value;
            d.push_back(value);
            if(!returnval) {throw "error";}
            count=count2+1;
        }
        return returnval;
    }
    catch(...)
    {
        //(*err)<<"error; impossible to extract "<< n <<" values from the given string"<<std::endl;
        return 0;
    }
}


bool TBOX_s::Stringtondoubles(std::string s, int n, std::vector<double> &d)
{
    return StringtonTs<double>(s,n,d);
}
//***************************
//Here is a more general function for ints
bool TBOX_s::Stringtonints(std::string s, int n, std::vector<int> &d)
{
    return StringtonTs<int>(s,n,d);
}
//---------------------------------------------------
//same as above for float
bool TBOX_s::Stringtonfloats(std::string s, int n, std::vector<float> &d)
{
    return StringtonTs<float>(s,n,d);
}

  //-------------------------------------------------------------
  //Extract an int from a string, letting the caller know where the pointer
  //is now for extraction of something else from the same string
template<class T>
bool TBOX_s::GetT(std::string s, unsigned int &offset, T &val)
{
    try{
        int count=offset;
        unsigned int count2=0;
        while (isspace(s.at(count)))
        {
            count++;
            offset++;
            if (count==s.size()) {return false;}
        }
        count2 = count;

        while (count2<s.size() && !isspace(s.at(count2)))
        {
            count2++;
            offset++;
        }

        //if you are at the end of string do not increment offset.
        //If you are not at string end, put offset to next position.
        if (count2!=s.size()) offset++;
        std::string temp;
        temp.assign(s,count,count2-count);
        std::istringstream stream(temp);
        stream.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
        stream>>val;
        return true;
    }
    catch(...)
    {
        (*err)<<"error; impossible to extract a value from the given string at this offset"<<std::endl;
        return 0;
    }
}




bool TBOX_s::GetInt(std::string s, unsigned int &offset, int &val)
{
    return GetT<int>(s,offset,val);
}
//-------------------------------------------------------
//Same as above but for float
bool TBOX_s::GetDouble(std::string s, unsigned int &offset, double &val)
{
    return GetT<double>(s,offset,val);
}
//---------------------------------------------------------
bool TBOX_s::GetFloat(std::string s, unsigned int &offset, float &val)
{
    return GetT<float>(s,offset,val);
}

   //----------------------------------
         //Look for a char string with whitespace on the far end ( a "word"
         //in the literary sense). The function is not destructive of the
         //original string and returns only the offset of that space in t
bool TBOX_s::GetWord(std::string s, unsigned int &t)
{
    std::string dummy;
    return GetWord(s,t,dummy);
}

bool TBOX_s::GetWord(std::string s, unsigned int &t, std::string &out)
{
    try{
        unsigned int count=t;
        while(isspace(s.at(count)))
        {
            ++count;
            if (count==s.size()) return false;
            t++;
        }
        while(t<s.size() && !isspace(s.at(t)))
        {
            t++;
        }
        std::string temp;
        temp.assign(s,count,t-count);
        out = temp;
        //out.clear();
        //out.assign(s,count,t-count);
        return true;
    }
    catch(...)
    {
        (*err)<<"error; impossible to extract a word from the given string at the given position"<<std::endl;
        return 0; 
    }
}


   //---------------------------
         //Strip leading whitespace from a char string. Nondestructive
bool TBOX_s::Strip(std::string a, unsigned int& offset)
   {
       std::string dummy;
       return Strip(a,offset,dummy);
   }

bool TBOX_s::Strip(std::string a, unsigned int& offset,std::string &out)
   {
   if ( a.size()<=offset ) return false;
   while(isspace(a.at(offset))) { offset++; }
   std::string temp;
   temp.assign(a,offset,a.size()-offset);
   out = temp;
   //out.clear();
   //out.assign(a,offset,a.size()-offset);
   return true;
   }


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
               //will return string to n but begin at a delimiter
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
template<class T>
bool TBOX_s::StringtonTs(std::string s, int numpts, std::vector<T> &pts, char delim)
{
    try{
        unsigned int cptr = Instringlastchar(s, delim);
        if (cptr==-1)
            return false;
        //advance beyond the target
        ++cptr;
        if (cptr>=s.size())
            return false;
        std::string temp;
        temp.assign(s,cptr,s.size()-cptr);
        return StringtonTs(temp, numpts, pts);
    }
    catch(...)
    {
        (*err)<<"Error while extracting values form the given string"<<std::endl;
        return 0;
    }
}


bool TBOX_s::Stringtondoubles(std::string s, int numpts, std::vector<double> &pts, char delim)
{
    return StringtonTs<double>(s, numpts, pts , delim);
}



bool TBOX_s::Stringtonfloats(std::string s, int numpts, std::vector<float> &pts, char delim)
{
    return StringtonTs<float>(s, numpts, pts , delim);
}

bool TBOX_s::Stringtonints(std::string s, int numpts, std::vector<int> &pts, char delim)
{
    return StringtonTs<int>(s, numpts, pts , delim);
}

}//end of namespace