.. _label_as_color_transform:

.. image:: /_images/icons/as_color_transform.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: CC Icon

****************
asColorTransform
****************

A node that transforms a color from **any** input color model, to **any** output color model, respecting the color space definitions set in synColor CMS options. An option will be added later to override this, in a way similar to what is presently done in :ref:`the luminance shader <label_as_luminance>`.

Parameters
==========

.. bogus directive to silence warnings::

-----

Color Attributes
----------------

*Input Color*:
    The color being transformed.

*Input Space*:
    The input color space to transform from. It can be one of

    * RGB
    * HSV [#]_
    * HSL [#]_
    * CIE XYZ [#]_
    * CIE xyY [#]_
    * CIE L\*a\*b\* :cite:`COTE:COTE338`
    * CIE L\*u\*v\* [#]_
    * CIE LCh :sub:`ab` [#]_
    * CIE LCh :sub:`uv` [#]_
    * CIE UCS 1960 [#]_

*Output Space*:
    The output color space to transform to. It can be one of

    * RGB
    * HSV
    * HSL
    * CIE XYZ
    * CIE xyY
    * CIE L\*a\*b\* :cite:`COTE:COTE338`
    * CIE L\*u\*v\*
    * CIE LCh :sub:`ab`
    * CIE LCh :sub:`uv`
    * CIE UCS 1960

-----

Outputs
=======

*Result*:
    The transformed color.

-----

.. _label_as_color_transform_screenshots:

Screenshots
===========

Some examples of color transformations.

.. thumbnail:: /_images/screenshots/color_transform/as_color_transform_RGB.png
   :group: shots_color_transform_group_A
   :width: 10%
   :title:

   Original RGB image to be transformed. The working/render space was (scene-linear), sRGB/Rec.709 primaries, D65 whitepoint.

.. thumbnail:: /_images/screenshots/color_transform/as_color_transform_RGB_to_HSV.png
   :group: shots_color_transform_group_A
   :width: 10%
   :title:

   RGB image transformed to HSV. [#]_

.. thumbnail:: /_images/screenshots/color_transform/as_color_transform_RGB_to_CIELAB.png
   :group: shots_color_transform_group_A
   :width: 10%
   :title:

   RGB image transformed to CIE L\*a\*b\*.

.. thumbnail:: /_images/screenshots/color_transform/as_color_transform_RGB_to_CIELCh_uv.png
   :group: shots_color_transform_group_A
   :width: 10%
   :title:

   RGB image transformed to CIE LCh :sub:`uv`. 

.. thumbnail:: /_images/screenshots/color_transform/as_color_transform_RGB_to_CIELAB_to_CIEXYZ.png
   :group: shots_color_transform_group_A
   :width: 10%
   :title:

   RGB image transformed to CIE L\*a\*b\*, then transformed from CIE L\*a\*b\* to CIE XYZ. Primaries chromaticity coordinates and white point (and potential chromatic adaptation transform) are taken into consideration.

.. thumbnail:: /_images/screenshots/color_transform/as_color_transform_RGB_to_CIELAB_to_CIEXYZ_to_HSV.png
   :group: shots_color_transform_group_A
   :width: 10%
   :title:

   RGB image transformed to CIE L\*a\*b\*, then from CIE L\*a\*b\* to CIE XYZ, and from CIE XYZ to HSV.

.. thumbnail:: /_images/screenshots/color_transform/as_color_transform_RGB_to_CIELAB_to_CIEXYZ_to_HSV_to_RGB.png
   :group: shots_color_transform_group_A
   :width: 10%
   :title:

   RGB image transformed to CIE L\*a\*b\*, then from CIE L\*a\*b\* to CIE XYZ, from CIE XYZ to HSV, and finally from HSV to RGB.

.. thumbnail:: /_images/screenshots/color_transform/difference_of_xforms.png
   :group: shots_color_transform_group_A
   :width: 10%
   :title:

   Difference between original RGB image, and converted image from RGB to CIE L\*a\*b\*, to CIE XYZ, to HSV, to RGB.

-----

.. rubric:: Footnotes

.. [#] Hue, Saturation, Value color space, https://en.wikipedia.org/wiki/HSL_and_HSV
.. [#] Hue, Saturation, Lightness color space, https://en.wikipedia.org/wiki/HSL_and_HSV
.. [#] The CIE XYZ color space, https://en.wikipedia.org/wiki/CIE_1931_color_space
.. [#] CIE xyY, https://en.wikipedia.org/wiki/CIE_1931_color_space 
.. [#] CIE L\*u\*v\* color space, https://en.wikipedia.org/wiki/CIELUV
.. [#] Cylindrical representation of the CIELAB color space, LCh :sub:`ab` co, https://en.wikipedia.org/wiki/Lab_color_space#Cylindrical_representation:_CIELCh_or_CIEHLC
.. [#] Cylindrical representation of the CIELUV color space, CIE LCh :sub:`uv`, https://en.wikipedia.org/wiki/Lab_color_space#Cylindrical_representation:_CIELCh_or_CIEHLC
.. [#] Uniform color space, CIE UCS 1960, https://en.wikipedia.org/wiki/CIE_1960_color_space

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

