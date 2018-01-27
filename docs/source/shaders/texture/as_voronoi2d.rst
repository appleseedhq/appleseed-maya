.. _label_as_voronoi2D:

.. fix_img_align::

|

.. image:: /_images/icons/asVoronoi2D.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Voronoi2D Icon

asVoronoi2D
***********

A procedural 2D Worley :cite:`Worley:1996:CTB:237170.237267` like noise shader, that outputs not only the resulting color, but the four nearest features to the evaluated point, their respective positions, and their cell color IDs. See also :cite:`Ebert:2002:TMP:572337`.

|

Parameters
----------

.. bogus directive to silence warning::

-----

Color Parameters
^^^^^^^^^^^^^^^^

*Color 1*
    Primary cell color.

*Color 2*
    Secondary cell color.

*Contrast*
    Contrast between primary and secondary cell color.

-----

Recursion Parameters
^^^^^^^^^^^^^^^^^^^^

*Amplitude*
    Controls the amplitute at each octave, including the starting iteration.

*Octaves*
    Number of iterations to perform, higher values lead to increasing detail, but increased computational cost as well.

*Lacunarity*
    Defines how large the gaps are in the cell noise with increasing octaves, higher values lead to higher gaps, lower values to small gaps.

*Persistence*
    The persistence of the fractal is a gain factor to apply to the amplitude at each iteration, but it only has an effect when the shader is set to the mode *pebbles*.

-----

Cell Parameters
^^^^^^^^^^^^^^^

*Density*
    The density of the cells, with higher values resulting in a higher number of cells in the same area.

*Jittering*
    How random the placement of the cells is, with low values resulting in a ordered grid of cells, and higher values resulting in aleatory placement of cells.

*Metric*
    Which metric to choose to calculate the distance from cell to feature points. There are several to choose from, resulting in different types of patterns.

