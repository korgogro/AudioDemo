/*
  ==============================================================================

    dspTools.h
    Created: 4 Jan 2016 1:26:46pm
    Author:  ignasi

  ==============================================================================
*/

#ifndef DSPTOOLS_H_INCLUDED
#define DSPTOOLS_H_INCLUDED

#ifndef DECLICK_SHORT
#define DECLICK_SHORT 0.010
#endif

#ifndef DECLICK_LONG
#define DECLICK_LONG  0.150
#endif

#ifndef DECLICK_EXTRA_LONG
#define DECLICK_EXTRA_LONG  0.500
#endif

#ifndef TWOSQRT
#define TWOSQRT 1.4142135623730950488016887242096
#endif




/* // a simple attack-release linear ramp window to be used as declicker */

class avWIN {
    
public:
    
    avWIN(){
        
        samplingRate = 44100.;
        
        amplitude=0.;
        attackphase=releasephase=false;
        
        trigger=false;
        attack=0.001;
        release=0.001;
        output=0.;
    }
    
    ~avWIN(){}
    
    void setSamplingRate ( double _samplingRate){
        
    }
    
    void setAttack(double _attack){
        attack=1/(samplingRate*((.5 + pow(32. * _attack, 2))/1000.0));
    }
    
    void setRelease(double _release){
        release=1/(samplingRate*((.5 + pow(32. * _release, 2))/1000.));
    }
    
    
    double ar(){
        
        if ( trigger==true && attackphase==false )
        {
            releasephase=false;
            attackphase=true;
        }
        
        if (amplitude>=1.0)
        {
            amplitude=1.0;
        }
        
        if (attackphase==true)
        {
            if(attack>0)
            {
                output=( amplitude += attack );
            }
            else
            {
                output=1.0;
            }
            
        }
        
        if (trigger==false && releasephase==false )
        {
            attackphase=false;
            releasephase=true;
        }
        
        if (releasephase==true && amplitude>0.)
        {
            output=( amplitude -= release );
        }
        
        if (amplitude<=0.)
        {
            amplitude=0.;
            output=0.;
        }
        
        return output;
        
    } // attack release
    
    
    void trigger_on(){
        trigger=true;
    }
    
    void trigger_off(){
        trigger=false;
    }
    
private:
    
    double samplingRate;
    double amplitude;
    bool attackphase,releasephase;
    
    bool trigger;
    
    double attack; //in ms
    double release; //in ms
    double output;
    
};

class DCremoval{
    /*
     from: Yehar's digital sound processing tutorial for the braindead!
     version 2003.01.21
     http://www.iki.fi/o/dsp/dspstuff.txt
     
     *** DC removal ***
     Filters 0 Hz completely away, does not attenuate basses above 5Hz in
     a 44100Hz sampling rate system. This is a realtime routine. For
     example in sample editors, subtracting the average of all the
     samplepoints from the whole sampledata usually does better job.
     
     Init:
     pos = 0
     speed = 0
     
     Loop:
     speed = speed + (input(t) - pos) * 0.000004567
     pos = pos + speed
     speed = speed * 0.96
     output(t) = (input(t)-pos)
     */
    
    double pos;
    double speed;
    double output;
    double input;
    
public:
    DCremoval(){output = input = speed= pos==0.;};
    ~DCremoval(){};
    double removeDC (double _input){
        input= _input;
        speed = speed + (input - pos) * 0.000004567;
        pos=pos+speed;
        speed*=.96;
        output=input-pos;
        return (output);
    };
    
};

class ParamSmooth
{
    // 3 step average : this is an empirical aproach
    
    double smoothed, x1, x2,  x3, x4, x5, x6,  new_value;
    
public:
   // ParamSmooth(){ smoothed=x1=x2=new_value=1.;};
    //~ParamSmooth(){};
    
   // void setNewValue(double _new_value) { new_value= _new_value; x1=x2; x2=smoothed; };
   // double getValue() { smoothed=(x1+x2+new_value)/3.; x1=x2; x2=new_value; return smoothed;};
    
    // 7 step average :
    ParamSmooth() { smoothed=x1=x2=x3=x4=x5=x6=new_value=1.;};
    ~ParamSmooth(){};
     void setNewValue(double _newvalue) { new_value= _newvalue; x1=x2; x2=x3; x3=x4; x4=x5; x5=x6; x6=smoothed; };
     double getValue(){ smoothed=(x1+x2+x3+x4+x5+x6+new_value)/7.; x1=x2; x2=x3; x3=x4; x4=x5; x5=x6; x6=new_value; return smoothed;};
};

class SignalClamper
{
    // Never allow levels higher that 0dBfs = +1.0f ~ -1.0f //
    
    float* input;
    float finput;
    
public:
    SignalClamper(){input = new float[2]; input[0]=input[1]=0.; finput=0;};
    ~SignalClamper(){};
    
    float* ClampSignal(float* _input) {input=_input; if(input[0]>1.){input[0]=1.;}; if(input[0]<-1.){input[0]=-1.;}; if(input[1]>1.){input[1]=1.;}; if(input[1]<-1.){input[1]=-1.;};return(input);};
    float ClampSignal(float _input){finput=_input; if(finput>1.){finput=1.;}; if(finput<-1.){finput=-1.;};return(finput);};
};

class Modulator
{
    
    float* modulate_carrier;
    float* modulate_modulator;
    float* output;
    
public:
    Modulator(){
        modulate_carrier = new float[2];
        modulate_modulator = new float[2];
        output = new float[2];
    };
    
