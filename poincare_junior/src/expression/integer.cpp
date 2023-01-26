#include <new>
#include <algorithm>
#include "integer.h"
#include "rational.h"
#include <poincare_junior/src/memory/value_block.h>
#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <omgpj/arithmetic.h>
#include <omg/print.h>

namespace PoincareJ {

/* WorkingBuffer */

WorkingBuffer::WorkingBuffer() :
  m_start(initialStartOfBuffer()),
  m_remainingSize(initialSizeOfBuffer())
{
}

native_uint_t * WorkingBuffer::allocate(size_t size) {
  assert(size < IntegerHandler::k_maxNumberOfNativeDigits + 1);
  if (size > m_remainingSize) {
    // TODO: set the error type to be "Integer computation requires to much space"/"edition pool overflowed"
    ExceptionCheckpoint::Raise();
    return nullptr;
  }
  native_uint_t * allocatedMemory = m_start;
  m_start += size;
  m_remainingSize -= size;
  return allocatedMemory;
}

void WorkingBuffer::garbageCollect(std::initializer_list<IntegerHandler *> keptIntegers) {
  uint8_t * previousStart = reinterpret_cast<uint8_t *>(m_start);
  uint8_t * previousEnd = reinterpret_cast<uint8_t *>(m_start + m_remainingSize);
  m_start = initialStartOfBuffer();
  m_remainingSize = initialSizeOfBuffer();
  for (IntegerHandler * integer : keptIntegers) {
    // TODO: assert that the Integer are sorted by digits() pointer
    if (previousStart <= integer->digits() && integer->digits() < previousEnd) {
      uint8_t nbOfNativeDigits = integer->numberOfDigits<native_uint_t>();
      native_uint_t * newDigitsPointer = allocate(nbOfNativeDigits);
      memmove(newDigitsPointer, integer->digits(), nbOfNativeDigits * sizeof(native_uint_t));
      *integer = IntegerHandler(newDigitsPointer, nbOfNativeDigits, integer->sign());
    }
  }
}

/* IntegerHandler */

IntegerHandler::Digits::Digits(const uint8_t * digits, uint8_t numberOfDigits) {
  if (numberOfDigits == 1) {
    m_digit = digits[0];
  } else {
    m_digits = numberOfDigits == 0 ? nullptr : digits;
  }
}

template <typename T>
IntegerHandler::IntegerHandler(const T * digits, uint8_t numberOfDigits, NonStrictSign sign) {
  uint8_t sizeInByte = numberOfDigits * sizeof(T);
  const uint8_t * byteDigits = reinterpret_cast<const uint8_t *>(digits);
  while (sizeInByte > 0 && byteDigits[sizeInByte - 1] == 0) {
    sizeInByte--;
  }
  new (this) IntegerHandler(byteDigits, sizeInByte, sign);
}

EditionReference IntegerHandler::pushOnEditionPool() {
  if (isZero()) {
    return EditionReference::Push<BlockType::Zero>();
  }
  if (isOne()) {
    return EditionReference::Push<BlockType::One>();
  }
  if (isTwo()) {
    return EditionReference::Push<BlockType::Two>();
  }
  if (isMinusOne()) {
    return EditionReference::Push<BlockType::MinusOne>();
  }
  if (isSignedType<int8_t>()) {
    return EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(*this));
  }
  EditionPool * pool = EditionPool::sharedEditionPool();
  TypeBlock typeBlock(sign() == NonStrictSign::Negative ? BlockType::IntegerNegBig : BlockType::IntegerPosBig);
  EditionReference reference = EditionReference(Node(pool->pushBlock(typeBlock)));
  pool->pushBlock(m_numberOfDigits);
  pushDigitsOnEditionPool();
  pool->pushBlock(m_numberOfDigits);
  pool->pushBlock(typeBlock);
  return reference;
}

void IntegerHandler::pushDigitsOnEditionPool() {
  EditionPool * pool = EditionPool::sharedEditionPool();
  assert(m_numberOfDigits < k_maxNumberOfDigits);
  for (size_t i = 0; i < m_numberOfDigits; i++) {
    pool->pushBlock(ValueBlock(digit(i)));
  }
}

template <typename T>
T IntegerHandler::to() {
  /* TODO: use the previous Integer::approximate implementation which stops when
   * the mantissa is complete */
  T approximation = 0.0f;
  if (numberOfDigits() == 0) {
    return approximation;
  }
  for (uint8_t i = numberOfDigits() - 1; i > 0; i--) {
    approximation += static_cast<T>(digit(i));
    approximation *= k_digitBase;
  }
  approximation += static_cast<T>(digit(0));
  return static_cast<int8_t>(m_sign) * approximation;
}

/* Getters */

const uint8_t * IntegerHandler::digits() const {
  if (usesImmediateDigit()) {
    return &m_digitAccessor.m_digit;
  }
  return m_digitAccessor.m_digits;
}

uint8_t IntegerHandler::digit(int i) const {
  assert(m_numberOfDigits > i);
  return digits()[i];
}

template <typename T>
uint8_t IntegerHandler::numberOfDigits() const {
  assert(sizeof(T) > 1 && sizeof(uint8_t) == 1);
  uint8_t nbOfDigits = numberOfDigits();
  if (nbOfDigits == 0) {
    return 0;
  }
  return Arithmetic::CeilDivision<uint8_t>(nbOfDigits, sizeof(T));
}

template <typename T>
T IntegerHandler::digit(int i) const {
  assert(i >= 0);
  if (i >= numberOfDigits<T>()) {
    return 0;
  }
  if (i == numberOfDigits<T>() - 1) {
    uint8_t numberOfByteDigits = numberOfDigits();
    uint8_t numberOfRemainderDigits = numberOfByteDigits % sizeof(T);
    if (numberOfRemainderDigits > 0) {
      // We manually build the last incomplete digit
      T newDigit = 0;
      for (size_t index = 0; index < numberOfRemainderDigits; index++) {
        uint8_t byteDigit = digit(--numberOfByteDigits);
        newDigit = (newDigit << Bit::k_numberOfBitsInByte) | byteDigit;
      }
      return newDigit;
    }
  }
  return (reinterpret_cast<const T *>(digits()))[i];
}

/* Properties */

bool IntegerHandler::isZero() const {
  assert(m_numberOfDigits != 0 || m_sign == NonStrictSign::Positive); // TODO: should we represent -0?
  return m_numberOfDigits == 0;
}

template <typename T>
bool IntegerHandler::isSignedType() const {
  size_t maxNumberOfDigits = sizeof(T)/sizeof(uint8_t);
  return m_numberOfDigits < maxNumberOfDigits || (m_numberOfDigits == maxNumberOfDigits && digit(maxNumberOfDigits - 1) <= INT8_MAX);
}

template <typename T>
bool IntegerHandler::isUnsignedType() const {
  size_t maxNumberOfDigits = sizeof(T)/sizeof(uint8_t);
  return m_numberOfDigits <= maxNumberOfDigits && m_sign == NonStrictSign::Positive;
}

IntegerHandler::operator int8_t() const {
  assert(isSignedType<int8_t>()); return numberOfDigits() == 0 ? 0 : static_cast<int8_t>(m_sign) * digit(0);
}

IntegerHandler::operator uint8_t() const {
  assert(isUnsignedType<uint8_t>());
  return numberOfDigits() == 0 ? 0 : digit(0);
}

/* Arithmetics */

int IntegerHandler::Compare(const IntegerHandler & i, const IntegerHandler & j) {
  if (i.sign() != j.sign()) {
    return i.sign() == NonStrictSign::Negative ? -1 : 1;
  }
  return static_cast<int8_t>(i.sign()) * Ucmp(i, j);
}

