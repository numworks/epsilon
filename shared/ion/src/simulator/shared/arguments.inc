/* Syntax:
 * BOOL_ARG(variable, longFrom, shortForm or "" if none, description)
 *   -> adds bool args.variable which is true if the arg is given
 * TEXT_ARG(variable, longFrom, shortForm, description, parameterUsageName)
 *   -> adds const char* args.variable which nullptr if not given
 *      and pointing to the string given by --longForm string if given
 */

BOOL_ARG(help, "--help", "-help", "Show this help message")
TEXT_ARG(stateFile, "--load-state-file", "-l",
         "Fetch events from an .nws state file", "statefile.nws")

BOOL_ARG(headless, "--headless", "-h", "Run without the SDL")

TEXT_ARG(language, "--language", "",
         "Start with the given language and associated country",
         "en|fr|nl|pt|de|es")

TEXT_ARG(openApp, "--open-app", "",
         "Open given application instead of the home menu", "application")

#if ION_SIMULATOR_FILES
BOOL_ARG(
    limitStackUsage, "--limit-stack-usage", "",
    "Fake a limited stack somewhat similar to what is available on the device")
TEXT_ARG(screenshotPath, "--take-screenshot", "",
         "Save the content of the screen as a png after the last event",
         "output.png")
TEXT_ARG(allScreenshotsFolder, "--take-all-screenshots", "",
         "Save the content of the screen as a png after every event",
         "output_directory")
#endif

#if ION_SIMULATOR_EXTERNAL_APP
TEXT_ARG(externalApp, "--nwb", "", "Load and launch an simulator external app",
         "app.nwb")
TEXT_ARG(externalAppData, "--nwb-external-data", "",
         "Pass a data file to the simulated external app", "data.bin")
#endif

#if ION_LOG_EVENTS_NAME
BOOL_ARG(doNotLogEvents, "--hide-events", "", "Do not print event names")
#endif

BOOL_ARG(computeScreenshotsHash, "--compute-hash", "",
         "Compute a cumulative hash of the content of the screen after each "
         "event and print it")
BOOL_ARG(headlessStateFile, "--headless-state-file", "",
         "Assume the given state file as no header and use default values "
         "(useful to the fuzzer)")

#undef BOOL_ARG
#undef TEXT_ARG
