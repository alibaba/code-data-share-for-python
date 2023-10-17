$ErrorActionPreference = 'Stop'

pip install -U nox pyperf

nox -t test_import_third_party_perf

pyperf compare_to perf-import-3.8-raw.json perf-import-3.8-cds.json --table | Tee-Object -FilePath "perf-import-3.8.table"
pyperf compare_to perf-import-3.9-raw.json perf-import-3.9-cds.json --table | Tee-Object -FilePath "perf-import-3.9.table"
pyperf compare_to perf-import-3.10-raw.json perf-import-3.10-cds.json --table | Tee-Object -FilePath "perf-import-3.10.table"
pyperf compare_to perf-import-3.11-raw.json perf-import-3.11-cds.json --table | Tee-Object -FilePath "perf-import-3.11.table"
pyperf compare_to perf-import-3.12-raw.json perf-import-3.12-cds.json --table | Tee-Object -FilePath "perf-import-3.12.table"
