/*
  ==============================================================================

    filter1.h
    Created: 4 Jan 2016 1:11:37pm
    Author:  ignasi

  ==============================================================================
*/


#ifndef FILTER1_H_INCLUDED
#define FILTER1_H_INCLUDED


class filter1 {
 
public :

    filter1(){
        
        declick.setAttack(DECLICK_LONG);
        declick.setRelease(DECLICK_LONG);
        declick.trigger_on();
        samplingRate = 44100.;
        cutoffModSignal = 0.;
        cutoffModDepth = 0.;
        declick.setSamplingRate(samplingRate);
        freq_max=samplingRate/6.;
        freq_min=40.;
        ln_freq_max=log(freq_max);
        ln_freq_min=log(freq_min);
        exp_freq=CUTOFF=440.;
        
        resonance=QFACTOR=1.;
        x1=y1=z1=c1=x2=y2=z2=c2=r1=r2=0.;
        
        AudioOut = 0.;
        
        isPlaying=true;
    };
    
    ~filter1(){};
    
    void setSamplingRate ( double _samplingRate ){
        samplingRate = _samplingRate;
        freq_max=samplingRate/6.;
        declick.setSamplingRate(samplingRate);
    };
    
    void setPlay(bool _isPlaying){
        // ON-OFF (bypass)
        isPlaying = _isPlaying;
        if(isPlaying==true)  declick.trigger_on();
        if(isPlaying==false) declick.trigger_off();

    };

    void setFrequency(double _value){
        // _value is in 0-1 range.
        exp_freq = pow(3., (_value * (ln_freq_max-ln_freq_min)) + ln_freq_min ) - 17.5497;
        
        if (exp_freq < freq_min){exp_freq = freq_min;}
        if (exp_freq > freq_max){exp_freq = freq_max;}
        
        smoothFreq.setNewValue(exp_freq);
    };

    void setResonance(double _value){
        //_value from 0. to 1.
        resonance=pow(7.*_value,2);
        if (resonance<1.) resonance = 1.;
        smoothResonance.setNewValue(resonance);
    };

    void patchCutoffModSignal(double _cutoffModSignal){
        cutoffModSignal=_cutoffModSignal ;
    };

    void setCutoffModDepth(double _cutoffModDepth){
        cutoffModDepth = _cutoffModDepth;
        smoothCutoffModDepth.setNewValue(cutoffModDepth);
    };


double getAudioOut(double _input){
       
        const double AudioIn = _input;
        const double declicker = declick.ar();
        const double frequency = smoothFreq.getValue();
        
        cutoffModSignal*=smoothCutoffModDepth.getValue();
        
        if(cutoffModSignal>=0)
        {
            CUTOFF=frequency+(freq_max-frequency)*cutoffModSignal;
        } else {
            CUTOFF=frequency+(frequency-freq_min)*cutoffModSignal;
        }
        
        QFACTOR=smoothResonance.getValue();
        
        // z1=cos(TWOPI*CUTOFF/SAMPLING_RATE);
        
        double phase, remainder;
        phase=fmod(512.*CUTOFF/samplingRate, 512.);
        remainder= phase - floor(phase);
        z1 =
            (1-remainder) * table.cosBuf512 ( (int) (1 + phase) )
            +
            remainder * table.cosBuf512 ( (int) (2 + phase ) );
        
        c1=2-2*z1;
        r1=(TWOSQRT*sqrt(-pow((z1-1.0),3.0))+QFACTOR*(z1-1))/(QFACTOR*(z1-1));
        x1=x1+(AudioIn-y1)*c1;
        y1=y1+x1;
        x1=x1*r1;
        AudioOut=(-y1);

        /* crossfade  */
        AudioOut = AudioIn * (1.-declicker) + AudioOut * declicker;
        
        return AudioOut;
    }

private:

    double exp_freq, freq_max, ln_freq_max, freq_min, ln_freq_min, resonance;
    double AudioOut;
    double CUTOFF, QFACTOR;
    LookUpTable table;

    bool isPlaying;
    
    avWIN declick;
    ParamSmooth smoothFreq, smoothResonance, smoothCutoffModDepth;

    double x1,x2;//speed
    double y1,y2;//pos
    double z1,z2;//pole
    double c1,c2,r1,r2;//filter coefficient
    
    double samplingRate;

    /* patches */
    double cutoffModSignal;
    double cutoffModDepth;

};

#endif  // FILTER1_H_INCLUDED
