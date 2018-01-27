.. _label_as_noise2D:

.. fix_img_align::

|

.. image:: /_images/icons/asNoise2D.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Noise2D Icon

asNoise2D
*********

A fractal noise node, with recursion, and an ample choice of noise primitives.

|

Parameters
----------

.. bogus directive to silence warning::

-----

Color Parameters
^^^^^^^^^^^^^^^^

*Color 1*
    Primary color.

*Color 2*
    Secondary color.

*Contrast*
    Contrast between primary and secondary colors.

-----

Noise Parameters
^^^^^^^^^^^^^^^^

*Type*
    The noise primitive used. It can take the following values

        * Perlin :cite:`Perlin:1985:IS:325165.325247`
        * Simplex :cite:`Perlin:2002:IN:566654.566636`
        * Value
        * Voronoise [#]_ :cite:`Worley:1996:CTB:237170.237267`
        * Gabor :cite:`Galerne:2012:GNE:2185520.2185569`, :cite:`Lagae:2009:PNU:1531326.1531360`

*Intensity*
    The global noise intensity.

*X Frequency*
    The noise frequency along the *x* direction.

*Y Frequency*
    The noise frequency along the *y* direction.

*Ridges*
    Toggling this checkbox will enable the *Ridged Noise* mode. A noise mode more suited to modelling the appearance of crests, mountains, when used to drive a displacement or bump map.

*Inflection*
    Enabling this checkbox will force the noise function to return a absolute value (if the noise was in [-1,1] range to begin with, otherwise it won't have any effect).

*Signed Noise*
    Enabling this checkbox makes the noise function return values in the [-1,1] range, and disabling it will return values in [0,1] range.

Motion Parameters
^^^^^^^^^^^^^^^^^

*Animated Noise*
    Enabling this checkbox will animate the noise along time.

*Frame Time*
    Frame time, typically the frame number.

*Time Scale*
    Global time scale, affects the frame time.

Periodic Parameters
^^^^^^^^^^^^^^^^^^^

*Periodic*
    Enabling this checkbox will enable periodic noise, with a user-set *x* and *y* period.

*X Period*
    The *x* period when using periodic noise.

*Y Period*
    The *y* period when using periodic noise.

Voronoise Parameters
^^^^^^^^^^^^^^^^^^^^

*Smoothness*
    Controls the smoothness of the generalized Voronoi noise, with low values having a sharp cell boundary, and high values having a smooth Perlin noise like appearance.

*Jittering*
    Controls the jittering of the Voronoi cells, with low values producing a regular cell grid, and high values producing a randomized cell grid.

Gabor Parameters
^^^^^^^^^^^^^^^^

*Anisotropy*
    This parameter controls the type of Gabor noise used. It can take the values

        * Isotropic
        * Anisotropic
        * Hybrid

*Direction*
    Anisotropy vector to use when the Gabor noise *Anisotropy* mode is set to *Anisotropic*.

*Bandwidth*
    The bandwidth for the Gabor noise.

*Impulses*
    The number of impulses for the Gabor noise.

*Filter Noise*
    Enabling this checkbox will produce antialiased noise.

-----

Recursion Parameters
^^^^^^^^^^^^^^^^^^^^

*Amplitude*
    Initial noise amplitude before recursion.

*Octaves*
    The number of interations to perform.

*Cascade*
    The type of iteration to perform. It can be

        * Additive
        * Multiplicative

    In  the first case, the results of each iteration are accumulated, and in the second case, they are multiplied with the previous product.

*Lacunarity*
    Control for the gap between successive noise frequencies (sucessive octaves).

*Offset*
    Controls the multifractality.

*Gain*
    Controls the contrast of the fractal.

*Distortion*
    This parameter distorts the domain of the coordinates for each frequency.

-----

Outputs
^^^^^^^

*Output Color*
    The color resulting from ghe *Features Mode* choice.

*Output Alpha*
    The alpha resulting from the *Features Mode* choice, usually luminance of the color only.

-----

.. _label_noise2d_screenshots:

Screenshots
-----------

Some examples of what can be achieved, and is provided as presets.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_coral.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Gabor noise used as the noise *primitive* set to *Hybrid* anisotropy mode, in a recursive manner, with successive frequencies accumulated. That is, with the *Cascade Mode* set to additive. This helps creating the appearance of a coral like structure, specially if used to drive a displacement or bump.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_corrosion_soft.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   A texture generated using inflected signed Value noise, with successive noise frequencies accumulated, creating the appearance of a soft corrosion like texture. Using this texture as a mask to :ref:`asLayerShader <label_as_layer_shader>` to blend a metal and a rust like material, or as a mask to blend colors for a :ref:`asMetal <label_as_metal>` node, produces good results.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_granitical.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   A signed Perlin noise, with the product of 8 frequencies, that is, with the *Cascade Mode* set to *Multiplicative*. This texture would work well as a mask to map or to ramp colors for granite, specially when used in conjunction with a :ref:`asSubsurface <label_as_subsurface>` node.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_metalaging.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Generalized Voronoi, also known as *Voronoise*, with medium jittering in order not to completely break the patterning order, and low smoothness. When sucessive frequencies are accumulated, it helps creating the appearance of galvanized metal. It would work great with :ref:`asMetal <label_as_metal>`.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_ridged.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Ridged multifractal, which works great driving displacements or bumps to create the appearance of terrain ridges.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_turbulence.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Turbulence using inflected signed Perlin noise.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_viral.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Appearance of virus or bacteria, created using inflected and ridged Gabor noise set to *Hybrid* anisotropy, with successive frequencies accumulated.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_weave.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Appearance of weave patterns using inflected and ridged Gabor noise set to *Hybrid* anisotropy, with successive frequencies accumulated. This would work well as a base texture to threshold in order to drive a transparency mask, and as a texture controlling displacement or bump, color mapping.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_zebra.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Finally, a texture with the appearance of zebra patterns, created signed Gabor noise set to *Anisotropic* mode, with successive frequencies accumulated.

-----

.. rubric:: Footnotes

.. [#] Also known as generalized Voronoi. See `Inigo Quilez article on voronoise <http://www.iquilezles.org/www/articles/voronoise/voronoise.htm>`_.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

