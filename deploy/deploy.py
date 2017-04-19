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

import sys
import os
import shutil
import argparse
import platform
from distutils.dir_util import copy_tree


def copy_plugins(args):
    if platform.system().lower() in ['linux']:
        plugin_ext = '.so'
    elif platform.system().lower() in ['windows']:
        plugin_ext = '.mll'
    else:
        print 'Error: Unsupported platform'
        sys.exit(0)

    plugins_dir = os.path.join(args.directory, 'plug-ins')
    if not os.path.exists(plugins_dir):
        os.makedirs(plugins_dir)

    print 'Copying appleseedMaya plugin'
    shutil.copy(
        os.path.join(args.build_path, 'src', 'appleseedmaya', 'appleseedMaya' + plugin_ext),
        plugins_dir
    )

def main():
    parser = argparse.ArgumentParser(description='deploy maya plugin')

    parser.add_argument('-b', '--build-path', metavar='build-path', help='set the path to the build directory')
    parser.add_argument('directory', help='destination directory')
    args = parser.parse_args()

    print 'Deploying appleseedMaya to %s...' % args.directory

    deploy_dir = os.path.dirname(os.path.realpath(__file__))

    print 'Copying module file'
    shutil.copy(os.path.join(deploy_dir, 'appleseedMaya.mod'), args.directory)

    root_dir = os.path.join(deploy_dir, '..')

    print 'Copying icons...'
    copy_tree(os.path.join(root_dir, 'icons'), os.path.join(args.directory, 'icons'))

    print 'Copying presets...'
    copy_tree(os.path.join(root_dir, 'presets'), os.path.join(args.directory, 'presets'))

    print 'Copying resources...'
    copy_tree(os.path.join(root_dir, 'resources'), os.path.join(args.directory, 'resources'))

    print 'Copying scripts...'
    copy_tree(os.path.join(root_dir, 'scripts'), os.path.join(args.directory, 'scripts'))

    print 'Copying plugins...'
    copy_plugins(args)

if __name__ == '__main__':
    main()
