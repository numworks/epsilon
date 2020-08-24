#include<ion.h>

namespace Ion {

const size_t ChunkBytes = 64;

const size_t HashVariables = 8;
// First 32 bits of the fractional parts of the square roots of the first 8 primes 2..19
const uint32_t k_h[HashVariables] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };

// First 32 bits of the fractional parts of the cube roots of the first 64 primes 2..311
const uint32_t k_k[64] = {
   0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
   0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
   0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
   0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
   0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
   0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };

template <class T>
void bigEndianMemcpy(uint8_t * target, T object) {
  // Perform a memcpy of an object at an address, using big-endian Endianness.
  size_t num = sizeof(T);
  for (size_t i = 1; i <= num; ++i) {
    // Bytes are copied right to left, from least to most significant byte
    target[num-i] = static_cast<uint8_t>(object & 0xFF);
    object = (object >> 8);
  }
}

void computeDigest(uint8_t * digest, uint32_t * h) {
  // Compute digest from hash value
  for (size_t i = 0; i < HashVariables; ++i) {
    // Copying hash value variable into digest
    bigEndianMemcpy<uint32_t>(digest + i * sizeof(uint32_t), h[i]);
  }
}

uint32_t rotate(uint32_t a, size_t c) {
  // Circularly rotate to the right a's bits by c
  return (a >> c) | (a << (32 - c));
}

void processChunk(uint8_t * chunk, uint32_t * h) {
  // Update Hash Values using chunk
  // Create a 64-entry message schedule array of 32-bit words
  const size_t messageScheduleArrayWords = 64;
  uint32_t w[messageScheduleArrayWords];
  /* Note : Spacial complexity could be improved by only keeping last 16 words
   * from message schedule array, and computing it in compression function loop.
  */
  // Cast chunk as an array of 16 uint32_t (64 * 8 bits to 16 * 32 bits)
  uint32_t * chunk32 = reinterpret_cast<uint32_t *>(chunk);
  // Copy chunk into first 16 words of the message schedule array (Big-endian)
  for (int i = 0; i < 16; ++i) {
    bigEndianMemcpy<uint32_t>(reinterpret_cast<uint8_t *>(w + i), chunk32[i]);
  }
  // Extend the first 16 words into the remaining 48 words of the message schedule array
  for (int i = 16; i < messageScheduleArrayWords; ++i) {
      uint32_t s0 = rotate(w[i-15], 7) ^ rotate(w[i-15], 18) ^ (w[i-15] >>  3);
      uint32_t s1 = rotate(w[i-2], 17) ^ rotate(w[i-2], 19) ^ (w[i-2] >> 10);
      w[i] = w[i-16] + s0 + w[i-7] + s1;
  }

  // Initialize working variables to current hash value
  uint32_t v[HashVariables];
  memcpy(v, h, HashVariables * sizeof(uint32_t));

  // Compression function main loop
  for (int i = 0; i < messageScheduleArrayWords; ++i) {
    uint32_t S1 = rotate(v[4], 6) ^ rotate(v[4], 11) ^ rotate(v[4], 25);
    uint32_t ch = (v[4] & v[5]) ^ (~v[4] & v[6]);
    uint32_t temp1 = v[7] + S1 + ch + k_k[i] + w[i];
    uint32_t S0 = rotate(v[0], 2) ^ rotate(v[0], 13) ^ rotate(v[0], 22);
    uint32_t maj = (v[0] & v[1]) ^ (v[0] & v[2]) ^ (v[1] & v[2]);
    uint32_t temp2 = S0 + maj;
    v[7] = v[6];
    v[6] = v[5];
    v[5] = v[4];
    v[4] = v[3] + temp1;
    v[3] = v[2];
    v[2] = v[1];
    v[1] = v[0];
    v[0] = temp1 + temp2;
  }

  // Add the compressed chunk to the current hash value
  for (int i = 0; i < HashVariables; ++i) {
    h[i] += v[i];
  }
}

void loadChunk(const uint8_t * data, uint32_t dataLength, uint8_t * chunk, int chunkIndex) {
  // Load data into chunk
  size_t dataOffset = chunkIndex * ChunkBytes;
  if ( dataLength >= (chunkIndex + 1) * ChunkBytes ) {
    // Chunk only contains data
    memcpy(chunk, data + dataOffset, ChunkBytes);
    return;
  }

  int lastDataChunkLength = 0;

  if ( dataLength >= dataOffset) {
    // There is data left to copy. It is strictly shorter than chunk size
    lastDataChunkLength = dataLength - dataOffset;
    // Copying data
    memcpy(chunk, data + dataOffset, lastDataChunkLength);
    // Setting a bit to 1 right after data
    memset(chunk + lastDataChunkLength, 0x80, 1);
    // Setting following chunk's bits to 0
    memset(chunk + lastDataChunkLength + 1, 0, ChunkBytes - lastDataChunkLength - 1);
  } else {
    // Data and bit 1 are in previous chunk. Setting chunk's bits to 0
    memset(chunk, 0, ChunkBytes);
  }

  if (ChunkBytes - lastDataChunkLength > sizeof(uint64_t)) {
    // There is enough space left for dataLength
    uint64_t bitLength = static_cast<uint64_t>(dataLength) * 8;
    // Copying bitLength as bigEndian uint64 at the end of last chunk
    bigEndianMemcpy<uint64_t>(chunk + ChunkBytes - sizeof(uint64_t), bitLength);
  }
}

int totalchunks(uint32_t dataLength) {
  /* Data is appended with a single bit at 1, followed by bits at 0.
   * DataLength is appended at the end of the last chunk.
   * | - - - - - - - - - - - - - - - -| ... | - - - - - - - - - - - - - - - -|
   * |                         data                 | 1 | 0...0 | dataLength |
   * Examples :
   * dataLength+8 | Chunks | Disposition
   *     55 + 8   |   1    |  |data,1,0...0,dataLength|
   *     56 + 8   |   2    |  |data[0-55],1,0...0| + |0...0,dataLength|
   *     72 + 8   |   2    |  |data[0-63]| + |data[64-71],1,0...0,dataLength| */
  return ((dataLength + sizeof(uint64_t)) / ChunkBytes) + 1;
}

void sha256(const uint8_t * data, uint32_t dataLength, uint8_t * digest) {
  // Initialize hash value variables
  uint32_t h[HashVariables];
  for (int i = 0; i < HashVariables; ++i) {
    h[i] = k_h[i];
  }
  // Create chunk
  uint8_t chunk[ChunkBytes];
  // Process chunks
  for (int i = 0; i < totalchunks(dataLength); ++i) {
    loadChunk(data, dataLength, chunk, i);
    processChunk(chunk, h);
  }
  computeDigest(digest, h);
}

}