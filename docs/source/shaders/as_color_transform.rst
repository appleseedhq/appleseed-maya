.. _label_as_color_transform:

.. image:: ../images/as_color_transform.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: CC Icon

****************
asColorTransform
****************

A node that transforms a color from **any** input color model, to **any** output color model, respecting the color space definitions set in synColor CMS options. An option will be added later to override this, in a way similar to what is presently done in :ref:`label_as_luminance`.

Parameters
==========

.. bogus directive to silence warnings::

-----

Color Attributes
----------------

*Input Color*
    The color being transformed.

*Input Space*
    The input color space to transform from. It can be one of

        * RGB
        * HSV
        * HSL
        * XYZ
        * xyY
        * L\*a\*b\* :cite:`COTE:COTE338`
        * L\*u\*b\*
        * LCH:subscript:`ab`
        * LCH:subscript:`uv`
        * UCS

* Output Space*
    The output color space to transform to. It can be one of

        * RGB
        * HSV
        * HSL
        * XYZ
        * xyY
        * L\*a\*b\*
        * L\*u\*b\*
        * LCH:subscript:`ab`
        * LCH:subscript:`uv`
        * UCS

Outputs
=======

*Result*
    The transformed color.

-----

.. _label_as_color_transform_screenshots:

Screenshots
===========

Some examples of color transformations.

.. thumbnail:: /images/luminance/luminance_colorramp_workingspace_rec709.png
   :group: shots_luminance_group_A
   :width: 10%
   :title:

   Example of color transformation

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

