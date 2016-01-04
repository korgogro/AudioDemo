/*
  ==============================================================================

   This file is part of the Projucer live coding demo created for
   the JUCE Summit 2015.
   Copyright (c) 2015 - ROLI Ltd.

   Use this code at your own risk & have some fun with C++ live coding!

   This code is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.

  ==============================================================================
*/

#ifndef SOUNDCOMPONENT_H_INCLUDED
#define SOUNDCOMPONENT_H_INCLUDED

//==============================================================================
class MainComponent   : public Component,
                        public AudioIODeviceCallback,
                        public MidiInputCallback,
                        public Button::Listener,
                        public Slider::Listener

{
public:
    //==========================================================================
    MainComponent()
        : midiKeyboardState(),
          midiKeyboardComponent (midiKeyboardState, MidiKeyboardComponent::Orientation::horizontalKeyboard)
    {
        audioDeviceManager.initialiseWithDefaultDevices (0, 2);
        audioDeviceManager.addMidiInputCallback (String::empty, this);
        audioDeviceManager.addAudioCallback (this);
        
        StringArray devices = MidiInput::getDevices();
        for (int i = 0; i < devices.size(); ++i)
            audioDeviceManager.setMidiInputEnabled (devices[i], true);
        
        midiKeyboardState.addListener (&midiCollector);

        synth.addSound (new SquareSound());
        
        for (int i = 0; i < maxNumVoices; ++i)
        {
            synth.addVoice (new SquareVoice());
        }

        addAndMakeVisible (oscilloscope);
        addAndMakeVisible (midiKeyboardComponent);
        
        addAndMakeVisible (onOffButton);
        onOffButton.setClickingTogglesState(true);
        onOffButton.setToggleState(true, dontSendNotification);
        masterPower=true; // should we initialize the bool here ¿?
        onOffButton.setButtonText ("ON");
        onOffButton.addListener(this);
        onOffButton.addShortcut(KeyPress(juce::KeyPress::spaceKey));
        
        addAndMakeVisible (masterVolume);
        masterVolume.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        masterVolume.setTextBoxStyle(Slider::NoTextBox, true, 1, 1);
        masterVolume.setRange (0., 1.0);
        masterVolume.setSkewFactorFromMidPoint (0.3);
        masterVolume.setValue (1., dontSendNotification);
        masterVolumeFactor = 1.;
        masterVolume.addListener (this);
        
        // AMP env ============================================== //
        
        addAndMakeVisible (AMPenvA);
        AMPenvA.setSliderStyle(juce::Slider::LinearBarVertical);
        AMPenvA.setTextBoxStyle(Slider::NoTextBox, true, 1, 1);
        AMPenvA.setRange (0., 127.0);
        AMPenvA.setSkewFactorFromMidPoint (0.32);
        AMPenvA.setValue (10., dontSendNotification);
        AMPenvA.addListener (this);
        
        
        addAndMakeVisible (AMPenvD);
        AMPenvD.setSliderStyle(juce::Slider::LinearBarVertical);
        AMPenvD.setTextBoxStyle(Slider::NoTextBox, true, 1, 1);
        AMPenvD.setRange (0., 127.0);
        AMPenvD.setSkewFactorFromMidPoint (0.32);
        AMPenvD.setValue (10., dontSendNotification);
        AMPenvD.addListener (this);
        
        addAndMakeVisible (AMPenvS);
        AMPenvS.setSliderStyle(juce::Slider::LinearBarVertical);
        AMPenvS.setTextBoxStyle(Slider::NoTextBox, true, 1, 1);
        AMPenvS.setRange (0., 127.0);
        AMPenvS.setSkewFactorFromMidPoint (0.32);
        AMPenvS.setValue (80., dontSendNotification);
        AMPenvS.addListener (this);
        
        addAndMakeVisible (AMPenvR);
        AMPenvR.setSliderStyle(juce::Slider::LinearBarVertical);
        AMPenvR.setTextBoxStyle(Slider::NoTextBox, true, 1, 1);
        AMPenvR.setRange (0., 127.0);
        AMPenvR.setSkewFactorFromMidPoint (0.32);
        AMPenvR.setValue (20., dontSendNotification);
        AMPenvR.addListener (this);
        
          // EG2 env ============================================== //
        
        addAndMakeVisible (EG2envA);
        EG2envA.setSliderStyle(juce::Slider::LinearBarVertical);
        EG2envA.setTextBoxStyle(Slider::NoTextBox, true, 1, 1);
        EG2envA.setRange (0., 127.0);
        EG2envA.setSkewFactorFromMidPoint (0.32);
        EG2envA.setValue (10., dontSendNotification);
        EG2envA.addListener (this);
        
        addAndMakeVisible (EG2envD);
        EG2envD.setSliderStyle(juce::Slider::LinearBarVertical);
        EG2envD.setTextBoxStyle(Slider::NoTextBox, true, 1, 1);
        EG2envD.setRange (0., 127.0);
        EG2envD.setSkewFactorFromMidPoint (0.32);
        EG2envD.setValue (10., dontSendNotification);
        EG2envD.addListener (this);
        
        addAndMakeVisible (EG2envS);
        EG2envS.setSliderStyle(juce::Slider::LinearBarVertical);
        EG2envS.setTextBoxStyle(Slider::NoTextBox, true, 1, 1);
        EG2envS.setRange (0., 127.0);
        EG2envS.setSkewFactorFromMidPoint (0.32);
        EG2envS.setValue (80., dontSendNotification);
        EG2envS.addListener (this);
        
        addAndMakeVisible (EG2envR);
        EG2envR.setSliderStyle(juce::Slider::LinearBarVertical);
        EG2envR.setTextBoxStyle(Slider::NoTextBox, true, 1, 1);
        EG2envR.setRange (0., 127.0);
        EG2envR.setSkewFactorFromMidPoint (0.32);
        EG2envR.setValue (20., dontSendNotification);
        EG2envR.addListener (this);

        
        setSize (800, 600);
    }

    //==========================================================================
    ~MainComponent()
    {
        audioDeviceManager.removeAudioCallback (this);
        audioDeviceManager.removeMidiInputCallback (String::empty, this);
    }
    
    //==========================================================================
    void resized() override
    {
        const int margin = 10;
        Rectangle<int> r = getLocalBounds();
        midiKeyboardComponent.setBounds (r.removeFromBottom (8*margin));
        oscilloscope.setBounds (r.removeFromBottom(2*margin));
        
        onOffButton.setBounds (margin,     margin, 5 * margin, (int)(2.5 * margin));
        masterVolume.setBounds(margin, 4 * margin, 5 * margin,         5  * margin);
        
        AMPenvA.setBounds(10 * margin, 4 * margin, 3 * margin, 10 * margin);
        AMPenvD.setBounds(13 * margin, 4 * margin, 3 * margin, 10 * margin);
        AMPenvS.setBounds(16 * margin, 4 * margin, 3 * margin, 10 * margin);
        AMPenvR.setBounds(19 * margin, 4 * margin, 3 * margin, 10 * margin);
        
        EG2envA.setBounds(23 * margin, 4 * margin, 3 * margin, 10 * margin);
        EG2envD.setBounds(26 * margin, 4 * margin, 3 * margin, 10 * margin);
        EG2envS.setBounds(29 * margin, 4 * margin, 3 * margin, 10 * margin);
        EG2envR.setBounds(32 * margin, 4 * margin, 3 * margin, 10 * margin);
    }
    
    void paint (Graphics& g){
    
        const int gmargin = 10;
        g.fillAll (Colours::white);
        
        g.setColour(juce::Colours::teal);
        g.fillRect(0, 0, getWidth(), getHeight());
        g.setColour(juce::Colours::white);
        g.setFont(Font("OpenDyslexic", 18.0f, normal));
        
        g.drawText("VOLUME", (int)(1.5 * gmargin), 9 * gmargin, 5 * gmargin , (int)(2.5 * gmargin), true);
        
        g.drawText("AMP EG", 15 * gmargin, 1  * gmargin, 7 * gmargin, (int)(2.5 * gmargin),true);
        
        g.drawText("EG 2", 28 * gmargin, 1  * gmargin, 7 * gmargin, (int)(2.5 * gmargin),true);
       
        g.drawText("A",      11 * gmargin, 14 * gmargin, 3 * gmargin, (int)(2.5 * gmargin),true);
        g.drawText("D",      14 * gmargin, 14 * gmargin, 3 * gmargin, (int)(2.5 * gmargin),true);
        g.drawText("S",      17 * gmargin, 14 * gmargin, 3 * gmargin, (int)(2.5 * gmargin),true);
        g.drawText("R",      20 * gmargin, 14 * gmargin, 3 * gmargin, (int)(2.5 * gmargin),true);
        
        g.drawText("A",      24 * gmargin, 14 * gmargin, 3 * gmargin, (int)(2.5 * gmargin),true);
        g.drawText("D",      27 * gmargin, 14 * gmargin, 3 * gmargin, (int)(2.5 * gmargin),true);
        g.drawText("S",      30 * gmargin, 14 * gmargin, 3 * gmargin, (int)(2.5 * gmargin),true);
        g.drawText("R",      33 * gmargin, 14 * gmargin, 3 * gmargin, (int)(2.5 * gmargin),true);
    }

    //==========================================================================
    void audioDeviceIOCallback (const float** /*inputChannelData*/,
                                int           /*numInputChannels*/,
                                float**       outputChannelData,
                                int           numOutputChannels,
                                int           numSamples) override
    {
        AudioBuffer<float> buffer (outputChannelData, numOutputChannels, numSamples);
        buffer.clear();
        
        MidiBuffer incomingMidi;
        midiCollector.removeNextBlockOfMessages (incomingMidi, numSamples);
       
        if(masterPower)
        {
            synth.renderNextBlock (buffer, incomingMidi, 0, numSamples);
            buffer.applyGain(masterVolumeFactor);
            oscilloscope.pushBuffer (outputChannelData[0], numSamples);
        }
    }

    //==========================================================================
    void audioDeviceAboutToStart (AudioIODevice* device) override
    {
        const double sampleRate = device->getCurrentSampleRate();
        midiCollector.reset (sampleRate);
        synth.setCurrentPlaybackSampleRate (sampleRate);
        
    }

    //==========================================================================
    virtual void audioDeviceStopped() override
    {
    }
    
    //==========================================================================
    void handleIncomingMidiMessage (MidiInput* /*source*/,
                                    const MidiMessage& message) override
    {
        midiCollector.addMessageToQueue (message);
    }

    void buttonClicked (Button* buttonThatWasClicked)
    {
        if (buttonThatWasClicked == &onOffButton)                                                      // [3]
        {
            masterPower = static_cast<bool>(buttonThatWasClicked->getToggleState());
            if(masterPower){onOffButton.setButtonText ("ON"); }
            else{ onOffButton.setButtonText ("OFF");}
        }
        
    }
    
    void sliderValueChanged (Slider* slider)
    {
        if (slider == &masterVolume)
        {
            masterVolumeFactor = masterVolume.getValue();
        }
        else if  (slider == &AMPenvA)
        {
            for (int i = 0; i < maxNumVoices; ++i)
            {
                synth.getVoice(i)->controllerMoved(100, (int) AMPenvA.getValue());
            }
        }
        else if  (slider == &AMPenvD)
        {
            for (int i = 0; i < maxNumVoices; ++i)
            {
                synth.getVoice(i)->controllerMoved(101, (int) AMPenvD.getValue());
            }
        }
        else if  (slider == &AMPenvS)
        {
            for (int i = 0; i < maxNumVoices; ++i)
            {
                synth.getVoice(i)->controllerMoved(102, (int) AMPenvS.getValue());
            }
        }
        else if  (slider == &AMPenvR)
        {
            for (int i = 0; i < maxNumVoices; ++i)
            {
                synth.getVoice(i)->controllerMoved(103, (int) AMPenvR.getValue());
            }
        }
    }
    
    
private:
    //==========================================================================
    AudioDeviceManager audioDeviceManager;
    Oscilloscope oscilloscope;
    Synthesiser synth;
    MidiMessageCollector midiCollector;
    MidiKeyboardState midiKeyboardState;
    MidiKeyboardComponent midiKeyboardComponent;
    const int maxNumVoices = 16;
    
    TextButton onOffButton;
    bool masterPower;
    Slider
    masterVolume,
    AMPenvA,
    AMPenvD,
    AMPenvS,
    AMPenvR,
    EG2envA,
    EG2envD,
    EG2envS,
    EG2envR
    ;
    double masterVolumeFactor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};



#endif  // SOUNDCOMPONENT_H_INCLUDED
