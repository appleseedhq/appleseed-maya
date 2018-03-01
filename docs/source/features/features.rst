.. _label_features:

********
Features
********

appleseed-maya is an appleseed plugin for Autodesk® Maya®.

Main exporter features
======================

Presently the exporter supports:

  * Rendering to Maya's RenderView
  * Batch Rendering
  * Appleseed project export (\*.appleseed files) or packed appleseed project files (\*.appleseedz)
  * Image Based Lighting via appleseed Physical Sky, and Dome Light
  * Support for most of Maya's hypershade nodes (see :ref:`label_supported_nodes`)
  * Several appleseed specific materials, texture, and utility nodes. See :ref:`Custom appleseed shaders <label_shaders>` for detailed information
  * Shader overrides with diagnostic modes, ambient occlusion geometric information
  * Final render denoising :cite:`Boughida:2017:BCD:3128450.3128464`
  * Support for custom OSL shaders as Maya shading nodes
  * Support for `OpenColorIO (or OCIO) <https://github.com/imageworks/OpenColorIO>`_
  * Partial support [#]_ for Maya's SynColor [#]_ color management system. 

.. include:: limitations.rst

-----

.. rubric:: Footnotes

.. [#] For now, the input device transforms supported for the ingested material are listed in :ref:`asTexture Color Management options <label_as_texture_color_management>`.

.. [#] Maya's native color management system, providing equivalent functionality to OpenColorIO. Not all input transformations are supported. See `Autodesk Color Management Supplemental Information <https://knowledge.autodesk.com/support/maya/learn-explore/caas/CloudHelp/cloudhelp/2016/ENU/Maya/files/GUID-BB4F38CF-6AA8-4D35-96DD-7F75D62FD3A7-htm.html>`_ for more details.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

