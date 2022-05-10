#!/usr/bin/env bash

set -e

pip install nox pyperf

sudo -n env "PATH=$PATH" pyperf system tune || true

nox -s pyperformance_looong

pyperf compare_to pyperformance-3.8-raw.json pyperformance-3.8-cds-site.json pyperformance-3.8-cds.json --table | tee pyperformance-3.9.table
pyperf compare_to pyperformance-3.9-raw.json pyperformance-3.9-cds-site.json pyperformance-3.9-cds.json --table | tee pyperformance-3.9.table
pyperf compare_to pyperformance-3.10-raw.json pyperformance-3.10-cds-site.json pyperformance-3.10-cds.json --table | tee pyperformance-3.10.table

pyperf compare_to \
  pyperformance-3.8-raw.json pyperformance-3.8-cds-site.json pyperformance-3.8-cds.json \
  pyperformance-3.9-raw.json pyperformance-3.9-cds-site.json pyperformance-3.9-cds.json \
  pyperformance-3.10-raw.json pyperformance-3.10-cds-site.json pyperformance-3.10-cds.json \
  --table | tee pyperformance-3.9.table
