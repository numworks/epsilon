# Poincaré

## Memory management

Expressions/Layout can exist in 3 different locations:
- in FileSystem (or alternatively in App::Snapshot),
- in CachePool,
- in EditionPool.

### FileSystem expressions (or App::Snapshot)

Expressions there are just a memory 'dump' of the sequential trees. There are never modified. They're added or removed using the FileSystem API. When stored in an App::Snapshot, they're added/removed using the specific API of the model contained in the App::Snapshot.

### CachePool

Expressions have a temporary lifetime. They're referred to by a id and they're deleted when the cache is full (no new identifiers to attribute) or when the Edition Pool needs more space.
We still have to decide which cache algorithm we choose: LRU, ref-counter, FIFO, LFU.
Marc idea: the invalidating function should take into account: last used, time-to-compute, space-taken

Because they're lifetime is unknown, we use CacheReference to interact with them. CacheReference is the combination of a potentially cached tree identifier and a function to rebuild the tree from some data if the tree has been removed from cache.

#### Cache invalidation

Any tree manipulation is done through the CachePool::execute function which sets an Exception Checkpoint before starting the tree manipulation. EditionPool functions that are likely to overflow the pool can raise an exception with will longjmp to this checkpoint.

### EditionPool

The EditionPool contains the tree that is being edited.

At the end of a procedure, only one single tree exists on the EditionPool but within a procedure editing the tree, the EditionPool can host several trees.

We use EditionReference to interact with Node while they're being edited. EditionReference are an internal tool that should not be available outside Poincare.


## Tree representation

Trees are represented by sequentially compact blocks of 1 byte.

### Block

A block is the elementary object. It can hold a type (Addition, Integer etc) or a value.

### Node

Nodes are composed of several blocks. When they're composed by more than 1 block, they start and end by the same TypeBlock. This enables to easily find the parent of a node by scanning backwards.

Examples:
1.
[INT][LENGTH][DIGIT0][DIGIT1]...[DIGITN][LENGTH][INT]
2.
[ADDITION][LENGTH][ADDITION]
3.
[COSINE]

### Trees

Tree are derived from parsing nodes.
Tree class also refers to a constexpr constructor of nodes sequences.

### Pseudo-virtuality - interfaces

We don't want to make block virtual to keep their size < 1 byte. We reimplement a virtuality with homemade v-table based on the block types.

Do we?

13th October: what is the +/- of interfaces?

Policy based Design?
  // EExpressionInterface<LayoutPolicy, SimplificationPolicy>...
  // What about decreasing v-table sizes?
  // EExpressionInterface
  // InternalEExpressionInterface
  // AlgebraicEExpressionInterface, FunctionEExpressionInterface etc?
p

Let define:
N number of block types
F number of functions

Interfaces requires:
-> 1 switch of N entries (to decide the right interface for one block type)
-> N v-tables of size F
(-> N consexpr instances of interfaces)

C-style switch requires:
-> F switchs of size < N

FxN < Nx(F + 1)

We'll go for C-style.



## Interruption

Checkpoints outside of a tree edition are easily maintained thanks to Cache References. We can also easily add Checkpoints in the EditionPool but I'm not even sure this will be necessary.


## Polynomial

Representation?
http://www.maia.ub.es/dsg/sam07/sam07poly.pdf

This is an optimization! Anyway, we still to recognize polynomials from their tree structure.

If we add the sparse representation:
- list of variables (x, y)
- list of pairs (coefficient, (exponent variables 1, exponent variable 2)
We would try to keep a fixed-size pair which would force integer < 2^31 coefficients? Therefore, we won't be able to represent all polynomials.

Variables
Numerator
Denominator
rational_simplify/normal

## Number

Nodes: Zero, One, Two, Integer, ShortInteger, Rational, ShortRational

Operations on Integers are done on IntegerHandler.
Operations on Rational are done on Expression
StaticIntegerInterface --> DefaultInterface, ShortIntegerInterface
...

## Ideas

We could create a node "link" to another tree to avoid duplicating whole subtree (when expanding multiplication for instance)

## QUESTIONS

Is this easy enough to implement methods transforming trees?

## TODOs

Implement a profiler mecanism to identify Poincare hot paths.

Divide Poincare folder to hide the internal tools (node, EditionReference, Interfaces...) and expose the useful API (solver, dataset, CacheReference etc).

Optional optimization: editing a subtree which is at the beginning of the edited tree is very memcpy expensive. We could move the subtree at the end of the edition pool before doing multiple modification and reinsert it in the tree afterwards.

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

Instead of stocking meta data in a header AND a footer we could store them only in a header and suffix nodes with their length. This means that all nodes are at least 2-block long but variable-sized nodes are shorter (integer, string...)

Cache invalidation:

Attempt 1:
When trying to push on sandbox the block that overflows, the sandbox automatically empty the cache, copy the whole sandbox upstream and repush the block.
--> This means that tree block addresses might become invalid without any notice in tree-manipulating function...

Attempt 2:
When trying to push the overflowing block on the sandbox, it raises an exception. All entry points of Poincaré are wrapped by an handler doing:
void cacheHandler(action) {
  if (setCheckpoint) {
    action()
  } else {
    cache->removeLastUsedTrees();
    action();
  }
}

### Interfaces

Convention:
Interface is the common interface of all nodes

ExpressionInterface is a virtual access to all expression functions that most subclasses override.
We can add a method to expression if at least one third of its subclasses override it. Why did we chose the trade-off?
If we add a method to expression, we add N v-table entries; with N the number of ExpressionInterface subclasses. If we rather create another hierarchy of interfaces like TrigonometryInterface, we create (n is the number of overriding subclasses):
- n subclasses, ie, n v-table
- n instances of these subclasses
- a switch on n node type that return the right subclass instance.
So we choose to do this when 3n < N.

 We don't forget to assert false all methods that should not exist on ExpressionInterface subclasses

 Approximater implements approximate: division, addition implements are ApproximationInterface but not some specific expressions that are always reduces in Default reduction and that never reappears in reduceForApproximation

  InternalExpressionInterface:
   - reduceForApproximation (recognize /, - and reintroduce them to ensure the most precise approximation)
   - beautify (some change just to return beautiful results - no root at denominator etc)

 Node::??Interface returns a nullptr if the type was wrong?

### Matrix, List, Unit?

 - matrix reduction
 - list reduction
 - unit reduction
 -


 List is a complicated thing!
 We want list of matrices, list of float (with optimality), list of units. Le noeud list doit pouvoir prendre n'importe quoi comme éléments/ List de taille fixe ? Peut-etre de types de liste ? ExpressionList et List
 List >> Matrix >> Units

 - basicReduction
 Checkpoint
 - bubble-up list, matrix, unit

 Si un bubble up ne fonctionne pas car il génère trop de noeuds, bubbleUpWithApproximation qui réduit les noeufs au fur et à mesure

 - routine approximateToScalar


 Parcours des enfants optimals ??

## Random

Félix's idea: replace all random nodes by indexed system variables. This way, several random() can't be factorized and expanding one random two several will give several nodes that are evaluated to the same value.

## Dependency

Root(expression, dependencies)?

## Memoization of expression properties

Memoize inside the node (??) some computed properties (for ex, the sign?) to avoid recomputing it n times.


## Exam mode + non-CAS country

Flag 'I used forbidden CAS' to be returned with all expressions' public methods?

## Rules for developping

Avoid childAtIndex: use nextTree() or NodeIterator

When editing children of a tree, iterate in the right direction to always change the downstream children
