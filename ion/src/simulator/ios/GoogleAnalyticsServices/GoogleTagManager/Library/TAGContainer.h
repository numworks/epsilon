// Copyright 2013 Google Inc. All rights reserved.
/** @file */

#import <Foundation/Foundation.h>

@class TAGContainer;

/**
 * Refresh types for container callback.
 */
typedef enum {
  /** Refresh from a saved container. */
  kTAGContainerCallbackRefreshTypeSaved,

  /** Refresh from the network. */
  kTAGContainerCallbackRefreshTypeNetwork,
} TAGContainerCallbackRefreshType;

/**
 * Ways in which a refresh can fail.
 */
typedef enum {
  /** There is no saved container. */
  kTAGContainerCallbackRefreshFailureNoSavedContainer,

  /** An I/O error prevented refreshing the container. */
  kTAGContainerCallbackRefreshFailureIoError,

  /** No network is available. */
  kTAGContainerCallbackRefreshFailureNoNetwork,

  /** A network error has occurred. */
  kTAGContainerCallbackRefreshFailureNetworkError,

  /** An error on the server. */
  kTAGContainerCallbackRefreshFailureServerError,

  /** An error that cannot be categorized. */
  kTAGContainerCallbackRefreshFailureUnknownError
} TAGContainerCallbackRefreshFailure;

/**
 * A protocol implemented by the application to execute a custom tag.
 */
@protocol TAGFunctionCallTagHandler <NSObject>
  /**
   * Handler is given the tag name and a dictionary of named parameters.
   *
   * @param tagName The same name by which the handler was registered. It
   *     is provided as a convenience to allow a single handler to be registered
   *     for multiple function call tags.
   * @param parameters The named parameters for the function call. The
   *     dictionary may contain <code>NSString</code>, <code>NSNumber</code>
   *     (double, int, or boolean), <code>NSDictionary</code>, or
   *     <code>NSArray</code>.
   * @return The evaluated result, which can be an <code>NSString</code> or
   *     <code>NSNumber</code>.
   */
- (void)execute:(NSString *)tagName parameters:(NSDictionary *)parameters;

@end

/**
 * A protocol implemented by the application to calculate the value of a
 * function call macro.
 */
@protocol TAGFunctionCallMacroHandler <NSObject>
/**
 * Returns an object which is the calculated value of the macro.
 * Handler is given the macro name and a dictionary of named parameters.
 *
 * @param macroName The same name by which the handler was registered. It
 *     is provided as a convenience to allow a single handler to be registered
 *     for multiple function call macros.
 * @param parameters The named parameters for the function call. The
 *     dictionary may contain <code>NSString</code>, <code>NSNumber</code>
 *     (double, int, or boolean), <code>NSDictionary</code>, or
 *     <code>NSArray</code>.
 * @return The evaluated result, which can be an <code>NSString</code> or
 *     <code>NSNumber</code>.
 */
- (id)valueForMacro:(NSString *)macroName parameters:(NSDictionary *)parameters;

@end

/**
 * A protocol that a client may implement to receive
 * information when the contents of the container has been successfully
 * loaded or failed to load.
 *
 * You may rely on the fact that
 * TAGContainerCallback::containerRefreshBegin:refreshType:
 * will be called for a given @ref TAGContainerCallbackRefreshType before its
 * associated TAGContainerCallback::containerRefreshSuccess:refreshType: or
 * TAGContainerCallback::containerRefreshFailure:failure:refreshType:, but
 * shouldn't make any other assumptions about ordering. In particular, there
 * may be two refreshes outstanding at once
 * (both @ref kTAGContainerCallbackRefreshTypeSaved and
 * @ref kTAGContainerCallbackRefreshTypeNetwork), or a
 * @ref kTAGContainerCallbackRefreshTypeSaved refresh
 * may occur before a @ref kTAGContainerCallbackRefreshTypeNetwork refresh.
 */
@protocol TAGContainerCallback <NSObject>

/**
 * Called before the refresh is about to begin.
 *
 * @param container The container being refreshed.
 * @param refreshType The type of refresh which is starting.
 */
- (void)containerRefreshBegin:(TAGContainer *)container
                  refreshType:(TAGContainerCallbackRefreshType)refreshType;

