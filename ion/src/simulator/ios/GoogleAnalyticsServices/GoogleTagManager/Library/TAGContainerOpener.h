// Copyright 2013 Google Inc. All rights reserved.
/** @file */

#import <Foundation/Foundation.h>

@class TAGContainer;
@class TAGManager;

/** The choices of how to open a container. */
typedef enum {
  /**
   * Specifies that opening a non-default container (saved or retreived from
   * the network) is preferred, including stale containers (ones that
   * haven't been recently refreshed from the server).
   */
  kTAGOpenTypePreferNonDefault,
  /**
   * Specifies that opening a fresh version of the container (one that has been
   * recently refreshed from the server) is preferred.
   */
  kTAGOpenTypePreferFresh
} TAGOpenType;

/**
 * A class that implements this protocol waits for the container to be available and
 * provides access to the container. This protocol is deprecated, and will be removed from
 * a future version of the SDK.
 */
@protocol TAGContainerFuture

/**
 * Waits for the container to be available and returns the requested container.
 * This method will block until the container is available. The dispatch queue
 * will continue to run while the call is blocked.
 *
 * @return The requested container.
 * @see TAGContainerOpener::openContainerWithId:tagManager:openType:timeout:
 */
- (TAGContainer *)get;

/**
 * Returns whether the container is available. This container could be the
 * default container. To check if it is the default container, use
 * TAGContainer::isDefault.
 *
 * @return YES if the container is available. NO if TAGContainerFuture::get:
 *     will block and wait for the container to be available.
 */
- (BOOL)isDone;

@end

/**
 * A class that implements this protocol will receive a notification when a
 * container is available for use. Developers can pass an instance of a class that implements
 * this protocol to
 * TAGContainerOpener::openContainerWithId:tagManager:openType:timeout:notifier:
 */
@protocol TAGContainerOpenerNotifier

/**
 * Called when the container is available.
 *
 * @param container The requested container.
 */
- (void)containerAvailable:(TAGContainer *)container;

@end

/**
 * A helper class for opening containers.
 *
 * This is a wrapper around TAGManager::openContainerById:callback: method for
 * callers that provides support for timeouts.
 *
 * The following is a sample showing waiting up to 0.1 seconds for the container
 * to be loaded before reverting to the default container:
 *
 <pre>
 *     NSTimeInterval timeout = 0.1;
 *     TagManager *tagManager = [TagManager instance];
 *     TAGContainerFuture *future =
 *         [TAGContainerOpener openContainerWithId:@"GTM-XXXX"
 *                                      tagManager:tagManager
 *                                        openType:kTAGOpenTypePreferNonDefault
 *                                         timeout:&timeout];
 *     TAGContainer *container = [future get];
 </pre>
 *
 * If the caller wants to be asynchronously notified when the container is
 * available but wants to manually specify the timeout to 0.5 seconds, then the
 * caller should subclass TAGContainerOpenerNotifier, make the call to
 * TAGContainerOpener::openContainerWithId:tagManager:openType:timeout:notifier:
 * with timeout set to 0.5, and add the implementation to
 * TAGContainerOpenerNotifier::containerAvailable: for handling the container
 * available notification.
 */
@interface TAGContainerOpener : NSObject

// @cond
/**
 * TAGContainerOpener should not be instantiated directly. Use
 * openContainerWithId:tagManager:timeout:openType:
 * or openContainerWithId:tagManager::timeout:openType:notifier:.
 */
- (id)init __attribute__((unavailable));
// @endcond

