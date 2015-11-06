#ifndef VesselsClass_h
#define VesselsClass_h

#include <ebVESSCOL.h>         //vessel collection
#include <ebVESSCOLUTILS.h>

class VesselsClass
{
public:
    VesselsClass();
    void SetFileName(const char* filename) {m_FileName.assign(filename); m_Modified = 1; };
    void SetConcatenate(bool concatenate) { m_Concatenate = concatenate; m_Modified = 1;}
    void SetResample(bool resample) { m_Resample = resample; m_Modified = 1;}
    void SetReconnect(bool reconnect) {m_Reconnect = reconnect; m_Modified = 1;}
    eb::VESSCOL GetOutput()  const;//Do not update automatically the vessels;
    void SetSampling(unsigned int sampling) {m_Sampling = sampling; m_Modified = 1;}
    void Update();
    bool IsValid() const;
private:
    void ConcatenateVessels(int par, int child, eb::VESSCOL* tempvc);
    //void RecalcLists();
    int Concatenate();
    void Resample();
    void Reconnect();
    std::string m_FileName;
    bool m_Concatenate;
    bool m_Reconnect;
    bool m_Resample;
    unsigned int m_Sampling;
    eb::VESSCOL vc;
    bool m_Modified;
    bool m_IsValid;
};

#endif