int8_t IntegerHandler::Ucmp(const IntegerHandler & a, const IntegerHandler & b) {
  if (a.numberOfDigits() < b.numberOfDigits()) {
    return -1;
  } else if (a.numberOfDigits() > b.numberOfDigits()) {
    return 1;
  }
  assert(a.numberOfDigits<native_uint_t>() == b.numberOfDigits<native_uint_t>());
  for (uint16_t i = 0; i < a.numberOfDigits<native_uint_t>(); i++) {
    // Digits are stored most-significant last
    uint8_t aDigit = a.digit<native_uint_t>(a.numberOfDigits()-i-1);
    uint8_t bDigit = b.digit<native_uint_t>(b.numberOfDigits()-i-1);
    if (aDigit < bDigit) {
      return -1;
    } else if (aDigit > bDigit) {
      return 1;
    }
  }
  return 0;
}

EditionReference IntegerHandler::Addition(const IntegerHandler & a, const IntegerHandler & b) {
  WorkingBuffer workingBuffer;
  return Sum(a, b, false, &workingBuffer).pushOnEditionPool();
}

EditionReference IntegerHandler::Subtraction(const IntegerHandler & a, const IntegerHandler & b) {
  WorkingBuffer workingBuffer;
  return Sum(a, b, true, &workingBuffer).pushOnEditionPool();
}

IntegerHandler IntegerHandler::Sum(const IntegerHandler & a, const IntegerHandler & b, bool inverseBNegative, WorkingBuffer * workingBuffer, bool oneDigitOverflow) {
  NonStrictSign bSign = inverseBNegative ? InvertSign(b.sign()) : b.sign();
  IntegerHandler usum;
  if (a.sign() == bSign) {
    usum = Usum(a, b, false, workingBuffer, oneDigitOverflow);
    usum.setSign(bSign);
  } else {
    /* The signs are different, this is in fact a subtraction
     * s = a+b = (abs(a)-abs(b) OR abs(b)-abs(a))
     * 1/abs(a)>abs(b) : s = sign*udiff(a, b)
     * 2/abs(b)>abs(a) : s = sign*udiff(b, a)
     * sign? sign of the greater! */
    if (Ucmp(a, b) >= 0) {
      usum = Usum(a, b, true, workingBuffer, oneDigitOverflow);
      usum.setSign(a.sign());
    } else {
      usum = Usum(b, a, true, workingBuffer, oneDigitOverflow);
      usum.setSign(bSign);
    }
  }
  return usum;
}

IntegerHandler IntegerHandler::Usum(const IntegerHandler & a, const IntegerHandler & b, bool subtract, WorkingBuffer * workingBuffer, bool oneDigitOverflow) {
  uint8_t size = std::max(a.numberOfDigits<native_uint_t>(), b.numberOfDigits<native_uint_t>());
  if (!subtract) {
    // Addition can overflow
    size++;
  }
  native_uint_t * sumBuffer = workingBuffer->allocate(std::min<uint8_t>(size, k_maxNumberOfNativeDigits + oneDigitOverflow));
  bool carry = false;
  for (uint8_t i = 0; i < size; i++) {
    native_uint_t aDigit = a.digit<native_uint_t>(i);
    native_uint_t bDigit = b.digit<native_uint_t>(i);
    native_uint_t result = (subtract ? aDigit - bDigit - carry : aDigit + bDigit + carry);
    if (i < k_maxNumberOfNativeDigits + oneDigitOverflow) {
      sumBuffer[i] = result;
    } else {
      if (result != 0) {
        // TODO: set the error type to be "Integer computation overflowed"
        ExceptionCheckpoint::Raise();
        return IntegerHandler();
      }
    }
    if (subtract) {
      carry = (aDigit < result) || (carry && aDigit == result); // There's been an underflow
    } else {
      carry = (aDigit > result) || (bDigit > result); // There's been an overflow
    }
  }
  size = std::min<uint8_t>(size, k_maxNumberOfNativeDigits + oneDigitOverflow);
  return IntegerHandler(sumBuffer, size);
}