/**
 * Note: This method is deprecated.  Use the notifier version of openContainerWithId.
 *
 * Waits up to <code>timeout</code> seconds for a container to be loaded
 * (non default or fresh depending on the specified <code>openType</code>)
 * and returns a <code>TAGContainerFuture</code>.
 *
 * <p>If the open type is <code>kTAGOpenTypePreferNonDefault</code>, a
 * non-default (saved or retrieved from network) container is loaded and the
 * <code>TAGContainerFuture</code> is unblocked as soon as one of the
 * following happens:
 * <ul>
 *   <li>a saved container is loaded.
 *   <li>if there is no saved container, a network container is loaded
 *    or a network error occurs.
 *   <li>the timer expires.
 * </ul>
 * If a network error occurs or the timer expires, TAGContainerFuture::get
 * may return a default container.
 *
 * <p>If the open type is <code>kTAGOpenTypePreferFresh</code>, a fresh (saved
 * or retrieved from network) container is loaded and the TAGContainerFuture is
 * unblocked as soon as one of the following happens:
 * <ul>
 *   <li>a saved fresh container is loaded.
 *   <li>if there is no saved container or saved container is stale, a network
 *   container is loaded or a network error occurs.
 *   <li>the timer expires.
 * </ul>
 * If a network error occurs or the timer expires, TAGContainerFuture::get
 * may contain defaults or a stale saved container.
 *
 * <p>If you call one of the openContainer methods a second time with a
 * given <code>containerId</code>, a <code>TAGContainerFuture</code> will be
 * returned whose TAGContainerFuture::get will return the same container as
 * the first call did.
 *
 * @param containerId The ID of the container to load.
 * @param tagManager The TAGManager for getting the container.
 * @param timeout The maximum number of seconds to wait to load the container
 *     from the saved store. If nil, TAGContainerOpener::defaultTimeout will be
 *     used.
 * @param openType The choice of how to open the container.
 * @return A TAGContainerFuture that will wait for up to <code>timeout</code>
 *     and return the container when it is available.
 */
+ (id<TAGContainerFuture>)
    openContainerWithId:(NSString *)containerId
             tagManager:(TAGManager *)tagManager
               openType:(TAGOpenType)openType
                timeout:(NSTimeInterval *)timeout
__attribute__((deprecated("Use The notifier version of TAGContainerOpener openContainerWithId.")));

/**
 * Waits up to <code>timeout</code> seconds for a container to be loaded
 * (non default or fresh depending on the specified <code>openType</code>)
 * and calls a notifier when the container is available.
 *
 * <p>If the open type is <code>kTAGOpenTypePreferNonDefault</code>, a
 * non-default (saved or retrieved from network) container is loaded and passed
 * into the notifier. The notifier is called as soon as one of the following
 * happens:
 * <ul>
 *   <li>a saved container is loaded.
 *   <li>if there is no saved container, a network container is loaded
 *    or a network error occurs.
 *   <li>the timer expires.
 * </ul>
 * If a network error occurs or the timer expires, the container passed into
 * the notifier is a default container.
 *
 * <p>If the open type is <code>kTAGOpenTypePreferFresh</code>, a fresh (saved
 * or retrieved from network) container is loaded and passed into the notifier.
 * The notifier is called as soon as one of the following happens:
 * <ul>
 *   <li>a saved fresh container is loaded.
 *   <li>if there is no saved container or saved container is stale, a network
 *   container is loaded or a network error occurs.
 *   <li>the timer expires.
 * </ul>
 * If a network error occurs or the timer expires, the container passed into
 * the notifier may contain defaults or a stale saved container.
 *
 * <p>If you call one of the openContainer methods a second time with a
 * given <code>containerId</code>, the same container returned from the
 * previous call will be passed into the notifier as soon as
 * it's available.
 *
 * @param containerId The ID of the container to load.
 * @param tagManager The TAGManager used for getting the container.
 * @param openType The choice of how to open the container.
 * @param timeout The maximum number of seconds to wait to load the container
 *     from the saved store. If nil, TAGContainerOpener::defaultTimeout will be
 *     used.
 * @param notifier An optional notifier which will be called when the
 *     container is available. It will be called with the fresh container, if
 *     available and loaded before the timeout; otherwise it'll be called with
 *     a non-fresh container. Note that the notifier may be called from a
 *     different thread.
 */
+ (void) openContainerWithId:(NSString *)containerId
                  tagManager:(TAGManager *)tagManager
                    openType:(TAGOpenType)openType
                     timeout:(NSTimeInterval *)timeout
                    notifier:(id<TAGContainerOpenerNotifier>)notifier;

/**
 * Returns the default timeout for requesting the container.
 *
 * @return The default timeout in seconds.
 */
+ (NSTimeInterval)defaultTimeout;

@end
