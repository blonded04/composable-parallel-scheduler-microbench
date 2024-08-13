import json
import sys
import matplotlib.pyplot as plt
from multiprocessing import Pool
from matplotlib.pyplot import cm
from functools import partial
import itertools
import time
import math
import numpy as np
import re
import os
from matplotlib.gridspec import GridSpec
from matplotlib.ticker import AutoMinorLocator

HPX_MODES = [
    "HPX_STATIC",
    "HPX_ASYNC"
]
PROACTIVE_MODES = [
    "PROACTIVE_STATIC"
]
OMP_MODES = [
    "OMP_STATIC",
    "OMP_DYNAMIC_NONMONOTONIC",
    "OMP_DYNAMIC_MONOTONIC",
    "OMP_GUIDED_MONOTONIC",
    "OMP_GUIDED_NONMONOTONIC"
    "OMP_RUNTIME_af",
    "OMP_RUNTIME_awf",
    "OMP_RUNTIME_fac2",
    "OMP_RUNTIME_fac",
    "OMP_RUNTIME_mfsc",
    "OMP_RUNTIME_tap",
    "OMP_RUNTIME_tfss",
]
TASKFLOW_MODES = [
    "TASKFLOW_GUIDED",
    "TASKFLOW_DYNAMIC",
    "TASKFLOW_STATIC"
]
TBB_MODES = [
    "TBB_AUTO",
    "TBB_SIMPLE",
    "TBB_AFFINITY",
    # "TBB_CONST_AFFINITY"
]
EIGEN_MODES = [
    "EIGEN_SIMPLE",
    "EIGEN_STATIC",
    "EIGEN_TIMESPAN",
    "EIGEN_TIMESPAN_GRAINSIZE"
]

COLORS = "ybgrcmk"

filtered_modes = set()
# filtered_modes.update(OMP_MODES)
# filtered_modes.update(TBB_MODES)
# filtered_modes.update(EIGEN_MODES)

# thesis (final):
filtered_modes.update(["EIGEN_TIMESPAN_GRAINSIZE", "TASKFLOW_DYNAMIC", "TASKFLOW_STATIC"
                       "OMP_STATIC", "OMP_DYNAMIC_NONMONOTONIC", "OMP_GUIDED_NONMONOTONIC",
                       "TBB_AFFINITY", "TBB_SIMPLE", "TBB_AUTO"])

filtered_benchmarks = set()

omp_runtime = True


def split_bench_name(s):
    s = s.split("_")
    prefix = []
    for part in s:
        if part.islower():
            prefix.append(part)
        else:
            break
    return "_".join(prefix), "_".join(s[len(prefix) :])


def generate_md_table(benchmarks):
    header_row = ["name"] + list(benchmarks.keys())
    table = "| " + " | ".join([r[r.startswith("bench_") and len("bench_"):] for r in header_row]) + " |" + "\n"
    # name | bench1 | bench2 | bench3
    # tbb_simple | 1 | 2 | 3
    # tbb_auto | 1 | 2 | 3
    # ...
    results = {}
    for bench_name, results_by_mode in benchmarks.items():
        for name, value in results_by_mode.items():
            results.setdefault(name, {})[bench_name] = value

    for name, values in sorted(results.items()):
        row = []
        for col in header_row[1:]:
            row.append(values.get(col, ""))
        table += "| " + name + " | " + " | ".join(row) + " |" + "\n"
    return table


def save_figure(path, fig, name):
    fig.savefig(os.path.join(path, name + ".jpeg"), bbox_inches="tight", format="jpeg")
    fig.savefig(
        os.path.join(path, name + ".svg"), bbox_inches="tight", format="svg", dpi=1200
    )