EditionReference IntegerHandler::Multiplication(const IntegerHandler & a, const IntegerHandler & b) {
  WorkingBuffer workingBuffer;
  return Mult(a, b, &workingBuffer).pushOnEditionPool();
}

IntegerHandler IntegerHandler::Mult(const IntegerHandler & a, const IntegerHandler & b, WorkingBuffer * workingBuffer, bool oneDigitOverflow) {
  // TODO: would be Karatsuba or Toom-Cook multiplication worth it?
  // TODO: optimize for squaring?
  uint8_t size = std::min(a.numberOfDigits<native_uint_t>() + b.numberOfDigits<native_uint_t>(), k_maxNumberOfNativeDigits + oneDigitOverflow); // Enable overflowing of 1 digit

  native_uint_t * multBuffer = workingBuffer->allocate(size);
  memset(multBuffer, 0, size * sizeof(native_uint_t));

  uint16_t carry = 0;
  for (uint8_t i = 0; i < a.numberOfDigits<native_uint_t>(); i++) {
    double_native_uint_t aDigit = a.digit<native_uint_t>(i);
    carry = 0;
    for (uint8_t j = 0; j < b.numberOfDigits<native_uint_t>(); j++) {
      double_native_uint_t bDigit = b.digit<native_uint_t>(j);
      /* The fact that aDigit and bDigit are double_native is very important,
       * otherwise the product might end up being computed on single_native size
       * and then zero-padded. */
      double_native_uint_t p = aDigit * bDigit + carry; // TODO: Prove it cannot overflow double_native type
      native_uint_t * l = (native_uint_t *)&p;
      if (i + j < k_maxNumberOfNativeDigits + oneDigitOverflow) {
        p += multBuffer[i + j];
        multBuffer[i + j] = l[0];
      } else {
        if (l[0] != 0) {
          // TODO: set the error type to be "Integer computation overflowed"
          ExceptionCheckpoint::Raise();
          return IntegerHandler();
        }
      }
      carry = l[1];
    }
    if (i + b.numberOfDigits<native_uint_t>() < k_maxNumberOfNativeDigits + oneDigitOverflow) {
      multBuffer[i + b.numberOfDigits<native_uint_t>()] += carry;
    } else {
      if (carry != 0) {
        // TODO: set the error type to be "Integer computation overflowed"
        ExceptionCheckpoint::Raise();
        return IntegerHandler();
      }
    }
  }
  return IntegerHandler(multBuffer, size, a.sign() == b.sign() ? NonStrictSign::Positive : NonStrictSign::Negative);
}

std::pair<EditionReference, EditionReference> IntegerHandler::Division(const IntegerHandler & numerator, const IntegerHandler & denominator) {
  WorkingBuffer workingBuffer;
  auto [quotient, remainder] = Udiv(numerator, denominator, &workingBuffer);
  if (!remainder.isZero() && numerator.sign() == NonStrictSign::Negative) {
    quotient = Usum(quotient, IntegerHandler(1), false, &workingBuffer);
    remainder = Usum(denominator, remainder, true, &workingBuffer); // |denominator|-remainder
  }
  quotient.setSign(numerator.sign() == denominator.sign() ? NonStrictSign::Positive : NonStrictSign::Negative);
  EditionReference q = quotient.pushOnEditionPool();
  EditionReference r = remainder.pushOnEditionPool();
  return std::make_pair(q, r);
}

std::pair<IntegerHandler, IntegerHandler> IntegerHandler::Udiv(const IntegerHandler & numerator, const IntegerHandler & denominator, WorkingBuffer * workingBuffer) {
  /* Modern Computer Arithmetic, Richard P. Brent and Paul Zimmermann
   * (Algorithm 1.6) */
  // TODO: implement Svoboda algorithm or divide and conquer methods
  assert(!denominator.isZero());
  if (Ucmp(numerator, denominator) < 0) {
    return std::make_pair(IntegerHandler(static_cast<int8_t>(0)), numerator);
  }
  /* Let's call beta = 1 << 16 */
  /* Normalize numerator & denominator:
   * Find A = 2^k*numerator & B = 2^k*denominator such as B > beta/2
   * if A = B*Q+R (R < B) then numerator = denominator*Q + R/2^k. */
  half_native_uint_t b = denominator.digit<half_native_uint_t>(denominator.numberOfDigits<half_native_uint_t>()-1);
  half_native_uint_t halfBase = 1 << (16-1);
  int pow = 0;
  assert(b != 0);
  while (!(b & halfBase)) {
    b = b << 1;
    pow++;
  }
  IntegerHandler A = numerator.multiplyByPowerOf2(pow, workingBuffer);
  IntegerHandler B = denominator.multiplyByPowerOf2(pow, workingBuffer);

  /* A = a[0] + a[1]*beta + ... + a[n+m-1]*beta^(n+m-1)
   * B = b[0] + b[1]*beta + ... + b[n-1]*beta^(n-1) */
  int n = B.numberOfDigits<half_native_uint_t>();
  int m = A.numberOfDigits<half_native_uint_t>() - n;
  // qDigits is (m+1)-lengthed array of half_native_uint_t
  size_t qSizeInNative = Arithmetic::CeilDivision((m + 1) * sizeof(half_native_uint_t), sizeof(native_uint_t));
  half_native_uint_t * qDigits = reinterpret_cast<half_native_uint_t *>(workingBuffer->allocate(qSizeInNative));
  // Create a IntegerHandler to easily garbage collect
  IntegerHandler Q(reinterpret_cast<uint8_t *>(qDigits), qSizeInNative * sizeof(native_uint_t));
  // The quotient q has at maximum m+1 half digits but we set an extra half digit to 0 to enable to easily convert it from half native digits to native digits
  memset(qDigits, 0, (m + 1 + 1) * sizeof(half_native_uint_t));
  // betaMB = B*beta^m
  IntegerHandler betaMB = B.multiplyByPowerOfBase(m, workingBuffer);
  if (IntegerHandler::Compare(A, betaMB) >= 0) { // A >= B*beta^m
    qDigits[m] = 1; // q[m] = 1
    IntegerHandler newA = Usum(A, betaMB, true, workingBuffer, true); // A-B*beta^m
    workingBuffer->garbageCollect({&B, &Q, &betaMB, &newA});
    A = newA;
  }
  native_uint_t base = 1 << 16;
  for (int j = m - 1; j >= 0; j--) {
    half_native_uint_t bnMinus1 = B.digit<half_native_uint_t>(n-1);
    assert(bnMinus1 != 0);
    native_uint_t qj2 = ((native_uint_t)A.digit<half_native_uint_t>(n+j) * base + (native_uint_t)A.digit<half_native_uint_t>(n+j-1)) / bnMinus1; // (a[n+j]*beta+a[n+j-1])/b[n-1]
    half_native_uint_t baseMinus1 = (1 << 16) - 1; // beta-1
    qDigits[j] = qj2 < (native_uint_t)baseMinus1 ? (half_native_uint_t)qj2 : baseMinus1; // std::min(qj2, beta -1)
    IntegerHandler betaJM = B.multiplyByPowerOfBase(j, workingBuffer); // betaJM = B*beta^j
    IntegerHandler qBj = Mult(qDigits[j], betaJM, workingBuffer, true);
    IntegerHandler newA = IntegerHandler::Sum(A, qBj, true, workingBuffer, true); // A-q[j]*beta^j*B
    workingBuffer->garbageCollect({&B, &Q, &betaMB, &betaJM, &newA});
    A = newA;
    if (A.sign() == NonStrictSign::Negative) {
      while (A.sign() == NonStrictSign::Negative) {
        qDigits[j] = qDigits[j]-1; // q[j] = q[j]-1
        newA = Sum(A, betaJM, false, workingBuffer, true); // A = B*beta^j+A
        workingBuffer->garbageCollect({&B, &Q, &betaMB, &betaJM, &newA});
        A = newA;
      }
    }
    workingBuffer->garbageCollect({&B, &Q, &betaMB, &A});
  }
  IntegerHandler remainder = A;
  if (pow > 0 && !remainder.isZero()) {
    IntegerHandler newRemainder = remainder.divideByPowerOf2(pow, workingBuffer);
    workingBuffer->garbageCollect({&Q, &newRemainder});
    remainder = newRemainder;
  }
  int qNumberOfDigitsInByte = (m + 1) * sizeof(half_native_uint_t);
  uint8_t * qDigitsInByte = reinterpret_cast<uint8_t *>(qDigits);
  while (qDigitsInByte[qNumberOfDigitsInByte - 1] == 0 && qNumberOfDigitsInByte > 1) {
    qNumberOfDigitsInByte--;
  }
  return std::pair<IntegerHandler, IntegerHandler>(IntegerHandler(qDigitsInByte, qNumberOfDigitsInByte), remainder);
}

IntegerHandler IntegerHandler::multiplyByPowerOf2(uint8_t pow, WorkingBuffer * workingBuffer) const {
  assert(pow < 32);
  uint8_t nbOfNativeDigits = numberOfDigits<native_uint_t>();
  native_uint_t * buffer = workingBuffer->allocate(nbOfNativeDigits + 1);
  native_uint_t carry = 0;
  for (uint8_t i = 0; i < nbOfNativeDigits; i++) {
    buffer[i] = digit<native_uint_t>(i) << pow | carry;
    carry = pow == 0 ? 0 : digit<native_uint_t>(i) >> (32 - pow);
  }
  buffer[numberOfDigits()] = carry;
  return IntegerHandler(buffer, carry ? nbOfNativeDigits + 1 : nbOfNativeDigits);
}

IntegerHandler IntegerHandler::divideByPowerOf2(uint8_t pow, WorkingBuffer * workingBuffer) const {
  assert(pow < 32);
  uint8_t nbOfNativeDigits = numberOfDigits<native_uint_t>();
  native_uint_t * buffer = workingBuffer->allocate(nbOfNativeDigits);
  native_uint_t carry = 0;
  for (int i = nbOfNativeDigits - 1; i >= 0; i--) {
    buffer[i] = digit<native_uint_t>(i) >> pow | carry;
    carry = pow == 0 ? 0 : digit<native_uint_t>(i) << (32 - pow);
  }
  return IntegerHandler(buffer, buffer[nbOfNativeDigits - 1] > 0 ? nbOfNativeDigits : nbOfNativeDigits - 1);
}

// return this*(2^16)^pow
IntegerHandler IntegerHandler::multiplyByPowerOfBase(uint8_t pow, WorkingBuffer * workingBuffer) const {
  uint8_t nbOfHalfNativeDigits = numberOfDigits<half_native_uint_t>();
  uint8_t resultNbOfHalfNativeDigit = nbOfHalfNativeDigits + pow;
  uint8_t resultNbOfNativeDigit = Arithmetic::CeilDivision<uint8_t>(resultNbOfHalfNativeDigit, 2);
  half_native_uint_t * buffer = reinterpret_cast<half_native_uint_t *>(workingBuffer->allocate(resultNbOfNativeDigit));
  /* The number of half digits of the built integer is nbOfHalfDigits+pow.
   * Still, we set an extra half digit to 0 to easily convert half digits to
   * digits. */
  memset(buffer, 0, sizeof(native_uint_t) * resultNbOfNativeDigit);
  for (uint8_t i = 0; i < nbOfHalfNativeDigits; i++) {
    buffer[i + pow] = digit<half_native_uint_t>(i);
  }
  return IntegerHandler(buffer, resultNbOfHalfNativeDigit);
}

