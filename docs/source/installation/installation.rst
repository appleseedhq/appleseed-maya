.. _label_installation:

************
Installation
************

.. dummy directive:

Manual Setup
============

**1. Set environment variables**

.. code-block:: bash
   :linenos:

    export THIS_DIR=<path to appleseed-maya source>
    export MAYA_PLUG_IN_PATH=${BUILD_DIR}/src/appleseedmaya
    export MAYA_SCRIPT_PATH=${THIS_DIR}/scripts
    export PYTHONPATH=$PYTHONPATH:${APPLESEED_DIR}/lib/python2.7:${THIS_DIR}/scripts
    export XBMLANGPATH=${THIS_DIR}/icons
    export MAYA_PRESET_PATH=${THIS_DIR}/presets:$MAYA_PRESET_PATH
    export APPLESEED_SEARCHPATH=${APPLESEED_DIR}/sandbox/shaders/maya

**2. Launch Maya and load the plugin**

-----

Using a module (.mod)
=====================

To be written here.

.. seealso:: How to install modules in Maya

