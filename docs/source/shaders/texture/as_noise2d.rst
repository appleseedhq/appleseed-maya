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

        * Perlin
        * Simplex
        * Value
        * Voronoise
        * Gabor

*Intensity*
    The global noise intensity.

*X Frequency*
    The noise frequency along the X direction.

*Y Frequency*
    The noise frequency along the Y direction.

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
    Maximum number of iterations.

*Cascade*
    The type of iteration to perform. It can be

        * Additive
        * Multiplicative

    In  the first case, the results of each iteration are accumulated, and in the second case, they are multiplied with the previous product.

*Lacunarity*
    Control for the gap between successive noise frequencies.

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

Some examples of feature output modes and metrics.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_corrosion_soft.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Euclidian metric, with the first feature nearest to the evaluated cell.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_fBm.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Euclidian metric, with the second feature nearest to the evaluated cell.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_granitical.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Euclidian metric, with the first nearest featured divided by the second nearest feature.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_metalaging.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Euclidian metric, with the first and second nearest features to the cell added.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_ridged.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Euclidian metric, set to *pebbles* mode, one of the many possible combinations of expressions involving the four nearest features to the cell.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_turbulence.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Nearest feature to the cell with the Minkowski metric with P parameter set to 0.5.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_viral.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Euclidian metric, with the difference between the second nearest feature and the nearest feature.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_weave.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Euclidian metric, with the cell IDs of the fourth nearest feature.

.. thumbnail:: /_images/screenshots/noise2d/as_noise2d_zebra.png
   :group: shots_noise2d_group_A
   :width: 10%
   :title:

   Euclidian metric, with the cell IDs of the fourth nearest feature.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

