const $ = id => document.getElementById(id);
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
refresh(); setInterval(refresh, 1000);
window.addEventListener('resize', () => { if (lastData) render(lastData); });