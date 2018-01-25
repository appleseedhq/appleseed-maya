.. _label_as_luminance:

.. fix_img_align::

|
 
.. image:: /_images/icons/asLuminance.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Luminance Icon

asLuminance
***********

A node that returns the luminance of a color, respecting the color space
definitions (that is, the chromaticity coordinates of the primaries and the
white point).

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Color Attributes
^^^^^^^^^^^^^^^^

*Input Color*
    The color being evaluated

-----

Color Space
^^^^^^^^^^^

*Derive From Maya CMS*
    Uses the render space definitions from Maya's synColor. This will set the chromaticity coordinates of the RGB primaries, and the white point, standardized in the color space chosen in the synColor configuration.
    When this is not set, the user **must** set the appropriate options matching its choice of rendering/working space.

.. important:: appleseed and appleseed-maya don't yet take `OpenColorIO <http://opencolorio.org/>`_ into account, so this parameter considers the working space definitions from synColor **only**. If you wish to use OCIO you **must** set the appropriate color space and white point settings. The default is (scene-linear) sRGB/Rec.709 primaries, with D65 whitepoint. 

*Input Color Space*
    The color space chosen as render/working space. It allows the user to choose one of the following

    * ACES 2065-1 AP0 (D60) :cite:`7289895`
    * ACEScg AP1 (D60) :cite:`Duiker:2015:ACC:2791261.2791273`
    * Rec.2020 (D65) :cite:`6784055`
    * DCI-P3 (DCI) :cite:`7290729`
    * sRGB/Rec.709 (D65)
    * Chromaticity Coordinates

.. hint::
   
   When choosing *Chromaticity Coordinates*, the user **must** enter the xy chromaticity coordinates of the R,G,B primaries, and **must** choose the whitepoint, either in the form of one of the available standard illuminants, in the xy chromacity coordinates of the whitepoint, or via the correlated color temperature.


*R xy Coordinates*
    The xy chromaticity coordinates of the red primary.

*G xy Coordinates*
    The xy chromaticity coordinates of the green primary.

*B xy Coordinates*
    The xy chromaticity coordinates of the blue primary.

*White Point*
    The white point definition, which can be one of the following options:

    * Standard Illuminant D50
    * Standard Illuminant D55
    * Standard Illuminant D60
    * Standard Illuminant D65
    * Standard Illuminant D75
    * DCI White Point
    * White Point E
    * Correlated Color Temperature
    * White Point Chromaticity Coordinates

.. _label_color_temperature:

*Color Temperature*
    The input color temperature value in Kelvin degrees, from 1667K to 25000K.

*W xy Coordinates*
    The xy chromacity coordinates of the white point.

-----

Outputs
-------

*Result*
    The luminance of the input color.

-----

.. _label_as_luminance_screenshots:

Screenshots
-----------

Some examples of the output luminance of the input color ramp, rendered in (scene linear) Rec.709 space, standard illuminant D65, with different color spaces and whitepoints chosen. The mismatches in color spaces are for illustration purposes. If the settings cannot be derived automatically from your DCC application, then the choice of color space should match your choice or render/working space.

.. thumbnail:: /_images/screenshots/luminance/luminance_colorramp_workingspace_rec709.png
   :group: shots_luminance_group_A
   :width: 10%
   :title:

   Original color ramp, synColor render/working space set to (scene-linear) sRGB/Rec.709 primaries and D65 white point.

.. thumbnail:: /_images/screenshots/luminance/luminance_colorramp_workingspace_rec709_from_CMS.png
   :group: shots_luminance_group_A
   :width: 10%
   :title:

   Luminance of input color, with settings automatically retrieved from Maya's synColor CMS preferences.

.. thumbnail:: /_images/screenshots/luminance/luminance_colorramp_set_ACES_AP0.png
   :group: shots_luminance_group_A
   :width: 10%
   :title:

   Original color ramp, with CMS settings disabled, and the input space overriden to ACES 2065-1 AP0, D60 whitepoint.

.. thumbnail:: /_images/screenshots/luminance/luminance_colorramp_set_ACES_AP1.png
   :group: shots_luminance_group_A
   :width: 10%
   :title:

   Original color ramp, with CMS settings disabled, and the input space overriden to ACEScg AP1, D60 whitepoint.

.. thumbnail:: /_images/screenshots/luminance/luminance_colorramp_set_Rec2020.png
   :group: shots_luminance_group_A
   :width: 10%
   :title:

   Original color ramp, with CMS settings disabled, and the input space overriden to Rec.2020, D65 whitepoint.

.. thumbnail:: /_images/screenshots/luminance/luminance_colorramp_set_DCIP3.png
   :group: shots_luminance_group_A
   :width: 10%
   :title:

   Original color ramp, with CMS settings disabled, and the input space overriden to DCI-P3, DCI whitepoint.

.. thumbnail:: /_images/screenshots/luminance/luminance_colorramp_explicit_coords_adobergb.png
   :group: shots_luminance_group_A
   :width: 10%
   :title:

   Original color ramp, with CMS settings disabled, and the input color space set to *xy chromacitity coordinates*, which were then set to the RGB chromaticity coordinates of the AdobeRGB 1998 color space, with a D65 whitepoint.

.. thumbnail:: /_images/screenshots/luminance/luminance_compared.png
   :group: shots_luminance_group_A
   :width: 10%
   :title:

   Starting from the bottom, the original (scene-linear Rec.709, D65) color ramp, and above it, its luminance with coefficients for Rec.709, Rec.2020, DCI-P3, ACEScg AP1, ACES 2065-1 AP0, explicit chromaticities set to AdobeRGB 1998, and color ramp again at the top.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

