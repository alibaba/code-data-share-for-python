$ErrorActionPreference = 'Stop'

pip install -U nox pyperf

if ($env:GITHUB_ACTIONS -eq "true") {
    nox -s pyperformance # GA has a 6h timeout for each job
} else {
    nox -s pyperformance_looong
}

pyperf compare_to pyperformance-3.8-raw.json pyperformance-3.8-cds-site.json pyperformance-3.8-cds.json --table | Tee-Object -FilePath "pyperformance-3.9.table"
pyperf compare_to pyperformance-3.9-raw.json pyperformance-3.9-cds-site.json pyperformance-3.9-cds.json --table | Tee-Object -FilePath "pyperformance-3.9.table"
pyperf compare_to pyperformance-3.10-raw.json pyperformance-3.10-cds-site.json pyperformance-3.10-cds.json --table | Tee-Object -FilePath "pyperformance-3.10.table"
pyperf compare_to pyperformance-3.11-raw.json pyperformance-3.11-cds-site.json pyperformance-3.11-cds.json --table | Tee-Object -FilePath "pyperformance-3.11.table"
pyperf compare_to pyperformance-3.12-raw.json pyperformance-3.12-cds-site.json pyperformance-3.12-cds.json --table | Tee-Object -FilePath "pyperformance-3.12.table"
