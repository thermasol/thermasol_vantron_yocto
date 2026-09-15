#!/usr/bin/env python3
"""Fallback .deb builder used when dpkg-deb's internal tar invocation keeps
hitting the pseudo/tar host-compat bug (GNU tar >= 1.35 on Ubuntu 24.04 issues
*at()-family syscalls pseudo's fd-tracking loses on deep directory trees).

Builds a .deb using Python's tarfile module (which walks directories via plain
os.stat/open, not the openat-with-cached-fd pattern that breaks pseudo) plus
`ar` to combine the three members, exactly matching the standard Debian binary
package format: debian-binary, control.tar.gz, data.tar.gz in that order.

Usage matches the one dpkg-deb invocation form package_deb.bbclass ever uses:
    pydeb-build.py -b <root> <outdir>
"""
import sys
import os
import tarfile
import subprocess
import tempfile


def read_control_fields(control_path):
    fields = {}
    with open(control_path) as f:
        for line in f:
            line = line.rstrip("\n")
            if not line:
                break
            if ":" in line:
                k, v = line.split(":", 1)
                fields[k.strip()] = v.strip()
    return fields


def _truncate_mtime(tarinfo):
    # tarfile emits a PAX extended header to preserve sub-second mtime
    # precision even when format=GNU_FORMAT is requested (GNU format has no
    # way to represent fractional seconds, so tarfile falls back to a PAX
    # record rather than losing precision). Real GNU tar just silently
    # truncates to whole seconds in --format=gnu mode instead, which this
    # host's dpkg can actually parse — match that behavior explicitly rather
    # than relying on format= alone.
    tarinfo.mtime = int(tarinfo.mtime)
    return tarinfo


def make_data_filter(exclude_dir_name):
    def _filter(tarinfo):
        parts = tarinfo.name.split("/")
        if exclude_dir_name in parts:
            return None
        return _truncate_mtime(tarinfo)
    return _filter


def build_deb(root, outdir):
    control_dir = os.path.join(root, "DEBIAN")
    control = read_control_fields(os.path.join(control_dir, "control"))
    pkg = control["Package"]
    ver = control["Version"]
    arch = control["Architecture"]
    debname = "%s_%s_%s.deb" % (pkg, ver, arch)
    outpath = os.path.join(outdir, debname)

    with tempfile.TemporaryDirectory() as tmp:
        debbin_path = os.path.join(tmp, "debian-binary")
        control_tar_path = os.path.join(tmp, "control.tar.gz")
        data_tar_path = os.path.join(tmp, "data.tar.gz")

        with open(debbin_path, "w") as f:
            f.write("2.0\n")

        # format=GNU_FORMAT: tarfile defaults to PAX_FORMAT, which this host's
        # dpkg can't reliably parse on unpack ("corrupted filesystem tarfile
        # in package archive: unsupported PAX tar header type 'x'") — the same
        # reason external tar invocations are forced to --format=gnu.
        with tarfile.open(control_tar_path, "w:gz", format=tarfile.GNU_FORMAT) as tar:
            tar.add(control_dir, arcname=".", recursive=True, filter=_truncate_mtime)

        with tarfile.open(data_tar_path, "w:gz", format=tarfile.GNU_FORMAT) as tar:
            tar.add(root, arcname=".", recursive=True, filter=make_data_filter("DEBIAN"))

        os.makedirs(outdir, exist_ok=True)
        if os.path.exists(outpath):
            os.remove(outpath)
        subprocess.check_call(
            ["ar", "rc", outpath, debbin_path, control_tar_path, data_tar_path]
        )


def main(argv):
    if len(argv) != 4 or argv[1] != "-b":
        sys.stderr.write("usage: pydeb-build.py -b <root> <outdir>\n")
        return 1
    build_deb(argv[2], argv[3])
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
