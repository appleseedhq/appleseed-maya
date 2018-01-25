.. _label_as_swizzle:

.. fix_img_align::

|
 
.. image:: /_images/icons/asSwizzle.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Swizzle Icon

asSwizzle
*********

A node that rearranges the elements of an input color+alpha, or vector.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Color
^^^^^

*Input Color*
    The color to rearrange.

*Input Alpha*
    The alpha channel for the final RGBA output.

*Red Channel*
    What RGBA channel to use as the new *Red* channel.

*Green Channel*
    What RGBA channel to use as the new *Green* channel.

*Blue Channel*
    What RGBA channel to use as the new *Blue* channel.

*Alpha Channel*
    What RGBA channel to use as the new *Alpha* channel.

*Invert Red*
    Inverts the new *Red* channel. It assumes the color was in [0,1] range.

*Invert Green*
    Inverts the new *Green* channel. It assumes the color was in [0,1] range.

*Invert Blue*
    Inverts the new *Blue* channel. It assumes the color was in [0,1] range.

*Invert Alpha*
    Inverts the new *Alpha* channel. It assumes the color was in [0,1] range.

Vector
^^^^^^

*Vector Type*
    A vector type variable, it can be one of types

        * *normal*
        * *vector*
        * *point*

*X Component*
    What XYZ component to use as the new *X* component.

*Y Component*
    What XYZ component to use as the new *Y* component.

*Z Component*
    What XYZ component to use as the new *Z* component. 

*Invert X*
    Inverts the new *X* component.

*Invert Y*
    Inverts the new *Y* component.

*Invert Z*
    Inverts the new *Z* component.

-----

Outputs
-------

*Output Color*
    The rearranged color.

*Output Alpha*
    The rearranged alpha.

*Output Vector*
    The rearranged vector.

