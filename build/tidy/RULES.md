# Epsilon tidy rules

## ETR1 - Global variables need to be resettable

We want to offer the ability to completely reset the state of Epsilon. For example, this is indeed useful in:
 - Fuzzing
 - On simulators, to easily re-load a state

Given that we don't make use of heap memory, the only remaining state is in global variables. We enforce two rules to make sure global variables can be reset.

### ETR1A - Local variables with global storage are forbidden
This rules states that local variables with static storage are forbidden. For example, this code is not accepted:
```c++
void foo() {
  static int a;
}
```
In that case, you should refactor your code to either use an instance variable or a global non-local variable.

### ETR1B - Non-constant global variables must be wrapped in GlobalBox
According to this rule, global variables must be wrapped in an `OMG::GlobalBox`.
```c++
class MyObject {
public:
  void myMethod();
}

OMG::GlobalBox<MyObject> Foo;

Foo.init();
Foo.shutdown();
Foo->myMethod();
```
