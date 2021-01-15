// Copyright 2013 Google Inc. All rights reserved.
/** @file */

#import <Foundation/Foundation.h>

#import "TAGContainer.h"
#import "TAGLogger.h"

@class TAGDataLayer;

/**
 * Mode for refreshing the container.
 */
typedef enum {
  /**
   * In this mode, containers are automatically refreshed from network every 12
   * hours. Developers can also call TAGContainer::refresh to manually
   * refresh containers.
   *
   * This is the default mode.
   */
  kTAGRefreshModeStandard,
  /**
   * In this mode, the default container is always used (the container will not load a version
   * from network, or from disk).
   *
   * This mode allows developers to add new key/value pairs locally and
   * then to use the Plist or JSON default container to test them quickly
   * without having to add those key/value pairs to the container using
   * the GTM UI.
   *
   * This mode is intended for development only and not for shipping code.
   */
  kTAGRefreshModeDefaultContainer,
} TAGRefreshMode;


typedef NS_ENUM(NSUInteger, TAGDispatchResult) {
  kTAGDispatchNoData,
  kTAGDispatchGood,
  kTAGDispatchError
};

/**
 * A class that is the mobile implementation of Google Tag Manager (GTM).
 *
 * Sample usage:
<pre>
 *      TAGContainer *container =
 *          [[TAGManager instance] openContainerById:myContainerId];
 *      NSString *value = [container stringForKey:@"myKey"];
 *
 *      TAGDataLayer *dataLayer =
 *          [[TAGManager instance] dataLayer];
 *      [dataLayer push:@{@"event": @"openScreen",
 *                        @"screenName": @"Main Page"}];
</pre>
 *
 * A container is a collection of macros, rules, and tags. It is created within
 * the GTM application (http://www.google.com/tagmanager), and is assigned
 * a container ID. This container ID is the one used within this API.
 *
 * The TAGContainer class provides methods for retrieving
 * macro values given the macro name. The routines
 * TAGContainer::booleanForKey:, TAGContainer::doubleForKey:,
 * TAGContainer::int64ForKey:, TAGContainer::stringForKey: return the current
 * value for the value collection macro name, depending on the rules associated with that macro
 * in the container.
 *
 * As an example, if your container has a value collection macro with a key "speed" whose
 * value is 32, and the enabling rule is Language is "en"; and another value collection macro
 * with a key "speed" whose value is 45, and the enabling rule is Language is not "en",
 * then making the following call:
 *
 <pre>
 *     [container longForKey:\@"speed"]
 </pre>
 *
 * will return either 32 if the current language of the device is English, or
 * 45 otherwise.

 * The data layer is a map holding generic information about the application.
 * The TAGDataLayer class provides methods to push and retrieve data from
 * the data layer. Pushing an <code>event</code> key to the data layer will
 * cause tags that match this event to fire.
 *
 * An initial version of the container is bundled with the application. It
 * should be placed as a resource in the bundle with name
 * <code>containerId</code> where <code>containerId</code> is the same container
 * ID you will use within this API. When you call
 * TAGManager::openContainerById:callback:, the container will be
 * returned with those bundled rules/tags/macros. You will create the container
 * in the UI and use the Download button to download it.
 *
 * You can modify the container in the UI and publish a new version. In that
 * case, the next time the mobile app refreshes the container from the network
 * (currently every 12 hours),
 * it will get that new version. When you call one of the get... routines, the
 * value will be computed using the most recent rules.
 *
 * The downloaded container is saved locally. When you call
 * TAGManager::openContainerById:callback:, it will first load the default
 * container, and will then asynchronously load any saved container. If none is
 * found or if it is older than 12 hours, it will try to retrieve a newer version
 * from the network. You can find the status of those asynchronous loads by
 * passing a TAGContainerCallback to TAGManager::openContainerById:callback:.
 *
 * Sometimes you may want to block until either a non-default container is
 * available, or until a recent fresh container is available. You can do that
 * by  using the callbacks in TAGManager::openContainerById:callback: or use
 * TAGContainerOpener.
 *
 * When you are finished with a container, call TAGContainer::close.
 */
