#!/usr/bin/python

#
# This source file is part of appleseed.
# Visit http://appleseedhq.net/ for additional information and resources.
#
# This software is released under the MIT license.
#
# Copyright (c) 2017 Esteban Tovagliari, The appleseedhq Organization
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the 'Software'), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

import argparse
from distutils.dir_util import copy_tree
import glob
import os
import platform
import shutil
import subprocess
import sys


#--------------------------------------------------------------------------------------------------
# Constants.
#--------------------------------------------------------------------------------------------------

VERSION = "0.1.0"


#--------------------------------------------------------------------------------------------------
# Utility functions.
#--------------------------------------------------------------------------------------------------

def info(message):
    print("  " + message)

def progress(message):
    print("  " + message + "...")

def fatal(message):
    print("Fatal: " + message + ". Aborting.")
    if sys.exc_info()[0]:
        print(traceback.format_exc())
    sys.exit(1)

def copy_glob(input_pattern, output_path):
    for input_file in glob.glob(input_pattern):
        shutil.copy(input_file, output_path)

def safe_delete_file(path):
    try:
        if os.path.exists(path):
            os.remove(path)
    except OSError:
        fatal("Failed to delete file '" + path + "'")

def on_rmtree_error(func, path, exc_info):
    # path contains the path of the file that couldn't be removed.
    # Let's just assume that it's read-only and unlink it.
    os.chmod(path, stat.S_IWRITE)
    os.unlink(path)

