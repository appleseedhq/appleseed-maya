.. _label_tutorial_as_color_transform:

*******************
Transforming Colors
*******************


In this example, we'll be adjusting the colors of an input texture. Ideally
this would be done offline, but sometimes that's either not convenient or
there is a specific need to do it online as a part of a particular effect,
for instance, driven by an expression or other parameters.

-----

.. only:: html

    .. css3image:: /_images/tutorials/tutorial2/pexel_chalk_original.png
        :scale: 30%
        :margin-left: 30px
        :margin-right: 30px
        :margin-top: 30px
        :margin-bottom: 30px
        :border-radius: 20px
        :align: right

.. only:: latex

    .. image:: /_images/tutorials/tutorial2/pexel_chalk_original.png
        :scale: 30%
        :align: center 

|
|

Given the example image, we want to adjust the *redness* or *greenness* of the
image, without changing its lightness, or giving it an overall tint shift. We
connect the output color to an
:ref:`asColorTransform<label_as_color_transform>` node's input.
The input color is *RGB* and we will be outputting to *CIELAB* [#]_ color
space, decomposing the output components. The first one is *lightness*, the
second one is a\* and the second one is b\*.

|
|
|
|

.. only:: html

    .. css3image:: /_images/tutorials/tutorial2/Lab_workflow_asColorTransform_network.png
       :scale: 30%
       :margin-left: 30px
       :margin-right: 30px
       :margin-top: 20px
       :margin-bottom: 30px
       :border-radius: 20px
       :align: left

.. only:: latex

   .. image:: /_images/tutorials/tutorial2/Lab_workflow_asColorTransform_network.png
      :scale: 30%
      :align: center

|

The shading network shows the texture note RGB output connected to the
:ref:`asColorTransform<label_as_color_transform>` , a *remapValue* node
changing one of the components, and the round trip back to RGB via a second
:ref:`asColorTransform<label_as_color_transform>` node.

|
|
|

.. only:: html

    .. css3image:: /_images/tutorials/tutorial2/Lab_workflow_asColorTransform_UI.png
       :scale: 50%
       :margin-left: 30px
       :margin-right: 30px
       :margin-top: 20px
       :margin-bottom: 30px
       :border-radius: 20px
       :align: right

.. only:: latex

   .. image:: /_images/tutorials/tutorial2/Lab_workflow_asColorTransform_UI.png
      :scale: 60%
      :align: center

|
|

Set the input to *RGB* and output to *CIELAB* color space.
The a\* component is responsible for the magenta/green opposition. A value of
0.5 is grey, lower values shift towards magenta, higher values shift towards
green.

The same applies to the b\* component, but this time it regard to the
blue/yellow opposition instead.

|
|
|
|

.. only:: html

    .. css3image:: /_images/tutorials/tutorial2/Lab_workflow_asColorTransform_network.png
       :scale: 30%
       :margin-left: 30px
       :margin-right: 30px
       :margin-top: 20px
       :margin-bottom: 30px
       :border-radius: 20px
       :align: left

.. only:: latex

    .. image:: /_images/tutorials/tutorial2/Lab_workflow_asColorTransform_network.png
       :scale: 40%
       :align: center

|

Since we just want to adjust the intensity of the magentas and greens in the image, the *Lightness* and *b* are unchanged. We connect these to another :ref:`asColorTransform<label_as_color_transform>` node's input components, and set its input mode to *CIELAB* and its output to *RGB*, then connect the *a* component to a standard Maya *remapValue* node's input value 

|
|

.. only:: html

    .. css3image:: /_images/tutorials/tutorial2/Lab_workflow_remapValue.png
       :scale: 40%
       :margin-left: 30px
       :margin-right: 30px
       :margin-top: 20px
       :margin-bottom: 30px
       :border-radius: 20px
       :align: right

.. only:: latex

    .. image:: /_images/tutorials/tutorial2/Lab_workflow_remapValue.png
       :scale: 40%
       :align: center

|
|
|
|

Leaving the position 0.5 with a value of 0.5 (greys unchanged, without a color
shift), we apply a S like curve. This S like curve will increase the magentas
and greens in a way similar to the S like tonal curves you are familiar with
in image editing applications.

|
|
|
|
|
|

.. only:: html

    .. css3image:: /_images/tutorials/tutorial2/Lab_workflow_remapValue_ramp.png
       :scale: 50%
       :margin-left: 30px
       :margin-right: 30px
       :margin-top: 20px
       :margin-bottom: 30px
       :border-radius: 20px
       :align: left

.. only:: latex

   .. image:: /_images/tutorials/tutorial2/Lab_workflow_remapValue_ramp.png
       :scale: 50%
       :align: center

|

The S like curve used. Conversely, a flattening curve would bring the magent
and greens towards the 0.5 values (the greys), flattening the colors.

|
|
|

If we wanted to do the same to the blues and yellows, we would be using the b*
components of the *CIELAB* color space instead.
One could also just want to increase the *blueness* of the image, or flatten
the greens, or affect the lightness of the image only, leaving its colors
unchanged.

|
|

-----

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

    Changing the *lightness* channel.

.. thumbnail:: /_images/tutorials/tutorial2/pexel_chalk_Lab_isolate_blue_only.png
    :group: asColorTransform group A
    :width: 10%
    :title:

    Desaturating only the blue chalk, raising the blues in the *b* channel towards the grey value 0.5.

-----

.. rubric:: Footnotes


.. [#] CIELAB color space, https://en.wikipedia.org/wiki/Lab_color_space
   
   Colorimetry - Part 4: CIE 1976 L*a*b* Colour Space `pdf <http://www.unife.it/scienze/astro-fisica/insegnamenti/ottica-applicata/materiale-didattico/colorimetria/CIE%20DS%20014-4.3.pdf>`_

