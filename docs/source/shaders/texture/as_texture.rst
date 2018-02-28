.. _label_as_texture:

.. fix_img_align::

|
 
.. image:: /_images/icons/asTexture.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Texture Icon

asTexture
*********

A texture lookup node with full control over OSL's texture() call.

|

Parameters
----------

.. bogus directive to silence warning::

-----

Texture
^^^^^^^

*Filename*
    The texture filename

*Atlas Type*
    The texture atlas type, it can be one of

        * None (ordinary texture, the *default*)
        * ZBrush [#]_
        * Mudbox
        * Mari [#]_

.. note::

   Though a full reference of UV tiles is outside the scope of this document, it suffices to say here that the ZBrush UV tiles pattern is in the form *u<N>_v<N>* with the tiles starting at 0, whilst Mudbox shares the same pattern but starts at 1. Mari uses UDIM tiles.

*Color*
    The default color to use if the texture lookup fails for any reason.

*Alpha*
    The default alpha value if there is none in the texture file, or if the lookup of the alpha channel fails for any reason.

*Starting Channel*
    The starting channel for the texture lookup. For an *RGBA* texture, the starting channel is 0, which is also the default.

*S Blur Amount*
    The amount of blur along the *s* texture coordinate, defaulting to 0.

*T Blur Amount*
    The amount of blur along the *t* texture coordinate, defauling to 0.

*S Filter Width*
    A scaling factor for the size of the texture filter as defined by the differentials or implicitly by the differentials of the *s* texture coordinates, with a default value of 1.0. A value of 0.0 turns off texture filtering.

*T Filter Width*
    A scaling factor for the size of the texture filter as defined by the differentials or implicitly by the differentials of the *t* texture coordinates, with a default value of 1.0. A value of 0.0 turns off texture filtering.
                                                                      
*S Wrap*
    The texture wrapping mode along the *s* direction, which can be one of

        * Default (the texture system default)
        * Black
        * Periodic
        * Clamp
        * Mirror

*T Wrap*
    The texture warpping mode along the *t* direction, which can be one of

        * Default (the texture system default)
        * Black
        * Periodic
        * Clamp
        * Mirror

*Interpolation Method*
    The texture interpolation method, which can take the following values

        * Smart Cubic (default)
        * Cubic
        * Linear
        * Closest

.. seealso::
   
   `This link on texture filtering <https://en.wikipedia.org/wiki/Texture_filtering>`_ for more details.

.. _label_as_texture_color_management:

Color Management
^^^^^^^^^^^^^^^^

*Enable CMS*
    Toggles the color management options *on* or *off*.

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

*Rendering RGB Primaries*
    It allows the user to set the RGB primaries of the rendering or working space, and it should match the choice of rendering/working space of the renderer.
    It can take the following values

        * sRGB/Rec.709
        * Rec.2020
        * DCI-P3
        * ACES
        * ACEScg

Texture Coordinates
^^^^^^^^^^^^^^^^^^^

*UV Coords*
    The *uv* texture coordinates.

*UV Filter Size*
    The computed filter size for the *uv* texture coordinates.

-----

Outputs
-------

*Output Color*
    The color resulting from the *Features Mode* choice.

*Output Alpha*
    The alpha resulting from the *Features Mode* choice, usually luminance of the color only.

*Output Single Channel*
    The output when the texture lookup is made on a greyscale image.

-----

.. rubric:: Footnotes

.. [#] For the ZBrush and Mudbox case, the UV tiles are assumed to be separated by underscores.

.. [#] This note assumes however, that the UDIM pattern will always come last before the filename extension. That is, if you are using an animated sequence or frames of an animated sequence, then the padded frame numbers **must** come before the UDIM pattern. I.e, ``<filename>.<padded frame numbers>.<UDIM>.<extension>.``

.. [#] See `ITU-R BT.1886 recommendation <https://www.itu.int/rec/R-REC-BT.1886-0-201103-I/en>`_ for details on the electro-optical transfer function.

.. [#] Because it makes no sense whatsoever to use colorimetry on non-color information or data, such as normal maps, or Z depth, motion vectors, and so on.

.. [#] sRGB shares the same CIE xy chromaticity coordinates with `ITU-R BT.709/Rec.709 <https://www.itu.int/rec/R-REC-BT.709/en>`_ , hence this node refers to the RGB primaries shared by these two color spaces as *sRGB/Rec.709*.

.. [#] See `encoding characteristics of AdobeRGB <http://www.color.org/chardata/rgb/adobergb.xalter>`_ specification.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

