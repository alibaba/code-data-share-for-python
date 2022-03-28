import pyperf
import json


def bench_module_load_attr(loops):
    range_it = range(loops * 100000)
    t0 = pyperf.perf_counter()

    for _ in range_it:
        # 20 loads each
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder
        json.loads
        json.JSONDecoder

    dt = pyperf.perf_counter() - t0
    return dt


def main():
    runner = pyperf.Runner()
    runner.metadata['description'] = "Access attribute/method of a module"

    cds_mode = None
    try:
        import cds

        cds_mode = cds._cds.flags.mode
    except ImportError:
        pass

    if cds_mode == 1:
        bench_module_load_attr(1)
    else:
        runner.bench_time_func('module_load_attr', bench_module_load_attr)


if __name__ == "__main__":
    main()
