#import <Foundation/Foundation.h>

/**
 * Represents an object missing in an NSArray. If the data layer currently
 * contains:
<pre>
 *   {
 *     myArray: [1, 2, 3]
 *   }
</pre>
 * and you want to update the third element to 5, while leaving other elements
 * alone, you'd use <code>kTAGDataLayerNotPresent</code> as the 1st and 2nd
 * elements (if you were to use a <code>NSNull</code> object,
 * the 1st and 2nd elements would be replaced).
 * <p>
 * So, after executing:
<pre>
 *  [dataLayer push:@{@"myArray",
 *      @[kTAGDataLayerObjectNotPresent, kTAGDataLayerObjectNotPresent, @5}];
</pre>
 * then, the data layer will contain:
<pre>
 *   {
 *     myArray: [1, 2, 5]
 *   }
</pre>
 */
extern id kTAGDataLayerObjectNotPresent;

/**
 * The data layer is a dictionary holding generic information about the
 * application. It uses a standard set of keys so it can be read by any party
 * that understands the specification. The data layer state is updated
 * through its API. For example, an app might start with the following
 * dataLayer:
<pre>
 *   {
 *     title: "Original screen title"
 *   }
</pre>
 *
 * As the state/data of an app can change, the app can update the dataLayer with a call such as:
<pre>
 *   [dataLayer push:@{@"title": @"New screen title"}];
</pre>
 *  Now the data layer contains:
<pre>
 *   {
 *     title: "New screen title"
 *   }
</pre>
 * After another push happens:
<pre>
 *   [dataLayer push:@{@"xyz": @3}];
</pre>
 * The dataLayer contains:
<pre>
 *   {
 *     "title": "New screen title",
 *     "xyz": 3
 *   }
</pre>
 * The following example demonstrates how array and map merging works. If the
 * original dataLayer contains:
<pre>
 *   {
 *     "items": @[@"item1", [NSNull null], @"item2", @{@"a": @"aValue", @"b": @"bValue"}]
 *   }
</pre>
 * After this push happens:
<pre>
 *   [dataLayer push:@{@"items":
 *       @[[NSNull null], @"item6", kTAGDataLayerObjectNotPresent, @{@"a": [NSNull null]}]}
</pre>
 * The dataLayer contains:
<pre>
 *   {
 *     "items": @[[NSNull null], @"item6", @"item2", @{@"a": [NSNull null], @"b": @"bValue"}]}
 *   }
</pre>
 * <p>Pushes happen synchronously; after the push, changes have been reflected
 * in the model.
 * <p>When an <code>event</code> key is pushed to the data layer, rules for tags
 * are evaluated and any tags matching this event will fire.
 * For example, given a container with a tag whose firing rules is that "event"
 * is equal to "openScreen", after this push:
<pre>
 *   [dataLayer push:@{@"event", @"openScreen"}];
</pre>
 * that tag will fire.
 */
@interface TAGDataLayer : NSObject

@property(readonly, nonatomic) NSDictionary *dataLayer;

/**
 * Pushes a key/value pair to the data layer. This is just a convenience
 * method that calls <code>push:@{key: value}</code>.
 */
- (void)pushValue:(id)value forKey:(id)key;

/**
 * Merges the given <code>update</code> object into the existing data model,
 * calling any listeners with the update (after the merge occurs).
 *
 * <p>It's valid for values in the dictionary (or embedded Arrays) to be
 * of type <code>NSNull</code>.
 * If you want to represent a missing value (like an empty index in a List),
 * use the <code>kTAGDataLayerObjectNotPresent</code> object.
 *
 * <p>This is normally a synchronous call.
 * However, if, while the thread is executing the push, another push happens
 * from the same thread, then that second push is asynchronous (the second push
 * will return before changes have been made to the data layer).  This second
 * push from the same thread can occur, for example, if a data layer push is
 * made in response to a tag firing. However, all updates will be processed
 * before the outermost push returns.
 * <p>If the <code>update</code> contains the key <code>event</code>, rules
 * will be evaluated and matching tags will fire.
 *
 * @param update The update object to process
 */
- (void)push:(NSDictionary*)update;

/**
 * Returns the object in the model associated with the given key.
 * If key isn't present, returns <code>nil</code>. If
 * <code>key</code> is present, but <code>NSNull</code>, returns
 * <code>NSNull</code>.
 *
 * <p>The key can can have embedded periods. For example:
 * a key of <code>"a.b.c"</code> returns the value of the "c" key in
 * the dictionary with key "b" in the dictionary with key "a" in the model.
 */
- (id)get:(NSString*)key;

@end
