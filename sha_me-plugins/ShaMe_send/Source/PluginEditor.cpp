/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "shame_strings.h"

//==============================================================================
ShaMe_sendAudioProcessorEditor::ShaMe_sendAudioProcessorEditor (ShaMe_sendAudioProcessor& p)
: AudioProcessorEditor (&p), processor (p)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	setSize (415, 300);
        
        addAndMakeVisible(title);
	
        addAndMakeVisible(bar);
	bar.setWantsKeyboardFocus(true);
	bar.nameField.addListener(this);
	bar.refreshButton.addListener(this);
	bar.forceUnlinkButton.addListener(this);
	
        addAndMakeVisible(console);
	
	processor.addChangeListener(this);
	postStatus();
}

ShaMe_sendAudioProcessorEditor::~ShaMe_sendAudioProcessorEditor()
{
	processor.removeChangeListener(this);
}

//==============================================================================
void ShaMe_sendAudioProcessorEditor::paint (Graphics& g)
{
	g.fillAll (Colour(BACKGROUND_COLOUR));
}

void ShaMe_sendAudioProcessorEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
        
        Rectangle<int> area (getLocalBounds());
        
        title.setBounds(area.removeFromTop(34).withTrimmedTop(5));
        bar.setBounds(area.removeFromTop(18));
        console.setBounds(area.withTrimmedTop(5));
}

void ShaMe_sendAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster *source) //override
{
	postShameErrorMessage(processor.initResult);
	postStatus();
}

void ShaMe_sendAudioProcessorEditor::textEditorReturnKeyPressed (TextEditor& textEditor)
{
	if (&textEditor == &bar.nameField) {
		
		juce::String name = bar.nameField.getText().toStdString().c_str();

		/** check if name is valid
		 */
		if (name.isEmpty()) {
			if (processor.binStatus > S_UNINITIALIZED) {
				processor.dettachShameWriter();
				
				postShameErrorMessage(processor.initResult);
				postStatus();
			}
			bar.nameField.unfocusAllComponents();
			return;
		}
		
		if (name.contains(juce::StringRef("/"))) {
			console.postError("Name cannot contain any slash");
			bar.nameField.unfocusAllComponents();
			return;
		}
			
		bar.nameField.unfocusAllComponents();
		
		processor.createShaMe(name);
		
		postShameErrorMessage(processor.initResult);
		postStatus();
	}
}

void ShaMe_sendAudioProcessorEditor::textEditorEscapeKeyPressed (TextEditor& textEditor)
{
	if (&textEditor == &bar.nameField) {
		
		bar.nameField.setText(processor.getName());
		bar.nameField.unfocusAllComponents();
	}
}

void ShaMe_sendAudioProcessorEditor::buttonClicked (Button* button)
{
	if (button == &bar.refreshButton) {
		/** Refresh hace lo mismo que create, pero toma el nombre del processor en lugar del nameField,
		    sólo si el writer está inicializado.
		 */
		if (processor.binStatus == S_UNINITIALIZED) {
			console.postMessage(String("Writer not initialized"));
			return;
		}
		
		juce::String name = processor.getName();
		
		if (name.isEmpty()) {
			return;
		}
		
		processor.createShaMe(name);
		
		postShameErrorMessage(processor.initResult);

		postStatus();
	}
	
	else if (button == &bar.forceUnlinkButton) {
		/**
		 Acá force unlink
		 Mejorar esto: force_unlink debería devolver algún valor y acá deberíamos mostrar
		 un resultado verdaro
		 */
		bar.nameField.clear();
		console.postMessage("Unlinked " + processor.getName());
		ledStatus = LED_NEUTRAL;
		bar.led.setState(ledStatus);
		processor.forceUnlink();
	}
}



void ShaMe_sendAudioProcessorEditor::postShameErrorMessage(int message)
{
	switch (message) {
		case E_SHAME_INIT_WRITER_SUCCESS:
			console.postSuccess(String::formatted(SUCCESSFULLY_CREATED, processor.getName().toStdString().c_str()));
			break;
			
		case E_SHAME_TOO_MUCH_DATA_TO_WRITE:
			console.postError(String::formatted(TOO_MUCH_DATA, processor.getName().toStdString().c_str()));
			break;
			
		case E_SHAME_READABLE_BUT_NOT_WRITABLE:
			console.postWarn(String::formatted(ALREADY_EXISTS, processor.getName().toStdString().c_str()));
			break;
			
		case E_SHAME_TRUNCATE_FAILED:
			console.postError(String::formatted(TRUNCATE_FAILED, processor.getName().toStdString().c_str()));
			break;
			
		case E_SHAME_MAP_FAILED:
			console.postError(String::formatted(SHAME_MAP_FAILED, processor.getName().toStdString().c_str(), errno));
			break;
			
		case E_SHAME_DETTACH_SUCCESS:
			console.postMessage(String::formatted(SUCCESSFULLY_DETTACHED));
			break;
			
		default:
			break;
	}
}

void ShaMe_sendAudioProcessorEditor::postStatus()
{
	/** Name
	 */
	bar.nameField.setText(processor.getName());
	
	/** Info
	 */
	if (processor.binStatus > S_CANT_READ) {
		int	vs	= processor.shame_write->vector_size;
		double	sr	= processor.shame_write->sample_rate;
		int attachedR	= processor.readersAttached();
		int attachedW	= processor.writersAttached();
		int dontMatchSR = processor.shame_write->attached_readers - processor.readersMatchSampleRate();
		int dontMatchVS = processor.shame_write->attached_readers - processor.readersMatchVectorSize();
		
		console.post(String::formatted("%d writers connected", attachedW));
		console.post(String::formatted("%d readers connected", attachedR));
		
		if (processor.binStatus < S_ALL_READERS_CAN_READ) {
			
			if (dontMatchSR)
				console.post(String::formatted(N_READERS_DONT_MATCH_SR, dontMatchSR, sr));
			
			if (dontMatchVS)
				console.post(String::formatted(N_READERS_DONT_MATCH_VS, dontMatchVS, vs));
		}
	}
	
	/** Led status & messages
	 */
	switch (processor.binStatus) {
		case S_UNINITIALIZED:
			bar.led.setState(LED_NEUTRAL);
			break;
			
		case S_CANT_READ:
			bar.led.setState(LED_ERROR);
			console.postError(String("Cannot read metadata"));
			break;
			
		case S_CANT_WRITE:
			bar.led.setState(LED_HALF_ERROR);
			console.postError(String("Cannot write sample data"));
			break;
			
		case S_NO_READERS_ATTACHED:
			bar.led.setState(LED_HALF_NEUTRAL);
			console.postMessage(String("No readers connected"));
			break;
			
		case S_NO_READERS_CAN_READ:
			bar.led.setState(LED_WARN);
			console.postWarn(String("No readers can read"));
			break;
			
		case S_SOME_READERS_CANT_READ:
			bar.led.setState(LED_HALF_WARN);
			console.postWarn(String("Some readers can't read"));
			break;
			
		case S_ALL_READERS_CAN_READ:
			bar.led.setState(LED_SUCCESS);
			console.postSuccess(String("All readers can read"));
			break;
			
		default:
			console.postError(String::formatted("unexpected binary status (%d)", processor.binStatus));
			break;
	}
	
	bar.nameField.unfocusAllComponents();
}
