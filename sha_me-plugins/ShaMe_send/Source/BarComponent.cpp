/*
 ==============================================================================
 
 BarComponent.cpp
 Created: 8 Oct 2017 2:32:07pm
 Author:  Antonio Ortega Brook
 
 ==============================================================================
 */

#include "../JuceLibraryCode/JuceHeader.h"
#include "BarComponent.h"


//==============================================================================
BarComponent::BarComponent()
{
	// In your constructor, you should add any child components, and
	// initialise any special settings that your component needs.
        setLookAndFeel(&shameLookAndFeel);
	
	addAndMakeVisible(nameLabel);
	nameLabel.setFont(Font("Helvetica", 12, Font::plain));
	nameLabel.setJustificationType(Justification::centredRight);
	nameLabel.setText("Name", NotificationType::dontSendNotification);
	nameLabel.setColour(Label::textColourId, Colour(0xffDDDDDD));
	
	addAndMakeVisible(nameField);
	//	nameField.setEscapeAndReturnKeysConsumed(false);
	nameField.setFont(Font("Helvetica", 12, Font::plain));
	nameField.setColour(TextEditor::textColourId, Colour(0xff4A4A4A));
	
	nameField.setColour(TextEditor::backgroundColourId, Colour(0x00ffffff));
	nameField.setColour(TextEditor::outlineColourId, Colour(0x00ffffff));
	nameField.setColour(TextEditor::focusedOutlineColourId, Colour(0x00ffffff));
	
	addAndMakeVisible(led);
        led.setState(LED_NEUTRAL);
	
	addAndMakeVisible(forceUnlinkButton);
	forceUnlinkButton.setButtonText("Unlink");
        forceUnlinkButton.setColour(TextButton::textColourOnId, Colour(0xff4A4A4A));
        forceUnlinkButton.setColour(TextButton::textColourOffId, Colour(0xff4A4A4A));
	
	
	addAndMakeVisible(refreshButton);
	refreshButton.setButtonText("Refresh");
        refreshButton.setColour(TextButton::textColourOnId, Colour(0xff4A4A4A));
        refreshButton.setColour(TextButton::textColourOffId, Colour(0xff4A4A4A));
}

BarComponent::~BarComponent()
{
}

void BarComponent::paint (Graphics& g)
{
	/* This demo code just fills the component's background and
	 draws some placeholder text to get you started.
	 
	 You should replace everything in this method with your own
	 drawing code..
	 */
//        Rectangle<float> nameFieldArea(getLocalBounds().removeFromLeft(175).translated(48, 0).toFloat().withTrimmedTop(0.5).withTrimmedBottom(3.5));
	Rectangle<float> nameFieldArea(getLocalBounds().removeFromLeft(175).translated(48, 0).toFloat());
	
	g.setColour(Colour(0xffD8D8D8));
	g.fillRoundedRectangle(nameFieldArea, 4);
	
	g.setColour(Colour(0xff979797));
	g.drawRoundedRectangle(nameFieldArea, 4, 1);
	
}

void BarComponent::resized()
{
	// This method is where you should set the bounds of any child
	// components that your component contains..
	
	Rectangle<int> area (getLocalBounds());
	Rectangle<float> nameFieldArea;
	
	nameLabel.setBounds(area.removeFromLeft(48).translated(0, 1));
	nameField.setBounds(area.removeFromLeft(175));
	led.setBounds(area.removeFromLeft(area.getHeight()).reduced(0).translated(6, 0));
	forceUnlinkButton.setBounds(area.removeFromRight(56).translated(-5, 0));
	refreshButton.setBounds(area.removeFromRight(56).translated(-9, 0));
}
