import os
import subprocess

import pandas as pd

N_RUNS = 30
NX_VALUES = [512, 1024, 2048]
N_ITER_VALUES = [1000]
EPS_VALUES = [1e-16]
THREADS_VALUES = [2, 4, 8, 16]
VECTORIZE = [True, False]
PARALLEL = [True, False]


def run_command_and_get_result(cmd):
    print(cmd)
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    (output, err) = process.communicate()
    exit_code = process.wait()
    if exit_code != 0:
        raise Exception("Command '%s' failed with exit code %d" %
                        (cmd, exit_code))
    return output.decode("utf-8")


def build(*, vectorize: bool = False, parallel: bool = False):
    template = "g++ -Wall -ffast-math {args} laplace{omp}.cxx -o laplace"
    args = ""
    omp = ""
    if vectorize:
        args += " -O3"
    if parallel:
        args += " -fopenmp"
        omp = "_omp"
    cmd = template.format(args=args, omp=omp)
    run_command_and_get_result(cmd)


def execute(nx, n_iter, eps, threads=None):
    if threads:
        os.environ["OMP_NUM_THREADS"] = str(threads)
    cmd = f"./laplace {nx} {n_iter} {eps}"
    output = run_command_and_get_result(cmd)
    return output


def parse_output(output: str, vectorized: bool):
    res, nx, n_iter, time, threads = output.split(",")
    res = float(res)
    nx = int(nx)
    n_iter = int(n_iter)
    time = float(time)
    threads = int(threads)
    return {
        "res": res,
        "nx": nx,
        "n_iter": n_iter,
        "time": time,
        "threads": threads,
        "vectorized": vectorized
    }


if __name__ == "__main__":
    lines = []
    for vectorize in VECTORIZE:
        for parallel in PARALLEL:
            build(vectorize=vectorize, parallel=parallel)
            for nx in NX_VALUES:
                for n_iter in N_ITER_VALUES:
                    for eps in EPS_VALUES:
                        if parallel:
                            for threads in THREADS_VALUES:
                                for _ in range(N_RUNS):
                                    output = execute(
                                        nx, n_iter, eps, threads=threads)
                                    parsed = parse_output(output, vectorize)
                                    lines.append(parsed)
                        else:
                            for _ in range(N_RUNS):
                                output = execute(
                                    nx, n_iter, eps, threads=threads)
                                parsed = parse_output(output, vectorize)
                                lines.append(parsed)
    df = pd.DataFrame(lines)
    df.to_csv("results.csv")
