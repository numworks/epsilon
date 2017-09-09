class PythonStdoutCatcher {
public:
  ~PythonStdoutCatcher();

  virtual void print(const char *str) const = 0;
  void enableCatch() const;
  void disableCatch() const;
} ;
