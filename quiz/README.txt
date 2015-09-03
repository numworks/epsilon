Quiz is a simple test framework.

To write a test using quiz, all you have to do is #include quiz.h, and then
define tests using the TEST(my_test_name) macro.

You should then add your test files to the "tests" variable in the Makefile.

Then running "make test" will compile and run your tests!
