#ifndef ESCHER_TELEMETRY_H
#define ESCHER_TELEMETRY_H

#if EPSILON_TELEMETRY
#define TELEMETRY_ID(x) \
  const char* telemetryId() const override { return (x); }
#else
#define TELEMETRY_ID(x)
#endif

#endif
