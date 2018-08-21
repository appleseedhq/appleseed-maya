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
wasn't possible to implement directly in `OSL <https://github.com/imageworks/OpenShadingLanguage>`_, or to provide functionality
that is inexistent and we feel might be useful to the end user.

Procedural Texture Nodes
========================

* :ref:`asNoise2D <label_as_noise2D>`
* :ref:`asNoise3D <label_as_noise3D>`
* :ref:`asTexture <label_as_texture>`
* :ref:`asTexture3D <label_as_texture3D>`
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
* :ref:`asFresnel <label_as_fresnel>`

General Utilities
=================

* :ref:`asAnisotropyVectorField <label_as_anisotropy_vector_field>`
* :ref:`asAttributes <label_as_attributes>`
* :ref:`asBump <label_as_bump>`
* :ref:`asCreateMask <label_as_create_mask>`
* :ref:`asDoubleShade <label_as_double_shade>`
* :ref:`asFalloffAngle <label_as_falloff_angle>`
* :ref:`asGlobals <label_as_globals>`
* :ref:`asIdManifold <label_as_id_manifold>`
* :ref:`asManifold2D <label_as_manifold2d>`
* :ref:`asRaySwitch <label_as_ray_switch>`
* :ref:`asSpaceTransform <label_as_space_transform>`
* :ref:`asSwizzle <label_as_swizzle>`
* :ref:`asTextureInfo <label_as_texture_info>`
* :ref:`asTriplanar <label_as_triplanar>`

Materials
=========

* :ref:`asBlackbody <label_as_blackbody>`
* :ref:`asDisneyMaterial <label_as_disney_material>`
* :ref:`asGlass <label_as_glass>`
* :ref:`asBlendShader <label_as_blend_shader>`
* :ref:`asMetal <label_as_metal>`
* :ref:`asPlastic <label_as_plastic>`
* :ref:`asSbsPBRMaterial <label_as_sbs_pbrmaterial>`
* :ref:`asStandardSurface <label_as_standard_surface>`
* :ref:`asSubsurface <label_as_subsurface>`
* :ref:`asSwitchSurface <label_as_switch_surface>`
* :ref:`asToon <label_as_toon>`