/**
 * Called when a refresh has successfully completed for the given refresh type.
 *
 * @param container The container being refreshed.
 * @param refreshType The type of refresh which completed successfully.
 */
- (void)containerRefreshSuccess:(TAGContainer *)container
                    refreshType:(TAGContainerCallbackRefreshType)refreshType;

/**
 * Called when a refresh has failed to complete for the given refresh type.
 *
 * @param container The container being refreshed.
 * @param failure The reason for the refresh failure.
 * @param refreshType The type of refresh which failed.
 */
- (void)containerRefreshFailure:(TAGContainer *)container
                        failure:(TAGContainerCallbackRefreshFailure)failure
                    refreshType:(TAGContainerCallbackRefreshType)refreshType;

@end

/**
 * A class that provides access to container values.
 * Container objects must be created via @ref TAGManager.
 * Once a container is created, it can be queried for key values which
 * may depend on rules established for the container.
 * A container is automatically refreshed periodically (every 12 hours), but
 * can also be manually refreshed with TAGContainer::refresh.
 */
@interface TAGContainer : NSObject

/**
 * The ID for this container.
 */
@property(readonly, nonatomic, copy) NSString *containerId;

/**
 * The last time (in milliseconds since midnight Jan 1, 1970 UTC) that this
 * container was refreshed from the network.
 */
@property(atomic, readonly) double lastRefreshTime;

// @cond
/**
 * Containers should be instantiated through TAGManager or TAGContainerOpener.
 */
- (id)init __attribute__((unavailable));
// @endcond

/**
 * Returns a <code>BOOL</code> representing the configuration value for the
 * given key. If the container has no value for this key, NO will be returned.
 *
 * @param key The key to lookup for the configuration value.
 */
- (BOOL)booleanForKey:(NSString *)key;

/**
 * Returns a <code>double</code> representing the configuration value for the
 * given key. If the container has no value for this key, 0.0 will be returned.
 *
 * @param key The key to lookup for the configuration value.
 */
- (double)doubleForKey:(NSString *)key;

/**
 * Returns an <code>int64_t</code> representing the configuration value for the
 * given key. If the container has no value for this key, 0 will be returned.
 *
 * @param key The key to lookup for the configuration value.
 */
- (int64_t)int64ForKey:(NSString *)key;

/**
 * Returns an <code>NSString</code> to represent the configuration value for the
 * given key. If the container has no value for this key, an empty string
 * will be returned.
 *
 * @param key The key to lookup for the configuration value.
 */
- (NSString *)stringForKey:(NSString *)key;

/**
 * Requests that this container be refreshed from the network.
 * This call is asynchronous, so the refresh may take place on another thread.
 */
- (void)refresh;

/**
 * Closes this container so that it will no longer be refreshed.
 * After closing, don't make any other calls to the container.
 */
- (void)close;

/**
 * Returns whether this is a default container, or one refreshed from the
 * server.
 */
- (BOOL)isDefault;

/**
 * Registers the given macro handler to handle a given function call macro.
 *
 * @param handler The handler for the macro. If the parameter is
 *     nil, the method unregisters any existing handler for that macro.
 * @param macroName The name of the macro which is being registered.
 */
- (void)registerFunctionCallMacroHandler:(id<TAGFunctionCallMacroHandler>)handler
                                forMacro:(NSString *)macroName;

/**
 * Given the name of a function call macro, returns the handler registered for
 * the macro.
 *
 * @return The handler registered for the macro.
 */
- (id<TAGFunctionCallMacroHandler>)
    functionCallMacroHandlerForMacro:(NSString *)functionCallMacroName;

/**
 * Registers the given tag handler to handle a given function call tag.
 *
 * @param handler The handler for the tag. If the parameter is
 *     nil, the method unregisters any existing handler for that tag.
 * @param tagName The name of the tag which is being registered.
 */
- (void)registerFunctionCallTagHandler:(id<TAGFunctionCallTagHandler>)handler
                                forTag:(NSString *)tagName;

/**
 * Given the name of a function call tag, returns the handler registered for
 * the tag.
 *
 * @return The handler registered for the tag.
 */
- (id<TAGFunctionCallTagHandler>)functionCallTagHandlerForTag:(NSString *)functionCallTagName;

@end
