
/*
  ==============================================================================

    oscSQU.h
    Created: 27 Dec 2015 2:49:16pm
    Author:  ignasi

  ==============================================================================
*/

#ifndef OSCSQU_H_INCLUDED
#define OSCSQU_H_INCLUDED



/*    .h   ============================*/

class oscSQU
{
    
    public :
    
    oscSQU(){
        phaseSQU=0.0;
        freqSQU=100.;
        carrier=-1.;
        sampleRate= 44100.;
    }
    
    ~oscSQU(){
    }
    
    void setFrequency(double _frequency){
        freqSQU = _frequency;
    }
    
    void setSampleRate(double _sampleRate){
        sampleRate = _sampleRate;
    }
    
    double getSignal(){
        
        if(phaseSQU<0.5)
        {
            carrier = -1.;
        }
        
        if(phaseSQU>=0.5)
        {
            carrier = 1.;
        }
        
        phaseSQU += ( freqSQU / sampleRate );
        
        if(phaseSQU >= 1.) phaseSQU = 0.0;
        
        return carrier;
        
    }
    
    double getFrequency(){
        return freqSQU;
    }
    
    void reset(){
        phaseSQU = 0.0;
        carrier = -1.;
        freqSQU=0.0;
        
    }

        
    
    private :
    
       double carrier, phaseSQU, freqSQU, sampleRate;
    
};





#endif  // OSCSQU_H_INCLUDED