.. _label_as_glass:

.. fix_img_align::

|

.. image:: /_images/icons/asGlass.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Glass BSDF

asGlass
*******

A Glass BSDF :cite:`Walter2007`, with volumetric absorption.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Surface Transmittance
^^^^^^^^^^^^^^^^^^^^^

*Transmittance Color*
    The color that is transmitted to the underlying glass medium. A color of 0 will mean no transmittance will take place, a color of one will mean full transmittance. This parameter is weighted by the next parameter, *transmittance amount*.

*Transmittance Amount*
    Transmittance amount, with a value of 0 meaning no light reaches the glass medium, and a value of 1.0 meaning a full amount reaching the glass medium. This is useful for compositing the glass BSDF with other BxDFs.

-----

Specular Parameters
^^^^^^^^^^^^^^^^^^^

*Reflection Tint*
    Overall tint for the reflection (BRDF) component of the glass BSDF.

*Refraction Tint*
    Overall tint for the refraction (BTDF) component of the glass BSDF.  

*Index of Refraction*
    Absolute index of refraction

*Roughness*
    The apparent surface roughness, affecting both the reflection and refraction equally.

*Anisotropy Amount*
    Overall intensity of the anisotropy effect, with a value of 0.0 representing isotropic specular highlights.

*Anisotropy Angle*
    Rotation angle for the anisotropic highlights, with the value in [0,1] range mapping to a rotation from 0 to 360 degrees.

*Anisotropy Vector Map*
    Also known as tangent field, encodes the anisotropy directions along X and Y in the Red and Green or Red and Blue channels of the image. appleseed expects values encoded in the Red and Green channels.

-----

Volume Material Parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^

*Volume Transmittance*
    Color of the volumetric absorption as the refracted ray travels within the medium.

*Transmittance Distance*
    The distance at which full absorption is supposed to occur. When this distance is set to 0, no absorption takes place. Lower values imply a stronger absorption, and higher values imply a weaker absorption effect as the ray would need to travel a greater distance for full absorption to take place.

-----

Bump Parameters
^^^^^^^^^^^^^^^

*Bump Normal*
    The unit length world space normal of the bumped surface.

.....

Advanced Parameters
^^^^^^^^^^^^^^^^^^^

*Ray Depth*
    The maximum ray depth a ray is allowed to bounce before being terminated.

-----

Outputs
-------

*Output Color*
    The BSDF output color.

*Output Transparency*
    The output transparency.

-----

.. note:: The output transparency is unused at the moment.

.. _label_asglass_screenshots:

Screenshots
-----------

.. thumbnail:: /_images/screenshots/glass/glass_ice_beckmann_green_volume_bump.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Glass with scalar bump mapping creating the appearance of ice. Using the Beckmann MDF with roughness set to 0.032, IOR to 1.489, and absorption with a slight green/cyan tint with distance set to 35 units.

.. thumbnail:: /_images/screenshots/glass/glass_engraving_w_absorption1.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Engraved glass with high roughness, using the Beckmann distribution, and a slight green, sodium glass like tint, with absorption.

.. thumbnail:: /_images/screenshots/glass/glass_ice_normal_mapped1.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Glass with heavy tangent space normal mapping creating the appearance of a rough ice cube. The MDF used was the Beckmann MDF, roughness 0.068, ior 1.467.

.. thumbnail:: /_images/screenshots/glass/glass_sharp_sodium_glass.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Smooth glass with slight green absorption, creating the appearance of sodium glass. The MDF used was the Beckmann MDF, and roughness was set at 0.001.

.. thumbnail:: /_images/screenshots/glass/glass_ice_anisotropy_absorption1.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Glass used to create the appearance of sea ice with blue absorption, mapped roughness, bump mapping, and anisotropy.

.. thumbnail:: /_images/screenshots/glass/glass_very_rough_with_high_bump.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Glass with a high frequency noise applied as bump mapping, with high intensity, creating the appearance of a very rough, almost diffuse like surface.

.. thumbnail:: /_images/screenshots/glass/glass_sharp_hammered_w_absorption.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Hammered glass with slight green absorption. Specular roughness set to 0.01 with the Beckmann MDF.

.. thumbnail:: /_images/screenshots/glass/glass_std_spread1.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Sharp glass with long specular highlight tails, creating the appearance of a thin veil over the specular highlights. Using Student's t-distribution with a specular roughness of 0.05, specular spread of 0.25, and with a IOR set to 1.64.

.. thumbnail:: /_images/screenshots/glass/glass_beckmann_engraving2.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Engraved rough glass using the Beckmann distribution, and a IOR of 1.5.

.. thumbnail:: /_images/screenshots/glass/glass_beckmann_ice_normalmap2.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Tangent space normal mapped glass, creating the appearance of mildly rough ice, using the Beckmann MDF.

.. thumbnail:: /_images/screenshots/glass/glass_engraving_green_absorption.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Engraved glass with texture mapped roughness, Student's t-distribution, green absorption.

.. thumbnail:: /_images/screenshots/glass/glass_engraving_rough2.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Art nouveau like engraved glass, with a greyscale pattern driving the specular roughness, and a coloured pattern driving the absorption color. Using the GGX distribution, IOR set to 1.5.

.. thumbnail:: /_images/screenshots/glass/glass_engraving_green_absorption2.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Engraving texture showing transition from smooth to moderately rough engraving details and green absorption.

.. thumbnail:: /_images/screenshots/glass/glass_dragon_engraving_ggx.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Engraved glass with moderate to high roughness using the GGX distribution.

.. thumbnail:: /_images/screenshots/glass/glass_artdeco_absorption.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Art deco texture driving the absorption color with a greyscale version controlling the surface roughness. Using the Beckmann MDF and an IOR of 1.55.

.. thumbnail:: /_images/screenshots/glass/glass_engraving_rough_absorption.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Glass with mapped high roughness engraving and absorption.

.. thumbnail:: /_images/screenshots/glass/glass_smooth_beckmann_blue.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Smooth glass with blue absorption, Beckmann MDF.

.. thumbnail:: /_images/screenshots/glass/glass_ice_rough_normalmapped.png
   :group: shots_as_glass_group_A
   :width: 10%
   :title:

   Rough ice, with tangent space normal mapping, texture driving the specular highlights roughness, blue absorption.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

