#ifndef ESCHER_INVOCATION_H
#define ESCHER_INVOCATION_H

namespace Escher {

class Invocation {
 public:
  typedef bool (*Action)(void* context, void* sender);
  Invocation(Action a, void* c);

  template <typename T>
  using TemplatedAction = bool (*)(T* context, void* sender);

  /* If you change this function, make sure it does not create code for each
   * invocation. */
  template <typename T>
  static Invocation Builder(TemplatedAction<T> action, T* context) {
    return Invocation(reinterpret_cast<Action>(action), context);
  }

  bool perform(void* sender);

 private:
  Action m_action;
  void* m_context;
};

}  // namespace Escher

#endif
