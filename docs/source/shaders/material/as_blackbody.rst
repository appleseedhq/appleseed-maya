.. _label_as_blackbody:

.. fix_img_align::

|
 
.. image:: /_images/icons/asBlackbody.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Blackbody EDF

asBlackbody
***********

A blackbody radiator material :cite:`CGF:CGF1986` shader, with an optional glossy specular term on top, which also outputs the black body radiator color besides the output closure.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Incandescence Parameters
^^^^^^^^^^^^^^^^^^^^^^^^

*Incandescence Type*
    The type of color for the emitance distribution function, it can be

    1. Constant
    2. Blackbody

    When in *Blackbody* mode it uses physically based values, that might have extremely high intensities as the temperature increases.

*Color*
    The color to use when in *Constant* mode, ignored in *Blackbody* mode.

*Incandescence Amount*
    The overall intensity of the incandescence, it can be over 1.0.

*Temperature Scale*
    A scaling factor for the temperature value used to compute the black body radiation. Unlike *incandescence amount*, this actually affects the output color, with lower values generating warmer colors and lower energy levels, and higher values generating cooler values with higher energy levels.

*Temperature*
    The temperature to which the perfect black body is heated to emit electro-magnetic radiation.

*Normalize Area*
    When objects are deformed, their surface area might change. Without this option, the intensities would stay the same regardless of the deformation of the object to which the shader is attached. It this option enabled, the intensities will change taking into account the object surface area, in order to keep the amount of emitted energy constant.

*Tonemap Color*
    As temperature increases, the black body radiator emits color in bluer wavelenghts, but the amount of energy emitted is extreme. In order to avoid extremely hight intensities, this option allows the user to *tonemap* the resulting black body color.

-----

Specular Parameters
^^^^^^^^^^^^^^^^^^^

*Specular Amount*
    The intensity of the specular term.

*Specular Roughness*
    The apparent surface roughness of the specular term.

*Index of Refraction*
    Absolute index of refraction

.. note:: This term is a simple dielectric specular term using the *GGX* :cite:`heitz:hal-00996995` microfacet distribution function, with energy compensation for high roughness values.

-----

Bump Parameters
^^^^^^^^^^^^^^^

*Bump Normal*
    The unit length world space normal of the bumped surface.

-----

Matte Opacity Parameters
^^^^^^^^^^^^^^^^^^^^^^^^

*Enable Matte Opacity*
    Parameter that toggles matte holdouts.

*Matte Opacity*
    Matte opacity scaling factor.

*Matte Opacity Color*
    Holdout color.

-----

Advanced Parameters
^^^^^^^^^^^^^^^^^^^

*Ray Depth*
    The maximum ray depth a ray is allowed to bounce before being terminated.

-----

Outputs
-------

*Output Color*
    The combined EDF+BRDF output color.

*Output Matte Opacity*
    The matte holdout.

*Output Blackbody Color*
    The black body radiator color.

-----

.. _label_as_blackbody_screenshots:

Screenshots
-----------

.. thumbnail:: /_images/screenshots/blackbody/as_blackbody_shot2_tv_static.png
   :group: shots_as_blackbody_group_A
   :width: 10%
   :title:

   Blackbody set to *constant* mode, with a TV static noise textures as the base color, and a relatively smooth specular reflection on top, glass like.

.. thumbnail:: /_images/screenshots/blackbody/as_blackbody_shot4_tv_static.png
   :group: shots_as_blackbody_group_A
   :width: 10%
   :title:

   Yet another CRT TV. The incandescence intensity drives the overall intensity of the EDF, allowing the user to create stronger illumination effects.

.. thumbnail:: /_images/screenshots/blackbody/as_blackbody_shot7_blackbody.png
   :group: shots_as_blackbody_group_A
   :width: 10%
   :title:

   Now with the mode set to *blackbody*, temperature set to 4300K, and appleseed's 2D noise, with a recursive flow noise, applied to the *temperature scale*. Unlike incandescence intensity, a temperature scale will generate varying color from warmer tones (lower temperatures) to bluer tones (higher temperatures). Tonemapping was on to control the energy in the scene.

.. thumbnail:: /_images/screenshots/blackbody/as_blackbody_shot8_blackbody.png
   :group: shots_as_blackbody_group_A
   :width: 10%
   :title:

   A simple V ramp as the *temperature scale*, showing the change of temperature from warmer to whiter as it approaches a 6500K value. The black body radiation values were tonemapped and the intensity changed with *intensity amount* in order to keep overall intensities under control.

.. thumbnail:: /_images/screenshots/blackbody/as_blackbody_shot9_constant.png
   :group: shots_as_blackbody_group_A
   :width: 10%
   :title:

   A simple constant color with a GGX specular term on top.

.. thumbnail:: /_images/screenshots/blackbody/as_blackbody_shot10_constant.png
   :group: shots_as_blackbody_group_A
   :width: 10%
   :title:

   Another poorly tuned TV, with a relatively smooth specular term on top using the GGX MDF. The *incandescence amount* drives the overall intensity of the lighting.

.. thumbnail:: /_images/screenshots/blackbody/as_blackbody_shot11_constant.png
   :group: shots_as_blackbody_group_A
   :width: 10%
   :title:

   Yet another noisy TV, showing the color bands in the ground, as *incandescence amount* is set to a value of 5.0 to increase the overall EDF intensities. The index of refraction of the specular layer is set to 1.47, matching the IOR of a general dense glass.

.. thumbnail:: /_images/screenshots/blackbody/as_blackbody_shot13_blackbody.png
   :group: shots_as_blackbody_group_A
   :width: 10%
   :title:

   A circular gradient showing the overall effect of using the *temperature scale* when the temperature is set to 11000K. From 0K to 798K there is no visible light emitted. From 798K onwards there is visible light emitted, with warmer tones, increasing in energy and shifting towards neutral then bluer tones as temperature increases.

.. thumbnail:: /_images/screenshots/blackbody/as_blackbody_shot14_blackbody.png
   :group: shots_as_blackbody_group_A
   :width: 10%
   :title:

   Finally, a facing ratio modulating a noisy fractal texture, which also drives the specular intensity of a rough GGX specular term, creating the appearance of a basaltic like material.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

