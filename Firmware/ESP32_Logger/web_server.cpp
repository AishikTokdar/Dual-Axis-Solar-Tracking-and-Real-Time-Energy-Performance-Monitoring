#include "web_server.h"
#include "data_logger.h"
#include "dashboard_generated.h"
#include <WebServer.h>
#include <WiFi.h>
#include <esp_system.h>
// This API has no request bodies. Keep untrusted bodies out of the framework's
// multipart parser, and bound header memory and waiting without blocking sampling.
class DashboardServer : public WebServer {
    char request[2048];
    size_t used=0;
    size_t _currentClientWrite(const char *buffer,size_t length) override {
        size_t written=WebServer::_currentClientWrite(buffer,length);
        if (written!=length) _currentClient.stop();
        return written;
    }
    size_t _currentClientWrite_P(PGM_P buffer,size_t length) override {
        size_t written=WebServer::_currentClientWrite_P(buffer,length);
        if (written!=length) _currentClient.stop();
        return written;
    }
    void finish() {
        _currentClient.stop(); _currentClient=WiFiClient();
        _currentStatus=HC_NONE; used=0;
    }
    void dispatch() {
        _currentVersion=1; _chunked=false; _contentLength=CONTENT_LENGTH_NOT_SET;
        _responseHeaders="";
        sendHeader("Cache-Control","no-store");
        sendHeader("X-Content-Type-Options","nosniff");
        sendHeader("X-Frame-Options","DENY");
        DashboardRequest parsed;
        int status=ParseDashboardRequest(request,_currentClient.localIP().toString().c_str(),parsed);
        if (status!=200) { send(status,"text/plain","Request rejected"); return; }
        _currentMethod=parsed.post ? HTTP_POST : HTTP_GET;
        const char *path=parsed.path;
        _currentUri=path; _currentHandler=nullptr;
        for (RequestHandler *handler=_firstHandler; handler; handler=handler->next()) {
            if (handler->canHandle(_currentMethod,_currentUri)) { _currentHandler=handler; break; }
        }
        _handleRequest();
    }
public:
    DashboardServer() : WebServer(80) {}
    void handleClient() override {
        if (_currentStatus==HC_NONE) {
            _currentClient=_server.available();
            if (!_currentClient) return;
            _currentClient.setTimeout(1); // WiFiClient takes seconds, not milliseconds.
            _currentStatus=HC_WAIT_READ; _statusChange=millis(); used=0;
        }
        if (!_currentClient.connected() || millis()-_statusChange>1000) { finish(); return; }
        for (unsigned budget=0; budget<256 && _currentClient.available(); ++budget) {
            int c=_currentClient.read();
            if (c<=0 || used==sizeof(request)-1) { finish(); return; }
            request[used++]=static_cast<char>(c); request[used]=0;
            if (used>=4 && !memcmp(request+used-4,"\r\n\r\n",4)) {
                dispatch(); finish(); return;
            }
        }
    }
};
static DashboardServer server;
static uint32_t boot_id=0;
static String number(float x,unsigned precision=3) { return isfinite(x)?String(x,precision):String("null"); }
static void data() {
    const PowerSample &s=logger.latest;
    String j; j.reserve(1400);
    j="{\"boot_id\":"+String(boot_id)+",\"sample_id\":"+String(s.id)+",\"uptime_ms\":"+String(s.uptime_ms);
    j+=",\"sample_age_ms\":"+String(millis()-s.uptime_ms)+",\"valid\":"+(s.valid?"true":"false");
    j+=",\"voltage_v\":"+number(s.voltage_v,5)+",\"current_ma\":"+number(s.current_ma,5);
    j+=",\"power_mw\":"+number(s.power_mw,5)+",\"device_energy_wh\":"+String(logger.device_energy.watt_hours,6);
    j+=",\"session_energy_wh\":"+String(logger.session_energy.watt_hours,6);
    j+=",\"utilization_pct\":"+number(s.power_mw/(PANEL_RATED_W*10.0f));
    j+=",\"recording\":"+String(logger.recording?"true":"false")+",\"full\":"+(logger.full?"true":"false");
    j+=",\"count\":"+String(logger.count)+",\"capacity\":"+String(logger.entries ? LOG_CAPACITY : 0);
    uint32_t elapsed=logger.count ? logger.entries[logger.count-1].elapsed_ms : 0;
    j+=",\"elapsed_ms\":"+String(elapsed)+",\"covered_ms\":"+String(logger.session_energy.covered_ms);
    j+=",\"gap_count\":"+String(logger.session_energy.gaps)+",\"graph\":[";
    unsigned start=(logger.graph_head+GRAPH_CAPACITY-logger.graph_count)%GRAPH_CAPACITY;
    for (unsigned i=0;i<logger.graph_count;++i) { if(i) j+=","; j+=number(logger.graph[(start+i)%GRAPH_CAPACITY]); }
    j+="]}"; server.send(200,"application/json",j);
}
static void csv() {
    if(logger.recording) { server.send(409,"text/plain","Stop recording before export"); return; }
    server.sendHeader("Content-Disposition","attachment; filename=solar_session.csv");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN); server.send(200,"text/csv","");
    server.sendContent("Time_s,Voltage_V,Current_mA,Power_mW,Energy_Wh,Valid\n");
    char row[160];
    for (unsigned i=0;i<logger.count;++i) {
        if (!server.client().connected()) break;
        const LogEntry &e=logger.entries[i];
        if(e.valid) snprintf(row,sizeof(row),"%.3f,%.5f,%.5f,%.5f,%.7f,1\n",e.elapsed_ms/1000.0,e.voltage_v,e.current_ma,e.power_mw,e.energy_wh);
        else snprintf(row,sizeof(row),"%.3f,,,,%.7f,0\n",e.elapsed_ms/1000.0,e.energy_wh);
        server.sendContent(row);
    }
    server.sendContent("");
}
void Web_Begin() {
    boot_id=esp_random();
    server.on("/",HTTP_GET,[](){server.send_P(200,"text/html",DASHBOARD_HTML);});
    server.on("/data",HTTP_GET,data);
    server.on("/start",HTTP_POST,[](){
        if(logger.recording) {server.send(409,"text/plain","Already recording");return;}
        if(logger.count) {server.send(409,"text/plain","Export and reset the previous session first");return;}
        if (!logger.start()) {server.send(503,"text/plain","Recording memory unavailable");return;}
        server.send(200,"application/json","{\"ok\":true}");
    });
    server.on("/stop",HTTP_POST,[](){logger.stop();server.send(200,"application/json","{\"ok\":true}");});
    server.on("/reset",HTTP_POST,[](){logger.reset();server.send(200,"application/json","{\"ok\":true}");});
    server.on("/csv",HTTP_GET,csv);
    server.onNotFound([](){server.send(404,"text/plain","Not found");});
    server.begin();
}
void Web_Update() { server.handleClient(); }
