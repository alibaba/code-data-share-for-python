import os
import sys

import pyperf

PACKAGES = {
    'requests': None,
}


def add_cmdline_args(cmd, args):
    cmd.append(args.package)


if __name__ == "__main__":
    runner = pyperf.Runner(add_cmdline_args=add_cmdline_args)

    parser = runner.argparser
    parser.add_argument("package", choices=PACKAGES)

    runner.metadata['description'] = "Performance of import statement of different packages."
    args = runner.parse_args()

    package = PACKAGES[args.package] or args.package

    command = [sys.executable, "-c", f"import {package}"]

    cds_mode = None
    try:
        import cds

        cds_mode = cds._cds.flags.mode
    except ImportError:
        pass

    if cds_mode == 1:
        from subprocess import run, DEVNULL

        run(command, env={'PYCDSMODE': 'TRACE', 'PYCDSLIST': 'test.lst'}, stdout=DEVNULL, stderr=DEVNULL)
    else:
        # command[2] = f'import cds; cds.share("test.img"); ' + command[2]
        os.environ['PYCDSMODE'] = 'SHARE'
        os.environ['PYCDSARCHIVE'] = 'test.img'
        # os.environ.pop('PYCDSMODE')
        # os.environ.pop('PYCDSARCHIVE')
        runner.bench_command(f'import_{package}', command)
