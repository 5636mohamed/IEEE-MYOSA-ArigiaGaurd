/**
 * @file data_logger.h
 * @brief Circular buffer telemetry and clinical audit logging for DecubiGuard.
 * @author Mohamed Elhariry
 */

#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include "config.h"

#define MAX_LOG_ENTRIES 128

struct LogEntry {
    uint32_t timestamp_ms;
    uint8_t posture_id;
    float heart_rate;
    float resp_rate;
    float pressure_mmhg;
    float cumulative_pti;
    float humidity_rh;
    float temperature_c;
    uint8_t alert_code;
};

class DataLogger {
public:
    DataLogger();
    bool begin();
    void logEvent(const LogEntry& entry);
    uint16_t getCount() const;
    bool getEntry(uint16_t index, LogEntry& out) const;
    void dumpToSerial();

private:
    LogEntry logs[MAX_LOG_ENTRIES];
    uint16_t writeIndex;
    uint16_t totalLogged;
};

#endif // DATA_LOGGER_H
