.. _label_installation:

************
Installation
************

Setup
=====


**1. Set environment variables**

.. code-block:: bash
   :linenos:

    export MAYA_PLUG_IN_PATH=${BUILD_DIR}/src/appleseedmaya
    export MAYA_SCRIPT_PATH=${THIS_DIR}/scripts
    export PYTHONPATH=$PYTHONPATH:${APPLESEED_DIR}/lib/python2.7:${THIS_DIR}/scripts
    export XBMLANGPATH=${THIS_DIR}/icons
    export MAYA_PRESET_PATH=${THIS_DIR}/presets:$MAYA_PRESET_PATH
    export APPLESEED_SEARCHPATH=${APPLESEED_DIR}/sandbox/shaders/maya

**2. Launch Maya and load the plugin**

