.. _label_as_triplanar:

.. fix_img_align::

|
 
.. image:: /_images/icons/asTriPlanar.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Triplanar Icon

asTriplanar
***********

A triplanar projection shader, allowing individual texture(s) to be projected along the object's X, Y and Z axis, and blended. It can be used to blend colors or tangent space normal maps.

|

Parameters
----------

.. bogus directive to silence warning::

-----

Projection
^^^^^^^^^^

*Blend Mode*
    Allowing the user to choose between blending colors, or blending tangent space normal maps [#]_. It can take the values

        * Color
        * Tangent Normal

*Blend Softness*
    Controls the transition softness between the projections, with higher values having a faded and very wide transition, and lower values having sharper well defined transition areas.

*Surface Point*
    The point being shaded.

*Coordinate Space*
    The coordinate system to use, it can be one of

        * Object Space
        * World Space

X Axis
~~~~~~

*Solid Color*
    Solid color for the X axis projection.

*Texture Filename*
    The texture to use for the X axis projection.

.. tip::

   Nothing prevents the user from using the same texture for all axis projections.

*Horizontal Frequency*
    The horizontal frequency of the texture for the X axis projection.

*Vertical Frequency*
    The vertical frequency of the texture for the X axis projection.

*Horizontal Offset*
    The horizontal offset of the texture for the X axis projection.

*Vertical Offset*
    The vertical offset of the texture for the X axis projection.

*Rotation*
    The rotation angle for the X axis projection, in degrees, between -360 and 360 degrees.

*S Wrap Mode*
    The texture wrapping mode along the *s* texture coordinate for the X axis projection. It can be one of

        * Default
        * Black
        * Periodic
        * Clamp
        * Mirror

*T Wrap Mode*
    The texture wrapping mode along the *t* texture coordinate for the X axis projection. It can be one of

        * Default
        * Black
        * Periodic
        * Clamp
        * Mirror

*S Flip*
    A flag that toggles the mirroring of the projected texture along the *s* texture coordinate.

*T Flip*
    A flag that toggles the mirroring of the projected texture along the *t* texture coordinate.

Color Management
""""""""""""""""

*Input Transfer Function*
    Applies an Electro-Optical Transfer Function, or EOTF, to the input texture, linearizing it.
    It can take the following values

        * None/Raw
        * sRGB
        * Rec.709
        * Gamma 2.2
        * Gamma 2.4
        * Gamma 2.6 (DCI)
        * Rec.1886 [#]_
        * Rec.2020

*RGB Primaries*
    It allows the user to set the RGB primaries that define the color space of the input texture, and can take the following values

        * Raw [#]_
        * sRGB/Rec.709 [#]_
        * AdobeRGB [#]_
        * Rec.2020 :cite:`6784055`
        * DCI-P3 :cite:`7290729`
        * ACES :cite:`7289895`
        * ACEScg :cite:`Duiker:2015:ACC:2791261.2791273`

Y and Z axis
~~~~~~~~~~~~

*(...)*

.. attention::

   The Y and Z axis projection parameters follow exactly the same structure as the X axis projection parameters and are omitted here for brevity.

Randomization
~~~~~~~~~~~~~

*Randomization*
    Allows the user to add variation to the triplanar node by randomly rotating the projection axis frame, with a value of 0.0 meaning no variation at all, and a value of 1.0 allowing rotations between -360.0 and 360.0 degrees.

*Manifold*
    An integer hash, usually provided by the :ref:`asIdManifold <label_as_id_manifold>` node. Lacking such a connection, it defaults to adding variation based on the object's assembly instance name.

Bump Mapping
^^^^^^^^^^^^

*Bump Mapping*
    The unit length bumped shading normal.

Color Management
^^^^^^^^^^^^^^^^

*Enable CMS*
    Toggles the color management options *on* or *off*.

*Rendering RGB Primaries*
    It allows the user to set the RGB primaries of the rendering or working space, and it should match the choice of rendering/working space of the renderer.
    It can take the following values

        * sRGB/Rec.709
        * Rec.2020
        * DCI-P3
        * ACES
        * ACEScg

-----

Outputs
-------

*Output Color*
    The color resulting from the *Features Mode* choice.

*Output Alpha*
    The alpha resulting from the *Features Mode* choice, usually luminance of the color only.

*Output Normal*
    The resulting blended tangent space normal maps as unit length normals in world space when the *Blend Mode* is set to *Tangent Normal*.

-----

.. rubric:: Footnotes

.. [#] See `blending in detail <http://blog.selfshadow.com/publications/blending-in-detail/>`_ for details on blending tangent space normals using several methods. This node uses the *Reoriented Normal Mapping* (or RNM) blend method to blend tangent space normals when *Blend Mode* is set to *Tangent Normal*.

.. [#] See `ITU-R BT.1886 recommendation <https://www.itu.int/rec/R-REC-BT.1886-0-201103-I/en>`_ for details on the electro-optical transfer function.

.. [#] Because it makes no sense whatsoever to use colorimetry on non-color information or data, such as normal maps, or Z depth, motion vectors, and so on.

.. [#] sRGB shares the same CIE xy chromaticity coordinates with `ITU-R BT.709/Rec.709 <https://www.itu.int/rec/R-REC-BT.709/en>`_ , hence this node refers to the RGB primaries shared by these two color spaces as *sRGB/Rec.709*.

.. [#] See `encoding characteristics of AdobeRGB <http://www.color.org/chardata/rgb/adobergb.xalter>`_ specification.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames


