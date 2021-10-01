import os
import subprocess


def run_command_and_get_result(cmd):
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    (output, err) = process.communicate()
    exit_code = process.wait()
    if exit_code != 0:
        raise Exception("Command '%s' failed with exit code %d" %
                        (cmd, exit_code))
    return output.decode("utf-8")


def clean():
    cmd = "make clean"
    run_command_and_get_result(cmd)


def build_raw():
    cmd = "make build"
    run_command_and_get_result(cmd)


def build_omp():
    cmd = "make build_omp"
    run_command_and_get_result(cmd)


def build_all():
    clean()
    build_raw()
    build_omp()


def execute(nx, n_iter, eps, mode="", threads=None):
    mode = "_" + mode if mode != "" else ""
    if threads:
        os.environ["OMP_NUM_THREADS"] = str(threads)
    cmd = f"./laplace{mode} {nx} {n_iter} {eps}"
    output = run_command_and_get_result(cmd)
    return output


if __name__ == "__main__":
    build_all()
    output = execute(512, 1000, 1e-6, mode="omp", threads=2)
    print(f"Output is {output}")
