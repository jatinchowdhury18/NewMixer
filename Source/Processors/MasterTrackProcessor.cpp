#include "MasterTrackProcessor.h"
#include "TrackProcessor.h"

MasterTrackProcessor::MasterTrackProcessor (OwnedArray<Track>& tracks) :
    tracks (tracks)
{
    formatManager.registerBasicFormats();

    deviceManager.initialiseWithDefaultDevices (2, 2);
    deviceManager.addAudioCallback (&player);

    audioOutputNode = addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::audioOutputNode));
    audioOutputNode->getProcessor()->setPlayConfigDetails (2, 2, getSampleRate(), getBlockSize());

    audioInputNode = addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::audioInputNode));
    audioInputNode->getProcessor()->setPlayConfigDetails (2, 2, getSampleRate(), getBlockSize());

    for (auto track : tracks)
    {
        trackNodes.add (addNode (track->getProcessorPtr()));
        track->setProcessor (trackNodes.getLast()->getProcessor());
    }
    connectTracks();

    player.setProcessor (this);

    togglePlay();
}

MasterTrackProcessor::~MasterTrackProcessor()
{
    deviceManager.removeAudioCallback (&player);
}

void MasterTrackProcessor::connectTracks()
{
    for (auto trackNode : trackNodes)
    {
        for (int channel = 0; channel < 2; ++channel)
        {
            addConnection ({ { trackNode->nodeID,       channel },
                             { audioOutputNode->nodeID, channel } });
        }
    }
}

void MasterTrackProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiBuffer)
{
    if (! (isPlaying || isExporting))
    {
        buffer.clear();
        return;
    }

    AudioProcessorGraph::processBlock (buffer, midiBuffer);
}

void MasterTrackProcessor::togglePlay()
{
    if (! isPlaying)
        prepareToPlay (getSampleRate(), getBlockSize());
    else
        releaseResources();

    isPlaying = ! isPlaying;
}

void MasterTrackProcessor::addTrack (Track* track)
{
    trackNodes.add (addNode (track->getProcessorPtr()));
    auto trackNode = trackNodes.getLast();
    track->setProcessor (trackNode->getProcessor());

    for (int channel = 0; channel < 2; ++channel)
    {
        addConnection ({ { audioInputNode->nodeID, channel },
                         { trackNode->nodeID,      channel } });

        addConnection ({ { trackNode->nodeID,       channel },
                         { audioOutputNode->nodeID, channel } });
    }
}

void MasterTrackProcessor::removeTrack (Track* track)
{
    ReferenceCountedObjectPtr<Node> nodeToDelete;
    for (int i = 0; i < trackNodes.size(); i++)
        if (track->getProcessor() == trackNodes[i]->getProcessor())
            nodeToDelete = trackNodes.removeAndReturn (i);

    removeNode (nodeToDelete.get());
    nodeToDelete.reset();
}

void MasterTrackProcessor::prepareToExport (ExportInfo exportInfo)
{
    deviceManager.removeAudioCallback (&player);

    for (auto track : tracks)
        track->stopTimer();

    setNonRealtime (true);
    prepareToPlay (exportInfo.sampleRate, exportInfo.samplesPerBlock);

    isExporting = true;

    // this is needed to build the rendering sequence first,
    // before the first actual buffer is processed
    getAudioBuffer (exportInfo.samplesPerBlock);
}

void MasterTrackProcessor::restoreAfterExporting()
{
    isExporting = false;

    releaseResources();
    setNonRealtime (false);

    AudioDeviceManager::AudioDeviceSetup setup;
    deviceManager.getAudioDeviceSetup (setup);
    prepareToPlay (setup.sampleRate, setup.bufferSize);

    deviceManager.addAudioCallback (&player);

    for (auto track : tracks)
        track->startTimer (track->getAutoHelper()->timerInterval);

    exportListeners.call (&MasterTrackProcessor::Listener::exportCompleted);
    exportListeners.clear();
}

AudioSampleBuffer MasterTrackProcessor::getAudioBuffer (int bufferLength)
{
    AudioSampleBuffer buffer (getTotalNumOutputChannels(), bufferLength);
    MidiBuffer midiMessages;

    buffer.clear();
    processBlock (buffer, midiMessages);

    return buffer;
}

void MasterTrackProcessor::exportToFile (ExportInfo exportInfo, ThreadWithProgressWindow* progress)
{
    const int numBlocksEnd = 5; //num blocks silence buffer before ending??
    const int64 timeOutSamples = (int64) exportInfo.sampleRate; //1 second forced timeout
    const float exportSilence = 0.0001f; //-80 dB

    FileOutputStream* fileStream (exportInfo.exportFile.createOutputStream());
    AudioFormat* format = formatManager.findFormatForFileExtension (ExportInfo::getStringForFormat (exportInfo.format));
    std::unique_ptr<AudioFormatWriter> writer (format->createWriterFor (fileStream, exportInfo.sampleRate,
                                                                        getTotalNumOutputChannels(),
                                                                        exportInfo.bitDepth, {}, 0));

    AudioSampleBuffer buffer;

    int64 currentSample = 0;
    int numBlocksSilent = 0;
    while (numBlocksSilent <= numBlocksEnd)
    {
        buffer = getAudioBuffer (exportInfo.samplesPerBlock);

        //Handle tail after last clip end
        if (currentSample >= (exportInfo.lengthSamples - exportInfo.samplesPerBlock * numBlocksEnd))
        {
            if (buffer.getMagnitude (0, exportInfo.samplesPerBlock) < exportSilence)
            {
                if (numBlocksSilent == numBlocksEnd)
                    buffer.applyGainRamp (0, exportInfo.samplesPerBlock, 1.0f, 0.0f); //fade out last buffer

                numBlocksSilent++;
            }
            else if (numBlocksSilent < numBlocksEnd) //reset count if output goes back above -80dB
                numBlocksSilent = 0;

            if (currentSample > exportInfo.lengthSamples + timeOutSamples) //Force time out
            {
                buffer.applyGainRamp (0, exportInfo.samplesPerBlock, 1.0f, 0.0f);
                numBlocksSilent = numBlocksEnd + 1;
            }
        }

        writer->writeFromAudioSampleBuffer (buffer, 0, exportInfo.samplesPerBlock);

        currentSample += exportInfo.samplesPerBlock;

        //@TODO move to own function??
        for (auto node : trackNodes)
            dynamic_cast<TrackProcessor*> (node->getProcessor())->setStartSample (currentSample);

        for (auto track : tracks)
            track->renderAutomationExport();

        progress->setProgress ((double) currentSample / exportInfo.lengthSamples);
    }

    // Process 1 extra block of data once stopped to allow fade variables to update
    
    //globalTimer.setPlaying (false, true);
    buffer = getAudioBuffer (exportInfo.samplesPerBlock);
}
