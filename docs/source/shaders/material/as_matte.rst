.. _label_as_matte:

.. fix_img_align::

|

.. image:: /_images/icons/asMatte.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Matte holdout shader

asMatte
*******

A matte holdout shader allowing the user to specify an arbitrary color and matte opacity.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Surface Parameters
^^^^^^^^^^^^^^^^^^

*Color*
    The input material to pass-through for non-camera rays.

.. note::

   Most materials have their own matte controls, however the user might wish to blend materials, with for instance, the :ref:`asBlendShader node <label_as_blend_shader>`, and for such it might be preferable to set this matte node at the root of the shader graph.

-----

Matte Parameters
^^^^^^^^^^^^^^^^

*Enable Matte*
    Checkbox that toggles matte holdouts on or off.

*Matte Color*
    The color for the matte, only visible for *camera* rays.

*Matte Opacity*
    The opacity value written to the alpha channel.

-----

Outputs
-------

*Output Color*
    The pass-through closure output.

*Output Matte Opacity*
    The matte holdout.

-----

.. _label_as_matte_screenshots:

Screenshots
-----------

.. thumbnail:: /_images/screenshots/matte/as_matte_shot1.jpg
   :group: shots_as_matte_group_A
   :width: 10%
   :title:

   Blue plastic exterior with a red holdout matte fully transparent, with a metallic interior sphere. The non camera rays show the blue plastic, while the primary rays show the user-set color for the matte (red).

.. thumbnail:: /_images/screenshots/matte/as_matte_shot1_alpha.jpg
   :group: shots_as_matte_group_A
   :width: 10%
   :title:

   The matte for the previous image, fully transparent for the exterior shape.

.. thumbnail:: /_images/screenshots/matte/as_matte_shot2.jpg
   :group: shots_as_matte_group_A
   :width: 10%
   :title:

   White exterior shape, showing the diffuse reflections of the internal red shape. The internal red shape has a green matte partially transparent.

.. thumbnail:: /_images/screenshots/matte/as_matte_shot2_alpha.jpg
   :group: shots_as_matte_group_A
   :width: 10%
   :title:

   The partially transparent matte of the previous shot.

.. thumbnail:: /_images/screenshots/matte/as_matte_shot3.jpg
   :group: shots_as_matte_group_A
   :width: 10%
   :title:

   Exterior shape using glass, with a light grey interior diffuse sphere with a transparent matte set to blue color.

.. thumbnail:: /_images/screenshots/matte/as_matte_shot3_alpha.jpg
   :group: shots_as_matte_group_A
   :width: 10%
   :title:

   The transparent matte of the interior sphere.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

