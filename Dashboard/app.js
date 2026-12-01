const $ = id => document.getElementById(id);
function display(v, d = 2) { return Number.isFinite(v) ? v.toFixed(d) : '--'; }
function render(d) {
  $('connection').textContent = d.valid ? 'Live' : 'Sensor unavailable';
  $('voltage').textContent = display(d.voltage_v, 3);
  $('current').textContent = display(d.current_ma, 2);
  $('power').textContent = display(d.power_mw, 1);
  $('energy').textContent = display(d.session_energy_wh, 5);
  $('count').textContent = d.count + ' / ' + d.capacity;
  $('recording').textContent = d.recording ? 'Recording' : 'Idle';
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
async function refresh() {
  try {
    const r = await fetch('/data', { cache: 'no-store' });
    if (r.ok) render(await r.json());
  } catch (e) {
    $('connection').textContent = 'Offline';
  }
}
for (const cmd of ['start', 'stop', 'reset']) {
  $(cmd).addEventListener('click', () => fetch('/' + cmd, { method: 'POST' }).then(refresh));
}
setInterval(refresh, 1000);