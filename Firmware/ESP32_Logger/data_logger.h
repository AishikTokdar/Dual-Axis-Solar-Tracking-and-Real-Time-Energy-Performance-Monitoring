#pragma once
#include "config.h"
#include <memory>
#include <new>
#include "energy_integrator.h"
struct PowerSample {
    uint32_t id=0, uptime_ms=0;
    float voltage_v=NAN, current_ma=NAN, power_mw=NAN;
    bool valid=false;
};
struct LogEntry { uint32_t elapsed_ms; float voltage_v,current_ma,power_mw,energy_wh; bool valid; };
class DataLogger {
public:
    PowerSample latest;
    EnergyIntegrator device_energy, session_energy;
    std::unique_ptr<LogEntry[]> entries{new (std::nothrow) LogEntry[LOG_CAPACITY]};
    float graph[GRAPH_CAPACITY];
    unsigned count=0, graph_head=0, graph_count=0;
    bool recording=false, full=false;
    uint32_t session_start=0;
    void ingest(PowerSample sample);
    bool start();
    void stop() { recording=false; }
    void reset();
};
extern DataLogger logger;
