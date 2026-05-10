#!/bin/bash
set -e


./cpp/build/traffic_model "$1"
python3 python/analyzer.py result/result_csv.csv