EditionReference IntegerHandler::Power(const IntegerHandler & i, const IntegerHandler & j) {
  assert(j.sign() == NonStrictSign::Positive);
  if (j.isZero()) {
    // TODO : handle 0^0.
    assert(!i.isZero());
    return IntegerHandler(1).pushOnEditionPool();
  }
  // Exponentiate by squaring : i^j = (i*i)^(j/2) * i^(j%2)
  IntegerHandler i1(1);
  IntegerHandler i2(i);
  IntegerHandler exp(j);
  WorkingBuffer workingBuffer;
  while (!exp.isOne()) {
    auto [quotient, remainder] = Udiv(exp, IntegerHandler(2), &workingBuffer);
    exp = quotient;
    /* The integers given to garbageCollect have to be sorted. We keep trace of
     * the order of exp and i1 in order to respect this assertion. */
    bool i1AfterExp = false;
    if (remainder.isOne()) {
      IntegerHandler i1i2 = Mult(i1, i2, &workingBuffer);
      workingBuffer.garbageCollect({&i2, &exp, &i1i2});
      i1 = i1i2;
      i1AfterExp = true;
    }
    IntegerHandler squaredI2 = Mult(i2, i2, &workingBuffer);
    workingBuffer.garbageCollect({i1AfterExp ? &exp : &i1, i1AfterExp? &i1 : &exp, &squaredI2});
    i2 = squaredI2;
  }
  workingBuffer.garbageCollect({&i1, &i2});
  return Mult(i1, i2, &workingBuffer).pushOnEditionPool();
}

EditionReference IntegerHandler::Factorial(const IntegerHandler & i) {
  assert(i.sign() == NonStrictSign::Positive);
  IntegerHandler j(2);
  IntegerHandler result(1);
  WorkingBuffer workingBuffer;
  while (Ucmp(i, j) >= 0) {
    result = Mult(j, result, &workingBuffer);
    j = Usum(j, IntegerHandler(1), false, &workingBuffer);
    workingBuffer.garbageCollect({&result, &j});
  }
  return result.pushOnEditionPool();
}

/* Integer */

EditionReference Integer::Push(const char * digits, size_t length, OMG::Base base) {
  EditionReference result = IntegerHandler(static_cast<uint8_t>(0)).pushOnEditionPool();
  NonStrictSign sign = NonStrictSign::Positive;
  if (digits != nullptr && *digits == '-') {
    sign = NonStrictSign::Negative;
    digits++;
    length--;
  }
  assert(digits != nullptr);
  IntegerHandler baseInteger(static_cast<uint8_t>(base));
  for (size_t i = 0; i < length; i++) {
    EditionReference multiplication = IntegerHandler::Multiplication(Integer::Handler(result), baseInteger);
    result = result.replaceTreeByTree(multiplication);
    IntegerHandler digit = IntegerHandler(OMG::Print::DigitForCharacter(*digits++));
    digit.setSign(sign);
    EditionReference addition = IntegerHandler::Addition(Integer::Handler(result), digit);
    result = result.replaceTreeByTree(addition);
  }
  return result;
}

// TODO: tests

IntegerHandler Integer::Handler(const Node expression) {
  assert(Rational::Denominator(expression).isOne());
  return Rational::Numerator(expression);
}

bool Integer::IsUint8(const Node expression) {
  return expression.block()->isInteger() && Integer::Handler(expression).isUnsignedType<uint8_t>();
}

uint8_t Integer::Uint8(const Node expression) {
  assert(IsUint8(expression));
  return static_cast<uint8_t>(Integer::Handler(expression));
}

}

template float PoincareJ::IntegerHandler::to<float>();
template double PoincareJ::IntegerHandler::to<double>();
