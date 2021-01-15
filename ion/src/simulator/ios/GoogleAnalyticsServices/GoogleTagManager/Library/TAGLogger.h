// Copyright 2013 Google Inc. All rights reserved.
/** @file */

#import <Foundation/Foundation.h>

/**
 * Log Level setting.
 */
typedef enum {
  /** Log level of Verbose. */
  kTAGLoggerLogLevelVerbose,

  /** Log level of Debug. */
  kTAGLoggerLogLevelDebug,

  /** Log level of Info. */
  kTAGLoggerLogLevelInfo,

  /** Log level of Warning. */
  kTAGLoggerLogLevelWarning,

  /** Log level of Error. */
  kTAGLoggerLogLevelError,

  /** Log level of None. */
  kTAGLoggerLogLevelNone
} TAGLoggerLogLevelType;

/**
 * A protocol for error/warning/info/debug/verbose logging.
 *
 * By default, Google Tag Manager logs error/warning messages and
 * ignores info/debug/verbose messages. You can install your own logger
 * by setting the TAGManager::logger property.
 */
@protocol TAGLogger<NSObject>

/**
 * Logs an error message.
 *
 * @param message The error message to be logged.
 */
- (void)error:(NSString *)message;

/**
 * Logs a warning message.
 *
 * @param message The warning message to be logged.
 */
- (void)warning:(NSString *)message;

/**
 * Logs an info message.
 *
 * @param message The info message to be logged.
 */
- (void)info:(NSString *)message;

/**
 * Logs a debug message.
 *
 * @param message The debug message to be logged.
 */
- (void)debug:(NSString *)message;

/**
 * Logs a verbose message.
 *
 * @param message The verbose message to be logged.
 */
- (void)verbose:(NSString *)message;

/**
 * Sets the log level. It is up to the implementation how the log level is used,
 * but log messages outside the log level should not be output.
 */
- (void)setLogLevel:(TAGLoggerLogLevelType)logLevel;

/**
 * Returns the current log level.
 */
- (TAGLoggerLogLevelType)logLevel;

@end
