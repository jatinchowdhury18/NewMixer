#include "AddTrack.h"

AddTrack::AddTrack (MainComponent* mc, int x, int y) :
    mc (mc),
    x (x),
    y (y)
{
    formatManager.registerBasicFormats();
}

AddTrack::~AddTrack()
{
    formatManager.clearFormats();
}

bool AddTrack::perform()
{
#if JUCE_ANDROID
    return;
#endif

    if (trackFiles.isEmpty())
    {
        FileChooser nativeFileChooser (TRANS ("Import Audio Files"), {},
                                       String ("*.wav"), true);

        if (nativeFileChooser.browseForMultipleFilesToOpen())
        {
            auto files = nativeFileChooser.getResults();
            for (auto& file : files)
            {
                //check file is same length as others
                if (mc->getTracks().size() >= 1)
                {
                    if (! checkLength (mc->getTracks().getFirst(), file))
                        break;
                }

                if (trackFiles.size() >= 1)
                {
                    if (! checkLength (trackFiles.getFirst(), file))
                        break;
                }
                trackFiles.add (file);
            }
        }
    }

    if (trackFiles.isEmpty())
        return false;

    bool createNewUuids = trackUuids.isEmpty();

    int i = 0;
    for (auto& file : trackFiles)
    {
        Track* newTrack;

        if (createNewUuids)
        {
            newTrack = new Track (file, {}, {}, mc->getNextColour());
            trackUuids.add (newTrack->getUuid());
        }
        else
        {
            newTrack = new Track (file, {}, {}, mc->getNextColour(), trackUuids[i]);
        }

        newTrack->trackNameChanged (file.getFileNameWithoutExtension());

        const auto thisX = (int) (x - (trackFiles.size() * TrackConstants::defaultDiameter / 2)
                                    + (i * TrackConstants::defaultDiameter) + TrackConstants::defaultDiameter / 2);

        ActionHelper::addTrack (newTrack, mc, thisX, y);
        ++i;
    }
    return true;
}

bool AddTrack::undo()
{    
    if (trackUuids.isEmpty())
        return false;

    for (auto uuid : trackUuids)
        ActionHelper::deleteTrack (ActionHelper::getTrackWithUuid (mc, uuid), mc);

    mc->repaint();
    return true;
}

bool AddTrack::checkLength (Track* track, const File& fileToCheck)
{
    const auto length = dynamic_cast<TrackProcessor*> (track->getProcessor())->getReader()->lengthInSamples;
    return checkLength (length, fileToCheck);
}

bool AddTrack::checkLength (const File& baseFile, const File& fileToCheck)
{
    std::unique_ptr<AudioFormatReader> baseReader (formatManager.createReaderFor (baseFile.createInputStream()));
    return checkLength (baseReader->lengthInSamples, fileToCheck);
}

bool AddTrack::checkLength (int64 baseLength, const File& fileToCheck)
{
    std::unique_ptr<AudioFormatReader> reader (formatManager.createReaderFor (fileToCheck.createInputStream()));

    if (reader->lengthInSamples != baseLength)
    {
        showLengthErrorBox();
        return false;
    }
    return true;
}

void AddTrack::showLengthErrorBox()
{
    NativeMessageBox::showMessageBoxAsync (AlertWindow::WarningIcon, String ("Invalid File"),
                                           String ("All files must be the same length"), mc);
}
