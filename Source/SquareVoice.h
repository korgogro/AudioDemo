/*
  ==============================================================================

    SquareVoice.h
    Created: 21 Dec 2015 5:47:26pm
    Author:  ignasi

  ==============================================================================
*/



#ifndef SQUAREVOICE_H_INCLUDED
#define SQUAREVOICE_H_INCLUDED

#include "ADSRenv.h"
#include "oscSQU.h"


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
        AMPenv.setSampleRate(samplingRate);
        EG2.setSampleRate(samplingRate);
        
        
        
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
        square.setFrequency( MidiMessage::getMidiNoteInHertz (midiNoteNumber) );
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
        const double _CV_ = (double)(newValue / 127.);
        
        switch ( _CC_ ){
            case 100 :
                AMPenv.setAttack(_CV_);
                break;
            case 101 :
                AMPenv.setDecay(_CV_);
                break;
            case 102 :
                AMPenv.setSustain(_CV_);
                break;
            case 103 :
                AMPenv.setRelease(_CV_);
                break;
            case 104 :
                EG2.setAttack(_CV_);
                break;
            case 105 :
                EG2.setDecay(_CV_);
                break;
            case 106 :
                EG2.setSustain(_CV_);
                break;
            case 107 :
                EG2.setRelease(_CV_);
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
                
                // const FloatType currentSample = static_cast<FloatType> (std::sin (currentAngle) * level * tailOff);
                const float currentSample = (float) (square.getSignal() * level * AMPenv.getCurve());
                
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
    

 
private:
    
    double level, samplingRate;
    oscSQU square;
    ADSRenv AMPenv, EG2;

};
    



#endif  // SQUAREVOICE_H_INCLUDED