#pragma once
#include <string.h>
#include <ctype.h>

struct DashboardRequest { char *path=nullptr; bool post=false; };
inline bool HeaderEquals(const char *a,const char *b) {
    while (*a && *b && tolower(static_cast<unsigned char>(*a))==tolower(static_cast<unsigned char>(*b))) { ++a; ++b; }
    return *a==*b;
}
// Parse only the bodyless API used here. Never delegate untrusted form data to
// the older framework parser. The caller supplies a bounded, terminated buffer.
inline int ParseDashboardRequest(char *request,const char *address,DashboardRequest &out) {
    char *end=strstr(request,"\r\n\r\n");
    if (!end || end[4]) return 400;
    for (const char *c=request; *c; ++c) {
        unsigned char value=static_cast<unsigned char>(*c);
        if ((value<32 && value!='\r' && value!='\n' && value!='\t') || value>=127) return 400;
    }
    char *line_end=strstr(request,"\r\n");
    *line_end=0;
    char *path=strchr(request,' ');
    if (!path) return 400;
    *path++=0;
    char *version=strchr(path,' ');
    if (!version || path[0]!='/') return 400;
    *version++=0;
    if (strcmp(version,"HTTP/1.1") && strcmp(version,"HTTP/1.0")) return 400;
    if (!strcmp(request,"POST")) out.post=true;
    else if (!strcmp(request,"GET")) out.post=false;
    else return 405;
    const char *host=nullptr,*action=nullptr;
    for (char *line=line_end+2; strncmp(line,"\r\n",2); line=line_end+2) {
        line_end=strstr(line,"\r\n");
        if (!line_end) return 400;
        *line_end=0;
        char *value=strchr(line,':');
        if (!value) return 400;
        *value++=0;
        while (*value==' ' || *value=='\t') ++value;
        char *tail=line_end;
        while (tail>value && (tail[-1]==' ' || tail[-1]=='\t')) *--tail=0;
        if (HeaderEquals(line,"Host")) {
            if (host) return 400;
            host=value;
        } else if (HeaderEquals(line,"X-Solar-Action")) {
            if (action) return 400;
            action=value;
        } else if (HeaderEquals(line,"Transfer-Encoding") || HeaderEquals(line,"Content-Type") ||
                   (HeaderEquals(line,"Content-Length") && strcmp(value,"0"))) return 400;
    }
    // Permit the advertised device IP only, preventing DNS rebinding.
    size_t length=strlen(address);
    if (!host || strncmp(host,address,length) || (host[length] && strcmp(host+length,":80"))) return 403;
    // A custom header requires browser CORS preflight, which this server denies.
    if (out.post && (!action || strcmp(action,"1"))) return 403;
    char *query=strchr(path,'?'); if (query) *query=0;
    out.path=path;
    return 200;
}
void Web_Begin();
void Web_Update();
