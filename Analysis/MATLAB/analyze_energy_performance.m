function summary = analyze_energy_performance(trackerPath, fixedPath, outputDir)
tracker = readSession(trackerPath);
fixed = readSession(fixedPath);
assert(abs(tracker.duration_s - fixed.duration_s) < 0.01, 'Observation durations must match.');
assert(fixed.energy_wh > 0, 'Fixed energy must be positive.');
summary = table([tracker.energy_wh; fixed.energy_wh], [tracker.peak_mw; fixed.peak_mw], ...
    [tracker.mean_mw; fixed.mean_mw], 'VariableNames', {'Energy_Wh', 'PeakPower_mW', 'MeanPower_mW'}, 'RowNames', {'Tracked', 'Fixed'});
gain = 100 * (tracker.energy_wh - fixed.energy_wh) / fixed.energy_wh;
if ~isfolder(outputDir), mkdir(outputDir); end
writetable(summary, fullfile(outputDir, 'summary.csv'), 'WriteRowNames', true);
fprintf('Gross energy difference: %.4f%%\n', gain);
end

function result = readSession(path)
t = readtable(path);
time = t.Time_s; power = t.Power_mW; valid = t.Valid == 1;
dt = diff(time);
pairs = valid(1:end-1) & valid(2:end) & dt <= 2.5;
energyTerms = (power(1:end-1) + power(2:end)) .* 0.5 .* dt / 3600000;
energy = sum(energyTerms(pairs)); covered = sum(dt(pairs));
result = struct('duration_s', time(end) - time(1), 'energy_wh', energy, 'peak_mw', max(power(valid)), ...
    'mean_mw', energy * 3600000 / covered, 'excluded_intervals', sum(~pairs));
end
