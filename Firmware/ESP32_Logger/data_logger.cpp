#include "data_logger.h"
DataLogger logger;
void DataLogger::ingest(PowerSample sample) {
    latest=sample;
    device_energy.update(sample.uptime_ms,sample.power_mw/1000.0f,sample.valid,MAX_SAMPLE_GAP_MS);
    graph[graph_head]=sample.valid ? sample.power_mw : NAN;
    graph_head=(graph_head+1)%GRAPH_CAPACITY;
    if (graph_count<GRAPH_CAPACITY) ++graph_count;
    if (!recording) return;
    if (count==0) session_start=sample.uptime_ms;
    session_energy.update(sample.uptime_ms,sample.power_mw/1000.0f,sample.valid,MAX_SAMPLE_GAP_MS);
    entries[count++]={sample.uptime_ms-session_start,sample.voltage_v,sample.current_ma,sample.power_mw,(float)session_energy.watt_hours,sample.valid};
    if (count>=LOG_CAPACITY) { recording=false; full=true; }
}
bool DataLogger::start() {
    if (!entries || recording || count) return false;
    count=0; full=false; session_start=0; session_energy.reset(); recording=true;
    return true;
}
void DataLogger::reset() {
    stop(); count=graph_head=graph_count=0; full=false; session_start=0;
    session_energy.reset(); device_energy.reset();
}
