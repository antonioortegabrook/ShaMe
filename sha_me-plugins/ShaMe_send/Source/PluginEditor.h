/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

#include "ShaMeColours.h"
#include "TitleComponent.h"
#include "BarComponent.h"
#include "ConsoleComponent.h"


//==============================================================================
/**
 */
class ShaMe_sendAudioProcessorEditor	:	public AudioProcessorEditor,
						protected TextEditor::Listener,
						public Button::Listener,
						public ChangeListener
{
public:
        ShaMe_sendAudioProcessorEditor (ShaMe_sendAudioProcessor&);
        ~ShaMe_sendAudioProcessorEditor();
        
        //==============================================================================
        void paint (Graphics&) override;
        void resized() override;
        
        BarComponent bar;
        ConsoleComponent console;
        
private:
//	ChangeListener	listener;
	
        TitleComponent title;
	int ledStatus;
	
	void changeListenerCallback(ChangeBroadcaster *source) override;
	
	void textEditorReturnKeyPressed (TextEditor&) override;
	void textEditorEscapeKeyPressed (TextEditor&) override;
	
	void buttonClicked (Button* button) override;
	
	void postStatus();
	
	void updateInfo();
	void postShameErrorMessage(int message);
	
        // This reference is provided as a quick way for your editor to
        // access the processor object that created it.
        ShaMe_sendAudioProcessor& processor;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShaMe_sendAudioProcessorEditor)
};
