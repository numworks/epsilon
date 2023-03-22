# Poincare visualization

You can visualize the evolution of the poincare cache pools by running:
* `make EXECUTABLE=test poincare_log_visualization` to see the evolution accross all unary tests
* `make SCENARIO=tests/calculation/calculation_addition.nws poincare_log_visualization` to see the evolution running a single scenario.


You can also wrap a snippet of code with:
```C++
ResetLogger(LoggerType::Edition);

/* Your code */

CloseLogger(LoggerType::Edition);
```
or
```C++
ResetLogger(LoggerType::Cache);

/* Your code */

CloseLogger(LoggerType::Cache);
```
to reduce the visualization only while executing the snippet.
