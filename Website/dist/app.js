const byId=id=>document.getElementById(id);
const componentData={
  stm32:{category:'Control processor',name:'STM32G031K8',description:'Reads four light dividers, filters each channel, and converts imbalance into bounded pan and tilt commands. Network traffic is handled by the other board.',specs:[['Inputs','PA4, PA1, PA5, PA0 / ADC1'],['Outputs','PA8 and PA10 / TIM1 PWM'],['Control target','5 ms per update; actual timing needs hardware verification']],boundary:'The feedback variable is light imbalance. Servo pulse width does not measure panel angle.',link:'assets/STM32_Architecture.png'},
  esp32:{category:'Measurement and local web',name:'ESP32',description:'Acquires a power sample on a nominal one-second schedule. Updates energy and recording state before serving cached data to the browser.',specs:[['Sensor bus','GPIO21 SDA / GPIO22 SCL'],['Recording','3,600 RAM records; volatile storage'],['Connectivity','Local Wi-Fi and a USB serial interface']],boundary:'HTTP and I2C work can delay acquisition. Actual timestamps, sample validity, and excluded intervals keep the data interpretation explicit.',link:'assets/ESP32_Architecture.png'},
  ldr:{category:'Directional light sensing',name:'Four LDR sensors',description:'Light-dependent resistors form four voltage dividers. A quadrant arrangement provides the horizontal and vertical imbalance used by the controller.',specs:[['Channels','Top-left, top-right, bottom-left, bottom-right'],['Acquisition','Three ADC readings averaged per channel'],['Conditioning','Independent scalar Kalman filters; Q = 2, R = 200']],boundary:'ADC counts are a directional proxy. They are not calibrated irradiance or a direct measurement of the sun angle.',link:'assets/tracker_wiring.png'},
  ina226:{category:'Electrical instrumentation',name:'INA226',description:'Measures bus voltage and shunt voltage over I2C. The logger derives current using the configured shunt and calculates electrical power from voltage and current.',specs:[['Address','0x40, subject to breakout address straps'],['Shunt','Configured 0.1 ohm'],['Acquisition','16-sample averaging; checked register reads']],boundary:'A configured shunt and calibration register do not establish reference-meter accuracy. Physical calibration records are still needed.',link:'assets/logger_wiring.png'},
  servos:{category:'Mechanical actuation',name:'Pan, tilt, and printed frame',description:'Two servo commands orient the panel using the third-party printed frame. Startup ramping, smoothing, and limits constrain the command sequence.',specs:[['Pan commands','500–2,000 microseconds'],['Tilt commands','500–1,300 microseconds'],['PWM frame','20 ms from the configured timer clock']],boundary:'Safe physical travel must be confirmed on the assembled mechanism. The STL frame is credited to its original designer; the project does not claim original mechanical CAD.',link:'assets/cad-attribution.txt'},
  panel:{category:'Source and operating point',name:'Solar panel and fixed load',description:'The panel rating is 6 V at 150 mA. A 47 ohm resistor defines the electrical operating point used by the single-channel measurement setup.',specs:[['Configured rating','0.9 W'],['Load','47 ohm, 5 W resistor'],['Comparison','Sequential tracked and fixed sessions']],boundary:'A fixed resistor does not perform maximum power point tracking. Actuator energy and incident irradiance are outside this measurement path.',link:'assets/hardware_schematic.pdf'}
};
document.querySelectorAll('[data-component]').forEach(button=>button.addEventListener('click',()=>{
  const item=componentData[button.dataset.component];
  document.querySelectorAll('[data-component]').forEach(other=>{const active=other===button;other.classList.toggle('active',active);other.setAttribute('aria-pressed',String(active));});
  byId('component-category').textContent=item.category;byId('component-name').textContent=item.name;byId('component-description').textContent=item.description;byId('component-boundary').textContent=item.boundary;byId('component-link').href=item.link;
  byId('component-link').textContent=button.dataset.component==='servos'?'Read mechanical CAD credits':(button.dataset.component==='esp32'?'Open measurement pipeline':'Open the wiring reference');
  byId('component-specs').replaceChildren(...item.specs.map(([label,value])=>{const row=document.createElement('div'),dt=document.createElement('dt'),dd=document.createElement('dd');dt.textContent=label;dd.textContent=value;row.append(dt,dd);return row;}));
}));

