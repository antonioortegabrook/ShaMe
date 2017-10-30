/*
 ==============================================================================
 
 LedComponent.h
 Created: 8 Oct 2017 10:02:42pm
 Author:  Antonio Ortega Brook
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ShaMeColours.h"

#define LED_HALF_NEUTRAL        0
#define LED_NEUTRAL		1
#define LED_HALF_SUCCESS	2
#define LED_SUCCESS		3
#define LED_HALF_ERROR		4
#define LED_ERROR		5
#define LED_HALF_WARN		6
#define LED_WARN		7


//==============================================================================
/*
 */
class LedComponent    : public Component
{
public:
	LedComponent();
	~LedComponent();
	
	void paint (Graphics&) override;
	void resized() override;
	
	void setState(int newState);
	
private:
	Rectangle<float> ledArea;
	Colour outerBorderColour, innerBorderColour, outerColour, innerColour;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LedComponent)
};
