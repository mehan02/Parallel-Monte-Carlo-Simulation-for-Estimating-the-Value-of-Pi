#!/usr/bin/env python3
# plot_results.py
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

res = pd.read_csv('../results/summary.csv')

# Get serial baseline time
serial_time = res[res['mode'] == 'serial']['time'].values[0] if 'serial' in res['mode'].values else None

# Separate by mode
for mode in res['mode'].unique():
    df = res[res['mode'] == mode].copy()
    # For openmp and mpi, workers is integer; for cuda it's "blocksxthreads"
    if mode in ['openmp','mpi']:
        df['workers'] = df['workers'].astype(int)
        df = df.sort_values('workers')
        baseline = serial_time if serial_time else df['time'].iloc[0]
        df['speedup'] = baseline / df['time']
        plt.figure()
        plt.plot(df['workers'], df['time'], marker='o')
        if serial_time:
            plt.axhline(y=serial_time, color='r', linestyle='--', label=f'Serial ({serial_time:.3f}s)')
            plt.legend()
        plt.xlabel('Workers (threads/processes)')
        plt.ylabel('Execution time (s)')
        plt.title(f'{mode.upper()}: Workers vs Execution time')
        plt.grid(True)
        plt.savefig(f'../results/{mode}_time.png')
        plt.close()

        plt.figure()
        plt.plot(df['workers'], df['speedup'], marker='o')
        plt.axhline(y=1.0, color='r', linestyle='--', label='Serial baseline')
        plt.legend()
        plt.xlabel('Workers (threads/processes)')
        plt.ylabel('Speedup (vs Serial)')
        plt.title(f'{mode.upper()}: Workers vs Speedup')
        plt.grid(True)
        plt.savefig(f'../results/{mode}_speedup.png')
        plt.close()
    elif mode == 'serial':
        # Skip serial for individual plots
        continue
    else:
        # CUDA: annotate workers column
        def workers_to_int(s):
            try:
                a,b = s.split('x')
                return int(a)*int(b)
            except:
                return 0
        df['workers_int'] = df['workers'].apply(workers_to_int)
        df = df.sort_values('workers_int')
        baseline = df['time'].iloc[0]
        df['speedup'] = baseline / df['time']
        plt.figure()
        plt.plot(df['workers_int'], df['time'], marker='o')
        plt.xlabel('Total CUDA threads (blocks x threads)')
        plt.ylabel('Execution time (s)')
        plt.title('CUDA: Threads vs Execution time')
        plt.grid(True)
        plt.savefig('../results/cuda_time.png')

        plt.figure()
        plt.plot(df['workers_int'], df['speedup'], marker='o')
        plt.xlabel('Total CUDA threads (blocks x threads)')
        plt.ylabel('Speedup')
        plt.title('CUDA: Threads vs Speedup')
        plt.grid(True)
        plt.savefig('../results/cuda_speedup.png')

print("Plots saved to ../results/")

