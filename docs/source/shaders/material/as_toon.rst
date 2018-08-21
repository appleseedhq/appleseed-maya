.. _label_as_toon:

.. fix_img_align::

|
 
.. image:: /_images/icons/asToon.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Toon BRDF

asToon
*********

A Non-Photo-Realistic/NPR :cite:`Gooch:1998:NLM:280814.280950` toon shader.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

.. _label_as_toon_incandescence_parameters:

Incandescence Parameters
^^^^^^^^^^^^^^^^^^^^^^^^

*Incandescence Weight*
    The amount of incandescence, not restricted to an upper bound of 1.0.

*Incandescence Attenuation*
    The amount of attenuation for the incandescence, based on the facing ratio towards the viewer. A high value will darken the values towards the edges.

Toon Control
~~~~~~~~~~~~

*Incandescence Tint*
    The incandescence color.

*Blending Mode*
    The blending mode for the *Incandescence Tint* parameter. It can be one of the following modes:

        * Darken
        * Multiply
        * Color Burn
        * Linear Burn
        * Lighten
        * Screen
        * Color Dodge
        * Linear Dodge
        * Overlay
        * Soft Light
        * Hard Light
        * Vivid Light
        * Linear Light
        * Pin Light
        * Difference
        * Exclusion
        * Subtract
        * Divide
        * Hue
        * Saturation
        * Color
        * Luminosity

.. seealso::

   The node :ref:`asBlendColor <label_as_blend_color>` for more details.

