/*
 ==============================================================================
 
 TitleComponent.cpp
 Created: 8 Oct 2017 2:31:29pm
 Author:  Antonio Ortega Brook
 
 ==============================================================================
 */

#include "../JuceLibraryCode/JuceHeader.h"
#include "TitleComponent.h"

//==============================================================================
TitleComponent::TitleComponent()
{
	// In your constructor, you should add any child components, and
	// initialise any special settings that your component needs.
	
}

TitleComponent::~TitleComponent()
{
}

void TitleComponent::paint (Graphics& g)
{
	/* This demo code just fills the component's background and
	 draws some placeholder text to get you started.
	 
	 You should replace everything in this method with your own
	 drawing code..
	 */
	
	g.setFont(Font("Helvetica", 18, Font::plain));
	g.setColour (Colour(0xff9B9B9B));
	g.drawText ("ShaMe send", getLocalBounds().withLeft(10),
		    Justification::centredLeft, true);   // draw some placeholder text
	
}

void TitleComponent::resized()
{
	// This method is where you should set the bounds of any child
	// components that your component contains..
	
}
