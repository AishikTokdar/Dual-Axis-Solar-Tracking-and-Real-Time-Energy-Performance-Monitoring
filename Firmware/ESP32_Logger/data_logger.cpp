#include "data_logger.h"
DataLogger logger;
static void add_energy(SimpleEnergy &e, uint32_t now, float w, bool valid) {
    if (e.has_prev && now > e.prev_ms && valid) {
        uint32_t dt = now - e.prev_ms;
        e.watt_hours += (double)(e.prev_w + w) * 0.5 * (double)dt / 3600000.0;
        e.covered_ms += dt;
    }
    e.prev_ms = now; e.prev_w = valid ? w : 0; e.has_prev = true;
}
void DataLogger::ingest(PowerSample sample) {
    latest = sample;
    add_energy(device_energy, sample.uptime_ms, sample.power_mw / 1000.0f, sample.valid);
    graph[graph_head] = sample.valid ? sample.power_mw : NAN;
    graph_head = (graph_head + 1) % GRAPH_CAPACITY;
    if (graph_count < GRAPH_CAPACITY) ++graph_count;
    if (!recording) return;
    if (count == 0) session_start = sample.uptime_ms;
    add_energy(session_energy, sample.uptime_ms, sample.power_mw / 1000.0f, sample.valid);
    entries[count++] = {sample.uptime_ms - session_start, sample.voltage_v, sample.current_ma, sample.power_mw, (float)session_energy.watt_hours, sample.valid};
    if (count >= LOG_CAPACITY) { recording = false; full = true; }
}
bool DataLogger::start() {
    if (!entries || recording || count) return false;
    count = 0; full = false; session_start = 0; session_energy = SimpleEnergy{}; recording = true; return true;
}
void DataLogger::reset() {
    stop(); count = graph_head = graph_count = 0; full = false; session_start = 0;
    session_energy = SimpleEnergy{}; device_energy = SimpleEnergy{};
}
