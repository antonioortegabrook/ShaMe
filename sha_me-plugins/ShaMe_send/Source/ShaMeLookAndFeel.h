/*
 ==============================================================================
 
 ShaMeLookAndFeel.h
 Created: 9 Oct 2017 2:09:39am
 Author:  Antonio Ortega Brook
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
 */
class ShaMeLookAndFeel    : public LookAndFeel_V4
{
public:
	ShaMeLookAndFeel()
	{
		// In your constructor, you should add any child components, and
		// initialise any special settings that your component needs.
		
	}
	
	void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
				   bool isMouseOverButton, bool isButtonDown) override
	{
		Rectangle<float> buttonArea = button.getLocalBounds().toFloat();
		
		if (isButtonDown)
			g.setColour (Colour(0xffBCBABA));
		else
			g.setColour (Colour(0xffD8D8D8));
		
		g.fillRoundedRectangle (buttonArea, 4);
		
		g.setColour (Colour(0xff979797));
		g.drawRoundedRectangle (buttonArea, 4, 1);
	}
	
	void drawButtonText (Graphics& g, TextButton& button, bool isMouseOverButton, bool isButtonDown) override
	{
//		Font font (getTextButtonFont (button, button.getHeight()));
		Font font ("Helvetica", button.getHeight(), Font::plain);
//		g.setFont (font);
		g.setFont(Font("Helvetica", 11.4, Font::plain));
		g.setColour (button.findColour (button.getToggleState() ? TextButton::textColourOnId
						: TextButton::textColourOffId)
			     .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
		
		const int yIndent = jmin (4, button.proportionOfHeight (0.3f));
		const int cornerSize = jmin (button.getHeight(), button.getWidth()) / 2;
		
		const int fontHeight = roundToInt (font.getHeight() * 0.6f);
		const int leftIndent  = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
		const int rightIndent = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
		const int textWidth = button.getWidth() - leftIndent - rightIndent;
		
		if (textWidth > 0)
			g.drawFittedText (button.getButtonText(),
					  leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2 + 2,
					  Justification::centred, 2);
			}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShaMeLookAndFeel)
};
