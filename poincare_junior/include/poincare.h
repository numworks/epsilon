#ifndef POINCAREJ_POINCARE_H
#define POINCAREJ_POINCARE_H

#if POINCARE_POOL_VISUALIZATION
#include <fstream>
#endif
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace PoincareJ {

void Init();
void Shutdown();

#if POINCARE_POOL_VISUALIZATION

enum class LoggerType { Cache = 0, Edition = 1, NumberOfLoggers };

std::ofstream& Logger(LoggerType type);
__attribute__((__used__)) void ResetLogger(LoggerType type);
__attribute__((__used__)) void CloseLogger(LoggerType type);
void Log(LoggerType type, const char* event, const void* blockAddress = nullptr,
         size_t blockSize = INT_MAX, const void* pointerAddress = nullptr);

#endif

#if POINCARE_TREE_LOG
void Indent(std::ostream& stream, int indentation);
#endif

}  // namespace PoincareJ

#endif
