#pragma once
#include <pgmspace.h>
const char DASHBOARD_HTML[] PROGMEM = R"SOLARHTML(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Dual-Axis Solar Tracking and Real-Time Energy Performance Monitoring</title>
<style>:root{--page:#f4f6f7;--surface:#fff;--ink:#17222b;--muted:#52616d;--line:#d8dfe4;--green:#176747;--blue:#245e8c}
*{box-sizing:border-box}body{margin:0;background:var(--page);color:var(--ink);font:16px/1.5 "Segoe UI",Arial,sans-serif}main{max-width:1380px;margin:auto;padding:28px}
.page-header{display:flex;justify-content:space-between;border-bottom:1px solid var(--line);padding-bottom:20px;margin-bottom:20px}
.readouts{display:grid;grid-template-columns:repeat(4,1fr);background:var(--surface);border:1px solid var(--line);margin-bottom:20px}
.readouts article{padding:20px;border-left:1px solid var(--line)}.readouts article:first-child{border-left:0}
.workspace{display:grid;grid-template-columns:2fr 1fr;gap:20px}.chart-panel,.session-panel{background:var(--surface);border:1px solid var(--line);padding:20px}
canvas{display:block;width:100%;height:260px}button,.button{padding:10px 14px;background:var(--green);color:#fff;border:1px solid var(--green);text-decoration:none;cursor:pointer}
.secondary{background:#fff;color:var(--ink);border-color:#aebac3}.secondary-actions{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-top:10px}</style>
</head>
<body>
<main>
<header class="page-header">
<div><p class="instrument-label">Solar tracking / electrical measurements</p><h1>Dual-Axis Solar Tracking and Real-Time Energy Performance Monitoring</h1></div>
<div class="connection-block"><p class="field-label">Acquisition status</p><strong id="connection" data-state="waiting">Connecting</strong><span id="response-time">No response yet</span></div>
</header>
<section class="readouts">
<article><h2>Panel voltage</h2><div class="numeric"><strong id="voltage">--</strong><span>V</span></div><p>INA226 bus measurement</p></article>
<article><h2>Panel current</h2><div class="numeric"><strong id="current">--</strong><span>mA</span></div><p>Current through the shunt</p></article>
<article class="power-readout"><h2>Output power</h2><div class="numeric"><strong id="power">--</strong><span>mW</span></div><p>Voltage multiplied by current</p></article>
<article class="energy-readout"><h2>Session energy</h2><div class="numeric"><strong id="energy">--</strong><span>Wh</span></div><p>Integrated during recording</p></article>
</section>
<div class="workspace">
<section class="chart-panel"><div class="panel-heading"><h2>Recent power samples</h2><span id="history-count">No samples</span></div><canvas id="chart"></canvas></section>
<section class="session-panel"><div class="panel-heading"><h2>Recording session</h2><strong id="recording">Idle</strong></div>
<dl class="session-values"><div><dt>Elapsed time</dt><dd id="elapsed">--</dd></div><div><dt>Recorded samples</dt><dd id="count">--</dd></div></dl>
<div class="recording-actions"><button id="start" type="button">Start recording</button><div class="secondary-actions"><button id="stop" type="button" class="secondary">Stop</button><a id="download" class="button secondary" href="/csv" download="solar_session.csv">Export CSV</a></div><button id="reset" type="button" class="reset">Reset measurements</button></div>
</section>
</div>
<footer><p id="status">Waiting for device.</p></footer>
</main><script>const $ = id => document.getElementById(id);
let pendingRefresh = null, actionBusy = false, online = false, lastData = null;
function display(v, d = 2) { return Number.isFinite(v) ? v.toFixed(d) : '--'; }
function render(d) {
  lastData = d; online = true;
  $('connection').textContent = d.valid ? 'Live' : 'Sensor unavailable';
  $('voltage').textContent = display(d.voltage_v, 3);
  $('current').textContent = display(d.current_ma, 2);
  $('power').textContent = display(d.power_mw, 1);
  $('energy').textContent = display(d.session_energy_wh, 5);
  $('count').textContent = d.count + ' / ' + d.capacity;
  $('recording').textContent = d.full ? 'Buffer full' : (d.recording ? 'Recording' : 'Idle');
  const c = $('chart'), ctx = c.getContext('2d');
  c.width = c.clientWidth; c.height = c.clientHeight;
  ctx.clearRect(0, 0, c.width, c.height);
  const vals = Array.isArray(d.graph) ? d.graph : [];
  $('history-count').textContent = vals.length + ' samples';
  const max = Math.max(100, ...vals.filter(Number.isFinite));
  ctx.strokeStyle = '#176747'; ctx.beginPath();
  vals.forEach((v, i) => {
    if (!Number.isFinite(v)) return;
    const x = vals.length > 1 ? i * c.width / (vals.length - 1) : c.width;
    const y = c.height * (1 - v / max);
    if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
  });
  ctx.stroke();
}
async function loadSnapshot() {
  try {
    const r = await fetch('/data', { cache: 'no-store', signal: AbortSignal.timeout(5000) });
    if (!r.ok) throw Error('Request failed');
    render(await r.json());
  } catch (e) { online = false; $('connection').textContent = 'Offline'; }
}
function refresh() {
  if (!pendingRefresh) pendingRefresh = loadSnapshot().finally(() => { pendingRefresh = null; });
  return pendingRefresh;
}
for (const cmd of ['start', 'stop', 'reset']) {
  $(cmd).addEventListener('click', () => {
    if (!actionBusy && online) {
      actionBusy = true;
      fetch('/' + cmd, { method: 'POST', signal: AbortSignal.timeout(5000) }).then(refresh).finally(() => { actionBusy = false; });
    }
  });
}
refresh(); setInterval(refresh, 1000);</script>
</body></html>)SOLARHTML";
