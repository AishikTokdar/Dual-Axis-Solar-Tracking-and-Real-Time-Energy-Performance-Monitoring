#pragma once
#include <pgmspace.h>
const char DASHBOARD_HTML[] PROGMEM = R"SOLARHTML(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Dual-Axis Solar Tracking and Real-Time Energy Performance Monitoring</title>
<style>:root{color-scheme:light;--page:#f4f6f7;--surface:#fff;--ink:#17222b;--muted:#52616d;--line:#d8dfe4;--green:#176747;--blue:#245e8c;--focus:#215f9e}
*{box-sizing:border-box}body{margin:0;background:var(--page);color:var(--ink);font:16px/1.5 "Segoe UI",Arial,sans-serif}main{max-width:1380px;margin:auto;padding:30px 36px 24px}h1,h2,p{margin:0}.page-header{display:grid;grid-template-columns:minmax(0,1fr) 210px;gap:48px;align-items:start;padding:0 0 26px;border-bottom:1px solid var(--line);margin-bottom:24px}.instrument-label{font-size:14px;color:var(--muted);margin-bottom:10px}h1{font-size:25px;line-height:1.3;font-weight:600;max-width:850px;letter-spacing:-.3px}.connection-block{border-left:1px solid var(--line);padding-left:24px;display:grid;gap:3px}.field-label,.connection-block>span{font-size:14px;color:var(--muted)}.connection-block strong{font-size:18px;font-weight:600}.connection-block strong[data-state=live],#recording[data-state=recording]{color:var(--green)}.connection-block strong[data-state=offline],.connection-block strong[data-state=invalid],.connection-block strong[data-state=stale]{color:#8a4229}.readouts{display:grid;grid-template-columns:repeat(4,minmax(0,1fr));background:var(--surface);border:1px solid var(--line);margin-bottom:24px}.readouts article{padding:22px 24px;min-width:0}.readouts article+article{border-left:1px solid var(--line)}.readouts h2{font-size:14px;font-weight:500;color:var(--muted)}.numeric{display:flex;align-items:baseline;gap:9px;margin:13px 0 8px;white-space:nowrap}.numeric strong{font-family:"Cascadia Mono",Consolas,monospace;font-size:clamp(28px,3vw,42px);line-height:1.3;font-weight:500;letter-spacing:-1.5px;font-variant-numeric:tabular-nums}.numeric span{font-size:16px;color:var(--muted)}.readouts p{color:var(--muted);font-size:14px}.power-readout .numeric strong{color:var(--green)}.energy-readout .numeric strong{color:var(--blue)}.workspace{display:grid;grid-template-columns:minmax(0,2.15fr) minmax(300px,1fr);gap:24px}.chart-panel,.session-panel,.measurement-details{background:var(--surface);border:1px solid var(--line);padding:24px}.panel-heading{display:flex;justify-content:space-between;align-items:baseline;gap:16px;margin-bottom:20px}h2{font-size:18px;font-weight:600}.panel-heading>span,.panel-heading>strong{font-size:14px}.panel-heading>span{color:var(--muted)}canvas{display:block;width:100%;height:285px}.chart-summary{font-size:14px;margin-top:12px}.note{font-size:14px;line-height:1.55;color:var(--muted);margin-top:12px}dl{margin:0}dt{font-size:14px;color:var(--muted)}dd{font-size:16px;font-weight:500;margin:0;font-variant-numeric:tabular-nums}.session-values div{display:flex;justify-content:space-between;gap:16px;padding:10px 0}.capacity-label{display:flex;justify-content:space-between;gap:16px;font-size:14px;color:var(--muted);margin-top:14px}progress{display:block;appearance:none;width:100%;height:7px;margin:9px 0 24px;border:0;border-radius:0;background:#e5e9ec;color:var(--green)}progress::-webkit-progress-bar{background:#e5e9ec}progress::-webkit-progress-value{background:var(--green)}progress::-moz-progress-bar{background:var(--green)}button,.button{display:inline-flex;justify-content:center;align-items:center;min-height:43px;padding:10px 16px;border:1px solid var(--green);border-radius:3px;background:var(--green);color:white;font:600 14px/1.4 "Segoe UI",Arial,sans-serif;cursor:pointer;text-align:center;text-decoration:none}button{width:100%}.secondary{background:#fff;color:var(--ink);border-color:#aebac3}.secondary-actions{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-top:10px}.reset{background:transparent;color:var(--muted);border-color:transparent;font-weight:400;min-height:38px;margin-top:5px}button:disabled,.disabled{background:#f3f5f6;color:#78858e;border-color:#d8dfe4;cursor:default}.reset:disabled{border-color:transparent;background:transparent}button:hover:not(:disabled),.button:hover:not(.disabled){filter:brightness(.95)}button:focus-visible,a:focus-visible{outline:3px solid var(--focus);outline-offset:3px}.measurement-details{margin-top:24px}.measurement-details h2{margin-bottom:20px}.measurement-details dl{display:grid;grid-template-columns:repeat(5,minmax(0,1fr));gap:24px}.measurement-details dt{margin-bottom:6px}.measurement-details .note{margin-top:22px;max-width:1080px}footer{display:flex;justify-content:space-between;align-items:baseline;gap:24px;padding-top:22px;font-size:14px;color:var(--muted)}footer p{max-width:900px}a{color:var(--blue)}footer a{white-space:nowrap}#status[data-error=true]{color:#8a4229}
@media(max-width:1050px){main{padding:24px}.readouts article{padding:20px 18px}.numeric{gap:6px}.numeric strong{font-size:31px}.workspace{grid-template-columns:minmax(0,1.7fr) minmax(280px,1fr);gap:18px}.chart-panel,.session-panel,.measurement-details{padding:20px}.measurement-details dl{grid-template-columns:repeat(3,minmax(0,1fr));gap:18px 24px}.page-header{gap:24px}}
@media(max-width:760px){main{padding:22px 16px}.page-header{grid-template-columns:1fr;gap:20px;padding-bottom:22px}.connection-block{border-left:0;padding-left:0;grid-template-columns:1fr 1fr;column-gap:16px}.connection-block .field-label{grid-row:1}.connection-block strong{grid-row:1;text-align:right;font-size:16px}.connection-block>span{grid-column:1 / -1}.readouts{grid-template-columns:repeat(2,minmax(0,1fr))}.readouts article:nth-child(3){border-left:0}.readouts article:nth-child(n+3){border-top:1px solid var(--line)}.readouts p{font-size:14px;line-height:1.4}.numeric strong{font-size:32px}.workspace{grid-template-columns:1fr}.measurement-details dl{grid-template-columns:repeat(2,minmax(0,1fr))}.chart-panel,.session-panel,.measurement-details{padding:18px}h1{font-size:23px}.panel-heading{margin-bottom:14px}canvas{height:250px}footer{display:block}footer a{display:inline-block;margin-top:12px}}
@media(max-width:380px){.readouts article{padding:16px 12px}.numeric strong{font-size:27px}.numeric span{font-size:14px}.panel-heading{gap:8px}.panel-heading>span{font-size:13px}}
</style>
</head>
<body>
<main>
<header class="page-header">
<div><p class="instrument-label">Solar tracking / electrical measurements</p><h1>Dual-Axis Solar Tracking and Real-Time Energy Performance Monitoring</h1></div>
<div class="connection-block"><p class="field-label">Acquisition status</p><strong id="connection" role="status" aria-live="polite" data-state="waiting">Connecting</strong><span id="response-time">No response yet</span></div>
</header>
<section class="readouts" aria-label="Electrical measurements">
<article><h2>Panel voltage</h2><div class="numeric"><strong id="voltage">--</strong><span>V</span></div><p>INA226 bus measurement</p></article>
<article><h2>Panel current</h2><div class="numeric"><strong id="current">--</strong><span>mA</span></div><p>Current through the shunt</p></article>
<article class="power-readout"><h2>Output power</h2><div class="numeric"><strong id="power">--</strong><span>mW</span></div><p>Voltage multiplied by current</p></article>
<article class="energy-readout"><h2>Session energy</h2><div class="numeric"><strong id="energy">--</strong><span>Wh</span></div><p>Integrated during recording</p></article>
</section>
<div class="workspace">
<section class="chart-panel" aria-labelledby="chart-title"><div class="panel-heading"><h2 id="chart-title">Recent power samples</h2><span id="history-count">No samples</span></div><canvas id="chart" role="img" aria-label="Power sample history" aria-describedby="chart-summary"></canvas><p id="chart-summary" class="chart-summary">Waiting for the first valid power sample.</p><p class="note">Sample order is shown at equal spacing. Missing readings break the trace; spacing does not represent elapsed time.</p></section>
<section class="session-panel" aria-labelledby="session-title"><div class="panel-heading"><h2 id="session-title">Recording session</h2><strong id="recording" data-state="idle">Idle</strong></div>
<dl class="session-values"><div><dt>Elapsed time</dt><dd id="elapsed">--</dd></div><div><dt>Recorded samples</dt><dd id="count">--</dd></div></dl>
<label class="capacity-label" for="capacity"><span>Buffer capacity</span><span id="capacity-percent">--</span></label><progress id="capacity" value="0" max="3600">0%</progress>
<div class="recording-actions"><button id="start" type="button" disabled>Start recording</button><div class="secondary-actions"><button id="stop" type="button" class="secondary" disabled>Stop</button><a id="download" class="button secondary disabled" href="/csv" download="solar_session.csv" aria-disabled="true" tabindex="-1">Export CSV</a></div><button id="reset" type="button" class="reset" disabled>Reset measurements</button></div>
<p class="note" id="session-note">Recording is stored in RAM. Export before resetting or powering off.</p></section>
</div>
<section class="measurement-details" aria-labelledby="details-title"><h2 id="details-title">Measurement details</h2><dl><div><dt>Sample age</dt><dd id="sample-age">--</dd></div><div><dt>Sample ID</dt><dd id="sample-id">--</dd></div><div><dt>Integrated time</dt><dd id="covered-time">--</dd></div><div><dt>Excluded intervals</dt><dd id="gaps">--</dd></div><div><dt>Rated-power utilization</dt><dd id="utilization">--</dd></div></dl><p class="note">Integrated time includes valid adjacent samples no more than 2.5 seconds apart. Utilization compares output with the configured 0.9 W panel rating; it is not solar conversion efficiency.</p></section>
<footer><p id="status" role="status" aria-live="polite">Waiting for the device. Acquisition runs independently of this page.</p></footer>
</main><script>const $ = id => document.getElementById(id);
let pendingRefresh=null, busy=false, actionBusy=false, online=false, lastData=null, lastResponseMs=0;
function display(value,digits=2){return Number.isFinite(value)?value.toFixed(digits):'--';}
function duration(ms){
 if(!Number.isFinite(ms)||ms<0)return '--';
 const seconds=Math.floor(ms/1000),minutes=Math.floor(seconds/60),hours=Math.floor(minutes/60);
 if(hours)return hours+' h '+String(minutes%60).padStart(2,'0')+' min '+String(seconds%60).padStart(2,'0')+' s';
 if(minutes)return minutes+' min '+String(seconds%60).padStart(2,'0')+' s';
 return seconds+' s';
}
function message(text,error=false){$('status').textContent=text;$('status').dataset.error=String(error);}
function draw(values){
 const canvas=$('chart'),ctx=canvas.getContext('2d'),ratio=Math.min(devicePixelRatio||1,2);
 const width=canvas.clientWidth,height=canvas.clientHeight;
 canvas.width=width*ratio;canvas.height=height*ratio;ctx.scale(ratio,ratio);
 const pad={left:49,right:12,top:24,bottom:30},w=Math.max(1,width-pad.left-pad.right),h=height-pad.top-pad.bottom;
 const finite=values.filter(Number.isFinite),maximum=Math.max(100,Math.ceil(Math.max(0,...finite)*1.15/100)*100);
 ctx.font='13px Segoe UI';ctx.textAlign='right';ctx.lineWidth=1;
 for(let i=0;i<=4;i++){
  const y=pad.top+i*h/4;ctx.strokeStyle='#e3e8eb';ctx.beginPath();ctx.moveTo(pad.left,y);ctx.lineTo(width-pad.right,y);ctx.stroke();
  ctx.fillStyle='#52616d';ctx.fillText((maximum*(1-i/4)).toFixed(0),pad.left-9,y+4);
 }
 ctx.textAlign='left';ctx.fillText('mW',pad.left,14);
 ctx.strokeStyle='#176747';ctx.lineWidth=2;ctx.beginPath();let connected=false;
 values.forEach((value,index)=>{
  if(!Number.isFinite(value)){connected=false;return;}
  const x=pad.left+(values.length>1?index*w/(values.length-1):w),y=pad.top+h*(1-value/maximum);
  if(connected)ctx.lineTo(x,y);else ctx.moveTo(x,y);connected=true;
 });ctx.stroke();ctx.fillStyle='#176747';
 values.forEach((value,index)=>{
  if(!Number.isFinite(value))return;
  const x=pad.left+(values.length>1?index*w/(values.length-1):w),y=pad.top+h*(1-value/maximum);
  ctx.beginPath();ctx.arc(x,y,index===values.length-1?3:1.5,0,Math.PI*2);ctx.fill();
 });
 ctx.fillStyle='#52616d';ctx.textAlign='left';ctx.fillText('Oldest',pad.left,height-6);ctx.textAlign='right';ctx.fillText('Latest',width-pad.right,height-6);
 $('history-count').textContent=values.length?values.length+' samples':'No samples';
 const latest=values.length?values[values.length-1]:null;
 const summary=finite.length?(Number.isFinite(latest)?'Latest power: '+display(latest,1)+' mW. ':'Latest power sample is unavailable. ')+finite.length+' valid readings in this history.':'No valid power readings in this history.';
 $('chart-summary').textContent=summary;canvas.setAttribute('aria-label','Power sample history. '+summary);
}
function controls(){
 const d=lastData,available=online&&d&&!actionBusy;
 $('start').disabled=!available||d.recording||d.count>0||d.capacity===0;
 $('stop').disabled=!available||!d.recording;
 $('reset').disabled=!available;
 const exportable=available&&!d.recording&&d.count>0;
 $('download').classList.toggle('disabled',!exportable);$('download').setAttribute('aria-disabled',String(!exportable));$('download').tabIndex=exportable?0:-1;
 for(const id of ['start','stop','reset'])$(id).setAttribute('aria-busy',String(actionBusy));
}
function freshness(){
 if(!lastData){controls();return;}
 const d=lastData,age=d.sample_age_ms+Math.max(0,Date.now()-lastResponseMs);
 const usable=online&&d.valid&&age<=2500&&[d.voltage_v,d.current_ma,d.power_mw].every(Number.isFinite);
 let state='live',label='Live';
 if(!online){state='offline';label='Offline';}
 else if(!d.valid){state='invalid';label='Sensor unavailable';}
 else if(age>2500){state='stale';label='Stale data';}
 $('connection').dataset.state=state;$('connection').textContent=label;
 $('sample-age').textContent=online?display(age/1000,1)+' s':'--';
 $('voltage').textContent=display(usable?d.voltage_v:null,3);$('current').textContent=display(usable?d.current_ma:null,2);
 $('power').textContent=display(usable?d.power_mw:null,1);$('utilization').textContent=usable?display(d.utilization_pct,1)+' %':'--';
 controls();
}
function render(d){
 lastData=d;lastResponseMs=Date.now();online=true;
 $('response-time').textContent='Last response '+new Date(lastResponseMs).toLocaleTimeString([], {hour:'2-digit',minute:'2-digit',second:'2-digit'});
 $('energy').textContent=display(d.session_energy_wh,5);$('count').textContent=d.count+' / '+d.capacity;
 $('elapsed').textContent=duration(d.elapsed_ms);$('covered-time').textContent=duration(d.covered_ms);
 $('gaps').textContent=Number.isFinite(d.gap_count)?String(d.gap_count):'--';$('sample-id').textContent=String(d.sample_id);
 const percent=d.capacity>0?Math.min(100,d.count/d.capacity*100):0;
 $('capacity').max=Math.max(1,d.capacity);$('capacity').value=d.count;$('capacity').textContent=display(percent,1)+'%';
 $('capacity-percent').textContent=d.capacity>0?display(percent,1)+'%':'Unavailable';
 $('recording').textContent=d.full?'Buffer full':(d.recording?'Recording':'Idle');$('recording').dataset.state=d.recording?'recording':'idle';
 $('session-note').textContent=d.capacity===0?'Recording memory is unavailable. Restart the device to retry allocation.':(d.full?'The buffer is full and recording has stopped. Export CSV, then reset for another session.':(d.count>0&&!d.recording?'Export your session, then reset before starting another recording.':'Recording is stored in RAM. Export before resetting or powering off.'));
 draw(Array.isArray(d.graph)?d.graph:[]);freshness();
}
async function loadSnapshot(){
 busy=true;
 try{
   const response=await fetch('/data',{cache:'no-store',signal:AbortSignal.timeout(5000)});
   if(!response.ok)throw Error('Measurement request failed');
   render(await response.json());
 }catch(error){online=false;$('connection').dataset.state='offline';$('connection').textContent='Offline';freshness();message('Device unreachable. Check the connection to the local network.',true);}
 finally{busy=false;}
}
function refresh(){
 freshness();
 if(!pendingRefresh)pendingRefresh=loadSnapshot().finally(()=>{pendingRefresh=null;});
 return pendingRefresh;
}
async function action(name){
 if(actionBusy||!online)return;
 if(name==='reset'&&!confirm('Clear recorded data and energy counters? Export your session first.'))return;
 actionBusy=true;controls();
 try{
   const response=await fetch('/'+name,{method:'POST',signal:AbortSignal.timeout(5000)});
   if(!response.ok)throw Error(await response.text());
   if(pendingRefresh)await pendingRefresh;
   await refresh();
  message(name==='start'?'Recording started. The next firmware sample begins the session.':(name==='stop'?'Recording stopped. Export CSV before resetting.':'Measurements reset. A new session can be started.'));
 }catch(error){message(error.message||'The operation could not be completed.',true);}
 finally{actionBusy=false;controls();}
}
for(const name of ['start','stop','reset'])$(name).addEventListener('click',()=>action(name));
$('download').addEventListener('click',event=>{if($('download').getAttribute('aria-disabled')==='true')event.preventDefault();});
window.addEventListener('resize',()=>{if(lastData)draw(lastData.graph||[]);});
message('Acquisition runs independently of this page. Stop recording before CSV export.');
refresh();setInterval(refresh,1000);
</script>
</body></html>
)SOLARHTML";
