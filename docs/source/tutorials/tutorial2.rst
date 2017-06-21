.. _label_tutorial2:

===================
Transforming Colors
===================

In this example, we'll be adjusting the colors of an input texture.
Ideally this would be done offline, but sometimes it's either not possible, or it's part of a particular effect, or expression driven.

.. figure:: /_images/tutorials/tutorial2/pexel_chalk_original.png
   :scale: 50%
   :alt: Original Image
   
   Given the example image
   
   we want to adjust the *redness* or *greeness* of the image, without changing its lightness or giving it an overall tint shift. We connect the output color to an :ref:`asColorTransform<label_as_color_transform>` node's input.


here



.. thumbnail:: /_images/tutorials/tutorial2/Lab_workflow_asColorTransform_network.png
   :width: 50%
   :title:

   Shading network showing texture node RGB to asColorTransform space, a remapValue changing one of the components, and a round trip back to RGB again via asColorTransform.  

The input color is *RGB*, and we will be outputting to *CIELAB* color space.
Decomposing the output components, the first one is *Lightness*, the second one is *a* and the third one is *b*.

.. thumbnail:: /_images/tutorials/tutorial2/Lab_workflow_asColorTransform_UI.png
   :width: 50%
   :title:

   Setting the input space to *RGB* and the output space to *CIELAB*.

The *a* component is responsible for the magenta/green opposition. A value of 0.5 does not shift the color, while lower values shift towards magenta, and higher values shift towards green.

The same applies to the *b* component, but this time in regard to the blue and yellow opposition instead.

Since we just want to adjust the intensity of the magentas and greens in the image, the *Lightness* and *b* are unchanged. We connect these to another :ref:`asColorTransform<label_as_color_transform>` node's input components, and set its input mode to *CIELAB* and its output to *RGB*.

We connect the *a* component to a standard Maya *remapValue* node's input value

.. thumbnail:: /_images/tutorials/tutorial2/Lab_workflow_remapValue.png
   :width: 50%
   :title:

   Remapping the *a* component of our texture, converted to the *CIELAB* color space.

and its output value to the second :ref:`asColorTransform<label_as_color_transform>` corresponding component input, and then proceed to edit the ramp.
Leaving the 0.5 position with a value of 0.5 (no shift in the greys) we apply a S like curve. This S like curve will increase the magentas and greens, in a way similar to the S like contrast curves you are familiar with in image editing applications.

.. thumbnail:: /_images/tutorials/tutorial2/Lab_workflow_remapValue_ramp.png
   :width: 50%
   :title:

   Our *S* like curve, leaving the greys neutral (position 0.5, value 0.5)

Conversely a flattening curve would bring the magenta and greens towards the 0.5 values, the greys, flattening the colors.

If we wanted to do the same to the blues and yellows, we would be using the *b* component of the *CIELAB* color space instead.

One could only just want to increase the *blueness* of the image, or flatten the greens, or affect the lightness of the image only, leaving its colors unchanged.

Screenshots
-----------

.. thumbnail:: /_images/tutorials/tutorial2/pexel_chalk_original.png
    :group: asColorTransform group A
    :width: 10%
    :title:

    Original image

.. thumbnail:: /_images/tutorials/tutorial2/pexel_chalk_Lab_a_remap.png
    :group: asColorTransform group A
    :width: 10%
    :title:

    Changing the magenta and green with a S like curve on the *a* channel.

.. thumbnail:: /_images/tutorials/tutorial2/pexel_chalk_Lab_b_remap.png
    :group: asColorTransform group A
    :width: 10%
    :title:

    Now changing the blues and yellows with an S like curve on the *b* channel.

.. thumbnail:: /_images/tutorials/tutorial2/pexel_chalk_Lab_L_remap_S.png
    :group: asColorTransform group A
    :width: 10%
    :title:

    Original image

.. thumbnail:: /_images/tutorials/tutorial2/pexel_chalk_Lab_isolate_blue_only.png
    :group: asColorTransform group A
    :width: 10%
    :title:

    Desaturating only the blue chalk, raising the blues in the *b* channel towards the grey value 0.5.

