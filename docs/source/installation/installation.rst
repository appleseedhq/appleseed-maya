.. _label_installation:

************
Installation
************

.. dummy directive:

Manual Setup
============

The plugin is a Maya module. To use it, simply uncompress the zip file and add the path containing the :file:`appleseed-maya.mod` file to your `Maya.env file <https://knowledge.autodesk.com/support/maya/learn-explore/caas/CloudHelp/cloudhelp/2016/ENU/Maya/files/GUID-8EFB1AC1-ED7D-4099-9EEE-624097872C04-htm.html>`_, or to the `MAYA_MODULE_PATH environment variable <https://knowledge.autodesk.com/support/maya/learn-explore/caas/CloudHelp/cloudhelp/2015/ENU/Maya/files/Environment-Variables-File-path-variables-htm.html>`_.

Example:

**1. Add to your ${HOME}/maya/2017/Maya.env file**

.. code-block:: bash
   :linenos:

   MAYA_MODULE_PATH = /path/to/appleseed-maya-dir

**2. Append to your MAYA_MODULE_PATH env var directly**

.. code-block:: bash
   :linenos:

   export MAYA_MODULE_PATH = /path/to/appleseed-maya-dir:${MAYA_MODULE_PATH}

Change the file paths and environment variable setting syntax according to your operating system.

|

