#!/bin/bash
g++ cpp/main.cpp -o cpp/traffic_model
./cpp/traffic_model "$1"
python3 python/analyzer.py result/result_csv.csv