# returns new plot
def plot_benchmark(type, benchmarks, title, verbose):
    scaled_sums_by_name = {}
    for params, bench_results in benchmarks.items():
        max_value = min(bench_results.values())
        for name, res in bench_results.items():
            scaled_sums_by_name.setdefault(name, 0)
            scaled_sums_by_name[name] += res / max_value
    # average relative difference with the best solution
    min_value = min(scaled_sums_by_name.values())
    scaled_sums_by_name = {k: v / min_value for k, v in scaled_sums_by_name.items()}
    table_row = {title: {name: f"{res:.2f} us (x{res :.2f})" for name, res in scaled_sums_by_name.items()}}

    params_count = len(benchmarks)
    if params_count == 1:
        fig, axis = plt.subplots(
            params_count,
            2 if verbose else 1,
            figsize=(36 if verbose else 16, params_count * 6),
            squeeze=False,
        )
        iter = 0
        for params, bench_results in benchmarks.items():
            bench_results = sorted(bench_results.items(), key=lambda x: x[1])
            min_time = sorted(bench_results, key=lambda x: x[1])[0][1]
            params_str = ""
            # if params != "":
            #     params_str = " with params " + params
            axis[iter][0].barh(*zip(*bench_results), zorder=3)
            axis[iter][0].xaxis.set_major_locator(plt.MaxNLocator(nbins=12))
            axis[iter][0].xaxis.set_minor_locator(AutoMinorLocator(5))
            if verbose:
                if "Throughput" == type:
                    axis[iter][0].set_xlabel(
                        title + params_str + ", processed iterations (higher is better)",
                        fontsize=14,
                    )
                    axis[iter][1].set_xlabel(
                        title + params_str + ", normalized (lower is better)", fontsize=14
                    )
                else:
                    axis[iter][0].set_xlabel(
                        title + params_str + ", absolute time (lower is better), us",
                        fontsize=14,
                    )
                    axis[iter][1].set_xlabel(
                        title + params_str + ", normalized (higher is better)", fontsize=14
                    )
                axis[iter][1].barh(
                    *zip(*[(name, min_time / time) for name, time in bench_results]), zorder=3
                )
                axis[iter][1].xaxis.set_major_locator(plt.MaxNLocator(nbins=12))
                axis[iter][1].xaxis.set_minor_locator(AutoMinorLocator(5))
            iter += 1
        for axs in axis:
            for ax in axs:
                if verbose:
                    ax.tick_params(axis="both", which="major", labelsize=14)
                    fig.tight_layout()
                else:
                    ax.tick_params(axis="both", which="major", labelsize=20)
                ax.grid(which='major', color='#DDDDDD', linewidth=0.8)
                ax.grid(which='minor', color='#EEEEEE', linewidth=0.5)
        return fig, table_row
    else:
        fig, ax = plt.subplots(figsize=(10, 6))

        styles = itertools.cycle(["-", "--", "-.", ":"])
        colors = itertools.cycle(COLORS)
        inverted = {}
        if "Throughput" == type:
            for params, bench_results in benchmarks.items():
                for name, value in bench_results.items():
                    inverted.setdefault(name, {})[params] = value
            params_str = ""
            style = next(styles)
            color = next(colors)
            for name, bench_results in sorted(inverted.items()):
                if not params_str:
                    params_str = [x.split(':')[0] for x in list(bench_results.keys())[0].split(';')][0]
                if "spmv" in title:
                    bench_results = {str(int([x.split(':')[1] for x in k.split(';')][0]) + 195): v for k, v in bench_results.items()}
                elif 'reduce' in title:
                    bench_results = {str(int([x.split(':')[1] for x in k.split(';')][0]) + 51): v for k, v in bench_results.items()}
                else:
                    bench_results = {[x.split(':')[1] for x in k.split(';')][0]: v for k, v in bench_results.items()}
                ax.plot(bench_results.keys(), bench_results.values(), marker='o', linestyle=style, color=color, label=name)
                color = next(colors)
                if color == COLORS[0]:
                    style = next(styles)
            ax.set_xlabel(f'Params ({params_str})', fontsize=14)
            ax.set_ylabel('Items', fontsize=14)
        else:
            for params, bench_results in benchmarks.items():
                for name, value in bench_results.items():
                    # print(name, value, math.log(value))
                    inverted.setdefault(name, {})[params] = math.log10(value)
            params_str = ""
            style = next(styles)
            color = next(colors)
            for name, bench_results in sorted(inverted.items()):
                if not params_str:
                    params_str = ', '.join([x.split(':')[0] for x in list(bench_results.keys())[0].split(';')])
                if not params_str:
                    params_str = ', '.join([x.split(':')[0] for x in list(bench_results.keys())[0].split(';')])
                if "spmv" in title:
                    bench_results = {str(int([x.split(':')[1] for x in k.split(';')][0]) + 195): v for k, v in bench_results.items()}
                elif 'reduce' in title:
                    bench_results = {str(int([x.split(':')[1] for x in k.split(';')][0]) + 51): v for k, v in bench_results.items()}
                else:
                    bench_results = {[x.split(':')[1] for x in k.split(';')][0]: v for k, v in bench_results.items()}
                # print(bench_results)
                ax.plot(bench_results.keys(), bench_results.values(), marker='o', linestyle=style, color=color, label=name)
                color = next(colors)
                if color == COLORS[0]:
                    style = next(styles)
            ax.set_xlabel(f'Params ({params_str})', fontsize=14)
            ax.set_ylabel('Time, log10(us)', fontsize=14)
        ax.set_title(title)
        ax.yaxis.set_major_locator(plt.MaxNLocator(nbins=12))
        ax.yaxis.set_minor_locator(AutoMinorLocator(5))
        ax.legend()
        plt.tight_layout()
        plt.grid()
        return fig, table_row


