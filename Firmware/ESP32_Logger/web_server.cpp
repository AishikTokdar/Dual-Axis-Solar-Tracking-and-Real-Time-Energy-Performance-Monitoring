#include "web_server.h"
#include "data_logger.h"
#include "dashboard_generated.h"
#include <WebServer.h>
#include <WiFi.h>
static WebServer server(80);
static String number(float x, unsigned precision = 3) { return isfinite(x) ? String(x, precision) : String("null"); }
static void data() {
    const PowerSample &s = logger.latest;
    String j = "{"sample_id":" + String(s.id) + ","uptime_ms":" + String(s.uptime_ms);
    j += ","valid":" + String(s.valid ? "true" : "false");
    j += ","voltage_v":" + number(s.voltage_v, 5) + ","current_ma":" + number(s.current_ma, 5);
    j += ","power_mw":" + number(s.power_mw, 5) + ","session_energy_wh":" + String(logger.session_energy.watt_hours, 6);
    j += ","recording":" + String(logger.recording ? "true" : "false") + ","full":" + (logger.full ? "true" : "false");
    j += ","count":" + String(logger.count) + ","capacity":" + String(LOG_CAPACITY) + ","graph":[";
    unsigned start = (logger.graph_head + GRAPH_CAPACITY - logger.graph_count) % GRAPH_CAPACITY;
    for (unsigned i = 0; i < logger.graph_count; ++i) { if (i) j += ","; j += number(logger.graph[(start + i) % GRAPH_CAPACITY]); }
    j += "]}"; server.send(200, "application/json", j);
}
static void csv() {
    if (logger.recording) { server.send(409, "text/plain", "Stop recording before export"); return; }
    server.sendHeader("Content-Disposition", "attachment; filename=solar_session.csv");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN); server.send(200, "text/csv", "");
    server.sendContent("Time_s,Voltage_V,Current_mA,Power_mW,Energy_Wh,Valid
");
    char row[160];
    for (unsigned i = 0; i < logger.count; ++i) {
        const LogEntry &e = logger.entries[i];
        if (e.valid) snprintf(row, sizeof(row), "%.3f,%.5f,%.5f,%.5f,%.7f,1
", e.elapsed_ms / 1000.0, e.voltage_v, e.current_ma, e.power_mw, e.energy_wh);
        else snprintf(row, sizeof(row), "%.3f,,,,%.7f,0
", e.elapsed_ms / 1000.0, e.energy_wh);
        server.sendContent(row);
    }
    server.sendContent("");
}
void Web_Begin() {
    server.on("/", HTTP_GET, []() { server.send_P(200, "text/html", DASHBOARD_HTML); });
    server.on("/data", HTTP_GET, data);
    server.on("/start", HTTP_POST, []() { logger.start(); server.send(200, "application/json", "{"ok":true}"); });
    server.on("/stop", HTTP_POST, []() { logger.stop(); server.send(200, "application/json", "{"ok":true}"); });
    server.on("/reset", HTTP_POST, []() { logger.reset(); server.send(200, "application/json", "{"ok":true}"); });
    server.on("/csv", HTTP_GET, csv);
    server.begin();
}
void Web_Update() { server.handleClient(); }
