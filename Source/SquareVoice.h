/*
  ==============================================================================

    SquareVoice.h
    Created: 21 Dec 2015 5:47:26pm
    Author:  ignasi

  ==============================================================================
*/



#ifndef SQUAREVOICE_H_INCLUDED
#define SQUAREVOICE_H_INCLUDED

#include "DISTorsion.h"
#include "dspTools.h"
#include "LookUpTables.h"
#include "filter1.h"
#include "oscSAW.h"
#include "oscSQU.h"
#include "ADSRenv.h"


//==============================================================================
/** A demo synth sound .. */
class SquareSound : public SynthesiserSound
{
public:
    SquareSound() {}
    
    bool appliesToNote (int /*midiNoteNumber*/) override  { return true; }
    bool appliesToChannel (int /*midiChannel*/) override  { return true; }
};



//==============================================================================
/** A simple demo synth voice that just plays a Square wave.. */
class SquareVoice  : public SynthesiserVoice
{
public:
    
    
    SquareVoice()
    {
        samplingRate=getSampleRate();
        square.setSampleRate(samplingRate);
        square2.setSampleRate(samplingRate);
        AMPenv.setSampleRate(samplingRate);
        EG2.setSampleRate(samplingRate);
        FILT1.setSamplingRate(samplingRate);
        detuneOsc1 = detuneOsc2 = detuneOsc3 = detuneOsc4 = 0.;
        XmodEngaged = false;
    }
    
    bool canPlaySound (SynthesiserSound* sound) override
    {
        return dynamic_cast<SquareSound*> (sound) != nullptr;
    }
    
    void startNote (int midiNoteNumber, float velocity,
                    SynthesiserSound* sound,
                    int currentPitchWheelPosition) override
    {
        level = velocity * 0.15;
        
        const double noteFreqHz = MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        const double note2FreqHz = MidiMessage::getMidiNoteInHertz(midiNoteNumber-1);
        
        square.setFrequency ( noteFreqHz + ( detuneOsc1 * ( noteFreqHz - note2FreqHz ) ));
        square2.setFrequency( noteFreqHz + ( detuneOsc2 * ( noteFreqHz - note2FreqHz ) ));
        square3.setFrequency( noteFreqHz + ( detuneOsc3 * ( noteFreqHz - note2FreqHz ) ));
        square4.setFrequency( noteFreqHz + ( detuneOsc4 * ( noteFreqHz - note2FreqHz ) ));

        AMPenv.triggerOn();
        EG2.triggerOn();
    }
    
    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        AMPenv.triggerOff();
        EG2.triggerOff();
    }
    
    void pitchWheelMoved (int /*newValue*/) override
    {
        // can't be bothered implementing this for the demo!
    }
    
    void controllerMoved (int controllerNumber, int newValue) override
    {
        const int _CC_ = controllerNumber;
        const int _CV_ = newValue;
        
        switch ( _CC_ ){
                
                // Envelopes
            case 100 :
                AMPenv.setAttack((double)( _CV_ / 127. ));
                break;
            case 101 :
                AMPenv.setDecay((double)( _CV_ / 127. ));
                break;
            case 102 :
                AMPenv.setSustain((double)( _CV_ / 127.));
                break;
            case 103 :
                AMPenv.setRelease((double)( _CV_ / 127.));
                break;
            case 104 :
                EG2.setAttack((double)( _CV_ / 127. ));
                break;
            case 105 :
                EG2.setDecay((double)( _CV_ / 127. ));
                break;
            case 106 :
                EG2.setSustain((double)( _CV_ / 127. ));
                break;
            case 107 :
                EG2.setRelease((double)( _CV_ / 127. ));
                break;
                
                // Filter
            case 90 :
                FILT1.setPlay((bool) _CV_ );
                break;
            case 91 :
                FILT1.setFrequency((double) ( _CV_ / 127. ));
                break;
            case 92 :
                FILT1.setResonance((double) ( _CV_ / 127. ));
                break;
            case 93 :
                FILT1.setCutoffModDepth((double) ( _CV_ / 127. ));
                break;
                
                // OSC
            case 80:
                this->setDetune(_CV_);
                break;
            case 81:
                this->setXmod((bool)( _CV_));
                
                // DIST
            case 70 :
                DIST.setPlay((bool) ( _CV_ ));
                break;
            case 71:
                DIST.setLevel( (int) ( _CV_ ) ); // warning here !!
                break;
            case 72 :
                DIST.setGain( (double) ( _CV_ / 127.) );
                break;
                
            default :
                break;
        }
    }
    
    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if(square.getFrequency() != 0.0)
        {
            while (--numSamples >= 0)
            {
                FILT1.patchCutoffModSignal(  EG2.getCurve() );
                
                double oscsOut = ( square.getSignal() + square2.getSignal() + square3.getSignal() + square4.getSignal() ) / 4.;
                
                if(XmodEngaged){
                       oscsOut = square.getSignal() * square2.getSignal() * square3.getSignal() * square4.getSignal();
                }
                
                // double distOut = DIST.getAudioOut(oscsOut);
                
                // const FloatType currentSample = static_cast<FloatType> (std::sin (currentAngle) * level * tailOff);
                const double currentSample = (double) (
                                                       FILT1.getAudioOut ( oscsOut )
                                                              *
                                                            level
                                                              *
                                                        AMPenv.getCurve()
                                                              *
                                                          voiceAtenuator
                                                     
                                                        );
                
                for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample (i, startSample, currentSample);
                    
                    ++startSample;
                
                if (AMPenv.envStatus() == false)
                {
                    clearCurrentNote();
                    square.reset();
                    break;
                }
            }
        }
        
    }
    
    void setDetune(int _detune){
        detuneOsc1 = (double) ( _detune / 127. );
        detuneOsc2 = detuneOsc1 * ( -1. );
        detuneOsc3 = detuneOsc1 / 2.;
        detuneOsc4 = detuneOsc2 / 2.;
    }
    
    void setXmod ( bool _Xmod){
        XmodEngaged = _Xmod;
    }
    

 
private:
    
    double level, samplingRate, detuneOsc1, detuneOsc2, detuneOsc3, detuneOsc4;
    const double voiceAtenuator = 0.6;
    bool XmodEngaged;
    oscSQU square, square2, square3, square4;
    //oscSAW square, square2, square3, square4;
    ADSRenv AMPenv, EG2;
    filter1 FILT1;
    distorsion DIST;

};
    



#endif  // SQUAREVOICE_H_INCLUDED
