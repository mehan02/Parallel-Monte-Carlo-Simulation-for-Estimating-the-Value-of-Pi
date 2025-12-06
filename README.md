# SE3082 - Assignment 03
## Parallel Monte Carlo Simulation for Estimating the Value of Pi

**Course:** SE3082 - Parallel and Distributed Computing  
**Student:** [Your Name]  
**Program:** BSc (Hons) Information Technology - Year 3

---

## Project Structure

```
SE3082_Assignment03_MonteCarlo_Pi/
├── serial/              # Serial (single-threaded) implementation
│   ├── pi_serial.c      # Source code
│   └── Makefile         # Build script
├── openmp/              # OpenMP (shared-memory parallel) implementation
│   ├── pi_openmp.c      # Source code
│   └── Makefile         # Build script (auto-detects macOS/Linux)
├── mpi/                 # MPI (distributed-memory parallel) implementation
│   ├── pi_mpi.c         # Source code
│   └── Makefile         # Build script
├── cuda/                # CUDA (GPU parallel) implementation
│   └── pi_cuda.cu       # Source code (run on Google Colab)
├── analysis/            # Performance analysis tools
│   └── plot_results.py  # Python script to generate graphs
├── results/             # Experimental results
│   ├── summary.csv      # Combined results from all runs
│   ├── serial_*.csv     # Serial run outputs
│   ├── openmp_*.csv     # OpenMP run outputs
│   ├── mpi_*.csv        # MPI run outputs
│   └── *.png            # Generated performance graphs
├── report/              # Assignment report
│   ├── SE3082_MonteCarlo_Pi_Report.md
│   └── *.png            # Graphs for report
├── README.md            # This file
└── .gitignore           # Git ignore rules
```

---

## What Each Folder Does

### `serial/` - Serial Implementation
- **Purpose:** Baseline single-threaded Monte Carlo Pi estimation
- **How it works:** Uses `rand()` to generate random points, counts points inside quarter circle
- **Output:** CSV with MODE, SAMPLES, INSIDE, PI, ERROR, TIME

### `openmp/` - OpenMP Parallel Implementation  
- **Purpose:** Shared-memory parallelization using threads
- **How it works:** Divides samples among threads, each thread uses `rand_r()` with unique seed, combines results with atomic operation
- **Key features:** Auto-detects macOS (clang+libomp) or Linux (gcc)

### `mpi/` - MPI Distributed Implementation
- **Purpose:** Distributed-memory parallelization using processes
- **How it works:** Divides samples among MPI ranks, each rank computes locally, uses `MPI_Reduce` to sum results
- **Key features:** Can scale across multiple nodes/machines

### `cuda/` - CUDA GPU Implementation
- **Purpose:** Massively parallel GPU computation
- **How it works:** Each CUDA thread generates samples using Xorshift32 RNG, uses `atomicAdd` to accumulate results
- **Run on:** Google Colab (no local GPU needed)

### `analysis/` - Graph Generation
- **Purpose:** Creates performance visualization graphs
- **Input:** `results/summary.csv`
- **Output:** PNG graphs showing execution time and speedup

### `results/` - Experimental Data
- **Contains:** Raw CSV outputs from each run + generated PNG graphs
- **summary.csv:** Combined data used by plot_results.py

### `report/` - Assignment Report
- **Contains:** Full report in Markdown format with embedded graphs

---

## How to Run from Scratch (Step by Step)

### Prerequisites
```bash
# macOS
brew install libomp open-mpi python3
pip3 install pandas matplotlib

# Linux (Ubuntu/Debian)
sudo apt install gcc libomp-dev openmpi-bin libopenmpi-dev python3-pip
pip3 install pandas matplotlib
```

### Step 1: Build and Run Serial
```bash
cd serial
make
./pi_serial 10000000
# Output: CSV format with PI estimate and TIME
```

### Step 2: Build and Run OpenMP
```bash
cd openmp
make
./pi_openmp 10000000 1    # 1 thread
./pi_openmp 10000000 2    # 2 threads
./pi_openmp 10000000 4    # 4 threads
./pi_openmp 10000000 8    # 8 threads
```

### Step 3: Build and Run MPI
```bash
cd mpi
make
mpirun -np 1 ./pi_mpi 10000000    # 1 process
mpirun -np 2 ./pi_mpi 10000000    # 2 processes
mpirun -np 4 ./pi_mpi 10000000    # 4 processes
```

### Step 4: Run CUDA on Google Colab
1. Go to [Google Colab](https://colab.research.google.com)
2. Create new notebook
3. Change runtime: `Runtime → Change runtime type → GPU`
4. Run these cells:

```python
# Cell 1: Check GPU
!nvidia-smi
```

```python
# Cell 2: Create CUDA file
%%writefile pi_cuda.cu
# (paste contents of cuda/pi_cuda.cu here)
```

```python
# Cell 3: Compile and run
!nvcc -O2 pi_cuda.cu -o pi_cuda
!./pi_cuda 10000000 256 256
```

### Step 5: Collect Results into summary.csv
After running all tests, create `results/summary.csv` with format:
```csv
mode,workers,time,samples,pi,error
serial,1,0.259,10000000,3.1424,0.0008
openmp,1,0.279,10000000,3.1416,0.0001
openmp,2,0.145,10000000,3.1417,0.0001
openmp,4,0.095,10000000,3.1409,0.0006
...
```

### Step 6: Generate Performance Graphs
```bash
cd analysis
pip3 install pandas matplotlib   # if not installed
python3 plot_results.py
# Graphs saved to results/*.png
```

---

## How Graph Generation Works

The `analysis/plot_results.py` script:

1. **Reads** `results/summary.csv`
2. **Extracts** serial baseline time for speedup calculation
3. **For each mode** (openmp, mpi):
   - Calculates speedup = serial_time / parallel_time
   - Creates **Time vs Workers** graph
   - Creates **Speedup vs Workers** graph
4. **Saves** PNG files to `results/` folder

**Generated graphs:**
- `openmp_time.png` - OpenMP execution time vs thread count
- `openmp_speedup.png` - OpenMP speedup vs thread count  
- `mpi_time.png` - MPI execution time vs process count
- `mpi_speedup.png` - MPI speedup vs process count

---

## Output CSV Format

All implementations output in consistent CSV format:

```
MODE,SERIAL|OPENMP|MPI|CUDA
SAMPLES,<number>
THREADS,<number>           # OpenMP only
PROCESSES,<number>         # MPI only
BLOCKS,<number>            # CUDA only
THREADS_PER_BLOCK,<number> # CUDA only
INSIDE,<count>
PI,<estimated_value>
ERROR,<absolute_error>
TIME,<seconds>
```

---

## Quick Test (Verify Everything Works)

```bash
# From project root
cd serial && make && ./pi_serial 1000000 && make clean && cd ..
cd openmp && make && ./pi_openmp 1000000 4 && make clean && cd ..
cd mpi && make && mpirun -np 4 ./pi_mpi 1000000 && make clean && cd ..
```

Expected output: Each shows `PI,3.14xxxxx` with small ERROR.

---

## Results Summary (10 million samples)

| Mode | Workers | Time (s) | Speedup |
|------|---------|----------|---------|
| Serial | 1 | 0.259 | 1.00x |
| OpenMP | 4 | 0.095 | 2.72x |
| MPI | 4 | 0.100 | 2.60x |

*Tested on Apple M1 (8-core)*
