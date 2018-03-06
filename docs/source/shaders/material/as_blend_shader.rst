.. _label_as_blend_shader:

.. fix_img_align::

|
 
.. image:: /_images/icons/asBlendShader.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Blend Shader Node

asBlendShader
*************

A shader node allowing the user to blend up to 8 BxDFs [#]_. Unlike with physically correct BxDF stacks or layers :cite:`Jakob:2014:CFR:2601097.2601139` , this node just blends the inputs according to the input mixing weights. There is no interaction in terms of light transport between the top and bottom shaders.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Blend Parameters
^^^^^^^^^^^^^^^^

*Layer 0*
    The first layer shader, from bottom to top order, with layer 0 at the bottom, and layer 7 at the top.

*Layer 0 Weight*
    The first layer weight. [#]_

*Layer 0 Visibility*
    A flag that toggles the respective layer contribution on or off.

*(...)*

.. note::

   The subsequent layers follow exactly the same structure and parameterization.

*Layer 7*
    The last layer shader, the layer at the top.

*Layer 7 Weight*
    The last layer weight.

*Layer 7 Visibility*
    The last layer visibility flag. Toggling the contribution of this layer on or off.

.. warning::

   This node does **not** layer colors, that is, it is meant to layer BxDF, EDF or other *closures*, not ordinary colors. Without a valid connection [#]_ to the input, nothing will be layered in the respective layer.

-----

Outputs
-------

*Output Color*
    The combined layers output.

-----

.. _label_as_blend_shader_screenshots:

Screenshots
-----------

.. thumbnail:: /_images/screenshots/blend_shader/as_blendshader_mix2.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   A basic plastic and metal layering via a checkerboard texture as the layering weight on one of the layers.

.. thumbnail:: /_images/screenshots/blend_shader/as_plastic_painted_wall.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   Example first layer. A plastic paint with the plastic BRDF.

.. thumbnail:: /_images/screenshots/blend_shader/as_metal_scratch.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   For the second layer, a metal BRDF, with anisotropy, GGX MDF, very low bump.

.. thumbnail:: /_images/screenshots/blend_shader/as_blendshader_plasticpaint_metal.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   The layering of the plastic paint layer over the metal layer, with a flaked paint texture.

.. thumbnail:: /_images/screenshots/blend_shader/as_plastic_painted_wall3.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   Another example, a painted layer with several coats of paint.

.. thumbnail:: /_images/screenshots/blend_shader/as_metal_rust_scratched1.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   With a dirty, rough metal layer on the bottom.

.. thumbnail:: /_images/screenshots/blend_shader/as_blendshader_paint_rustedmetal3.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   And the resulting blended shaders with a cracked paint mask. The bumps were adjusted so that the top layer gets added the mask *bump*, and the bottom layer the inverse of the mask *bump*, to try and bring a subtle amount of edge detail to the blend.

.. thumbnail:: /_images/screenshots/blend_shader/as_blendshader_paintlayers.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   A third layer on top of the previous two, with varying opacity from a semi-transparent paint coating.

.. thumbnail:: /_images/screenshots/blend_shader/as_blendshader_paintlayers3.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   A fourth layer on top of the previous two, with varying opacity from a semi-transparent paint coating as well.

.. thumbnail:: /_images/screenshots/blend_shader/as_blendshader_mix.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   A basic plastic and metal layering via a checkerboard texture as the layering weight on one of the layers.

.. thumbnail:: /_images/screenshots/blend_shader/as_plastic_painted_wall2.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   Example first layer. A plastic paint with the plastic BRDF.

.. thumbnail:: /_images/screenshots/blend_shader/as_metal_scratch2.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   For the second layer, a metal BRDF, with anisotropy, GGX MDF, very low bump.

.. thumbnail:: /_images/screenshots/blend_shader/as_blendshader_plasticpaint_metal2.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   The layering of the plastic paint layer over the metal layer, with a flaked paint texture.

.. thumbnail:: /_images/screenshots/blend_shader/as_plastic_painted_wall4.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   Another example, a painted layer with several coats of paint.

.. thumbnail:: /_images/screenshots/blend_shader/as_metal_rust_scratched2.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   With a dirty, rough metal layer on the bottom.

.. thumbnail:: /_images/screenshots/blend_shader/as_blendshader_paint_rustedmetal4.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   And the resulting blended shaders with a cracked paint mask. The bumps were adjusted so that the top layer gets added the mask *bump*, and the bottom layer the inverse of the mask *bump*, to try and bring a subtle amount of edge detail to the blend.

.. thumbnail:: /_images/screenshots/blend_shader/as_blendshader_paintlayers2.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   A third layer on top of the previous two, with varying opacity from a semi-transparent paint coating.

.. thumbnail:: /_images/screenshots/blend_shader/as_blendshader_paintlayers4.png
   :group: shots_as_blend_shader_group_A
   :width: 10%
   :title:

   A fourth layer on top of the previous two, with varying opacity from a semi-transparent paint coating as well.

-----

.. rubric:: Footnotes

.. [#] See `BSDF definition <https://en.wikipedia.org/wiki/Bidirectional_scattering_distribution_function>`_.

.. [#] The *layering weight* on the first layer, *Layer 0*, will make a blend between 0 (black), and the *Layer 0 Color*.

.. [#] A valid connection would be any *closure*, so any material shader, such as Maya's Blinn, or appleseed's asSubsurface, asMetal, just to name a few. Color nodes such as Noise2D, simple selected colors, and so on, are **not** valid choices. To blend or composite colors, use the :ref:`asBlendColor <label_as_blend_color>` or :ref:`asCompositeColor <label_as_composite_color>` nodes instead.

----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

