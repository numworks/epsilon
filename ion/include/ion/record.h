#ifndef ION_RECORD_H
#define ION_RECORD_H

#include <stdint.h>
#include <stddef.h>

/* Record  : | Total Size | Type | Name | Body | */

class Record {
public:
  enum class Type : uint8_t {
    Null,
    Script
  };
  enum class ErrorStatus {
    None = 0,
    NameTaken = 1,
    NameTooLong = 2,
    NoEnoughSpaceAvailable = 3
  };
  Record(size_t * totalSize = nullptr, char * name = nullptr, Type type = Type::Null, char * body = nullptr);

  bool isNull() const;

  const char * name() const;
  ErrorStatus rename(const char * newName); // May fail if name taken or name too long

  const char * read() const;
  ErrorStatus write(const char * data, size_t size = 0); // May fail if no more space is available

  Type type();

  void remove(); // Will always succeed
  constexpr static size_t k_nameSize = 50;
  constexpr static size_t k_sizeSize = sizeof(size_t);
  constexpr static size_t k_typeSize = sizeof(Type);
protected:
  size_t bodySize() const;
  char * start();
  char * m_body;
  size_t * m_size;
private:
  char * m_name;
  Type m_type;
};

#endif
