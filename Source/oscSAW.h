/*
  ==============================================================================

    oscSAW.h
    Created: 7 Jan 2016 6:22:22pm
    Author:  ignasi

  ==============================================================================
*/

#ifndef OSCSAW_H_INCLUDED
#define OSCSAW_H_INCLUDED


class oscSAW
{
    
    public :
    
    oscSAW(){
        phaseSAW=0.0;
        freqSAW=100.;
        carrier=-1.;
        sampleRate= 44100.;
    }
    
    ~oscSAW(){
    }
    
    void setFrequency(double _frequency){
        freqSAW = _frequency;
        if(freqSAW==0.)freqSAW=0.0001;
    }
    
    void setSampleRate(double _sampleRate){
        sampleRate = _sampleRate;
    }
    
    double getSignal(){
        
        //Sawtooth generator. This is like a phasor but goes between -1 and 1
    
        carrier+=phaseSAW;
        if ( phaseSAW >= 1.0 ) phaseSAW -= 2.0;
        phaseSAW += double(freqSAW/sampleRate);
        
        return (carrier);
        
    }
    
    double getFrequency(){
        return freqSAW;
    }
    
    void reset(){
        phaseSAW = 0.0;
        carrier = -1.;
        freqSAW=100.0;
        
    }

        
    
    private :
    
       double carrier, phaseSAW, freqSAW, sampleRate;
    
};




#endif  // OSCSAW_H_INCLUDED
