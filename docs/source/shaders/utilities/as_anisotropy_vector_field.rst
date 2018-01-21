.. _label_as_anisotropy_vector_field:

.. fix_img_align::

|
 
.. image:: /_images/icons/asAnisotropyVectorField.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Anisotropy Vector Field Icon

asAnisotropyVectorField
***********************

A node that manipulates anisotropy vector maps and outputs a anisotropy vector for use in BxDFs that support anisotropy.

Parameters
----------

.. bogus directive to silence warnings::

-----

Field Parameters
^^^^^^^^^^^^^^^^

*Field Color*
    The anisotropy vector map color.

*Rotation Angle*
    A rotation angle, in degrees, from -360 to 360 degrees, to rotate the input anisotropy vector map. This is an absolute value to re-orient the initial anisotropy vector map, non-texturable.

*Field Mode*
    The input anisotropy vector map can have the anisotropy directions along X and Y, encoded in the Red and Green channels, or in the Red and Blue channels.
    Typically, RG encoding was used in applications such as PRman [#]_, while RG encoding in applications such as Modo [#]_ or Softimage [#]_.
    The values allowed are therefore

        * Red/Green as XY
        * Red/Blue as XY

-----

Rotation Parameters
^^^^^^^^^^^^^^^^^^^

*Rotation Value*
    This parameter rotates the RG or RB vector field, and unlike the *Rotation Angle* parameter, this parameter takes as inputs values between 0.0 and 1.0, and is texturable. Internally this is mapped to [0,360] degrees.

*Rotation Mode*
    The rotation computed earlier in *Rotation Value* can be added to the vector field, or the rotation can be applied to both directions along a central axis. That is, the rotation can be mapped from the input [0,1] values to [0,360] degrees (Absolute), or to [-360,360] degrees (Centered) - where a value of 0.5 means no rotation. Values below 0.5 means counter-clockwise rotation, and values above 0.5 imply clockwise rotation.
    The allowed modes are therefore

        * Centered
        * Absolute

----

Output Parameters
^^^^^^^^^^^^^^^^^

*Normalize Output*
    Parameter that forces a normalization of the final anisotropy vector. The default is to enable it.

-----

Surface Normal
^^^^^^^^^^^^^^

*Surface Normal*
    The world space, unit length shading normal, bump mapped. **Not** a tangent space nor a object space normal.

-----

Outputs
^^^^^^^

*Anisotropy Vector*
    The final re-oriented, RG or RB encoded anisotropy vector.

-----

.. _label_as_anisotropy_vector_field_screenshots:

Screenshots
-----------

Some examples of the output anisotropy_vector_field of the input color ramp, rendered in (scene linear) Rec.709 space, standard illuminant D65, with different color spaces and whitepoints chosen. The mismatches in color spaces are for illustration purposes. If the settings cannot be derived automatically from your DCC application, then the choice of color space should match your choice or render/working space.

.. thumbnail:: /_images/screenshots/anisotropy_vector_field/anisotropy_vector_field_colorramp_workingspace_rec709.png
   :group: shots_anisotropy_vector_field_group_A
   :width: 10%
   :title:

   Original color ramp, synColor render/working space set to (scene-linear) sRGB/Rec.709 primaries and D65 white point.

.. thumbnail:: /_images/screenshots/anisotropy_vector_field/anisotropy_vector_field_colorramp_workingspace_rec709_from_CMS.png
   :group: shots_anisotropy_vector_field_group_A
   :width: 10%
   :title:

   anisotropy_vector_field of input color, with settings automatically retrieved from Maya's synColor CMS preferences.

.. thumbnail:: /_images/screenshots/anisotropy_vector_field/anisotropy_vector_field_colorramp_set_ACES_AP0.png
   :group: shots_anisotropy_vector_field_group_A
   :width: 10%
   :title:

   Original color ramp, with CMS settings disabled, and the input space overriden to ACES 2065-1 AP0, D60 whitepoint.

.. thumbnail:: /_images/screenshots/anisotropy_vector_field/anisotropy_vector_field_colorramp_set_ACES_AP1.png
   :group: shots_anisotropy_vector_field_group_A
   :width: 10%
   :title:

   Original color ramp, with CMS settings disabled, and the input space overriden to ACEScg AP1, D60 whitepoint.

.. thumbnail:: /_images/screenshots/anisotropy_vector_field/anisotropy_vector_field_colorramp_set_Rec2020.png
   :group: shots_anisotropy_vector_field_group_A
   :width: 10%
   :title:

   Original color ramp, with CMS settings disabled, and the input space overriden to Rec.2020, D65 whitepoint.

.. thumbnail:: /_images/screenshots/anisotropy_vector_field/anisotropy_vector_field_colorramp_set_DCIP3.png
   :group: shots_anisotropy_vector_field_group_A
   :width: 10%
   :title:

   Original color ramp, with CMS settings disabled, and the input space overriden to DCI-P3, DCI whitepoint.

.. thumbnail:: /_images/screenshots/anisotropy_vector_field/anisotropy_vector_field_colorramp_explicit_coords_adobergb.png
   :group: shots_anisotropy_vector_field_group_A
   :width: 10%
   :title:

   Original color ramp, with CMS settings disabled, and the input color space set to *xy chromacitity coordinates*, which were then set to the RGB chromaticity coordinates of the AdobeRGB 1998 color space, with a D65 whitepoint.

.. thumbnail:: /_images/screenshots/anisotropy_vector_field/anisotropy_vector_field_compared.png
   :group: shots_anisotropy_vector_field_group_A
   :width: 10%
   :title:

   Starting from the bottom, the original (scene-linear Rec.709, D65) color ramp, and above it, its anisotropy_vector_field with coefficients for Rec.709, Rec.2020, DCI-P3, ACEScg AP1, ACES 2065-1 AP0, explicit chromaticities set to AdobeRGB 1998, and color ramp again at the top.

-----

.. rubric:: Footnotes

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

