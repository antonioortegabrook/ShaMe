/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */


#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "../../../sha_me-common/shame.c"


//==============================================================================
ShaMe_sendAudioProcessor::ShaMe_sendAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		  .withInput  ("Input",  AudioChannelSet::stereo(), true)
#endif
		  .withOutput ("Output", AudioChannelSet::stereo(), true)
#endif
		  )
#endif
{
//        ShaMe_sendAudioProcessor::setProcessingPrecision(ProcessingPrecision::doublePrecision);
	isWritable = false;
	binStatus = S_UNINITIALIZED;
}

ShaMe_sendAudioProcessor::~ShaMe_sendAudioProcessor()
{
	int dettach_err;
	char	formattedName[256];
	
	if (binStatus > S_UNINITIALIZED) {
		
		sprintf(formattedName, "%s", shameName.toStdString().c_str());
		
		dettach_err = dettach_shame(shame_write, formattedName, writerN, false);
		
		/* How should we handle dettach error here...?
		 */
	}
}

//==============================================================================
const String ShaMe_sendAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool ShaMe_sendAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool ShaMe_sendAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool ShaMe_sendAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double ShaMe_sendAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int ShaMe_sendAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int ShaMe_sendAudioProcessor::getCurrentProgram()
{
	return 0;
}

void ShaMe_sendAudioProcessor::setCurrentProgram (int index)
{
}

const String ShaMe_sendAudioProcessor::getProgramName (int index)
{
	return {};
}

void ShaMe_sendAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ShaMe_sendAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
	
        ShaMe_sendAudioProcessor::setLatencySamples(samplesPerBlock);
	
	if (!shameName.isEmpty() && binStatus == S_UNINITIALIZED)
		createShaMe(shameName);

	else if (binStatus > S_CANT_WRITE) {
		
		if (shame_write->sample_rate	!= sampleRate			||
		    shame_write->vector_size	!= samplesPerBlock		||
		    shame_write->nchannels	!= getTotalNumInputChannels()
		    ) {
			
			// resize shame
			binStatus = S_CANT_WRITE;
			createShaMe(shameName);
		}
	}
	
	sendChangeMessage();
        
        return;
}

void ShaMe_sendAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ShaMe_sendAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused (layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
/*	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;
*/
	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif
	
	return true;
#endif
}
#endif

void ShaMe_sendAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	ScopedNoDenormals noDenormals;
//	const int totalNumInputChannels  = getTotalNumInputChannels();
	const int totalNumOutputChannels = getTotalNumOutputChannels();
	
	if (isWritable) {
		
		double *sample_ptr = &shame_write->sample_data + shame_write->write_offset;
		
		for (int channel = 0; channel < totalNumOutputChannels; channel++) {
			
			const float *channelData = buffer.getReadPointer (channel);
			
			for (int i = 0; i < buffer.getNumSamples(); i++) {
				
				*sample_ptr++ = static_cast<double>(*channelData++);
			}
//		buffer.clear (channel, 0, buffer.getNumSamples());
		}
		
		int tmp_offset = shame_write->read_offset;
		
		shame_write->read_offset = shame_write->write_offset;
		shame_write->write_offset = tmp_offset;
	}
}

//==============================================================================
bool ShaMe_sendAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ShaMe_sendAudioProcessor::createEditor()
{
	return new ShaMe_sendAudioProcessorEditor (*this);
}

//==============================================================================
void ShaMe_sendAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
	MemoryOutputStream(destData, false).writeString(shameName);
}

void ShaMe_sendAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
	
	shameName.operator=(MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false).readString());
	
	if (getSampleRate() && getBlockSize() && getTotalNumInputChannels()) {
		
		if (!shameName.isEmpty()) {
			createShaMe(shameName);
			
			sendChangeMessage();
		}
	}
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new ShaMe_sendAudioProcessor();
}



juce::String ShaMe_sendAudioProcessor::getName()
{
	return shameName;
}

void ShaMe_sendAudioProcessor::createShaMe(juce::String name)
{
	int dettach_result;
	t_shame_error init_err;
	
	char	formattedName[256];
	double	sampleRate;
	int	samplesPerBlock;
	long	nchannels;
	
	sampleRate	= getSampleRate();
	samplesPerBlock	= getBlockSize();
	nchannels	= getTotalNumInputChannels();
	
	
	if (binStatus > S_UNINITIALIZED) {
		
		// do nothing if it's initialized
		//		if (!name.compare(shameName) && binStatus > S_CANT_READ) {
		if (!name.compare(shameName) && binStatus > S_CANT_WRITE) {
			binStatus = get_writer_status(shame_write, writerN);
			return;
		}
		
		
		sprintf(formattedName, "%s", shameName.toStdString().c_str());
		
		isWritable = false;
		dettach_result = dettach_shame(shame_write, formattedName, writerN, false);
		
		if (dettach_result != E_SHAME_DETTACH_SUCCESS) {
			initResult = dettach_result;
			// va a provocar SIGSEGV o algo parecido si el puntero no es válido
			binStatus = get_writer_status(shame_write, writerN);
			return;
		}
		
		binStatus = S_UNINITIALIZED;
	}
	
	
	isWritable = false;
	
	sprintf(formattedName, "%s", name.toStdString().c_str());
	
	init_err = init_shame_writer(&shame_write, formattedName, &writerN, sampleRate, samplesPerBlock, nchannels);
	
	shameName.operator=(name);
	
	binStatus = get_writer_status(shame_write, writerN);
	
	isWritable = binStatus > S_CANT_WRITE;

	initResult = init_err;
}




void ShaMe_sendAudioProcessor::dettachShameWriter()
{
	char	formattedName[256];
	int	dettach_err;
	
	sprintf(formattedName, "%s", shameName.toStdString().c_str());
	
	isWritable = false;
	dettach_err = dettach_shame(shame_write, formattedName, writerN, false);
	
	if (dettach_err == E_SHAME_DETTACH_SUCCESS) {
		binStatus = S_UNINITIALIZED;
		shameName.operator=(String());
	} else {
		binStatus = get_writer_status(shame_write, writerN);
		isWritable = binStatus > S_CANT_WRITE;
	}
	
	initResult = dettach_err;
}

void ShaMe_sendAudioProcessor::forceUnlink()
{
	char	formattedName[256];
	
	sprintf(formattedName, "%s", shameName.toStdString().c_str());
	
	force_unlink(formattedName);
	
	binStatus = S_UNINITIALIZED;
	shameName.operator=(juce::String());
	isWritable = false;
}

int ShaMe_sendAudioProcessor::readersMatchSampleRate()
{
	int match = clients_match_sample_rate(shame_write);
	return match;
}

int ShaMe_sendAudioProcessor::readersMatchVectorSize()
{
	int match = clients_match_vector_size(shame_write);
	return match;
}
