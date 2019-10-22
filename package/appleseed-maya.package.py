#!/usr/bin/python

#
# This source file is part of appleseed.
# Visit https://appleseedhq.net/ for additional information and resources.
#
# This software is released under the MIT license.
#
# Copyright (c) 2017-2018 Esteban Tovagliari, The appleseedhq Organization
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
import re
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

VERSION = "1.1.3"
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

        progress("Copying renderDesc")
        dir_util.copy_tree(os.path.join(self.settings.root_dir, "renderDesc"), os.path.join(self.settings.package_output_path, "renderDesc"))

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
            f.write("MAYA_RENDER_DESC_PATH +:= renderDesc\n")
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


# -------------------------------------------------------------------------------------------------
# Mac package builder.
# -------------------------------------------------------------------------------------------------

class MacPackageBuilder(PackageBuilder):

    SYSTEM_LIBS_PREFIXES = [
        "/System/Library/",
        "/usr/lib/libcurl",
        "/usr/lib/libc++",
        "/usr/lib/libbz2",
        "/usr/lib/libiconv",
        "/usr/lib/libSystem",
        "/usr/lib/libxml",
        "/usr/lib/libexpat",
        "/usr/lib/libz",
        "/usr/lib/libncurses",
        "/usr/lib/libobjc.A.dylib"
    ]

    def plugin_extension(self):
        return ".bundle"

    def generate_module_file(self):
        self.do_generate_module_file("mac")

    def copy_dependencies(self):
        progress("Mac-specific: Copying dependencies")

        # Create destination directory.
        lib_dir = os.path.join(self.settings.package_output_path, "lib")
        safe_make_directory(lib_dir)

        # Copy appleseed libraries.
        for lib in ["libappleseed.dylib", "libappleseed.shared.dylib"]:
            shutil.copy(os.path.join(os.path.expandvars(self.settings.appleseed_lib_path), lib), lib_dir)

        libs_to_check = set()
        all_libs = set()

        for bin in glob.glob(os.path.join(os.path.expandvars(self.settings.root_dir), "appleseed", "bin", "*")):
            libs = self.__get_dependencies_for_file(bin)
            libs_to_check = libs_to_check.union(libs)

        appleseedpython_libs = self.__get_dependencies_for_file(
                    os.path.join(self.settings.package_output_path, "scripts", "appleseed", "_appleseedpython.so"))
        libs_to_check = libs_to_check.union(appleseedpython_libs)

        # find all dependencies
        while libs_to_check:
            current_lib = libs_to_check.pop()
            for lib in self.__get_dependencies_for_file(current_lib):
                if lib not in all_libs:
                    libs_to_check.add(lib)

            all_libs.add(current_lib)

        if True:
            # Print dependencies.
            info("    Dependencies:")
            for lib in all_libs:
                info("      {0}".format(lib))

        # Copy needed libs to lib directory.
        for lib in all_libs:

            # Don't copy Python, this dependency will be relinked to
            # Maya's Python later in the packaging process
            if os.path.basename(lib) == "Python":
                continue

            if True:
                info("  Copying {0} to {1}...".format(lib, lib_dir))
            dest = os.path.join(lib_dir, os.path.basename(lib))

            # It is possible for the dylib to link to same lib in multiple paths
            # check that is not already copied
            if not os.path.exists(dest):
                shutil.copy(lib, lib_dir)

    def post_process_package(self):
        progress("Mac-specific: Post-processing package")
        self.__fixup_binaries()

    def __fixup_binaries(self):
        progress("Mac-specific: Fixing up binaries")
        self.__fix_permissions();
        self.set_libraries_ids()
        self.__change_library_paths_in_libraries()
        self.__change_library_paths_in_executables()

    def __fix_permissions(self):
        lib_dir = os.path.join(self.settings.package_output_path, "lib")
        for dirpath, dirnames, filenames in os.walk(lib_dir):
            for filename in filenames:
                ext = os.path.splitext(filename)[1]
                if ext == ".dylib" or ext == ".so":
                    lib_path = os.path.join(dirpath, filename)
                    self.run("chmod 777 {}".format(lib_path))

        bin_dir =  os.path.join(self.settings.package_output_path, "bin")
        for dirpath, dirnames, filenames in os.walk(bin_dir):
            for filename in filenames:
                ext = os.path.splitext(filename)[1]
                if ext != ".py" and ext != ".conf":
                    exe_path = os.path.join(dirpath, filename)
                    self.run("chmod 777 {}".format(exe_path))

    def set_libraries_ids(self):
        lib_dir = os.path.join(self.settings.package_output_path, "lib")
        for dirpath, dirnames, filenames in os.walk(lib_dir):
            for filename in filenames:
                ext = os.path.splitext(filename)[1]
                if ext == ".dylib" or ext == ".so":
                    lib_path = os.path.join(dirpath, filename)
                    self.__set_library_id(lib_path, filename)

        plugins_dir = os.path.join(self.settings.package_output_path, "plug-ins", self.settings.maya_version)
        for plugin_path in glob.glob(os.path.join(plugins_dir, "*.bundle")):
            filename = os.path.basename(plugin_path)
            self.__set_library_id(plugin_path, filename)

        python_plugin = os.path.join(self.settings.package_output_path, "scripts", "appleseed")
        for plugin_path in glob.glob(os.path.join(python_plugin, "*.so")):
            filename = os.path.basename(plugin_path)
            self.__set_library_id(plugin_path, filename)

    def __change_library_paths_in_libraries(self):
        lib_dir = os.path.join(self.settings.package_output_path, "lib")
        for dirpath, dirnames, filenames in os.walk(lib_dir):
            for filename in filenames:
                ext = os.path.splitext(filename)[1]
                if ext == ".dylib" or ext == ".so":
                    lib_path = os.path.join(dirpath, filename)
                    self.__change_library_paths_in_binary(lib_path)

        plugins_dir = os.path.join(self.settings.package_output_path, "plug-ins", self.settings.maya_version)
        for plugin_path in glob.glob(os.path.join(plugins_dir, "*.bundle")):
            filename = os.path.basename(plugin_path)
            self.__change_library_paths_in_binary(plugin_path)

        python_plugin = os.path.join(self.settings.package_output_path, "scripts", "appleseed")
        for plugin_path in glob.glob(os.path.join(python_plugin, "*.so")):
            filename = os.path.basename(plugin_path)
            self.__change_library_paths_in_binary(plugin_path)

    def __change_library_paths_in_executables(self):
        bin_dir =  os.path.join(self.settings.package_output_path, "bin")
        for dirpath, dirnames, filenames in os.walk(bin_dir):
            for filename in filenames:
                ext = os.path.splitext(filename)[1]
                if ext != ".py" and ext != ".conf":
                    exe_path = os.path.join(dirpath, filename)
                    self.__change_library_paths_in_binary(exe_path)

    # Can be used on executables and dynamic libraries.
    def __change_library_paths_in_binary(self, bin_path):
        progress("Patching {0}".format(bin_path))
        bin_dir = os.path.dirname(bin_path)
        lib_dir = os.path.join(self.settings.package_output_path, "lib")

        path_to_appleseed_lib = os.path.relpath(lib_dir, bin_dir)
        # fix_paths set to False because we must retrieve the unmodified dependency in order to replace it by the correct one.
        for lib_path in self.__get_dependencies_for_file(bin_path, fix_paths=False):
            lib_name = os.path.basename(lib_path)

            # relink python dependencies to maya's embed python
            if lib_name == "Python":
                maya_python = "@executable_path/../Frameworks/Python.framework/Versions/2.7/Python"
                self.__change_library_path(bin_path, lib_path,  maya_python)

            elif path_to_appleseed_lib == ".":
                self.__change_library_path(bin_path, lib_path, "@loader_path/{0}".format(lib_name))
            else:
                self.__change_library_path(bin_path, lib_path, "@loader_path/{0}/{1}".format(path_to_appleseed_lib, lib_name))

    def __set_library_id(self, target, name):
        self.run('install_name_tool -id "{0}" {1}'.format(name, target))

    def __change_library_path(self, target, old, new):
        self.run('install_name_tool -change "{0}" "{1}" {2}'.format(old, new, target))

    def __get_lib_search_paths(self, filepath):
        returncode, out, err = run_subprocess(["otool", "-l", filepath])
        if returncode != 0:
            fatal("Failed to invoke otool(1) to get rpath for {0}: {1}".format(filepath, err))

        lc_path_found = False

        rpaths = []
        # parse otool output for rpaths, there can be multiple
        for line in out.split("\n"):
            line = line.strip()

            if lc_path_found and line.startswith("path"):
                path_split = line.split(' ')
                if len(path_split) < 2:
                    fatal("Failed to parse line from otool(1) output: " + line)
                rpaths.append(path_split[1])
                lc_path_found = False

            if line == "cmd LC_RPATH":
                lc_path_found = True

        DYLD_LIBRARY_PATH = os.environ.get("DYLD_LIBRARY_PATH", "").split(":")
        DYLD_FALLBACK_LIBRARY_PATH = os.environ.get("DYLD_FALLBACK_LIBRARY_PATH", "").split(":")

        search_paths = []
        # DYLD_LIBRARY_PATH overrides rpaths
        for path in DYLD_LIBRARY_PATH:
            if os.path.exists(path):
                search_paths.append(path)

        for path in rpaths:
            if os.path.exists(path):
                search_paths.append(path)

        for path in DYLD_FALLBACK_LIBRARY_PATH:
            if os.path.exists(path):
                search_paths.append(path)

        return search_paths

    def __get_dependencies_for_file(self, filepath, fix_paths=True):
        filename = os.path.basename(filepath)

        search_paths = self.__get_lib_search_paths(filepath)
        loader_path = os.path.dirname(filepath)

        if True:
            info("Gathering dependencies for file")
            info("    {0}".format(filepath))
            info("with @loader_path set to")
            info("    {0}".format(loader_path))
            info("and @rpath search path to:")
            for path in search_paths:
                info("    {0}".format(path))

        returncode, out, err = run_subprocess(["otool", "-L", filepath])
        if returncode != 0:
            fatal("Failed to invoke otool(1) to get dependencies for {0}: {1}".format(filepath, err))

        libs = set()

        for line in out.split("\n")[1:]:    # skip the first line
            line = line.strip()

            # Ignore empty lines.
            if len(line) == 0:
                continue

            if line.startswith("@executable_path"):
                continue

            # Parse the line.
            m = re.match(r"(.*) \(compatibility version .*, current version .*\)", line)
            if not m:
                fatal("Failed to parse line from otool(1) output: " + line)
            lib = m.group(1)

            # Ignore self-references (why do these happen?).
            if lib == filename:
                continue

            # Ignore system libs.
            if self.__is_system_lib(lib):
                continue

            # Ignore Qt frameworks.
            if re.search(r"Qt.*\.framework", lib):
                continue

            if fix_paths:
                # handle no search paths case
                if not search_paths:
                    fixed_lib = lib.replace("@loader_path", loader_path)
                    if os.path.exists(fixed_lib):
                        lib = fixed_lib
                    else:
                        # Try to handle other relative libs.
                        candidate = os.path.join(loader_path, fixed_lib)
                        if not os.path.exists(candidate):
                            fatal("Unable to Resolve lib {}", lib)
                        lib = candidate

                for path in search_paths:
                    # Handle libs relative to @loader_path.
                    fixed_lib = lib.replace("@loader_path", loader_path)

                    # Handle libs relative to @rpath.
                    fixed_lib = fixed_lib.replace("@rpath", path)

                    if os.path.exists(fixed_lib):
                        lib = fixed_lib
                        break

                    # Try to handle other relative libs.
                    elif not os.path.isabs(fixed_lib):
                        candidate = os.path.join(loader_path, fixed_lib)
                        if not os.path.exists(candidate):
                            candidate = os.path.join(path, fixed_lib)
                        if os.path.exists(candidate):
                            info("Resolved relative dependency {0} as {1}".format(fixed_lib, candidate))
                            lib = candidate
                            break

            libs.add(lib)

        if True:
            info("Dependencies for file {0}:".format(filepath))
            for lib in libs:
                if os.path.isfile(lib):
                    info(u"      found:{0}".format(lib))
                else:
                    info(u"    missing:{0}".format(lib))

        # Don't check for missing dependencies if we didn't attempt to fix them.
        if fix_paths:
            for lib in libs:
                if not os.path.isfile(lib):
                    fatal("Dependency {0} could not be found on disk".format(lib))

        return libs

    def __is_system_lib(self, lib):
        for prefix in self.SYSTEM_LIBS_PREFIXES:
            if lib.startswith(prefix):
                return True

        return False


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
    elif os.name == "posix" and platform.mac_ver()[0] != "":
        package_builder = MacPackageBuilder(settings)
    elif os.name == "posix" and platform.mac_ver()[0] == "":
        package_builder = LinuxPackageBuilder(settings)
    else:
        fatal("Unsupported platform: " + os.name)

    package_builder.build_package()

if __name__ == "__main__":
    main()
