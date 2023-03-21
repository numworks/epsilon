# Poincare visualization

You can visualize the evolution of the poincare cache pools by running:
* `make EXECUTABLE=test poincare_log_visualization` to see the evolution accross all unary tests
* `make SCENARIO=tests/calculation/calculation_addition.nws poincare_log_visualization` to see the evolution running a single scenario.


You can also wrap a snippet of code with:
```C++
ResetCacheLogger();

/* Your code */

CloseCacheLogger();
```

to visualize the evolution of the pools only while executing the snippet.