let panActive=false,tiltActive=false;
function updateControl(){
  const x=Number(byId('light-x').value),y=Number(byId('light-y').value),ambient=Number(byId('ambient').value),base=ambient/100*4095;
  const clamp=value=>Math.max(0,Math.min(4095,Math.round(value)));
  const values=[clamp(base-x*2.5-y*2.5),clamp(base+x*2.5-y*2.5),clamp(base-x*2.5+y*2.5),clamp(base+x*2.5+y*2.5)];
  ['tl','tr','bl','br'].forEach((part,index)=>{byId('value-'+part).textContent=values[index];const level=25+values[index]/4095*43;byId('sensor-'+part).setAttribute('fill',`hsl(120 17% ${level}%)`);byId('value-'+part).setAttribute('fill',level>56?'#172e24':'#f3f7f1');});
  const mean=values.reduce((a,b)=>a+b,0)/4,pan=(values[1]+values[3]-values[0]-values[2])/2,tilt=(values[2]+values[3]-values[0]-values[1])/2,entry=50+mean/4095*30,exit=entry*.4;
  if(mean<100){panActive=tiltActive=false;}else{panActive=panActive?Math.abs(pan)>exit:Math.abs(pan)>=entry;tiltActive=tiltActive?Math.abs(tilt)>exit:Math.abs(tilt)>=entry;}
  byId('light-x-output').textContent=x+'%';byId('light-y-output').textContent=y+'%';byId('ambient-output').textContent=ambient+'%';
  byId('pan-imbalance').textContent=pan.toFixed(0)+' counts';byId('thresholds').textContent=entry.toFixed(0)+' / '+exit.toFixed(0)+' counts';
  const cx=252+x*.65,cy=169+y*.65;byId('light-position').setAttribute('cx',cx);byId('light-position').setAttribute('cy',cy);byId('light-cross').setAttribute('d',`M${cx-8} ${cy}h16M${cx} ${cy-8}v16`);
  let state='Hold position',reason='The light imbalance is within the idle band.';
  if(mean<100){state='Low-light hold';reason='Ambient is below 100 ADC counts. The controller holds its command.';}
  else if(panActive||tiltActive){state='Tracking active';const directions=[];if(panActive)directions.push(pan>0?'right':'left');if(tiltActive)directions.push(tilt>0?'bottom':'top');reason='The '+directions.join(' and ')+' side is brighter. The corresponding axis remains active until its smaller exit threshold is reached.';}
  else if(pan===0&&tilt===0){reason='The light is balanced across the four sensors.';}
  byId('control-state').textContent=state;byId('control-reason').textContent=reason;
}
['light-x','light-y','ambient'].forEach(id=>byId(id).addEventListener('input',()=>{document.querySelectorAll('[data-preset]').forEach(b=>{b.classList.remove('selected');b.setAttribute('aria-pressed','false');});updateControl();}));
document.querySelectorAll('[data-preset]').forEach(button=>button.addEventListener('click',()=>{
  const presets={balanced:[0,0,50],right:[55,0,50],dim:[0,0,1]};const values=presets[button.dataset.preset];
  ['light-x','light-y','ambient'].forEach((id,index)=>byId(id).value=values[index]);panActive=tiltActive=false;
  document.querySelectorAll('[data-preset]').forEach(other=>{const active=other===button;other.classList.toggle('selected',active);other.setAttribute('aria-pressed',String(active));});updateControl();
}));
updateControl();

const metricSettings={energy:{title:'Measured energy',unit:'Wh',digits:4},peak_power:{title:'Measured peak power',unit:'mW',digits:1},average_power:{title:'Measured mean power',unit:'mW',digits:1}};
let metrics={energy:{tracker:1.4125,fixed:.5707},peak_power:{tracker:670,fixed:315},average_power:{tracker:470.8,fixed:190.2}};
function selectMetric(key){
  const config=metricSettings[key],values=metrics[key],maximum=Math.max(values.tracker,values.fixed)||1;
  byId('comparison-title').replaceChildren(document.createTextNode(config.title+' '));const unit=document.createElement('span');unit.textContent=config.unit;byId('comparison-title').append(unit);
  byId('tracked-value').textContent=values.tracker.toFixed(config.digits)+' '+config.unit;byId('fixed-value').textContent=values.fixed.toFixed(config.digits)+' '+config.unit;
  byId('tracked-bar').style.width=(values.tracker/maximum*100)+'%';byId('fixed-bar').style.width=(values.fixed/maximum*100)+'%';
  document.querySelectorAll('[data-metric]').forEach(button=>{const active=button.dataset.metric===key;button.classList.toggle('active',active);button.setAttribute('aria-pressed',String(active));});
}
document.querySelectorAll('[data-metric]').forEach(button=>button.addEventListener('click',()=>selectMetric(button.dataset.metric)));
fetch('assets/project-data.json').then(response=>{if(!response.ok)throw new Error('Reference data unavailable');return response.json();}).then(data=>{
  if(!data.metrics||!Object.keys(metricSettings).every(key=>[data.metrics[key]?.tracker,data.metrics[key]?.fixed].every(value=>Number.isFinite(value)&&value>=0)))throw new Error('Invalid reference metrics');
  metrics=data.metrics;selectMetric('energy');
}).catch(()=>{selectMetric('energy');});

function updateEnergy(){
  const milliwatts=Number(byId('model-power').value),minutes=Number(byId('model-time').value),energy=milliwatts/1000*minutes/60;
  byId('model-power-output').textContent=milliwatts+' mW';byId('model-time-output').textContent=minutes+' min';byId('model-energy').textContent=energy.toFixed(4)+' Wh';
  byId('chart-time').textContent=minutes+' min';byId('chart-energy').textContent=energy.toFixed(4)+' Wh';const end=250-energy/.9*205;
  byId('energy-line').setAttribute('d',`M65 250L555 ${end}`);byId('energy-fill').setAttribute('d',`M65 250L555 ${end}L555 250Z`);
  byId('energy-desc').textContent=`At a constant ${milliwatts} milliwatts for ${minutes} minutes, calculated energy is ${energy.toFixed(4)} watt-hours.`;
}
['model-power','model-time'].forEach(id=>byId(id).addEventListener('input',updateEnergy));updateEnergy();
const video=document.querySelector('video');video.muted=true;video.addEventListener('volumechange',()=>{if(!video.muted)video.muted=true;});
