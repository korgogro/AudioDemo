/*
  ==============================================================================

    ADSRenv.h
    Created: 28 Dec 2015 5:05:15pm
    Author:  ignasi

  ==============================================================================
*/

#ifndef ADSRENV_H_INCLUDED
#define ADSRENV_H_INCLUDED


/*    .h    ================ */

class ADSRenv {
    
public:

    ADSRenv(){
        
        amplitude=releaselevel=0.;
        counter=0;
        trigger=status=false;
        max_env_attack = max_env_decay = max_env_release = 10.;
        attack = 1000;
        decay = 50;
        sustain = 1.;
        release = 50000;
        sampleRate = 44100;
        
    }
    
    ~ADSRenv(){
    };
    
    bool envStatus(){
        return status;
    }
    
    void setSampleRate(double _sampleRate){
        sampleRate = _sampleRate;
    }
    
    void setAttack(double _attack){
        attack = int ( _attack * sampleRate * max_env_attack);
    }
    
    void setDecay(double _decay){
        decay = int ( _decay * sampleRate * max_env_decay);
    }
    
    void setSustain(double _sustain){
        sustain = _sustain;;
    }
    
    void setRelease(double _release){
        release = int ( _release * sampleRate * max_env_decay);
    }
    
    void triggerOn(){
        trigger=status=true;
        counter=0;
        amplitude=0.;
    };
    
    void triggerOff(){
        trigger=false;
        releaselevel=amplitude;
    };
    
    
    double getCurve(){
        
        // this is a complete naïve aproach : sure the logic is incomplete, but it seems to work ... :)
        
        
        if (trigger==false)
        {
            if (amplitude>0)
            {
                if(release>0)
                {
                    amplitude-=double(releaselevel/release);
                }
                else
                {
                    amplitude=0.; counter=0.;
                }
            }
            else
            {
                amplitude=0.;
                counter=0;
                status= false;
            }
        }
        
        if(trigger==true)
        {
            
            if ( counter<=attack)
            {
                if(attack>0)
                {
                    amplitude+=1./attack;
                }
                else
                {
                    amplitude=1.;
                }
            }
            
            if ( amplitude>=1.)
            {
                amplitude=1.;
            }
            
            if ( counter>attack && counter<=(attack+decay))
            {
                if (decay>0)
                {
                    amplitude-=(1.-sustain)/decay;
                }
                else
                {
                    amplitude=sustain;
                }
            }
            
            if ( counter>(attack+decay))
            {
                amplitude=sustain;
            }
            
            
            counter++;
            
            
        }
        
        return amplitude;
        
    }
    
    
private :
    
    double sampleRate, amplitude, releaselevel, max_env_attack, max_env_decay, max_env_release;
    long unsigned int counter;
    
    bool trigger, status;
    
    long unsigned int attack; // in samples for internal calc.
    long unsigned int decay; // in samples for internal calc.
    double sustain; // 0. ~ 1. this is a level metrical
    long unsigned int release; // in samples for internal calc.
    
};



#endif  // ADSRENV_H_INCLUDED
