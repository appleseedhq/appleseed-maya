.. _label_as_vary_color:

.. fix_img_align::

|
 
.. image:: /_images/icons/asVaryColor.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Vary Color Icon

asVaryColor
***********

A color variation utility node, meant to automate the coloring and shading of large numbers of objects in a scene based on user-set criteria such as object or instance names or IDs.

Parameters
----------

.. bogus directive to silence warnings::

-----

Color Parameters
^^^^^^^^^^^^^^^^

*Input Color*
    The original input color.

*Color Mode*
    How to apply the manifold color to the input color. The manifold generated value can be added to the input color, it can modulate the input color, or it can bypass it completely, via the parameters

    * Add
    * Scale
    * Override

-----

Manifold Parameters
^^^^^^^^^^^^^^^^^^^

*Manifold Type*
    The manifold type to use for the variation. It can take the following values

        * Object Name
        * Object Instance Name
        * Assembly Name
        * Assembly Instance Name
        * Face ID
        * String Prefix
        * String Suffix
        * Find String

String Parameters
"""""""""""""""""

*Expression*
    When *Manifold Type* is set to *String Prefix*, *String Suffix* or *Find String*, the expression can be a regex [#]_ expression defining the pattern to search in the expression domain.

*Domain*
    The domain to search the expression for. The domains can take the values

        * Object Name
        * Object Instance Name
        * Assembly Name
        * Assembly Instance Name

*Seed*
    An extra seed to provide some measure of determinism in the resulting colors.

-----

Variation Parameters
^^^^^^^^^^^^^^^^^^^^

*Variation Mode*
    This parameter controls what exactly is going to be *randomized* or vary, according to the user-set options outlined earlier. One can vary the individual (and/or full) components of the input color in HSV [#]_ space, in RGB or in CIELAB [#]_ space. Accordingly this parameter takes the following values

        * HSV
        * RGB
        * CIE L\*a\*b\* 1976

HSV Variation Parameters
""""""""""""""""""""""""

*Vary Hue*
    The extent or scaling factor of the *hue* variation.

*Vary Saturation*
    The extent or scaling factor of the *saturation* variation.

*Vary Value*
    The extent or scaling factor of the *value* variation.

RGB Variation Parameters
""""""""""""""""""""""""

*Vary Red*
    The extent or scaling factor of the variation of the *red* channel.

*Vary Green*
    The extent or scaling factor of the variation of the *green* channel.

*Vary Blue*
    The extent or scaling factor of the variation of the *blue* channel.

CIELAB Variation Parameters
"""""""""""""""""""""""""""

*Vary L\\**
    The extent or scaling factor of the variation of the *lightness* or *L\\** channel.

*Vary a\\**
    The extent or scaling factor of the variation of the *a\\** channel.

*Vary b\\**
    The extent or scaling factor of the variation of the *b\\** channel.

-----

Outputs
-------

*Output Color*
    The resulting *randomized* color.

*Output Hash*
    An integer hash ID.

*Output ID*
    A color ID.

*Output Greyscale*
    A greyscale ID.

-----

.. rubric:: Footnotes

.. [#] Regular expressions, `or regex <https://en.wikipedia.org/wiki/Regular_expression>`_. If you're unfamiliar with it, it allows the creation of complex patterns for string and substring matching. You can validate your expressions `here at regex101 <https://regex101.com/>`_.

.. [#] A different color representation based on hue, saturation and value. See `HSV color space <https://en.wikipedia.org/wiki/HSL_and_HSV>`_ for more details.

.. [#] Also known as *Lab* color space, but it's in fact referring to CIE 1976 L\*a\*b\* color space, or CIELAB. See `CIELAB color space <https://en.wikipedia.org/wiki/Lab_color_space>`_ for more details.

