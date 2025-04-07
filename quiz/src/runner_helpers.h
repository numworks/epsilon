void flushGlobalDataNoPool();
void flushGlobalData();
void exception_run(void (*inner_main)(const char*, const char*, const char*,
                                      int chunkId, int numberOfChunks),
                   const char* testFilter, const char* fromFilter,
                   const char* untilFilter, int chunkId, int numberOfChunks);
