.. _label_as_manifold2d:

.. fix_img_align::

|
 
.. image:: /_images/icons/asManifold2D.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Manifold2D Icon

asManifold2D
************

A node that allows the user to transform UV coordinates, compute the filter sizes, define tiling, behavior outside of UV frame area, and more.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

.. _label_manifold2d_uv_coordinates:

UV Coordinates
^^^^^^^^^^^^^^

*UV Coordinates*
    The input UV coordinates to transform. If not set, they default to the U and V global primitive variables.

*UV Filter Size*
    Filter widths for the input UV coordinates. If not set, they might be computed if required further below in the shader.

*Bypass UV*
    Bypass UV transformations, but still allowing you to compute the UV filter widths if required.

*Compute Filters*
    Flag that toggles computing the UV coordinates filter widths.

-----

.. _label_manifold2d_uvframe:

UV Frame
^^^^^^^^

*Scale UV Frame*
    Scale the UV frame horizontally and vertically.

*Translate UV Frame*
    Translate the UV frame horizontally and vertically.

*Rotate UV Frame*
    Rotate the UV frame, where the unrestricted interval [0,1] maps to [0,360] degrees.

*Frame Center*
    Set the center of rotation X and Y coordinates.

*Wrap Along U*
    Set the U wrap mode for values outside the UV frame. These can be:

        * *Periodic*: where you have repeating tiles
        * *Mirror*: where you have mirrored repeating tiles
        * *Clamp*: coordinates clamped to the limits of the UV frame
        * *Default Color*: areas outside the UV frame take the default color set in nodes such as the :ref:`asTexture <label_as_texture>` node, in the form of its lookup failure color, or specific default color in other 2d texturing nodes. [#]_
        
*Wrap Along V*
    Set the V wrap mode for values outside the UV frame. These can be:

        * *Periodic*: where you have repeating tiles
        * *Mirror*: where you have mirrored repeating tiles
        * *Clamp*: coordinates clamped to the limits of the UV frame
        * *Default Color*: areas outside the UV frame take the default color set in nodes such as the :ref:`asTexture <label_as_texture>` node, in the form of its lookup failure color, or specific default color in other 2d texturing nodes.

-----

.. _label_manifold2d_transform_parameters:

Transform Parameters
^^^^^^^^^^^^^^^^^^^^

*UV Tiles*
    The number of UV tiles along U and V directions.

*Offset Tiles*
    An offset to apply to the tiles, along the U and V directions.

*Rotate Tiles*
    Rotate tiles along a center of rotation, where the input values on the unrestricted interval [0,1] map to [0,360] degrees rotation.

*Tiles Center*
    Center of rotation for tiles rotation.

*Tiles Coverage*
    Defines how much of the UV frame is covered by the texture.

-----

.. _label_manifold2d_tile_parameters:

Tile Parameters
^^^^^^^^^^^^^^^

*Mirror U*
    Mirror the tiles along the U direction.

*Mirror V*
    Mirror the tiles along the V direction.

*Wrap U*
    Wrap the tiles along the U direction.

*Wrap V*
    Wrap the tiles along the V direction.

*Stagger UV*
    Similar to Maya's *stagger*, it offsets alternate rows of tiles by half the tile width, creating a brickwall kind of effect.

*Swap UV*
    Swap the U and V coordinates.

-----

.. _label_manifold2d_noise_parameters:

Noise
^^^^^

*Distort UV*
    Applies a signed Perlin noise to the input UV coordinates, where the texturing coordinates are the original unmodified UV coordinates scaled by a factor of 15.0 to increase the noise frequency.

-----

.. _label_manifold2d_noise_outputs:

Outputs
-------

*UV Coords*
    The resulting UV coordinates.

*UV Filter Size*
    The filter widths for the UV coordinates, either bypassed, or final transformed UV coordinates.

-----

.. rubric:: Footnotes

.. [#] In the case of the :ref:`asTexture node <label_as_texture>` the color used outside the [0,1] UV frame is the lookup failure color. In other nodes, you might have the access in some nodes, to set a specific *default color*, in a way similar to what Maya does for example.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

