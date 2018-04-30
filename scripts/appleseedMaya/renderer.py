
#
# This source file is part of appleseed.
# Visit https://appleseedhq.net/ for additional information and resources.
#
# This software is released under the MIT license.
#
# Copyright (c) 2016-2018 Esteban Tovagliari, The appleseedhq Organization
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# Maya imports.
import maya.mel as mel


def createRenderMelProcedures():
    mel.eval('''
        global proc appleseedRenderProcedure(int $width, int $height, int $doShadows, int $doGlowPass, string $camera, string $option)
        {
            python("import appleseedMaya.renderGlobals");
            python("appleseedMaya.renderGlobals.createGlobalNodes()");
            appleseedRender -w $width -h $height -c $camera;
        }
        '''
             )

    mel.eval('''
        global proc appleseedBatchRenderProcedure(string $options)
        {
            python("import appleseedMaya.renderGlobals");
            python("appleseedMaya.renderGlobals.createGlobalNodes()");
            appleseedRender -batch $options;
        }
        '''
             )

    mel.eval('''
        global proc appleseedCancelBatchRenderProcedure()
        {
            batchRender;
        }
        '''
             )

    mel.eval('''
        global proc appleseedIprRenderProcedure(int $width, int $height, int $doShadows, int $doGlowPass, string $camera)
        {
            python("import appleseedMaya.renderGlobals");
            python("appleseedMaya.renderGlobals.createGlobalNodes()");
            appleseedProgressiveRender -w $width -h $height -c $camera -action "render";
        }
        '''
             )

    mel.eval('''
        global proc appleseedStartIprRenderProcedure(string $editor, int $resolutionX, int $resolutionY, string $camera)
        {
            python("import appleseedMaya.renderGlobals");
            python("appleseedMaya.renderGlobals.createGlobalNodes()");
            appleseedProgressiveRender -w $resolutionX -h $resolutionY -c $camera -action "start";
        }
        '''
             )

    mel.eval('''
        global proc appleseedStopIprRenderProcedure()
        {
            appleseedProgressiveRender -action "stop";
        }
        '''
             )

    mel.eval('''
        global proc appleseedRefreshIprRenderProcedure()
        {
            appleseedProgressiveRender -action "refresh";
        }
        '''
             )

    mel.eval('''
        global proc int appleseedIsRunningIprRenderProcedure()
        {
            int $result = `appleseedProgressiveRender -action "running"`;
            return $result;
        }
        '''
             )

    mel.eval('''
        global proc appleseedPauseIprRenderProcedure(string $editor, int $pause)
        {
            appleseedProgressiveRender -action "pause";
        }
        '''
             )

    mel.eval('''
        global proc appleseedChangeIprRegionProcedure(string $renderPanel)
        {
            appleseedProgressiveRender -action "region";
        }
        '''
             )
