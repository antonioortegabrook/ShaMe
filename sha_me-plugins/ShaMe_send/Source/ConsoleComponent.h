/*
  ==============================================================================

    ConsoleComponent.h
    Created: 8 Oct 2017 2:32:28pm
    Author:  Antonio Ortega Brook

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "ShaMeColours.h"

//==============================================================================
/*
*/
class ConsoleComponent    : public Component
{
public:
        ConsoleComponent();
        ~ConsoleComponent();
        
        void paint (Graphics&) override;
        void resized() override;
        
        void post(juce::String message);
        void postMessage(juce::String message);
        void postSuccess(juce::String message);
        void postWarn(juce::String message);
        void postError(juce::String message);
        
private:
        TextEditor console;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConsoleComponent)
};
