function summary = analyze_energy_performance(trackerPath, fixedPath, outputDir)
arguments
    trackerPath (1,1) string
    fixedPath (1,1) string
    outputDir (1,1) string
end
tracker = readSession(trackerPath);
fixed = readSession(fixedPath);
assert(abs(tracker.duration_s-fixed.duration_s)<0.01, 'Observation durations must match.');
assert(fixed.energy_wh>0, 'Fixed energy must be positive.');
summary = table([tracker.energy_wh;fixed.energy_wh], [tracker.peak_mw;fixed.peak_mw], ...
    [tracker.mean_mw;fixed.mean_mw], 'VariableNames', {'Energy_Wh','PeakPower_mW','MeanPower_mW'}, 'RowNames', {'Tracked','Fixed'});
gain = 100*(tracker.energy_wh-fixed.energy_wh)/fixed.energy_wh;
assert(isfinite(gain),'Energy gain exceeds the supported numeric range.');
if ~isfolder(outputDir), mkdir(outputDir); end
writetable(summary,fullfile(outputDir,'summary.csv'),'WriteRowNames',true);
fig=figure('Color','white');
tiledlayout(1,2);
nexttile;plot(tracker.time-tracker.time(1),tracker.power,'LineWidth',1.2);hold on;
plot(fixed.time-fixed.time(1),fixed.power,'LineWidth',1.2);
xlabel('Elapsed session time (s)');ylabel('Power (mW)');legend('Tracked','Fixed');title('Separate observation windows');
nexttile;bar([tracker.energy_wh,fixed.energy_wh]);xticklabels({'Tracked','Fixed'});
ylabel('Energy (Wh)');title(sprintf('Gross difference: %.2f%%',gain));
exportgraphics(fig,fullfile(outputDir,'energy_performance.png'),'Resolution',180);
fprintf('Gross energy difference: %.6f%%\n',gain);
disp(summary);
end
function result = readSession(path)
t=readtable(path);
required={'Time_s','Voltage_V','Current_mA','Power_mW','Energy_Wh','Valid'};
assert(all(ismember(required,t.Properties.VariableNames)), 'Missing required columns.');
time=t.Time_s;power=t.Power_mW;valid=t.Valid==1;
assert(height(t)>=2 && all(isfinite(time)) && all(time>=0) && all(diff(time)>0),'Invalid timestamps.');
assert(all(ismember(t.Valid,[0 1])),'Invalid validity flag.');
assert(all(isfinite(t.Energy_Wh)) && all(t.Energy_Wh>=0) && all(diff(t.Energy_Wh)>=0),'Invalid or reset energy accumulator.');
assert(all(isfinite(power(valid))) && all(power(valid)>=0),'Invalid power readings.');
assert(all(isfinite(t.Voltage_V(valid))) && all(t.Voltage_V(valid)>=0),'Invalid voltage.');
assert(all(isfinite(t.Current_mA(valid))) && all(t.Current_mA(valid)>=0),'Invalid current.');
assert(all(abs(power(valid)-t.Voltage_V(valid).*t.Current_mA(valid))<=max(0.1,0.001*power(valid))),'Inconsistent electrical units.');
dt=diff(time);pairs=valid(1:end-1)&valid(2:end)&dt<=2.5;
power(~valid)=NaN;
energyTerms=(power(1:end-1)+power(2:end)).*0.5.*dt/3600000;
energy=sum(energyTerms(pairs));covered=sum(dt(pairs));
assert(covered>0,'No integrable intervals.');
assert(isfinite(energy) && isfinite(covered) && isfinite(energy*3600000/covered),'Integrated values exceed the supported numeric range.');
result=struct('time',time,'power',power,'duration_s',time(end)-time(1),'energy_wh',energy,...
 'peak_mw',max(power,[],'omitnan'),'mean_mw',energy*3600000/covered,'excluded_intervals',sum(~pairs));
end
