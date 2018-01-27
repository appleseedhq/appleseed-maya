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

+------------------------+------------+----------+----------+
| Header row, column 1   | Header 2   | Header 3 | Header 4 |
| (header rows optional) |            |          |          |
+========================+============+==========+==========+
| body row 1, column 1   | column 2   | column 3 | column 4 |
+------------------------+------------+----------+----------+
| body row 2             | ...        | ...      |          |
+------------------------+------------+----------+----------+

These shaders are meant to either replace some of Maya's functionality that
wasn't possible to implement directly in `OSL <https://github.com/imageworks/OpenShadingLanguage>`_, or to provide functionality
that is inexistent and we feel might be useful to the end user.

Procedural Texture Nodes
========================

* :ref:`asNoise2D <label_as_noise2D>`
* :ref:`asNoise3D <label_as_noise3D>`
* :ref:`asTexture <label_as_texture>`
* :ref:`asVoronoi2D <label_as_voronoi2D>`
* :ref:`asVoronoi3D <label_as_voronoi3D>`

Color Utilities
===============

* :ref:`asBlendColor <label_as_blend_color>`
* :ref:`asColorTransform <label_as_color_transform>`
* :ref:`asCompositeColor <label_as_composite_color>`
* :ref:`asLuminance <label_as_luminance>`
* :ref:`asSwitchTexture <label_as_switch_texture>`
* :ref:`asVaryColor <label_as_vary_color>`

General Utilities
=================

* :ref:`asAnisotropyVectorField <label_as_anisotropy_vector_field>`
* :ref:`asAttributes <label_as_attributes>`
* :ref:`asCreateMask <label_as_create_mask>`
* :ref:`asDoubleShade <label_as_double_shade>`
* :ref:`asFalloffAngle <label_as_falloff_angle>`
* :ref:`asGlobals <label_as_globals>`
* :ref:`asIdManifold <label_as_id_manifold>`
* :ref:`asRaySwitch <label_as_ray_switch>`
* :ref:`asSpaceTransform <label_as_space_transform>`
* :ref:`asSwizzle <label_as_swizzle>`
* :ref:`asTextureInfo <label_as_texture_info>`

Materials
=========

* :ref:`asBlackbody <label_as_blackbody>`
* :ref:`asDisneyMaterial <label_as_disney_material>`
* :ref:`asGlass <label_as_glass>`
* :ref:`asLayerShader <label_as_layer_shader>`
* :ref:`asMetal <label_as_metal>`
* :ref:`asPlastic <label_as_plastic>`
* :ref:`asStandardSurface <label_as_standard_surface>`
* :ref:`asSubsurface <label_as_subsurface>`

