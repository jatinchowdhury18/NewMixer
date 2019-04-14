/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"

//==============================================================================
class NewMixerApplication  : public JUCEApplication
{
public:
    //==============================================================================
    NewMixerApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    //==============================================================================
    void initialise (const String& commandLine) override
    {
        // This method is where you should put your application's initialisation code..
        if (handleInternalCommandLineOperations (commandLine))
            return;

        mainWindow.reset (new MainWindow (getApplicationName(), commandLine));
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..
        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
        if (handleInternalCommandLineOperations (commandLine))
            return;
    }

    bool handleInternalCommandLineOperations (const String& commandLine)
    {
        if (commandLine.contains ("--unit-tests"))
            return handleUnitTests();
        else
            return false;
    }

    bool handleUnitTests()
    {
        unitTestRunner.runAllTests();

        for (int i = 0; i < unitTestRunner.getNumResults(); ++i)
        {
            if (unitTestRunner.getResult (i)->failures > 0)
            {
                setApplicationReturnValue (1);
                break;
            }
        }

        quit();
        return true;
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow    : public DocumentWindow
    {
    public:
        MainWindow (String name, String mode)  : DocumentWindow (name,
                                                    Desktop::getInstance().getDefaultLookAndFeel()
                                                                          .findColour (ResizableWindow::backgroundColourId),
                                                    DocumentWindow::allButtons)
        {
           #if JUCE_IOS || JUCE_ANDROID
            setContentOwned (new MainComponent ("Bridge"), true);
            setFullScreen (true);
           #else
            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent (mode), true);
            
            setResizable (true, false);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
            
            Image icon = ImageCache::getFromMemory (BinaryData::logo_256_png, BinaryData::logo_256_pngSize);
            setIcon (icon);
            getPeer()->setIcon (icon);
        }

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    class MyUnitTestRunner : public UnitTestRunner
    {
    public:
        MyUnitTestRunner() : abortTests (false) { }
        void setAbortingTests (bool shouldAbortTests)   { abortTests = shouldAbortTests; }
        bool shouldAbortTests() override                { return abortTests; }

#if JUCE_WINDOWS
        //on windows, override the default logMessage so that it prints to both VS and command-line console
        void logMessage (const String& message) override { DBG (message); std::cout << message << "\n"; }
#endif

    private:
        std::atomic<bool> abortTests;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyUnitTestRunner)
    };

    std::unique_ptr<MainWindow> mainWindow;
    MyUnitTestRunner unitTestRunner;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (NewMixerApplication)
