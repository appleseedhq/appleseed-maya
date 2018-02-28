.. _label_as_blend_color:

.. fix_img_align::

|
 
.. image:: /_images/icons/asBlendColor.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Blend Color Icon

asBlendColor
************

A node that allows the user to blend an input color with a second input color according to preset modes, similar to what is commonly found in image editing applications.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Color Parameters
^^^^^^^^^^^^^^^^

*Source Color*
    The input RGB color.

*Source*
    The input color weight.

*Blend Color*
    The second input color.

*Blend Weight*
    The second input color weight.

*Blend Mode*
    The blend mode [#]_ to use for the respective colors. Users of applications such as `GIMP <https://www.gimp.org/>`_ or Adobe® Photoshop® should be familiar with these blend operations.
    They can take the following values

        * Darken
        * Multiply
        * Color Burn
        * Linear Burn
        * Lighten
        * Screen
        * Color Dodge
        * Linear Dodge
        * Overlay
        * Soft Light
        * Hard Light
        * Vivid Light
        * Linear Light
        * Pin Light
        * Difference
        * Exclusion
        * Subtract
        * Divide
        * Hue
        * Saturation
        * Color
        * Luminosity

.. seealso::

   *Merging and Transformation of Raster Images for Cartoon Animation* :cite:`Wallace:1981:MTR:965161.806813`.

*Clamp Output*
    Checking this checkbox will clamp the output to the [0,1] range.

-----

Outputs
-------

*Output Color*
    The blended color result.

-----

.. rubric:: Footnotes

.. [#] See `this page on blend modes <https://en.wikipedia.org/wiki/Blend_modes>`_ for more information.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

