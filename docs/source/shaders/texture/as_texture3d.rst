.. _label_as_texture3D:

.. fix_img_align::

|
 
.. image:: /_images/icons/asTexture3D.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Texture3D Icon

asTexture3D
***********

A node that performs 3D lookup of a volume texture, typically a `Field3D <https://github.com/imageworks/Field3D/>`_ file.

|

Parameters
----------

.. bogus directive to silence warning::

-----

3D Texture
^^^^^^^^^^

*Filename*
    The volume texture, typically a *\*.f3d* [#]_ file.

*Starting Channel*
    The starting channel for the volume texture lookup.

*Color*
    The default color to use if the texture lookup fails.

*Channel Fill*
    The default scalar value to use for any channels requested but not present in the volume texture.

*Time*
    The time value to use if the texture specifies a time varying local transformation.

Wrapping
""""""""

*S Wrap*
    The wrapping mode along the *s* coordinate, it can be one of

        * Default
        * Black
        * Periodic
        * Clamp
        * Mirror

*T Wrap*
    The wrapping mode along the *t* coordinate, it can be one of

        * Default
        * Black
        * Periodic
        * Clamp
        * Mirror

*R Wrap*
    The wrapping mode along the *r* coordinate, it can be one of

        * Default
        * Black
        * Periodic
        * Clamp
        * Mirror

Blur
""""

*Blur Width*
    The amount of blur along the width of the volume, or the *s* direction.

*Blur Height*
    The amount of blur along the height of the volume, or *t* direction.

*Blur Depth*
    The amount of blur along the depth of the volume, or *r* direction.

Filter
""""""

*Width Filter*
    Scale for the size of the filter defined by the partial derivatives along the *s* direction or implicitly from *P*, with a value of 0 disabling filtering altogether.

*Height Filter*
    Scale for the size of the filter defined by the partial derivatives along the *t* direction or implicitly from *P*, with a value of 0 disabling filtering altogether.

*Depth Filter*
    Scale for the size of the filter defined by the partial derivatives along the *r* direction or implicitly from *P*, with a value of 0 disabling filtering altogether.

Coordinates
^^^^^^^^^^^

*Surface Point*
    The surface point being shaded.

*Coordinate System*
    The coordinate system to use for the volume, it can be

        * Object Space
        * World Space
        * Camera Space

-----

Outputs
-------

*Output Color*
    The color resulting from the *Features Mode* choice.

-----

.. rubric:: Footnotes

.. [#] These files are exported as a result of simulations done in other applications. In the case of Maya, one could for instance convert Maya's fluids to a Field3d file via the `Field3D Maya Plugin <https://github.com/magic-box/Field3DMayaPlugin>`_. Other simulation applications also allow exporter volume data as *\*.f3d*.

