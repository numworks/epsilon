extern "C" {

/* Exit is used when we intentionally exit want to stop like in the unit
 * tests. Freeze the device so that the error message can be read. */
__attribute__((noreturn)) void exit(int code) {
  /* Note : "while (!condition) {}" could be omitted by the compiler because
   * an iteration statement whose controlling expression is not a constant
   * expression, that performs no input/output operations, does not access
   * volatile objects, and performs no synchronization or atomic operations in
   * its body, controlling expression, or (in the case of a for statement) its
   * expression-3, may be assumed by the implementation to terminate.
   */
  while (true) {
  }
}
}
