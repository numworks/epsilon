IMPORTANT: For this app to work, edit AppDelegate.m and supply your tracking ID.

This example app illustrates how to easily add Google Analytics tracking to a
simple iOS application. The app delegate initializes the Google Analytics SDK,
sets the dispatch interval, enables uncaught exception tracking and the logging
of debug messages, and instantiates a tracker using the tracking ID given. Each
view controller that needs to be tracked inherits from GAITrackedViewController
and sets the 'trackedViewName' property to enable automatic view tracking with
the view name given.  The app also tracks the toggling of dispatching using HTTP
and HTTPS as events by directly calling the GAITracker event tracking method.

All images are from the public domain. They were downloaded from
http://public-domain-image.com
