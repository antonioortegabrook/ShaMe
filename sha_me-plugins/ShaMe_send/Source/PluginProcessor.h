/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//#include "../../../C_sources/sha_me/shame.h"
#include "shame.h"

//==============================================================================
/**
 */
class ShaMe_sendAudioProcessor	:	public AudioProcessor,
					public ChangeBroadcaster
{
public:
	//==============================================================================
	ShaMe_sendAudioProcessor();
	~ShaMe_sendAudioProcessor();
	
	//==============================================================================
	void prepareToPlay (double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;
	
#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
	
	void processBlock (AudioSampleBuffer&, MidiBuffer&) override;
	
	//==============================================================================
	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;
	
	//==============================================================================
	const String getName() const override;
	
	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect () const override;
	double getTailLengthSeconds() const override;
	
	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram (int index) override;
	const String getProgramName (int index) override;
	void changeProgramName (int index, const String& newName) override;
	
	//==============================================================================
	void getStateInformation (MemoryBlock& destData) override;
	void setStateInformation (const void* data, int sizeInBytes) override;
	
	//==============================================================================
	
	
	
	juce::String getName();
	void createShaMe(juce::String name);
	void dettachShameWriter();
	void forceUnlink();
	int readersAttached();
	int writersAttached();
	int readersMatchSampleRate();
	int readersMatchVectorSize();

		
	struct		shame *shame_write;
        int             binStatus;
        int             initResult;
	bool		thru;
	
private:
	
	int		fd_shm;
        juce::String	shameName;
        int		isWritable;
        int		writerN;

	
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShaMe_sendAudioProcessor)
};
