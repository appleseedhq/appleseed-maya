.. _label_features:

********
Features
********

appleseed-maya is an appleseed plugin for Autodesk® Maya®.

Main exporter features
======================

Presently the exporter supports:

  * Rendering to Maya's RenderVie
  * Batch Rendering
  * Appleseed project export (\*.appleseed files)
  * Image Based Lighting via appleseed Physical Sky, and Dome Light
  * Support for most of Maya's hypershade nodes (see :ref:`label_supported_nodes`)
  * Several appleseed specific materials, texture, and utility nodes. See :ref:`Custom appleseed shader <label_shaders>` for detailed information
  * Shader overrides with diagnostic modes, ambient occlusion geometric information
  * Final render denoising :cite:`Boughida:2017:BCD:3128450.3128464`
  * Support for custom OSL shaders as Maya shading nodes

.. include:: limitations.rst

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

