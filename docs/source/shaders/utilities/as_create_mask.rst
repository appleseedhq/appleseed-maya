.. _label_as_create_mask:

.. fix_img_align::

|
 
.. image:: /_images/icons/asCreateMask.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Create Mask Icon

asCreateMask
************

A node that allows the user to create a greyscale mask of a input color or texture.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Color Parameters
^^^^^^^^^^^^^^^^

*Input Color*
    The color used to create the mask from. It's expected to be *scene-linear*.

*Input Alpha*
    The alpha channel of the color used to create the mask from.

*Threshold Channel*
    The channel to use when applying a threshold in order to build a mask.
    It can be one of

        * Red
        * Green
        * Blue
        * Alpha
        * Hue
        * Saturation
        * Value
        * CIELAB L\* [#]_
        * CIELAB a\*
        * CIELAB b\*
        * Average
        * Luminance [#]_

*Threshold Value*
    The threshold value to apply to the channel chosen.

*Threshold Function*
    The function to use for the threshold. It can be one of

        * None
        * Step
        * Linear Step
        * Smooth Step
        * Exponential
        * Double Circled Seat [#]_
        * Double Circled Sigmoid [#]_
        * Smoother Step [#]_
        * Smoothest Step [#]_

*Threshold Contrast*
    The contrast to apply when the function is *Double Circled Seat* (a contrast flattening curve) or *Double Circled Sigmoid* (a contrast increasing curve).

*Threshold Lower Bound*
    The lower bound for the *smoothstep*, *smootherstep* and *smootheststep* functions.

*Threshold Upper Bound*
    The upper bound for the *smoothstep*, *smootherstep* and *smootheststep* functions.

-----

Outputs
-------

*Result*
    The new mask.

-----

.. _label_as_create_mask_screenshots:

Screenshots
-----------

Some examples showing some of the masks created with the modes outlined above.

.. thumbnail:: /_images/screenshots/create_mask/as_createmask_reference.png
   :group: shots_create_mask_group_A
   :width: 10%
   :title:

   A ColorChecker Classic reference shot.

.. thumbnail:: /_images/screenshots/create_mask/as_createmask_average.png
   :group: shots_create_mask_group_A
   :width: 10%
   :title:

   Greyscale mask created from the average of the R,G,B channels.

.. thumbnail:: /_images/screenshots/create_mask/as_createmask_hicontrast_double_circled_sigmoid.png
   :group: shots_create_mask_group_A
   :width: 10%
   :title:

   A sigmoid curve, increasing contrast on the average of the R,G,B channels.

.. thumbnail:: /_images/screenshots/create_mask/as_createmask_hue_thresholded_inverted_smoothest_step.png
   :group: shots_create_mask_group_A
   :width: 10%
   :title:

   A mask created from the hue of the input color or texture, after which a smoothest step function was applied with swapped lower and upper bounds, inverting the result.

.. thumbnail:: /_images/screenshots/create_mask/as_createmask_lowcontrast_double_circled_seat_from_avg.png
   :group: shots_create_mask_group_A
   :width: 10%
   :title:

   The average of the R,G,B channels modified by a *seat function*, the opposite of a sigmoid function, which will flatten the contrast. In image editing applications, the sigmoid is the typical *S* like curve one applies to increase contrast, and the *seat function* is the flattening curve.

.. thumbnail:: /_images/screenshots/create_mask/as_createmask_saturation.png
   :group: shots_create_mask_group_A
   :width: 10%
   :title:

   A mask created from the saturation of the image (when converted to *HSV* or *Hue*, *Saturation*, *Value*).

.. thumbnail:: /_images/screenshots/create_mask/as_createmask_thresholded_linearstep_bluechannel.png
   :group: shots_create_mask_group_A
   :width: 10%
   :title:

   The blue channel being thresholded by a *linearstep* function with a moderate lower bound.

.. thumbnail:: /_images/screenshots/create_mask/as_createmask_astar.png
   :group: shots_create_mask_group_A
   :width: 10%
   :title:

   A mask created from the *a\\** channel of the image when in CIELAB (or CIE 1976 L\\*a\\*b\\*) space.

.. thumbnail:: /_images/screenshots/create_mask/as_createmask_thresholded_smoothstep_red_green_opponency_astar.png
   :group: shots_create_mask_group_A
   :width: 10%
   :title:

   A mask created from the *a\\** channel (opponency of the *Red* and *Green* colors) of the color in *CIELAB* (or CIE 1976 L\*a\*b\*), to which a *smoothstep* function was applied in order to further shape the final output values.

-----

.. rubric:: Footnotes

.. [#] CIELAB or `CIE 1976 L*a*b* color space <https://en.wikipedia.org/wiki/Lab_color_space>`_.

.. [#] For this this assumes the input color is using the `ITU-R BT.709 <https://www.itu.int/rec/R-REC-BT.709/>`_/`Rec.709 RGB <https://en.wikipedia.org/wiki/Rec._709>`_ primaries. Once support for working or rendering space using other primaries other than Rec.709 is added to appleseed, this will be extended.

.. [#] A contrast flattening function, see `Double Circled Seat function <http://www.flong.com/texts/code/shapers_circ/>`_.

.. [#] A contrast increasing function, see `Double Circled Sigmoid function <http://www.flong.com/texts/code/shapers_circ/>`_.

.. [#] A smoother *smoothstep* function, with 0 first and second derivatives at x=0 and x=1.

.. [#] Like *smootherstep*, but with 0 third derivatives at x=0, and x=1.

