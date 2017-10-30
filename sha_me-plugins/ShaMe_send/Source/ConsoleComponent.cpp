/*
 ==============================================================================
 
 ConsoleComponent.cpp
 Created: 8 Oct 2017 2:32:28pm
 Author:  Antonio Ortega Brook
 
 ==============================================================================
 */

#include "../JuceLibraryCode/JuceHeader.h"
#include "ConsoleComponent.h"
#include "ShaMeColours.h"

//==============================================================================
ConsoleComponent::ConsoleComponent()
{
	// In your constructor, you should add any child components, and
	// initialise any special settings that your component needs.
	
	addAndMakeVisible(console);
	console.setReadOnly(true);
	console.setColour(TextEditor::backgroundColourId, Colour(TRANSPARENT_COLOUR));
	console.setColour(TextEditor::outlineColourId, Colour(TRANSPARENT_COLOUR));
	console.setColour(TextEditor::focusedOutlineColourId, Colour(TRANSPARENT_COLOUR));
	console.setColour(TextEditor::textColourId, Colour(TEXT_COLOUR_2));
	
	
	console.setMultiLine(true);
	console.setFont(Font("Helvetica", 11.7, Font::plain));
	console.setLineSpacing(1.2);
	console.setBorder(BorderSize<int>(2));
}

ConsoleComponent::~ConsoleComponent()
{
}

void ConsoleComponent::paint (Graphics& g)
{
	/* This demo code just fills the component's background and
	 draws some placeholder text to get you started.
	 
	 You should replace everything in this method with your own
	 drawing code..
	 */
	Rectangle<float> consoleArea (getLocalBounds().reduced(5).toFloat());
	
	g.setColour(Colour(TEXT_BACKGROUND_COLOUR_2));
	g.fillRoundedRectangle(consoleArea, 4);
	
	g.setColour(Colour(BORDER_COLOUR_2));
	g.drawRoundedRectangle(consoleArea, 4, 1);
}

void ConsoleComponent::resized()
{
	// This method is where you should set the bounds of any child
	// components that your component contains..
	
	Rectangle<int> area (getLocalBounds());
	
	console.setBounds(area.reduced(5));
}


void ConsoleComponent::post(juce::String message)
{
	console.insertTextAtCaret(message + "\n");
}

void ConsoleComponent::postMessage(juce::String message)
{
	console.setColour(TextEditor::textColourId, Colour(TEXT_BACKGROUND_COLOUR_1));
	console.insertTextAtCaret(CharPointer_UTF8 ("\xe2\x97\x8f "));
	console.setColour(TextEditor::textColourId, Colour(TEXT_COLOUR_2));
	console.insertTextAtCaret(message + "\n");
}

void ConsoleComponent::postSuccess(juce::String message)
{
	console.setColour(TextEditor::textColourId, Colour(SUCCESS_COLOUR));
	console.insertTextAtCaret(CharPointer_UTF8 ("\xe2\x97\x8f "));
	console.setColour(TextEditor::textColourId, Colour(TEXT_COLOUR_2));
	console.insertTextAtCaret(message + "\n");
}

void ConsoleComponent::postWarn(juce::String message)
{
	console.setColour(TextEditor::textColourId, Colour(WARN_COLOUR));
	console.insertTextAtCaret(CharPointer_UTF8 ("\xe2\x97\x8f "));
	console.setColour(TextEditor::textColourId, Colour(TEXT_COLOUR_2));
	console.insertTextAtCaret(message + "\n");
}

void ConsoleComponent::postError(juce::String message)
{
	console.setColour(TextEditor::textColourId, Colour(ERROR_COLOUR));
	console.insertTextAtCaret(CharPointer_UTF8 ("\xe2\x97\x8f "));
	console.setColour(TextEditor::textColourId, Colour(TEXT_COLOUR_2));
	console.insertTextAtCaret(message + "\n");
}
