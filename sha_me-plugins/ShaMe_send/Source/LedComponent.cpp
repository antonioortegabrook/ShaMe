/*
 ==============================================================================
 
 LedComponent.cpp
 Created: 8 Oct 2017 10:02:42pm
 Author:  Antonio Ortega Brook
 
 ==============================================================================
 */

#include "../JuceLibraryCode/JuceHeader.h"
#include "LedComponent.h"

//==============================================================================
LedComponent::LedComponent()
{
	// In your constructor, you should add any child components, and
	// initialise any special settings that your component needs.
	
	ledArea = getLocalBounds().reduced(1).toFloat();
	outerBorderColour = Colour(0xff979797);
	innerBorderColour = Colour(0xff979797);
	outerColour = Colour(0xffB3B3B3);
	innerColour = Colour(0xff4A4A4A);
	
}

LedComponent::~LedComponent()
{
}

void LedComponent::paint (Graphics& g)
{
	// outer
	g.setColour(outerColour);
	g.fillEllipse(ledArea);
	
	// border
	g.setColour(outerBorderColour);
	g.drawEllipse(ledArea, 1.5);
	
	// inner border
	g.setColour(innerBorderColour);
	g.drawEllipse(ledArea.reduced(4), 1.5);
	
	// inner
	g.setColour(innerColour);
	g.fillEllipse(ledArea.reduced(4));
}

void LedComponent::resized()
{
	// This method is where you should set the bounds of any child
	// components that your component contains..
	ledArea = getLocalBounds().reduced(1).toFloat();
}

void LedComponent::setState(int newState)
{
	switch (newState) {
		case LED_HALF_NEUTRAL:
			outerColour = Colour(0xffB3B3B3);
			innerColour = Colour(0xff4A4A4A);
			innerBorderColour = Colour(0xff4A4A4A);
//			innerBorderColour = Colour(0xff979797);
			break;
		case LED_NEUTRAL:
			outerColour = Colour(0xffB3B3B3);
			innerColour = Colour(0xffB3B3B3);
			innerBorderColour = Colour(0xffB3B3B3);
			break;
		case LED_HALF_SUCCESS:
			outerColour = Colour(SUCCESS_COLOUR);
			innerColour = Colour(0xff4A4A4A);
			innerBorderColour = Colour(0xff4A4A4A);
//			innerBorderColour = Colour(0xff979797);
			break;
		case LED_SUCCESS:
			outerColour = Colour(SUCCESS_COLOUR);
			innerColour = Colour(SUCCESS_COLOUR);
			innerBorderColour = Colour(SUCCESS_COLOUR);
			break;
		case LED_HALF_ERROR:
			outerColour = Colour(ERROR_COLOUR);
			innerColour = Colour(0xff4A4A4A);
			innerBorderColour = Colour(0xff4A4A4A);
//			innerBorderColour = Colour(0xff979797);
			break;
		case LED_ERROR:
			outerColour = Colour(ERROR_COLOUR);
			innerColour = Colour(ERROR_COLOUR);
			innerBorderColour = Colour(ERROR_COLOUR);
			break;
		case LED_HALF_WARN:
			outerColour = Colour(WARN_COLOUR);
			innerColour = Colour(0xff4A4A4A);
			innerBorderColour = Colour(0xff4A4A4A);
			break;
		case LED_WARN:
			outerColour = Colour(WARN_COLOUR);
			innerColour = Colour(WARN_COLOUR);
			innerBorderColour = Colour(WARN_COLOUR);
			break;
		default:
			break;
	}
	repaint();
}
