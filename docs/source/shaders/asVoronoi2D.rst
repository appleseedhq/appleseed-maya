.. _label_asVoronoi2D:
.. image:: ../images/asVoronoi2D.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Voronoi2D Icon

***********
asVoronoi2D
***********

A procedural 2D Worley noise like shader, outputs not only the resulting
color, but the 4 features near the evaluated point, their respective positions, and the color IDs.

==========
Parameters
==========


    Color Parameters
    ----------------

    *Color 1*
        Primary cell color.

        *Color 2*
            Secondary cell color.

        *Contrast*
            Contrast between primary and secondary cell color.


    Recursion Parameters
    --------------------

        *Amplitude*
            Controls the amplitute at each octave, including the starting iteration.

        *Octaves*
            Number of iterations to perform, higher values lead to increasing detail, but increased computational cost as well.

        *Lacunarity*
            Defines how large the gaps are in the cell noise with increasing octaves, higher values lead to higher gaps, lower values to small gaps.

        *Persistence*
            The persistence of the fractal.

    Cell Parameters
    ---------------

        *Density*
            The density of the cells, with higher values resulting in a higher number of cells.

        *Jittering*
            How random the placement of the cells is, with low values resulting in a ordered grid of cells, and huigh values resulting in aleatory placement of cells.

        *Metric*
            Which metric to choose to calculate the distance from cell to feature points. There are several to choose from, resulting in different types of patterns.

                * Euclidian distance
                * Sum of square difference
                * Tchebychev distance
                * Sum of absolute difference
                * Akritean distance
                * Minkowski metric
                * Karlsruhe metric

            The Minkowski metric is a generalized metric whose P parameter allows you to go from Euclidian distance to Tchebychev distance.
            The Akritean distance if a weighted mix of the Euclidian distance, and the Tchebychev distance.
            The Karlsruhe metric, also known as Moscow metric, is a radial metric, returns radial sections from a cell at the center.

        *Minkowski Parameter*
            Controls the metric, with a value of 1 being the Manhattan distance, 2 being the Euclidian distance, and higher values tending to the Tchebychev metric as the parameter approaches infinity.

        *Coverage*
            The Akritean distance coverage, or the weighting mix between the Euclidian distance and the Tchebychev distance.

        *Features Mode*
            The features mod to use when computing the output color. Note that the unmodified features and points are also output from the shader, giving the user greater creative potential.
            The feature modes set, and these are only starting points, are:

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


    Color Balance
    -------------

        The standard Maya color balance, gain, offset parameters.

    Effects
    -------

        The standard effects parameters

=======
Outputs
=======

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


