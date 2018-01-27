.. _label_as_falloff_angle:

.. fix_img_align::

|
 
.. image:: /_images/icons/asFalloffAngle.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Falloff Angle Icon

asFalloffAngle
**************

A node that takes as input two vectors, and returns a smoothly interpolated value between a user set lower and upper bounds for the angle between them.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Input Parameters
^^^^^^^^^^^^^^^^

*Vector 1*
    The first input vector to consider.

*Normalize Input 1*
    If the input vector is not unit length, enabling this checkbox will normalize it.

*Vector 2*
    The second input vector to consider.

*Normalize Input 2*
    If the input vector is not unit length, enabling this checkbox will normalize it.

-----

Interpolation Parameters
^^^^^^^^^^^^^^^^^^^^^^^^

*Smoothstep Lower Bound*
    The lower bound for the smooth interpolation for the angle between the two input vectors.

*Smoothstep Upper Bound*
    The upper bound for the smooth interpolation for the angle between the two input vectors.

*Smoothstep Function*
    The smooth interpolation [#]_ function to use, it can be one of

        * *Smooth Step*
        * *Smoother Step*
        * *Smoothest Step*

-----

Outputs
-------

*Result*
    The smoothly interpolated value.

-----

.. seealso::

   The `wikipedia page on smoothstep <https://en.wikipedia.org/wiki/Smoothstep>`_ for more information.

-----

.. rubric:: Footnotes

.. [#] Where *smootherstep* has 0 first and second derivatives, and *smootheststep* has 0 third derivatives as well.