def parse_benchmarks(folder_name):
    benchmarks_by_type = [{}, {}]
    for bench_file in os.listdir(folder_name):
        if not bench_file.endswith(".json"):
            continue
        with open(os.path.join(folder_name, bench_file)) as f:
            try:
                bench = json.load(f)
            except json.decoder.JSONDecodeError as e:
                print("Error while parsing", bench_file, e, ", skipping")
                continue
            name = bench_file.split(".")[0]
            bench_type, bench_mode = split_bench_name(name)
            if "benchmarks" in bench:
                if filtered_modes and bench_mode not in filtered_modes and not (omp_runtime and bench_mode.startswith('OMP_RUNTIME')):
                    continue
                bench_mode = bench_mode.removeprefix('EIGEN_')
                for res in bench["benchmarks"]:
                    params = ";".join(
                        x.group()[1:] for x in re.finditer(r"\/\w+:\d+", res["name"])
                    )
                    if "Throughput" not in res["name"]:
                        benchmarks_by_type[0].setdefault(bench_type, {}).setdefault(
                            params, {}
                        )[bench_mode] = res["real_time"]
                    else:
                        benchmarks_by_type[1].setdefault(bench_type, {}).setdefault(
                            params, {}
                        )[bench_mode] = 9000000 / res["real_time"]
            else:
                benchmarks_by_type[0].setdefault(bench_type, {}).setdefault("", {})[
                    bench_mode
                ] = bench
    return benchmarks_by_type


# TODO: refactor results and replace dicts with classes
def plot_scheduling_benchmarks(scheduling_times, verbose):
    # x for thread_idx, y for time
    min_times = [
        (
            bench_type,
            [
                [
                    min([t["trace"]["execution_start"] for t in tasks])
                    for tasks in iter["tasks"].values()
                ]
                for iter in results
            ],
        )
        for bench_type, results in scheduling_times.items()
    ]
    runtimes = list(set(name.split("_")[0] for (name, _) in min_times))
    runtimes.append("all")
    plots = {runtime: plt.subplots(figsize=(18, 6)) for runtime in runtimes}
    styles = itertools.cycle(["-", "--", "-.", ":"])
    colors = itertools.cycle(COLORS)
    style = next(styles)
    color = next(colors)
    max_y = []
    for bench_type, times in sorted(min_times):
        # todo: better way to visualize?
        # min time per thread for each idx in range of thread count
        times = np.asarray(times)
        # plot distribution of all times for iterations as scatter around time
        means = np.min(times, axis=0)
        means = np.sort(means, axis=0)
        _, ax = plots[bench_type.split("_")[0]]
        ax.plot(
            range(len(means)), means, label=bench_type, linestyle=style, color=color
        )
        plots["all"][1].plot(
            range(len(means)), means, label=bench_type, linestyle=style, color=color
        )
        color = next(colors)
        if color == COLORS[0]:
            style = next(styles)
        max_y.append(np.max(means))

    # ignore most slower method
    if len(max_y) > 1:
        top_limit = sorted(max_y)[-2]
        plots["all"][1].set_ylim(top=top_limit, bottom=0)

    for runtime in runtimes:
        _, ax = plots[runtime]
        ax.ticklabel_format(style="plain")
        if verbose:
            ax.set_title("Scheduling time, " + runtime)
        # use yticks bigger and xticks fontsize
        ax.tick_params(axis="both", which="major", labelsize=20)
        ax.set_xlabel("Index of thread (sorted by time of first task)", fontsize=20)
        ax.set_ylabel("Cycles", fontsize=20)
        ax.legend(fontsize=14)
        ylimit = ax.get_ylim()
        # make ~10 ticks on y axis but on round numbers (e.x. 500, 1000) only:
        step = (ylimit[1] - ylimit[0]) / 10
        # round step to round number (e.g. if it's 700, round up to 1000)
        step = 10 ** (len(str(int(step))) - 1) * (
            int(step) // 10 ** (len(str(int(step))) - 1) + 1
        )
        # round by step to smallest:
        bottom = math.floor(ylimit[0] / step) * step
        bottom = max(bottom, 0)
        ax.set_xlim(xmin=0)
        ax.set_yticks(np.arange(bottom + step, ylimit[1], step))
        ax.yaxis.set_major_locator(plt.MaxNLocator(nbins=12))
        ax.yaxis.set_minor_locator(AutoMinorLocator(5))
        ax.xaxis.set_major_locator(plt.MaxNLocator(nbins=12))
        ax.xaxis.set_minor_locator(AutoMinorLocator(5))
        ax.grid(which='major', color='#DDDDDD', linewidth=0.8)
        ax.grid(which='minor', color='#EEEEEE', linewidth=0.5)
    return plots


