.. _label_as_space_transform:

.. fix_img_align::

|
 
.. image:: /_images/icons/asSpaceTransform.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Space Transform Icon

asSpaceTransform
****************

A node meant to transform an input point, normal, or vector, from a coordinate system into another.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Input Parameters
^^^^^^^^^^^^^^^^

*Point to Transform*
    An point like variable to transform.

*Normal to Transform*
    A normal like variable to transform.

*Vector to Transform*
    A vector like variable to transform.

-----

Space Parameters
^^^^^^^^^^^^^^^^

*From Space*
    The origin coordinate system. It can be one of

        * *common* [#]_
        * *object* [#]_
        * *shader* [#]_
        * *world* [#]_
        * *camera* [#]_
        * *screen* [#]_
        * *raster* [#]_
        * *NDC* [#]_

*To Space*
    The destination coordinate system. It can be one of

        * *common*
        * *object*
        * *shader*
        * *world*
        * *camera*
        * *screen*
        * *raster*
        * *NDC*

*Normalize Output Vectors*
    When the input to transform is either of *vector* type or of *normal* type, this parameter will make sure the resulting transformed *vector* or *normal* are of unit length.

.. seealso::

    The `Open Shading Language documentation <https://github.com/imageworks/OpenShadingLanguage/blob/master/src/doc/osl-languagespec.pdf>`_.

-----

Outputs
-------

*Transformed Point*
    The transformed input point.

*Transformed Normal*
    The transformed input normal, optionally normalized.

*Transformed Vector*
    The transformed input vector, optionally normalized.

*Transform Matrix*
    The transformation matrix that transforms from *From Space* to *To Space*.

-----

.. rubric:: Footnotes

.. [#] The *common* coordinate system is the OSL equivalent of `RSL's <https://en.wikipedia.org/wiki/RenderMan_Shading_Language>`_ *current* coordinate system and is the coordinate system in which all values start and the one in which lighting calculations are performed. This is a renderer specific coordinate system. In appleseed this is equivalent to the *world* coordinate system.

.. [#] The *object* coordinate system refers to the local coordinate system of the geometry currently shaded.

.. [#] The *shader* coordinate system, refers to the coordinate system active by the time of the shader instantiation.

.. [#] The *world* coordinate system is the world coordinate system of the scene.

.. [#] The *camera* coordinate system refers to the coordinate system with its origin at the center of the camera lens, the *x* axis pointing right, the *y* axis pointing up, and the *z* axis pointing into the screen.

.. [#] The *screen* coordinate system refers to the coordinate system of the camera's image plane after the perspective transformation, if any, with the origin looking along the *z* axis of the *camera* coordinate system.

.. [#] The *raster* coordinate system refers to the 2D coordinate system with origin at the upper left corner of the image, and *xy* resolution at the bottom right corner of the image.

.. [#] The *NDC* coordinate system, or *Normalized Device Coordinates*, refers to the coordinate system with origin at the upper left corner of the image, and [1,1] at the xy coordinates of the bottom right of the image.

