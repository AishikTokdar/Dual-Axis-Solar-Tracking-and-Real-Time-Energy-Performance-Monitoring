function summary = analyze_energy_performance(trackerPath, fixedPath, outputDir)
tracker = readSession(trackerPath);
fixed = readSession(fixedPath);
summary = table([tracker.energy_wh; fixed.energy_wh], [tracker.peak_mw; fixed.peak_mw], ...
    [tracker.mean_mw; fixed.mean_mw], 'VariableNames', {'Energy_Wh', 'PeakPower_mW', 'MeanPower_mW'}, 'RowNames', {'Tracked', 'Fixed'});
gain = 100 * (tracker.energy_wh - fixed.energy_wh) / fixed.energy_wh;
if ~isfolder(outputDir), mkdir(outputDir); end
writetable(summary, fullfile(outputDir, 'summary.csv'), 'WriteRowNames', true);
fprintf('Gross energy difference: %.4f%%\n', gain);
end

function result = readSession(path)
t = readtable(path);
dt = diff(t.Time_s);
valid = t.Valid == 1;
pairs = valid(1:end-1) & valid(2:end);
energy = sum((t.Power_mW(1:end-1) + t.Power_mW(2:end)) .* 0.5 .* dt .* pairs / 3600000);
result = struct('energy_wh', energy, 'peak_mw', max(t.Power_mW(valid)), 'mean_mw', mean(t.Power_mW(valid)));
end
