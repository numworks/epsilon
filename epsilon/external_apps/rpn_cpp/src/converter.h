#pragma once

class Converter {
 public:
  static int Parse(const char* text);
  static void Serialize(int value, char* serialization, int size);
};
