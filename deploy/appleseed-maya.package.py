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

from __future__ import print_function
from distutils import archive_util, dir_util
from xml.etree.ElementTree import ElementTree
import glob
import os
import platform
import shutil
import stat
import subprocess
import sys
import time
import traceback
import urllib


#--------------------------------------------------------------------------------------------------
# Constants.
#--------------------------------------------------------------------------------------------------

VERSION = "1.1.2"
SETTINGS_FILENAME = "appleseed-maya.package.configuration.xml"


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


def exe(filepath):
    return filepath + ".exe" if os.name == "nt" else filepath


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


def safe_delete_directory(path):
    Attempts = 10
    for attempt in range(Attempts):
        try:
            if os.path.exists(path):
                shutil.rmtree(path, onerror=on_rmtree_error)
            return
        except OSError:
            if attempt < Attempts - 1:
                time.sleep(0.5)
            else:
                fatal("Failed to delete directory '" + path + "'")


def safe_make_directory(path):
    if not os.path.isdir(path):
        os.makedirs(path)


def pushd(path):
    old_path = os.getcwd()
    os.chdir(path)
    return old_path


def run_subprocess(cmdline):
    p = subprocess.Popen(cmdline, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    return p.returncode, out, err


def copy_glob(input_pattern, output_path):
    for input_file in glob.glob(input_pattern):
        shutil.copy(input_file, output_path)


#--------------------------------------------------------------------------------------------------
# Settings.
#--------------------------------------------------------------------------------------------------

class Settings:

    def load(self):
        self.this_dir = os.path.dirname(os.path.realpath(__file__))
        self.root_dir = os.path.join(self.this_dir, "..")

        print("Loading settings from " + SETTINGS_FILENAME + "...")
        tree = ElementTree()
        try:
            tree.parse(SETTINGS_FILENAME)
        except IOError:
            fatal("Failed to load configuration file '" + SETTINGS_FILENAME + "'")
        self.__load_values(tree)

        self.maya_version = self.__get_maya_version()
        if self.maya_version is None:
            fatal("Failed to determine Maya version from CMakeCache.txt file")

        self.plugin_version = self.__get_plugin_version()
        if self.plugin_version is None:
            fatal("Failed to determine appleseed-maya version from version.h file")

        self.print_summary()

    def __load_values(self, tree):
        self.platform = self.__get_required(tree, "platform")
        self.build_path = self.__get_required(tree, "build_path")
        self.bin_path = self.__get_required(tree, "bin_path")
        self.appleseed_bin_path = self.__get_required(tree, "appleseed_bin_path")
        self.appleseed_lib_path = self.__get_required(tree, "appleseed_lib_path")
        self.appleseed_shaders_path = self.__get_required(tree, "appleseed_shaders_path")
        self.appleseed_schemas_path = self.__get_required(tree, "appleseed_schemas_path")
        self.appleseed_settings_path = self.__get_required(tree, "appleseed_settings_path")
        self.appleseed_python_path = self.__get_required(tree, "appleseed_python_path")
        self.maketx_path = self.__get_required(tree, "maketx_path")
        self.package_output_path = self.__get_required(tree, "package_output_path")

    def __get_required(self, tree, key):
        value = tree.findtext(key)
        if value is None:
            fatal("Missing value \"{0}\" in configuration file".format(key))
        return value

    def __get_maya_version(self):
        # Find Maya include dir from CMake's cache.
        maya_include_dir = None
        with open(os.path.join(self.build_path, "CMakeCache.txt"), "r") as f:
            for line in f.readlines():
                if line.startswith("MAYA_INCLUDE_DIR:PATH="):
                    maya_include_dir = line.split("=")[1].strip()
                    break
        if maya_include_dir is None:
            return None

        # Find Maya version from Maya's MTypes.h header.
        with open(os.path.join(maya_include_dir, "maya", "MTypes.h"), "r") as f:
            for line in f.readlines():
                if "#define" in line:
                    if "MAYA_API_VERSION" in line:
                        tokens = line.split()
                        return tokens[-1][:4]
        return None

    def __get_plugin_version(self):
        major = -1
        minor = -1
        patch = -1
        maturity = None

        # Find the Maya include dir from CMake's cache.
        with open(os.path.join(self.root_dir, "src", "appleseedmaya", "version.h"), "r") as f:
            for line in f.readlines():
                if line.startswith("#define APPLESEED_MAYA_VERSION_"):
                    tokens = line.split()
                    if tokens[1] == "APPLESEED_MAYA_VERSION_MAJOR":
                        major = int(tokens[2])
                    elif tokens[1] == "APPLESEED_MAYA_VERSION_MINOR":
                        minor = int(tokens[2])
                    elif tokens[1] == "APPLESEED_MAYA_VERSION_PATCH":
                        patch = int(tokens[2])
                    elif tokens[1] == "APPLESEED_MAYA_VERSION_MATURITY":
                        maturity = tokens[2].strip("\"")

        if major == -1 or minor == -1 or patch == -1 or maturity == None:
            return None

        return "{0}.{1}.{2}-{3}".format(major, minor, patch, maturity)

    def print_summary(self):
        print("")
        print("  Platform:                        " + self.platform)
        print("  Maya version:                    " + self.maya_version)
        print("  appleseed-maya version:          " + self.plugin_version)
        print("  Build path:                      " + self.build_path)
        print("  Path to appleseed-maya binaries: " + self.bin_path)
        print("  Path to appleseed binaries:      " + self.appleseed_bin_path)
        print("  Path to appleseed libraries:     " + self.appleseed_lib_path)
        print("  Path to appleseed shaders:       " + self.appleseed_shaders_path)
        print("  Path to appleseed schemas:       " + self.appleseed_schemas_path)
        print("  Path to appleseed settings:      " + self.appleseed_settings_path)
        print("  Path to appleseed.python:        " + self.appleseed_python_path)
        print("  Path to maketx:                  " + self.maketx_path)
        print("  Output directory:                " + self.package_output_path)
        print("")


#--------------------------------------------------------------------------------------------------
# Base package builder.
#--------------------------------------------------------------------------------------------------

class PackageBuilder(object):

    def __init__(self, settings):
        self.settings = settings

    def build_package(self):
        print("Building package:")
        print("")
        self.orchestrate()
        print("")
        print("The package was successfully built.")

    def orchestrate(self):
        progress("Removing leftovers from previous invocations")
        safe_delete_directory(self.settings.package_output_path)

        progress("Creating deployment directory")
        safe_make_directory(self.settings.package_output_path)

        progress("Copying license")
        shutil.copy(os.path.join(self.settings.root_dir, "LICENSE.txt"), self.settings.package_output_path)

        progress("Copying icons")
        dir_util.copy_tree(os.path.join(self.settings.root_dir, "icons"), os.path.join(self.settings.package_output_path, "icons"))

        progress("Copying presets")
        dir_util.copy_tree(os.path.join(self.settings.root_dir, "presets"), os.path.join(self.settings.package_output_path, "presets"))

        progress("Copying resources")
        dir_util.copy_tree(os.path.join(self.settings.root_dir, "resources"), os.path.join(self.settings.package_output_path, "resources"))

        progress("Copying scripts")
        dir_util.copy_tree(os.path.join(self.settings.root_dir, "scripts"), os.path.join(self.settings.package_output_path, "scripts"))

        progress("Copying appleseed.python")
        dir_util.copy_tree(os.path.expandvars(self.settings.appleseed_python_path), os.path.join(self.settings.package_output_path, "scripts"))

        progress("Removing pyc files")
        for root, dirs, files in os.walk(os.path.join(self.settings.package_output_path, "scripts")):
            for f in files:
                if f.endswith(".pyc"):
                    safe_delete_file(os.path.join(root, f))

        progress("Generating module file")
        self.generate_module_file()

        progress("Copying binaries")
        self.copy_binaries()

        progress("Copying schemas")
        dir_util.copy_tree(os.path.expandvars(self.settings.appleseed_schemas_path), os.path.join(self.settings.package_output_path, "schemas"))
        safe_delete_file(os.path.join(self.settings.package_output_path, "schemas", ".gitignore"))

        progress("Downloading settings files")
        self.download_settings()

        progress("Copying shaders")
        self.copy_shaders()

        progress("Copying plugins")
        self.copy_plugins()

        progress("Copying dependencies")
        self.copy_dependencies()

        progress("Post-processing package")
        self.post_process_package()

        progress("Building final zip file")
        self.build_final_zip_file()

    def do_generate_module_file(self, maya_platform_id):
        with open(os.path.join(self.settings.package_output_path, "appleseed-maya.mod"), "w") as f:
            f.write("+ MAYAVERSION:{0} PLATFORM:{1} appleseed-maya {2} .\n".format(
                self.settings.maya_version,
                maya_platform_id,
                self.settings.plugin_version))
            f.write("plug-ins: plug-ins/{0}\n".format(self.settings.maya_version))
            f.write("PATH +:= bin\n")
            f.write("PYTHONPATH +:= scripts\n")
            f.write("APPLESEED_SEARCHPATH +:= shaders\n")
            f.write("MAYA_PRESET_PATH +:= presets\n")
            f.write("MAYA_CUSTOM_TEMPLATE_PATH +:= scripts/appleseedMaya/AETemplates\n")
            f.write("MAYA_SHELF_PATH +:= prefs/shelves\n")
            f.write("XBMLANGPATH +:= icons/%B\n")

    def copy_binaries(self):
        bin_dir = os.path.join(self.settings.package_output_path, "bin")
        safe_make_directory(bin_dir)

        binaries_to_copy = [exe("appleseed.cli")]
        for bin in binaries_to_copy:
            shutil.copy(os.path.join(os.path.expandvars(self.settings.appleseed_bin_path), bin), bin_dir)

        shutil.copy(os.path.expandvars(self.settings.maketx_path), bin_dir)

    def download_settings(self):
        settings_dir = os.path.join(self.settings.package_output_path, "settings")
        safe_make_directory(settings_dir)

        settings_to_download = ["appleseed.cli.xml"]
        for file in settings_to_download:
            urllib.urlretrieve(
                "https://raw.githubusercontent.com/appleseedhq/appleseed/master/sandbox/settings/{0}".format(file),
                os.path.join(settings_dir, file))

    def copy_shaders(self):
        shaders_dir = os.path.join(self.settings.package_output_path, "shaders")
        safe_make_directory(shaders_dir)

        for shader in glob.glob(os.path.join(os.path.expandvars(self.settings.appleseed_shaders_path), "maya", "*.oso")):
            shutil.copy(shader, shaders_dir)

        for root, dirs, files in os.walk(os.path.join(os.path.expandvars(self.settings.appleseed_shaders_path), "appleseed")):
            for f in files:
                if f.endswith(".oso"):
                    shutil.copy(os.path.join(root, f), shaders_dir)

    def copy_plugins(self):
        plugin_ext = self.plugin_extension()

        plugins_dir = os.path.join(self.settings.package_output_path, "plug-ins", self.settings.maya_version)
        safe_make_directory(plugins_dir)

        shutil.copy(
            os.path.join(self.settings.bin_path, "appleseedMaya" + plugin_ext),
            plugins_dir)

    def build_final_zip_file(self):
        package_name = "appleseed-maya{0}-{1}-{2}".format(
            self.settings.maya_version,
            self.settings.plugin_version,
            self.settings.platform)

        old_path = pushd(self.settings.package_output_path)
        archive_util.make_zipfile(os.path.join(self.settings.this_dir, package_name), ".")
        os.chdir(old_path)

    def run(self, cmdline):
        info("Running command line: {0}".format(cmdline))
        os.system(cmdline)


#--------------------------------------------------------------------------------------------------
# Linux package builder.
#--------------------------------------------------------------------------------------------------

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
        return ".so"

    def generate_module_file(self):
        self.do_generate_module_file("linux")

    def copy_dependencies(self):
        # Create the lib directory.
        lib_dir = os.path.join(self.settings.package_output_path, "lib")
        safe_make_directory(lib_dir)

        # Copy appleseed libraries.
        libraries_to_copy = ["libappleseed.so", "libappleseed.shared.so"]
        for lib in libraries_to_copy:
            shutil.copy(os.path.join(os.path.expandvars(self.settings.appleseed_lib_path), lib), lib_dir)

        # Get shared libs needed by binaries.
        all_libs = set()
        for bin in glob.glob(os.path.join(self.settings.package_output_path, "bin", "*")):
            libs = self.__get_dependencies_for_file(bin)
            all_libs = all_libs.union(libs)

        # Get shared libs needed by appleseed.python.
        libs = self.__get_dependencies_for_file(
            os.path.join(self.settings.package_output_path, "scripts", "appleseed", "_appleseedpython.so"))
        all_libs = all_libs.union(libs)

        # Get shared libs needed by libraries.
        lib_libs = set()
        for lib in all_libs:
            libs = self.__get_dependencies_for_file(lib)
            lib_libs = lib_libs.union(libs)

        all_libs = all_libs.union(lib_libs)

        # Copy all shared libraries.
        for lib in all_libs:
            shutil.copy(lib, lib_dir)

    def post_process_package(self):
        for bin in glob.glob(os.path.join(self.settings.package_output_path, "bin", "*")):
            self.run("chrpath -r \$ORIGIN/../lib " + bin)

        # for lib in glob.glob(os.path.join(self.settings.package_output_path, "lib", "*")):
        #     self.run("chrpath -d " + lib)

        plugins_dir = os.path.join(self.settings.package_output_path, "plug-ins", self.settings.maya_version)
        for plugin in glob.glob(os.path.join(plugins_dir, "*.so")):
            self.run("chrpath -r \$ORIGIN/../../lib " + plugin)

        appleseed_python_dir = os.path.join(self.settings.package_output_path, "scripts", "appleseed")
        for py_cpp_module in glob.glob(os.path.join(appleseed_python_dir, "*.so")):
            self.run("chrpath -r \$ORIGIN/../../lib " + py_cpp_module)

    def __is_system_lib(self, lib):
        for prefix in self.SYSTEM_LIBS_PREFIXES:
            if lib.startswith(prefix):
                return True
        return False

    def __get_dependencies_for_file(self, filename):
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
            if self.__is_system_lib(line):
                continue

            # Ignore appleseed libs.
            if "libappleseed" in line:
                continue

            libs.add(line.split()[2])

        return libs


#--------------------------------------------------------------------------------------------------
# Windows package builder.
#--------------------------------------------------------------------------------------------------

class WindowsPackageBuilder(PackageBuilder):

    def plugin_extension(self):
        return ".mll"

    def generate_module_file(self):
        self.do_generate_module_file("win64")

    def copy_dependencies(self):
        bin_dir = os.path.join(self.settings.package_output_path, "bin")

        dlls_to_copy = ["appleseed.dll", "appleseed.shared.dll"]
        for dll in dlls_to_copy:
            shutil.copy(os.path.join(self.settings.appleseed_bin_path, dll), bin_dir)

    def post_process_package(self):
        pass


#--------------------------------------------------------------------------------------------------
# Entry point.
#--------------------------------------------------------------------------------------------------

def main():
    print("appleseed-maya.package version " + VERSION)
    print("")

    settings = Settings()
    settings.load()

    if os.name == "nt":
        package_builder = WindowsPackageBuilder(settings)
    elif os.name == "posix" and platform.mac_ver()[0] == "":
        package_builder = LinuxPackageBuilder(settings)
    else:
        fatal("Unsupported platform: " + os.name)

    package_builder.build_package()

if __name__ == "__main__":
    main()