def plot_scheduling_dist(scheduling_dist, verbose):
    # plot heatmap for map of thread_idx -> tasks list
    row_count = len(scheduling_dist)
    fig = plt.figure(figsize=(18, 24))
    fig.suptitle("Scheduling distribution, results for each iteration")
    fig.tight_layout()
    gs = GridSpec(row_count, 3, figure=fig, width_ratios=[1, 1, 4])

    for iter in range(row_count):
        ax = fig.add_subplot(gs[iter, 0])
        if iter == 0:
            ax.set_title("Distribution of tasks to threads")
        # ax.get_figure().tight_layout()

        thread_count = len(scheduling_dist[iter]["tasks"])
        task_count = (
            max(
                max(t["index"] for t in tasks)
                for tasks in scheduling_dist[iter]["tasks"].values()
            )
            + 1
        )
        task_height = int(task_count / thread_count)
        data = np.ones((thread_count * task_height, task_count))
        for thread_id, tasks in sorted(
            scheduling_dist[iter]["tasks"].items(), key=lambda x: x[0]
        ):
            for t in tasks:
                idx = thread_count - 1 if thread_id == "-1" else int(thread_id)
                # data[idx, t["index"]] = 0
                # fill rectangle by zeros
                data[idx * task_height:(idx + 1) * task_height, t["index"]] = 0
        ax.set_ylabel("Thread")
        ax.set_xlabel("Task")
        ax.xaxis.set_label_position("top")
        ax.imshow(data, cmap="gray", origin="lower")

        def format_task(x, _):
            return str(int(x / task_height))

        ax.yaxis.set_major_formatter(format_task)

    # plot heatmap thread id, cpu id
    for iter in range(row_count):
        ax = fig.add_subplot(gs[iter, 1])
        if iter == 0:
            ax.set_title("Distribution of threads to cpus")
        ax.set_ylabel("Thread")
        ax.set_xlabel("Cpu")
        ax.xaxis.set_label_position("top")
        # ax.get_figure().tight_layout()

        thread_count = len(scheduling_dist[iter]["tasks"].keys())
        cpu_count = len(
            set(
                t["cpu"]
                for tasks in scheduling_dist[iter]["tasks"].values()
                for t in tasks
            )
        )
        cpus = {}  # mapping cpu_id -> idx
        threads = {}  # mapping thread_id -> idx
        data = np.ones((thread_count, cpu_count))
        for thread_id, tasks in scheduling_dist[iter]["tasks"].items():
            for t in tasks:
                if thread_id not in threads:
                    threads[thread_id] = len(threads)
                cpu = t["cpu"]
                if cpu not in cpus:
                    cpus[cpu] = len(cpus)
                data[threads[thread_id], cpus[cpu]] = 0
        ax.imshow(data, cmap="gray", origin="lower")

    for iter in range(row_count):
        ax = fig.add_subplot(gs[iter, 2])
        ax.set_ylabel("Clock ticks")
        ax.set_xlabel("Thread")
        # ax.get_figure().tight_layout()
        if iter == 0:
            ax.set_title(
                "Time since start of first executed task for each thread (sorted by time)"
            )

        thread_count = len(scheduling_dist[iter]["tasks"])
        times = [
            min(t["trace"]["execution_start"] for t in tasks)
            for tasks in scheduling_dist[iter]["tasks"].values()
        ]
        times = np.sort(np.asarray(times))
        ax.plot(range(len(times)), times, label="Start time")
        times_end = [
            min(
                scheduling_dist[iter]["end"] - t["trace"]["execution_end"]
                for t in tasks
            )
            for tasks in scheduling_dist[iter]["tasks"].values()
        ]
        time_end = min(times_end)
        # times_end = np.sort(np.asarray(times_end))
        ax.plot(range(len(times)), [time_end] * len(times_end), label="End time")
        ax.legend()
    return fig


