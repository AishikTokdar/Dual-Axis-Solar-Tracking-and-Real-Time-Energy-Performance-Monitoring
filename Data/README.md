# Data

| Location | Contents |
|---|---|
| [Tracked session](Raw/session-001/tracker.csv) | Tracked-panel device export; 10,801 samples over 10,800 seconds |
| [Fixed session](Raw/session-001/fixed.csv) | Fixed-panel device export; 10,801 samples over 10,800 seconds |
| [Session metadata](Raw/session-001/metadata.json) | Session parameters |
| [Reported summary](Reference/reported_summary.csv) | Aggregate values from hardware experiments |
| [Metadata template](session_metadata.example.json) | Template for future captures |

[Schema](Schema.md) defines columns, units, and sample validity rules. Save new measurements under `Raw/<session-id>/` alongside a completed `metadata.json`, or use `Raw/local/` (git-ignored) for private working captures. For sessions longer than the ESP32's 3,600-sample RAM buffer, collect data with `Tools/capture_session.py`.
