/*
  ==============================================================================

    distorsion.h
    Created: 5 Jan 2016 5:50:29pm
    Author:  ignasi

  ==============================================================================
*/

#ifndef DISTORSION_H_INCLUDED
#define DISTORSION_H_INCLUDED

#include "dspTools.h"
#include "LookUpTables.h"

class distorsion {
    
    public  :
    
    distorsion(){
        declickDistor.setAttack(DECLICK_SHORT);
        declickDistor.setRelease(DECLICK_SHORT);
        declicked_Distor=0.f;
        phase=0.f;
        distortedOut=0.f;
        smoothedGain = 0.f;
        LevelStage= 1.;
        smoothGain.setNewValue(0.);
        isPlayingDistor=false;
        distorMode=1;
        

    };

    ~distorsion(){};
    
    void    setGain(float _gain);
    
    double   getGain(){return smoothGain.getValue();};
    
    void    setLevel(int _level){LevelStage=abs(_level);};
    
    int     getLevel(){return LevelStage;};
    
    void    setPlay(bool _value){
        isPlayingDistor = _value;
        if (isPlayingDistor==true){declickDistor.trigger_on();}
        if (isPlayingDistor==false){declickDistor.trigger_off();}
    };
    
    void    setDistorMode ( int _distorMode){distorMode=_distorMode;};
    
    double getAudioOut(double audioIN){
        declicked_Distor=declickDistor.ar();
        smoothedGain=smoothGain.getValue();
        
        /* === linear interpolated lookup  arcsin table to optimize asin function calls :   */
        // ... instead of :
        // phase[0]=asin(audioIN[0]);
        // phase[1]=asin(audioIN[1]);
        // ... we do that :
        
        double idxremainder1;
        double readIdx1 = 256. * ( 1. + audioIN); // rectify input to 0 ~ +2 and map to 512 table values for lookup
        if ( readIdx1>=511 ) readIdx1 -=512; // make it warp arround
        idxremainder1 = readIdx1 - floor(readIdx1); // see where it falls in between
        
        if(distorMode==2){
            phase=(double)((1-idxremainder1) * table.arcSinXXBuf512(1+ (long) readIdx1) + idxremainder1 * table.arcSinXXBuf512(2+(long) readIdx1)); //compute the linear interpolation
        } else if(distorMode==1){
            phase=(double)((1-idxremainder1) * table.arcSinXBuf512(1+ (long) readIdx1) + idxremainder1 * table.arcSinXBuf512(2+(long) readIdx1)); //compute the linear interpolation
        }
        else{
            phase=(double)((1-idxremainder1) * table.arcSinBuf512(1+ (long) readIdx1) + idxremainder1 * table.arcSinBuf512(2+(long) readIdx1)); //compute the linear interpolation
        }
        
       
        /*==== END interpolated lookup arcsin ==*/
        
        double idxremainder3, readIdx3; // prepare variables for sin lookup inside distortion loop
        
        if(declicked_Distor>0. )
        {
            for (int n=1; n<=LevelStage; n+=2)
            {
                
                // intstead of calling sin function ...
                //   distortedOut[0]+=float((2./(table.PInValue1024[n]))*sin(float(n)*phase[0]));
                //   distortedOut[1]+=float((2./(table.PInValue1024[n]))*sin(float(n)*phase[1]));
                // ... we interpolate :
                
                readIdx3 = fmod((float(n))*(256. + (256. *phase)),512);
                
                idxremainder3 = readIdx3 - floor(readIdx3);
                
                distortedOut+=float((2./(table.PInValue1024(n)))* (float)((1-idxremainder3) * table.sinBuf512(1+ (long) readIdx3) + idxremainder3 * table.sinBuf512(2+(long) readIdx3)));
                
            }
            
           double AudioOut=(declicked_Distor*smoothedGain*distortedOut)  +  ((1.f-declicked_Distor)*audioIN);
           AudioOut=DCremover.removeDC(AudioOut);
            
            distortedOut=0.f;
            
            return AudioOut;
            
        }
        else if (declicked_Distor==0. )
        {
            // true bypass
            return audioIN;    } else { return audioIN ;}
        

    };
    
    
    private :
    
    double phase;
    int LevelStage;
    int distorMode;
    
    bool isPlayingDistor;
    avWIN declickDistor;
    double declicked_Distor;
    ParamSmooth smoothGain;
    double smoothedGain;
    DCremoval DCremover;;
    
    
    double distortedOut;
    
    LookUpTable table;
    
};





#endif  // DISTORSION_H_INCLUDED