def plot_scheduling_dist_item(item, res_path, verbose):
    bench_mode, res = item
    time_before = time.time()
    fig = plot_scheduling_dist(res["results"], verbose)
    save_figure(res_path, fig, "scheduling_dist_" + bench_mode)
    plt.close()
    print(f"Plotting {bench_mode} took {time.time() - time_before}s")
    for iter in res["results"]:
        for thread_id, tasks in iter["tasks"].items():
            unique_cpus = set(t["cpu"] for t in tasks)
            if len(unique_cpus) > 1:
                print(
                    f"{bench_mode}: thread {thread_id} has tasks executed on differenet cpus: {unique_cpus}"
                )


if __name__ == "__main__":
    # fetch folder from args or use current folder
    folder_name = "raw_results"
    res_path = "bench_results"
    if not os.path.exists(res_path):
        os.makedirs(res_path)
    verbose = len(sys.argv) > 1 and sys.argv[1] == "--verbose"

    subdirs = [
        d
        for d in os.listdir(folder_name)
        if os.path.isdir(os.path.join(folder_name, d))
    ]
    # plot main benchmarks
    table = {}
    for subdir in subdirs:
        benchmarks = parse_benchmarks(os.path.join(folder_name, subdir))
        if subdir == "scheduling_dist":
            scheduling_times_by_suffix = {}
            for bench_mode, res in benchmarks[0]["scheduling_dist"][""].items():
                bench_mode, measure_mode = bench_mode.rsplit("_", 1)
                if filtered_modes and bench_mode not in filtered_modes:
                    continue
                bench_mode = bench_mode.removeprefix('EIGEN_')
                if measure_mode not in scheduling_times_by_suffix:
                    scheduling_times_by_suffix[measure_mode] = {}
                scheduling_times_by_suffix[measure_mode][bench_mode] = res["results"]

            for measure_mode, times in scheduling_times_by_suffix.items():
                print("Processing scheduling time", measure_mode)
                plots = plot_scheduling_benchmarks(times, verbose)
                for bench_type, plot in plots.items():
                    fig, ax = plot
                    current_res_path = os.path.join(res_path, "scheduling_time")
                    if not os.path.exists(current_res_path):
                        os.makedirs(current_res_path)
                    save_figure(
                        current_res_path,
                        fig,
                        "scheduling_time_" + bench_type + "_" + measure_mode,
                    )
                    plt.close()

            # plot scheduling dist

            current_res_path = os.path.join(res_path, "scheduling_dist")
            if not os.path.exists(current_res_path):
                os.makedirs(current_res_path)
            for item in benchmarks[0]["scheduling_dist"][""].items():
                plot_scheduling_dist_item(res_path=current_res_path, verbose=verbose, item=item)
        elif subdir != "trace_spin":
            current_res_path = os.path.join(res_path, subdir)
            if filtered_benchmarks and subdir not in filtered_benchmarks:
                continue
            if not os.path.exists(current_res_path):
                os.makedirs(current_res_path)
            for bench_type, bench in benchmarks[0].items():
                print("Processing", bench_type)
                fig, table_row = plot_benchmark("Latency", bench, bench_type, verbose)
                save_figure(current_res_path, fig, bench_type + "_latency")
                table = {**table, **table_row}
                plt.close()
            for bench_type, bench in benchmarks[1].items():
                print("Processing", bench_type)
                fig, _ = plot_benchmark("Throughput", bench, bench_type, verbose)
                save_figure(current_res_path, fig, bench_type + "_throughput")
                plt.close()
    # table for latencies only
    table_res = generate_md_table(table)
    print(table_res)
    with open(os.path.join(res_path, "table.md"), "w") as f:
        f.write(table_res)

