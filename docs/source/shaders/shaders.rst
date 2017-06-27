.. _label_shaders:

.. Adding a toctree here, hidden, and including it in the main TOC tree will
   flatten the content, instead of working as a nested sub-TOC tree.
   The only thing that seems to work is including the RST document, which 
   sadly will mean shaders.rst will be flattened instead.
   One would want a include directive, with a hidden option, just for TOC
   purposes, but that doesn't exist.

.. toctree::
    :name: shaderstoc
    :glob:
    :hidden:
    
    material/*
    texture/*
    utilities/*

************************
Custom appleseed shaders
************************

These shaders are meant to either replace some of Maya's functionality that
wasn't possible to implement directly in `OSL <http://opensource.imageworks.com/?p=osl>`_, or to provide functionality
that is inexistent and we feel might be useful to the end user.

Procedural Texture Nodes
========================

* :ref:`asVoronoi2D <label_as_voronoi2d>`
* :ref:`asVoronoi3D <label_as_voronoi3d>`

Color Utilities
===============

* :ref:`asColorTransform <label_as_color_transform>`
* :ref:`asLuminance <label_as_luminance>`

Materials
=========

* :ref:`asGlass <label_as_glass>`
* :ref:`asDisneyMaterial <label_as_disney_material>`