* Euclidian distance [#]_
* Sum of square difference
* Tchebychev distance [#]_
* Sum of absolute difference
* Akritean distance
* Minkowski metric [#]_
* Karlsruhe metric [#]_

The sum of the square difference is also known as the Manhattan metric [#]_.

The Minkowski metric is a generalized metric whose P parameter allows you to go from the Euclidian distance when P has a value of 2, to the Manhattan distance when P has a value of 1, and as P reaches infinity, it represents the Tchebychev metric.

The Akritean distance if a weighted mix of the Euclidian distance, and the Tchebychev distance.

The Karlsruhe metric, also known as Moscow metric, is a radial metric, returns radial sections from a cell at the center.

*Minkowski Parameter*
    Controls the metric, with a value of 1 being the Manhattan distance, 2 being the Euclidian distance, and higher values tending to the Tchebychev metric as the parameter approaches infinity.

*Coverage*
    The Akritean distance coverage, or the weighting mix between the Euclidian distance and the Tchebychev distance.

*Features Mode*
    The features mode to use when computing the output color.

* Feature 1, or nearest feature from the cell
* Feature 2, or second nearest feature from the cell
* Feature 3, or third nearest feature from the cell
* Feature 4, or fourth nearest feature from the cell
* F1 + F2, or sum of first and second nearest features
* F2 - F1, or difference between second and first nearest features
* F1 * F2, or product of first and second nearest features
* F1 / F2, or division of first nearest feature by second nearest feature
* F1 ^ F2, nearest feature raised to the second nearest feature
* Pebbles, a mode that resembles pebbles
* Cell ID 1, the ID of the nearest feature to the cell
* Cell ID 2, the ID of the second nearest feature to the cell
* Cell ID 3, the ID of the third nearest feature to the cell
* Cell ID 4, the ID of the fourth nearest feature to the cell

.. note::

   The unmodified features, points and their color IDs are also output from the shader, giving the user greater creative potential. The feature modes above are but a starting point.

-----

Color Balance
^^^^^^^^^^^^^

The standard Maya color balance, gain, offset parameters. Please consult Maya's documentation for more information on these controls.

-----

Effects
^^^^^^^

The standard Maya effects parameters. Please consult Maya's documentation for more information on these controls.

-----

Coordinates
^^^^^^^^^^^

The input UV coordinates, typically from an upstream *placement2d* node.

-----

Outputs
-------

*Output Color*
    The color resulting from ghe *Features Mode* choice.

*Output Alpha*
    The alpha resulting from the *Features Mode* choice, usually luminance of the color only.

*Output Features*
    An array of 4 floats, containing the four nearest features to the cell.

*Output Positions*
    An array of 4 points, containing the center of the four nearest features to the cell.

*Output IDs*
    An array of 4 colors, containing the color IDs of the four nearest features to the cell.

.. warning:: presently OSL does not allow connections from/to array elements, and appleseed-maya is not enabling the array outputs for now. This will be addressed in a future release.

-----

.. _label_voronoi2d_screenshots:

Screenshots
-----------

Some examples of feature output modes and metrics.

.. thumbnail:: /_images/screenshots/voronoi2d/voronoi2d_euclidian_f1.png
   :group: shots_voronoi2d_group_A
   :width: 10%
   :title:

   Euclidian metric, with the first feature nearest to the evaluated cell.

.. thumbnail:: /_images/screenshots/voronoi2d/voronoi2d_euclidian_f2.png
   :group: shots_voronoi2d_group_A
   :width: 10%
   :title:

   Euclidian metric, with the second feature nearest to the evaluated cell.

.. thumbnail:: /_images/screenshots/voronoi2d/voronoi2d_euclidian_f1_divided_by_f2.png
   :group: shots_voronoi2d_group_A
   :width: 10%
   :title:

   Euclidian metric, with the first nearest featured divided by the second nearest feature.

.. thumbnail:: /_images/screenshots/voronoi2d/voronoi2d_euclidian_f1_plus_f2.png
   :group: shots_voronoi2d_group_A
   :width: 10%
   :title:

   Euclidian metric, with the first and second nearest features to the cell added.

.. thumbnail:: /_images/screenshots/voronoi2d/voronoi2d_euclidian_pebbles.png
   :group: shots_voronoi2d_group_A
   :width: 10%
   :title:

   Euclidian metric, set to *pebbles* mode, one of the many possible combinations of expressions involving the four nearest features to the cell.

.. thumbnail:: /_images/screenshots/voronoi2d/voronoi2d_minkowski_p_0.5.png
   :group: shots_voronoi2d_group_A
   :width: 10%
   :title:

   Nearest feature to the cell with the Minkowski metric with P parameter set to 0.5.

.. thumbnail:: /_images/screenshots/voronoi2d/voronoi2d_euclidian_f2_minus_f1.png
   :group: shots_voronoi2d_group_A
   :width: 10%
   :title:

   Euclidian metric, with the difference between the second nearest feature and the nearest feature.

.. thumbnail:: /_images/screenshots/voronoi2d/voronoi2d_euclidian_cell_id4.png
   :group: shots_voronoi2d_group_A
   :width: 10%
   :title:

   Euclidian metric, with the cell IDs of the fourth nearest feature.

-----

.. rubric:: Footnotes

.. [#] The Euclidian distance or Euclidian metric, also known as :math:`L_2` norm, is the straight line distance between two points in Euclidian space.

.. [#] The Chebyshev (or Tchebychev) distance, also known as Chessboard distance or :math:`L_\infty` norm, is a metric on a vector space where the distance between two vectors is the greatest of their differences along any coordinate dimension.

.. [#] The Minkowski distance, also known as :math:`L_P` norm, is a metric which is a generalization of both the Euclidian distance and the Manhattan distance, being equal to the Manhattan distance when its *p* parameter is equal to 1, or equal to the Euclidian distance when its *p* parameter is equal to 2. On the limit as *p* approaches infinity, it is equal to the Chebyshev distance.

.. [#] In metric geometry, the Karlsruhe metric is a measure of distance that assumes travel is only possible along rays through the origin and circular arcs centered at the origin.

.. [#] The Manhattan distance, also known as *taxicab* metric or :math:`L_1` norm, is a metric in which the distance between two points is the sum of the absolute differences of their Cartesian coordinates.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

