# Writing and running tests

Quiz is a simple test framework.

## Write a test

To write a test using quiz, all you have to do is `#include <quiz.h>`, and then
define tests using the `TEST(my_test_name)` macro.

You should then add your test files to the "tests" variable in the Makefile.

## Run tests

Build the test.bin file (preferentially with DEBUG = 1) and run it with lldb.

### Arguments for running tests
- `--headless` : Prevent Epsilon from displaying the calculator's screen. (always run with this argument)

- `--filter my_test_name` or `-f my_test_name` : Only run one test.

- `--skip-assertions` or `-s` : Prevent the runner to stop when a test fails.
