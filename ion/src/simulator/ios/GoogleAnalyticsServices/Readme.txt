This is a combined SDK that includes both the Google Analytics SDK
and the Google Tag Manager SDK.

The libGoogleAnalyticsServices.a library contains code for both.
Within the subdirectories, you can find header files and documentation
for each.

The libAdIdAccess.a library contains functions that call the AdSupport
framework. If you would like your application to access the identifier for
advertisers (IDFA) and tracking flag provided by that framework through the
Google Tag Manager SDK macros, you will also need to add this library to your
XCode project.