*EDF Dark Color*
    The color to use in the darkest areas of this term [#]_.

*EDF Dark Level*
    The threshold at which to transition from darkest tones to mid-tones.

*EDF Midtone Color*
    The color to use for the mid-tones.

*EDF Midtone Level*
    The threshold at which to transition from mid-tones to bright areas.

*EDF Bright Color*
    The brightest color.

*EDF Softness*
    The transition softness between the tonal areas.

Advanced
~~~~~~~~

*Normalize by Area*
    When unchecked, the incandescence term is constant. When checked, it's scaled by the surface area of the object. With deforming or scaling objects, the incandescence might therefore change in order to keep the same energy level being emitted per surface area.

-----

.. _label_as_toon_diffuse_parameters:

Diffuse Parameters
^^^^^^^^^^^^^^^^^^

*Diffuse Weight*
    The contribution of the diffuse term to the toon shading, between 0 and 1.

Toon Control
~~~~~~~~~~~~

*Diffuse Tint*
    The color for the diffuse term.

*Blending Mode*
    The blending mode for the *Diffuse Tint* parameter. It can be one of the following modes:

        * Darken
        * Multiply
        * Color Burn
        * Linear Burn
        * Lighten
        * Screen
        * Color Dodge
        * Linear Dodge
        * Overlay
        * Soft Light
        * Hard Light
        * Vivid Light
        * Linear Light
        * Pin Light
        * Difference
        * Exclusion
        * Subtract
        * Divide
        * Hue
        * Saturation
        * Color
        * Luminosity

.. seealso::

   The node :ref:`asBlendColor <label_as_blend_color>` for more details.

*Shadow Color*
    The color to use in the shadow areas, or umbra.

*Shadow Level*
    The threshold at which to transition from shadow to mid-tones.

*Midtone Color*
    The color to use for the mid-tones, or penumbra.

*Midtone Level*
    The threshold at which to transition from mid-tones to bright areas.

*Highlight Color*
    The bright areas color.

*Diffuse Softness*
    The transition softness between the tonal areas.

Advanced
~~~~~~~~

*Diffuse Ray Depth*
    The maximum number of bounces allowed for *diffuse* paths.

-----

.. _label_as_toon_specular_parameters:

Specular Parameters
^^^^^^^^^^^^^^^^^^^

*Specular Weight*
    The amount of specular highlights to add to the shading, between 0 and 1.

*Specular Roughness*
    The apparent surface roughness of the specular highlights. This works in conjunction with the *Specular Softness* parameter to determine the softness of transition from highlights to non lit area. 
    If you want soft highlights, increase roughness and *Specular Softness*.
    If you want bigger yet sharper highlights, increase roughness and keep the *Specular Softness* low.

*Index of Refraction*
    The index of refraction for the specular term, with high values increasing the reflectivity, and low values decreasing it. This also works in conjunction with the *Specular Softness* control.

Anisotropy
~~~~~~~~~~

*Anisotropy Amount*
    Overall intensity of the anisotropy, with a value of 0.0 representing a isotropic specular highlight, and 1.0 fully anisotropic along the main anisotropy direction.

*Anisotropy Angle*
    Rotation angle for the anisotropic highlight in [0,1], mapping a rotation from 0 to 360 degrees.

*Anisotropy Mode*
    Toggles between accepting a direct texture map in the form of an anisotropy vector map, or between an explicit vector (or a connection to a node that generates such a vector [#]_ ).
    It can take the values

        * Anisotropy Map
        * Explicit Vector

*Anisotropy Map*
    Also known as tangent field, encodes the anisotropy directions along X and Y in the Red and Green or Red and Blue channels of the image. Appleseed expects values encoded in the Red and Green channels. Valid when the *Anisotropy Mode* is set to *Anisotropy Map* only.

*Anisotropy Direction*
    The explicit vector passed as the anisotropy direction. Valid when the *Anisotropy Mode* is set to *Explicit Vector* only.        

Toon Controls
~~~~~~~~~~~~~

*Specular Tint*
    The color for the specular term.

*Blending Mode*
    The blending mode for the *Specular Tint* parameter. It can be one of the following modes:

        * Darken
        * Multiply
        * Color Burn
        * Linear Burn
        * Lighten
        * Screen
        * Color Dodge
        * Linear Dodge
        * Overlay
        * Soft Light
        * Hard Light
        * Vivid Light
        * Linear Light
        * Pin Light
        * Difference
        * Exclusion
        * Subtract
        * Divide
        * Hue
        * Saturation
        * Color
        * Luminosity

.. seealso::

   The node :ref:`asBlendColor <label_as_blend_color>` for more details.

*Glossy Color*
    The color to use for the specular highlights.

*Glossy Level*
    The threshold at which to transition from specular highlights to areas without specular highlights.

*Glossy Softness*
    The transition softness between the highlights and areas without highlights. This works in conjunction with *Specular Roughness*, since a very rough specular highlight will still look sharp if the threshold between the lit areas have a harsh transition.
    On the other side this gives the user the ability to increase the size of the specular highlights, or decrease them, and vary their apparent softness independently.

*Facing Attenuation*
    A viewer Fresnel based attenuation factor. Higher values decrease the intensity of the specular reflection towards the viewer.

Advanced
~~~~~~~~

*Specular Ray Depth*
    The maximum number of specular bounces allowed for *specular* paths.

-----

.. _label_as_toon_rim_lighting_parameters:

Rim Lighting Parameters
^^^^^^^^^^^^^^^^^^^^^^^

*Rim Weight*
    Amount of contribution of the rim lighting effect.

*Rim Tint*
    The color for the rim lighting contribution blend.

*Blending Mode*
    The blending mode for the *Rim Tint* parameter. It can be one of the following modes:

        * Darken
        * Multiply
        * Color Burn
        * Linear Burn
        * Lighten
        * Screen
        * Color Dodge
        * Linear Dodge
        * Overlay
        * Soft Light
        * Hard Light
        * Vivid Light
        * Linear Light
        * Pin Light
        * Difference
        * Exclusion
        * Subtract
        * Divide
        * Hue
        * Saturation
        * Color
        * Luminosity

.. seealso::

   The node :ref:`asBlendColor <label_as_blend_color>` for more details.  

*Rim Softness*
    The softness of the rim lighting effect.

-----

Bump Parameters
^^^^^^^^^^^^^^^

*Bump Normal*
    The unit length world space normal of the bumped surface.

*Specular Normal*
    When using separate bump controls for the diffuse and specular terms, this is the unit length world space normal for the specular term.

*Bump Control*
    This allows the user to choose a single bump effect for both the diffuse and specular terms, or a separate bump effect for these terms. When using both, the *Bump Normal* parameter affects both terms.
    It can take the following values accordingly:

        * *Diffuse Affects Both*
        * *Split Bump*

-----

Transparency Parameters
^^^^^^^^^^^^^^^^^^^^^^^

*Transparency*
    The transparency color, affecting shadow tinting as well.

-----

Contour Parameters
^^^^^^^^^^^^^^^^^^

*Contour Color*
    The overall color of the outlines.

*Contour Opacity*
    The overall opacity of the outlines.

*Contour Width*
    The width of the outlines on the object.

*Contour Object*
    Generate the contours based on the object ID.

*Contour Material*
    Generate contours based on the material ID. Objects with the same material ID will share contours.

*Contour Occlusion*
    Generate contours based on depth differences between nearby points.

*Contour Creases*
    Generate contours based on the creases of the object [#]_, outlining sudden changes of the surface.

*Occlusion Threshold*
    The threshold value for the depth difference comparison between nearby points

*Crease Threshold*
    The threshold value for the creases based outlining, where low values result in outlines for smaller changes in the surface of the object.

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

Outputs
-------

*Output Color*
    The toon BRDF output color.

*Output Transparency*
    The resulting transparency color.

*Output Matte Opacity*
    The matte holdout.

-----

.. _label_as_toon_screenshots:

Screenshots
-----------

.. thumbnail:: /_images/screenshots/toon/as_toon_docs_render1_ibl.png
   :group: shots_as_toon_group_A
   :width: 10%
   :title:

   Toon shader with IBL. Though you have tonal control, IBL makes is still photo-realistic.

.. thumbnail:: /_images/screenshots/toon/as_toon_docs_render2_ibl.png
   :group: shots_as_toon_group_A
   :width: 10%
   :title:

   Toon shader with IBL and a point light. The point light makes the tonal progression much more noticeable.
   A viewer Fresnel term is attenuating the specular highlights.

.. thumbnail:: /_images/screenshots/toon/as_toon_docs_render3.png
   :group: shots_as_toon_group_A
   :width: 10%
   :title:

   Toon shader with an emissive background, a point light, and rim lighting to accentuate the emissive background effect.
   A viewer Fresnel term is attenuating the specular highlights.

.. thumbnail:: /_images/screenshots/toon/as_toon_docs_render4_aniso.png
   :group: shots_as_toon_group_A
   :width: 10%
   :title:

   Toon shader with emissive background, point light, and slightly anisotropic specular highlights.

.. thumbnail:: /_images/screenshots/toon/as_toon_docs_render5_outline.png
   :group: shots_as_toon_group_A
   :width: 10%
   :title:

   Toon shader with emissive background, soft rim lighting, and contours/outlines.

.. thumbnail:: /_images/screenshots/toon/as_toon_docs_render6_bump.png
   :group: shots_as_toon_group_A
   :width: 10%
   :title:

   Toon shader with emissive background, point light, fractal bump affecting both the diffuse and specular terms.

.. thumbnail:: /_images/screenshots/toon/as_toon_docs_render7_bump.png
   :group: shots_as_toon_group_A
   :width: 10%
   :title:

   Toon shader with emissive background, rim lighting, point light, and patterned cloth texture affecting the bump of the diffuse and specular terms. Contours/outlining is using the rate of change of the surface with a low threshold, which picks up the creases from the bump map.

.. thumbnail:: /_images/screenshots/toon/as_toon_docs_render8_bump.png
   :group: shots_as_toon_group_A
   :width: 10%
   :title:

   TOon shader with emissive background, rim lighting, point light, and concentric ripples with a very low frequency and low creases threshold for the outlines.

.. thumbnail:: /_images/screenshots/toon/as_toon_docs_render9_edf.png
   :group: shots_as_toon_group_A
   :width: 10%
   :title:

   Toon shader showing the attenuation of the incandescence term.

-----

.. rubric:: Footnotes

.. [#] Emittance Distribution Function.

.. [#] Such as the :ref:`anisotropy vector field node <label_as_anisotropy_vector_field>`.
   
.. [#] Based on the partial derivatives of the surface normal N along the U and V directions.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

