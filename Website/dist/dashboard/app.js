const $ = id => document.getElementById(id);
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
   const response=await fetch('/'+name,{method:'POST',headers:{'X-Solar-Action':'1'},signal:AbortSignal.timeout(5000)});
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
