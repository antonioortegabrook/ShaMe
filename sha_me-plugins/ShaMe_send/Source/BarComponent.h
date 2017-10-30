/*
  ==============================================================================

    BarComponent.h
    Created: 8 Oct 2017 2:32:07pm
    Author:  Antonio Ortega Brook

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "ShaMeColours.h"
#include "LedComponent.h"
#include "ShaMeLookAndFeel.h"

//==============================================================================
/*
*/
class BarComponent    : public Component
{
public:
	BarComponent();
	~BarComponent();
	
	void paint (Graphics&) override;
	void resized() override;
	
	TextEditor	nameField;
	LedComponent	led;
	TextButton	forceUnlinkButton;
	TextButton	refreshButton;
	
private:
	ShaMeLookAndFeel	shameLookAndFeel;
	Label			nameLabel;
	
	
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BarComponent)
};