@interface TAGManager : NSObject

/**
 * The logger to use for Google Tag Manager SDK. By default, Google Tag Manager
 * logs error/warning messages and ignores info/debug/verbose messages.
 * You can use your own customized logger by setting this property.
 */
@property(nonatomic, strong) id<TAGLogger> logger;

/**
 * The refresh mode used for Google Tag Manager SDK. Setting this to
 * @ref kTAGRefreshModeDefaultContainer allows the refresh
 * method to use only the default container for development purposes. Default is
 * @ref kTAGRefreshModeStandard.
 */
@property(nonatomic) TAGRefreshMode refreshMode;

/**
 * Call TAGDataLayer::push: method to push events and other data.
 */
@property(nonatomic, readonly, strong) TAGDataLayer *dataLayer;

/**
 * Returns a container.
 * Usually the returned container will be empty, but the loading will happen
 * asynchronously, so the returned container may be refreshed before it
 * is returned, after it is returned, or may never be refreshed if, for example,
 * there is no network connection during the lifetime of the container.
 *
 * Callback will be called as various things happen for
 * the container. At a minimum, TAGManager::openContainerById:callback: will
 * attempt to load a saved version of the container. If there is no saved
 * version, or if the saved version is out of date, attempt will be made to
 * load from the network.
 *
 * If TAGManager::openContainerById:callback: is called a second time for a
 * given <code>containerId</code>, <code>nil</code> will be returned unless
 * the previous opened container has already been closed.
 *
 * @param containerId The ID of the container to open.
 * @param callback An object whose various methods will be called during the
 *     loading process. Note that the methods may be called from different
 *     threads. In addition, they may be called before
 *     TAGManager::openContainerById:callback: returns.
 * @return The opened container.
 */
- (TAGContainer *)openContainerById:(NSString *)containerId
                           callback:(id <TAGContainerCallback>)callback;

/**
 * Returns the container associated with the given <code>containerId</code>;
 * returns <code>nil</code> if the container is not already open.
 */
- (TAGContainer *)getContainerById:(NSString *)containerId;

/**
 * Previews the app with the input url.
 *
 * The valid url must start with the following:
<pre>
 * tagmanager.c.\<app_name\>://preview/p?id=
</pre>
 * where &lt;app_name&gt; is the application name.
 *
 * @param url The preview url.
 * @return YES if the url is a valid tagmanager preview url.
 */
- (BOOL)previewWithUrl:(NSURL *)url;

/**
 * Gets the singleton instance of the TAGManager class, creating it if
 * necessary.
 *
 * @return The singleton instance of TAGManager.
 */
+ (TAGManager *)instance;


/**
 * Dispatches any pending network traffic generated by tags (arbitrary pixels, analytics beacons,
 * etc).
 */
- (void)dispatch;

/**
 * If this value is positive, tracking information will be automatically
 * dispatched every dispatchInterval seconds.
 * Otherwise, tracking information must be sent manually by
 * calling dispatch.
 *
 * By default, this is set to `120`, which indicates tracking information will
 * be dispatched automatically every 120 seconds.
 */
@property(nonatomic, assign) NSTimeInterval dispatchInterval;

/**
 * Dispatches the next pending network traffic in the queue, calling completionHandler when
 * the request has either been sent (returning kTAGDispatchGood) or an error has resulted
 * (returning kTAGDispatchError).  If there is no network connection or there is no data to send,
 * kTAGDispatchNoData is returned.
 *
 * Calling this method with a nil completionHandler is the same as calling dispatch.
 *
 * This method can be used for background data fetching in iOS 7.0 or later.
 *
 * It would be wise to call this when the application is exiting to initiate the submission of any
 * unsubmitted tracking information.
 */
- (void)dispatchWithCompletionHandler:(void (^)(TAGDispatchResult))completionHandler;

@end
