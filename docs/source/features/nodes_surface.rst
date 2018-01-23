.. _label_nodes_surface:

Surface
=======

.. cssclass:: table-striped table-condensed table-hover

=================== ==================
Nodes               Support Status  
=================== ================== 
GLSL Shader         **Unsupported**
ShaderFX shader     **Unsupported**
StingRayPBS         **Unsupported**
Anisotropic         Supported
Bifrost Aero
Bifrost Foam
Bifrost Liquid
Blinn               Supported
CgFX Shader         **Unsupported**
Hair Tube Shader
Lambert             Supported
Layered Shader      **Unsupported** [#]_
Ocean Shader
Phong               Supported
PhongE              Supported
Ramp Shader
Shading Map         **Unsupported**
Surface Shader
Use Background
=================== ==================

.. rubric:: Footnotes

.. [#] An alternative appleseed node is provided, :ref:`asLayerShader <label_as_layer_shader>` for the layering of shaders, and two shaders are provided to blend, and to composite textures. Namely :ref:`asBlendColor <label_as_blend_color>` and :ref:`asCompositeColor <label_as_composite_color>`. These provide functionality that greatly exceeds this node's capabilities.


