# Poincaré

## Memory management

Expressions/Layout can exist in 3 different locations:
- in FileSystem (or alternatively in App::Snapshot),
- in TreeCache,
- in TreeSandbox.

### FileSystem expressions (or App::Snapshot)

Expressions there are just a memory 'dump' of the sequential trees. There are never modified. They're added or removed using the FileSystem API. When stored in an App::Snapshot, they're added/removed using the specific API of the model contained in the App::Snapshot.

### TreeCache

Expressions have a temporary lifetime. They're referred to by a id and they're deleted when the cache is full or when the tree sandbox needs more space.
We still have to decide which cache algorithm we choose: LRU, ref-counter, FIFO, LFU.

Because they're lifetime is unknown, we use CachedTree to interact with them. CachedTree is the combination of a potentially cached tree id and a function to rebuild the tree from some data if the tree has been removed from cache.

#### Tree Cache invalidation

Any tree manipulation is done through the TreeCache::execute function which sets an Exception Checkpoint before starting the tree manipulation. TreeSandbox functions that are likely to overflow the pool can raise an exception with will longjmp to this checkpoint.

### TreeSandbox

The sandbox contain the tree that is being edited.

At the end of a procedure, only one single tree exists on the sandbox but within a procedure editing the tree, the sandbox can host several trees.


## Tree representation

Trees are represented by sequentially compact blocks of 1 byte.

### Blocks

A block is the elementary object. It can hold a type (Addition, Integer etc) or a value.

### Nodes

Nodes are composed of several blocks. When they're composed by more than 1 block, they start and end by the same TypeBlock. This enables to easily find the parent of a node by scanning backwards.

Examples:
1.
[INT][LENGTH][DIGIT0][DIGIT1]...[DIGITN][LENGTH][INT]
2.
[ADDITION][LENGTH][ADDITION]
3.
[COSINE]

### Trees

Tree are derived from parsing.

### Pseudo-virtuality

We don't want to make block virtual to keep their size < 1 byte. We reimplement a virtuality with homemade v-table based on the block types.


## Interruption

Checkpoints outside of a tree edition are easily maintained thanks to Cached trees. We can also easily add Checkpoints in the sandbox but I'm not even sure this will be necessary.

## QUESTIONS

Is this easy enough to implement methods transforming trees?

## Archive

### Tree representation

3 options to sequentially store expressions:
Let's take the example 4294967298 + cos(3)
(keeping in mind that 4294967298 = 2^32 + 2)

() represents nodes
[] represents blocks
Trees are derived from parsing

- option A
  Nodes have variable sizes that are computed dynamically
  ( + 2 ) (int 1 2) (cos ) (int 3)
- option B
  Nodes are composed of several blocks, the head blocks have always their
  first bit set 0 and the tail blocks have their first bit set to 1 (drawn by ^)
  ([+][^2])([int][^1][^2])([cos])([int][^3])
- option C
  Nodes are composed of several blocks. Block types and length are indicated as head and tail.
  ([+][2][+])([int][2][1][2][2][int])([cos])([int_short][3])

Comparison:
  +------------------+-----------------+------------------|-----------------+
  |                  | Option A        | Option B         | Option C        |
  |------------------+-----------------+------------------|-----------------|
  | Compacity        | ✓               | ✓                | x               |
  |                  |                 | 1 bit/block lost | Meta blocks for |
  |                  |                 |                  | nodes of size>1 |
  |------------------+-----------------+------------------|-----------------|
  | Parent retrieve  | x               | ✓                | ✓               |
  |                  | Has to scan the | Backward scan    | Backward scan   |
  |                  | whole pool      |                  |                 |
  |------------------+-----------------+------------------|-----------------|
  | Value extraction | ✓               | x                | ✓               |
  |                  | Maybe align     | Requires masks   | Maybe align     |
  |                  | float/double    |                  | float/double    |
  +------------------+-----------------+------------------+-----------------+

Optional optimizations:
- Align float and double by letting empty blocks
- Create special type for common integer INT_ONE or INT_SHORT

Cache invalidation:

Attempt 1:
When trying to push on sandbox the block that overflows, the sandbox automatically empty the cache, copy the whole sandbox upstream and repush the block.
--> This means that tree block addresses might become invalid without any notice in tree-manipulating function...

Attempt 2:
When trying to push the overflowing block on the sandbox, it raises an exception. All entry points of Poincaré are wrapted by an handler doing:
void cacheHandler(action) {
  if (setCheckpoint) {
    action()
  } else {
    cache->removeLastUsedTrees();
    action();
  }
}