def run_subprocess(cmdline):
    p = subprocess.Popen(cmdline, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    return p.returncode, out, err

def get_maya_version(build_dir):
    maya_include_dir = None

    # Find the Maya include dir from CMake's cache.
    f = open(os.path.join(build_dir, 'CMakeCache.txt'), 'r')
    lines = f.readlines()
    f.close()

    token = 'MAYA_INCLUDE_DIR:PATH=/usr/autodesk/maya2017/include'
    for line in lines:
        if line.startswith(token):
            maya_include_dir = line.split('=')[1].strip()
            break

    # Find the Maya version from Maya's MTypes.h header.
    f = open(os.path.join(maya_include_dir, 'maya', 'MTypes.h'), 'r')
    lines = f.readlines()
    f.close()

    for line in lines:
        if '#define' in line:
            if 'MAYA_API_VERSION' in line:
                tokens = line.split()
                return tokens[-1][:4]

def get_appleseed_maya_version():
    this_dir = os.path.dirname(os.path.realpath(__file__))
    src_dir = os.path.join(this_dir, '..', 'src', 'appleseedmaya')

    # Find the Maya include dir from CMake's cache.
    f = open(os.path.join(src_dir, 'version.h'), 'r')
    lines = f.readlines()
    f.close()

    major = -1
    minor = -1
    patch = -1

    for line in lines:
        if line.startswith('#define APPLESEED_MAYA_VERSION_'):
            tokens = line.split()
            if tokens[1] == 'APPLESEED_MAYA_VERSION_MAJOR':
                major = int(tokens[2])
            elif tokens[1] == 'APPLESEED_MAYA_VERSION_MINOR':
                minor = int(tokens[2])
            elif tokens[1] == 'APPLESEED_MAYA_VERSION_PATCH':
                patch = int(tokens[2])

    return (major, minor, patch)

#############################

class PackageBuilder(object):
    def __init__(self, args):
        self.directory = args.directory
        self.build_dir = args.build_dir
        self.appleseed_dir = args.appleseed_dir

        (self.major_version, self.minor_version, self.patch_version) = get_appleseed_maya_version()
        self.maya_version = get_maya_version(self.build_dir)

    def copy_binaries(self):
        bin_dir = os.path.join(self.directory, 'bin')
        if not os.path.exists(bin_dir):
            os.makedirs(bin_dir)

        binaries_to_copy = ['appleseed.cli', 'maketx']
        for bin in binaries_to_copy:
            shutil.copy(os.path.join(self.appleseed_dir, 'bin', bin), bin_dir)

    def copy_shaders(self):
        shaders_dir = os.path.join(self.directory, 'shaders')
        if not os.path.exists(shaders_dir):
            os.makedirs(shaders_dir)

        for shader in glob.glob(os.path.join(self.appleseed_dir, 'shaders', 'maya', '*.oso')):
            shutil.copy(shader, shaders_dir)

        for root, dirs, files in os.walk(os.path.join(self.appleseed_dir, 'shaders', 'appleseed')):
            for f in files:
                if f.endswith('.oso'):
                    shutil.copy(os.path.join(root, f), shaders_dir)

    def copy_plugins(self):
        plugin_ext = self.plugin_extension()

        plugins_dir = os.path.join(self.directory, 'plug-ins')
        if not os.path.exists(plugins_dir):
            os.makedirs(plugins_dir)

        shutil.copy(
            os.path.join(self.build_dir, 'src', 'appleseedmaya', 'appleseedMaya' + plugin_ext),
            plugins_dir
        )

    def build_package(self):
        info('Deploying appleseedMaya %s.%s.%s to %s...' % (
            self.major_version,
            self.minor_version,
            self.patch_version,
            self.directory
            ))

        info('Maya version = %s' % self.maya_version)

        # Remove old deploy directory
        #try:
        #    shutil.rmtree(args.directory, onerror=remove_readonly)
        #except Exception as e:
        #    print "Failed to remove previous deploy, error = ", str(e)
        #    sys.exit(0)

        info('Creating deploy dir: %s' % self.directory)
        if not os.path.exists(self.directory):
            os.makedirs(self.directory)

        this_dir = os.path.dirname(os.path.realpath(__file__))
        root_dir = os.path.join(this_dir, '..')

        info('Copying license...')
        shutil.copy(os.path.join(this_dir, '..', 'LICENSE.txt'), self.directory)

        info('Copying icons...')
        copy_tree(os.path.join(root_dir, 'icons'), os.path.join(self.directory, 'icons'))

        info('Copying presets...')
        copy_tree(os.path.join(root_dir, 'presets'), os.path.join(self.directory, 'presets'))

        info('Copying resources...')
        copy_tree(os.path.join(root_dir, 'resources'), os.path.join(self.directory, 'resources'))

        info('Copying scripts...')
        copy_tree(os.path.join(root_dir, 'scripts'), os.path.join(self.directory, 'scripts'))

        info('Copying appleseed.python...')
        copy_tree(os.path.join(self.appleseed_dir, 'lib', 'python2.7'), os.path.join(self.directory, 'scripts'))

        info('Removing pyc files...')
        for root, dirs, files in os.walk(os.path.join(self.directory, 'scripts')):
            for f in files:
                if f.endswith('.pyc'):
                    os.remove(os.path.join(root, f))

        info('Generating module file...')
        self.generate_module_file()

        info('Copying binaries...')
        self.copy_binaries()

        info('Copying schemas...')
        copy_tree(os.path.join(self.appleseed_dir, 'schemas'), os.path.join(self.directory, 'schemas'))

        info('Copying settings...')
        copy_tree(os.path.join(self.appleseed_dir, 'settings'), os.path.join(self.directory, 'settings'))

        info('Copying shaders...')
        self.copy_shaders()

        info('Copying plugins...')
        self.copy_plugins()

        info('Copying dependencies...')
        self.copy_dependencies()


class WindowsPackageBuilder(PackageBuilder):

    def plugin_extension(self):
        return '.mll'

    def generate_module_file(self):
        raise NotImplementedError()

    def copy_dependencies(self):
        raise NotImplementedError()

class LinuxPackageBuilder(PackageBuilder):

    SYSTEM_LIBS_PREFIXES = [
        "linux",
        "librt",
        "libpthread",
        "libGL",
        "libX",
        "libselinux",
        "libICE",
        "libSM",
        "libdl",
        "libm.so",
        "libgcc",
        "libc.so",
        "/lib64/ld-linux-",
        "libstdc++",
        "libxcb",
        "libdrm",
        "libnsl",
        "libuuid",
        "libgthread",
        "libglib",
        "libgobject",
        "libglapi",
        "libffi",
        "libfontconfig",
        "libutil",
        "libpython",
        "libxshmfence.so"
    ]

    def plugin_extension(self):
        return '.so'

    def generate_module_file(self):
        with open(os.path.join(self.directory, 'appleseedMaya.mod'), 'w') as f:
            f.write('+ appleseedMaya any .\n')
            f.write('PATH +:= bin\n\n')
            f.write('LD_LIBRARY_PATH +:= lib\n\n')
            f.write('PYTHONPATH +:= scripts\n\n')
            f.write('APPLESEED_SEARCHPATH := shaders\n')

    def copy_dependencies(self):
        # Create the lib directory.
        lib_dir = os.path.join(self.directory, 'lib')
        if not os.path.exists(lib_dir):
            os.makedirs(lib_dir)

        # Copy appleseed libraries.
        libraries_to_copy = ['libappleseed.so', 'libappleseed.shared.so']
        for lib in libraries_to_copy:
            shutil.copy(os.path.join(self.appleseed_dir, 'lib', lib), lib_dir)

        # Get shared libs needed by binaries.
        all_libs = set()
        for bin in glob.glob(os.path.join(self.directory, 'bin', '*')):
            libs = self.get_dependencies_for_file(bin)
            all_libs = all_libs.union(libs)

        # Get shared libs needed by appleseed.python
        libs = self.get_dependencies_for_file(
            os.path.join(self.directory, 'scripts', 'appleseed', '_appleseedpython.so'))
        all_libs = all_libs.union(libs)

        # Get shared libs needed by libraries.
        lib_libs = set()
        for lib in all_libs:
            libs = self.get_dependencies_for_file(lib)
            lib_libs = lib_libs.union(libs)

        all_libs = all_libs.union(lib_libs)

        # Copy all shared libraries.
        for lib in all_libs:
            shutil.copy(lib, lib_dir)

    def is_system_lib(self, lib):
        for prefix in self.SYSTEM_LIBS_PREFIXES:
            if lib.startswith(prefix):
                return True
        return False

    def get_dependencies_for_file(self, filename):
        returncode, out, err = run_subprocess(["ldd", filename])
        if returncode != 0:
            fatal("Failed to invoke ldd(1) to get dependencies for {0}: {1}".format(filename, err))

        libs = set()

        for line in out.split("\n"):
            line = line.strip()

            # Ignore empty lines.
            if len(line) == 0:
                continue

            # Ignore system libs.
            if self.is_system_lib(line):
                continue

            # Ignore appleseed libs.
            if 'appleseed' in line:
                continue

            libs.add(line.split()[2])

        return libs

#--------------------------------------------------------------------------------------------------
# Entry point.
#--------------------------------------------------------------------------------------------------


def main():
    print("appleseed.maya deploy version " + VERSION)

    parser = argparse.ArgumentParser(description='Deploy Maya plugin')

    parser.add_argument('-b', '--build-dir', metavar='build-dir', help='set the path to the build directory')
    parser.add_argument('-a', '--appleseed-dir', metavar='appleseed-dir', help='set the path to the appleseed directory')
    parser.add_argument('directory', help='destination directory')
    args = parser.parse_args()

    if os.name == "nt":
        package_builder = WindowsPackageBuilder(args)
    elif os.name == "posix" and platform.mac_ver()[0] == "":
        package_builder = LinuxPackageBuilder(args)
    else:
        fatal("Unsupported platform: " + os.name)

    package_builder.build_package()

if __name__ == '__main__':
    main()
