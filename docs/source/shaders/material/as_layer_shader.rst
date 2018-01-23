.. _label_as_layer_shader:

.. fix_img_align::

|
 
.. image:: /_images/icons/asLayerShader.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Layer Shader Node

asLayerShader
*************

A shader node allowing the user to layer up to 8 BxDFs. Unlike with physically correct BxDF layering :cite:`Jakob:2014:CFR:2601097.2601139` , this node just layers the inputs according to the input weights. There is no interaction in terms of light transport between the top and bottom layers.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Layers Parameters
^^^^^^^^^^^^^^^^^

*Layer 0*
    The first layer shader.

*Layer 0 Weight*
    The first layer weight.

*Layer 0 Normal*
    The first layer world space unit length shading (bump) normal.

*Layer 0 Visibility*
    A flag that toggles the respective layer contribution on or off.

|

*(...)*

.. note::

   The subsequent layers follow exactly the same structure and parameterization.

|

*Layer 7*
    The last layer shader.

*Layer 7 Weight*
    The last layer weight.

*Layer 7 Normal*
    The last layer world space unit lenght shading (bump) normal.

*Layer 7 Visibility*
    The last layer visibility flag. Toggling the contribution of this layer on or off.

-----

Outputs
^^^^^^^

*Output Color*
    The combined layers output.

*Layered Normal*
    The layered shading normal.

-----

.. _label_as_layer_shader_screenshots:

Screenshots
-----------

.. thumbnail:: /_images/screenshots/layer_shader/as_plastic_painted_wall.png
   :group: shots_as_layer_shader_group_A
   :width: 10%
   :title:

   Example first layer. A plastic paint with the plastic BRDF.

.. thumbnail:: /_images/screenshots/layer_shader/as_metal_scratch.png
   :group: shots_as_layer_shader_group_A
   :width: 10%
   :title:

   For the second layer, a metal BRDF, with anisotropy, GGX MDF, very low bump.

.. thumbnail:: /_images/screenshots/layer_shader/as_layershader_plasticpaint_metal.png
   :group: shots_as_layer_shader_group_A
   :width: 10%
   :title:

   The layering of the plastic paint layer over the metal layer, with a flaked paint texture.

.. thumbnail:: /_images/screenshots/layer_shader/as_plastic_painted_wall2.png
   :group: shots_as_layer_shader_group_A
   :width: 10%
   :title:

   Example first layer. A plastic paint with the plastic BRDF.

.. thumbnail:: /_images/screenshots/layer_shader/as_metal_scratch2.png
   :group: shots_as_layer_shader_group_A
   :width: 10%
   :title:

   For the second layer, a metal BRDF, with anisotropy, GGX MDF, very low bump.

.. thumbnail:: /_images/screenshots/layer_shader/as_layershader_plasticpaint_metal2.png
   :group: shots_as_layer_shader_group_A
   :width: 10%
   :title:

   The layering of the plastic paint layer over the metal layer, with a flaked paint texture.

-----

.. rubric:: Footnotes


----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