    ~Modulator(){ delete modulate_carrier, modulate_modulator, output;};
    
    float* modulate(float* _carrier, float* _modulator) {
        modulate_carrier = _carrier;
        modulate_modulator = _modulator;
        output[0]=modulate_carrier[0]*modulate_modulator[0];
        output[1]=modulate_carrier[1]*modulate_modulator[1];
        return (output);};
    
};

class Mix2
{
    float* input1;
    float* input2;
    float* output;
    float attenuator;
    
public:
    Mix2(){input1 = new float[2];input2 = new float[2];attenuator=.5f;input1[0]=input2[0]=0.;input1[1]=input2[1]=0.;};
    ~Mix2(){delete input1,input2;};
    void setAttenuator(float _value){attenuator= _value;}; // _value range 0. ~ 1.
    float* mix2(float* _input1, float* _input2){input1=_input1;input2=_input2;output[0]=(input1[0]+input2[0])*attenuator;(output[1]=input1[1]+input2[1])*attenuator;return (output);};
};

class Mix6
{
    float* input1;
    float* input2;
    float* input3;
    float* input4;
    float* input5;
    float* input6;
    float* output;
    float attenuator;
    
public:
    Mix6(){
        input1 = new float[2];input2 = new float[2];input3 = new float[2];input4 = new float[2];input5 = new float[2];input6 = new float[2];
        attenuator=1./6.;
        input1[0]=input2[0]=input3[0]=input4[0]=input5[0]=input6[0]=0.;
        input1[1]=input2[1]=input3[1]=input4[1]=input5[1]=input6[1]=0.;
        output=new float[2];
    };
    ~Mix6(){delete input1,input2,input3,input4,input5,input6,output;};
    void setAttenuator(float _value){attenuator= _value;}; // _value range 0. ~ 1.
    float* mix6(float* _input1, float* _input2,float* _input3, float* _input4,float* _input5, float* _input6)
    {
        input1=_input1;input2=_input2;input3=_input3;input4=_input4;input5=_input5;input6=_input6;
        output[0]=(input1[0]+input2[0]+input3[0]+input4[0]+input5[0]+input6[0])*attenuator;
        output[1]=(input1[1]+input2[1]+input3[1]+input4[1]+input5[1]+input6[1])*attenuator;
        return (output);
    };
};

class expFunc
{
    // usage : expFunc.getParam( _parameter-values);
    // gets y values range 0.f - 1.0 linearily distributed, and returns a sweet aprox to the same range with an inverted exponential distribution
    // feed this (1/((-x*x)+1.618179))-0.617699
    // here http://www.mathsisfun.com/data/function-grapher.php
    // to tricky adjust for the coeficients.
private:
    float y;
    public :
    expFunc(){ y =0.f;};
    ~expFunc(){};
    float getParam(float x)
    {
        y=(1/((-x*x)+1.618179))-0.617699;
        y<0.f?y=0.f:y>1.f?y=1.f:y;
        return (y);
    };
};




/* ============================================================================================= */
/* =======================  INTERPOLATION FUNCTIONS ============================================ */


//_______________________________________________________________________________________________
//

//  3rd order Spline interpollation
//  References : Posted by Dave from Muon Software, originally from Josh Scholar
//  http://www.musicdsp.org/archive.php?classid=5#62
//  interpolates between L0 and H0 taking the previous (L1) and next (H1) points into account

inline float ThirdInterp( float x, float L1, float L0, float H0, float H1)
{
    return
    L0 +
    .5f*
    x*(H0-L1 +
       x*(H0 + L0*(-2) + L1 +
          x*( (H0 - L0)*9 + (L1 - H1)*3 +
             x*((L0 - H0)*15 + (H1 - L1)*5 +
                x*((H0 - L0)*6 + (L1 - H1)*2 )))));
}

//_______________________________________________________________________________________________
//



//_______________________________________________________________________________________________
//

/*
 
 //  5-point spline interpollation
 
 //  References : Joshua Scholar, posted by David Waugh
 //  http://www.musicdsp.org/showArchiveComment.php?ArchiveID=60
 //  interpolates between L0 and H0 taking the previous (L1) and next (H1) points into account
 
 
 //nMask = sizeofwavetable-1 where sizeofwavetable is a power of two.
 double interpolate(double* wavetable, int nMask, double location)
 {
 
 // 5-point spline
 
 int nearest_sample = (int) location;
 double x = location - (double) nearest_sample;
 
 double p0=wavetable[(nearest_sample-2)&nMask];
 double p1=wavetable[(nearest_sample-1)&nMask];
 double p2=wavetable[nearest_sample];
 double p3=wavetable[(nearest_sample+1)&nMask];
 double p4=wavetable[(nearest_sample+2)&nMask];
 double p5=wavetable[(nearest_sample+3)&nMask];
 
 return p2 + 0.04166666666*x*((p3-p1)*16.0+(p0-p4)*2.0
 + x *((p3+p1)*16.0-p0-p2*30.0- p4
 + x *(p3*66.0-p2*70.0-p4*33.0+p1*39.0+ p5*7.0- p0*9.0
 + x *( p2*126.0-p3*124.0+p4*61.0-p1*64.0- p5*12.0+p0*13.0
 + x *((p3-p2)*50.0+(p1-p4)*25.0+(p5-p0)*5.0)))));
 };
 
 */

//_______________________________________________________________________________________________
//



/* ============================================================================================= */
/* ============================================================================================= */




#endif  // DSPTOOLS_H_INCLUDED
