/**
 * @file data_logger.cpp
 * @brief Implementations of circular buffer data logger for clinical audit compliance.
 * @author Abdelrahman Hamza, Abdelrahman Wael, Mohamed Elhariry
 */

#include "data_logger.h"
#include <Arduino.h>

DataLogger::DataLogger() : writeIndex(0), totalLogged(0) {}

bool DataLogger::begin() {
    writeIndex = 0;
    totalLogged = 0;
    return true;
}

void DataLogger::logEvent(const LogEntry& entry) {
    logs[writeIndex] = entry;
    writeIndex = (writeIndex + 1) % MAX_LOG_ENTRIES;
    if (totalLogged < MAX_LOG_ENTRIES) {
        totalLogged++;
    }
}

uint16_t DataLogger::getCount() const {
    return totalLogged;
}

bool DataLogger::getEntry(uint16_t index, LogEntry& out) const {
    if (index >= totalLogged) return false;
    out = logs[index];
    return true;
}

void DataLogger::dumpToSerial() {
    Serial.println(F("\n--- [AUDIT LOG DUMP] ---"));
    Serial.println(F("Time_ms,Posture,HR,RR,Pressure,PTI,Humidity,Temp,Alert"));
    for (uint16_t i = 0; i < totalLogged; i++) {
        const LogEntry& e = logs[i];
        Serial.print(e.timestamp_ms); Serial.print(',');
        Serial.print(e.posture_id); Serial.print(',');
        Serial.print(e.heart_rate, 1); Serial.print(',');
        Serial.print(e.resp_rate, 1); Serial.print(',');
        Serial.print(e.pressure_mmhg, 1); Serial.print(',');
        Serial.print(e.cumulative_pti, 1); Serial.print(',');
        Serial.print(e.humidity_rh, 1); Serial.print(',');
        Serial.print(e.temperature_c, 1); Serial.print(',');
        Serial.println(e.alert_code);
    }
    Serial.println(F("--- [END LOG DUMP] ---\n"));
}
