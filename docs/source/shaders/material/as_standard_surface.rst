.. _label_as_standard_surface:

.. fix_img_align::

|

.. image:: /_images/icons/asStandardSurface.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Standard Surface

asStandardSurface
*****************

A physically based material with a dielectric coating with absorption, and a substrate with optional subsurface scattering, refraction, volumetric absorption.

|

Parameters
----------

.. bogus directive to silence warning::

-----

Diffuse Parameters
^^^^^^^^^^^^^^^^^^

*Diffuse Weight*
    A scaling factor for the diffuse BRDF.

*Diffuse Color*
    The surface color.

*Diffuse Roughness*
    The diffuse roughness, with a value of 0, it corresponds to a Lambert diffuse term. Higher values *flatten* the appearance of the surface, giving it a chalky look. [#]_

-----

Subsurface Parameters
^^^^^^^^^^^^^^^^^^^^^

*Subsurface Weight*
    A weighting factor for the subsurface scattering term.

*Subsurface Mean Free Path (MFP)*
    Color controling how far light enters and travels within the medium.

*Subsurface MFP Scale*
    Overall scaling factor for the MFP color, which is expected in [0,1] range.
    Values above 1.0 are possible, resulting in increased translucency appearance.

Advanced
~~~~~~~~

*Subsurface Profile*
    The diffusion profiles to use in the BSSDRF [#]_. This parameter can take the following values:

    * Gaussian :cite:`dEon:2007:ERH:2383847.2383869`
    * Better Dipole
    * Normalized Diffusion :cite:`Christensen:2015:ARP:2775280.2792555`
    * Random Walk :cite:`Meng:2016:IDS:3071773.3071778`

-----

Translucency Parameters
^^^^^^^^^^^^^^^^^^^^^^^

*Translucency Weight*
    Controls the amount of thin translucency of the object.

*Translucency Color*
    Color affecting the thin translucency term.

-----

Specular Parameters
^^^^^^^^^^^^^^^^^^^

*Specular Color*
    Overall specular tint for the specular BRDF.

*Specular Roughness*
    The apparent surface roughness affecting the specular highlights.


Fresnel
~~~~~~~

*Fresnel Type*
    Allows the user to choose the specular mode, of a dielectric such as plastic or glass, or of a conductor or metal.

    .. note:: To use refraction, the mode **must** be set to *dielectric* and the index of refraction set. When in *conductor* mode, the *face tint* and *edge tint* parameters are used to derive the complex index of refraction instead :cite:`Gulbrandsen2014Fresnel` and no refraction is used.

*IOR*
    The index of refraction for the dielectric mode.

*Facing Tint*
    The reflectance at normal incidence.

*Edge Tint*
    Reflectance at grazing incidence.

Anisotropy
~~~~~~~~~~

*Anisotropy Amount*
    Overall intensity of the anisotropy effect, with a value of 0.0 representing a isotropic specular highlight.

*Anisotropy Angle*
    Rotation angle for the anisotropic highlight in [0,1], mapping a rotation from 0 to 360 degrees.

*Anisotropy Map*
    Also known as tangent field, encodes the anisotropy directions along X and Y in the Red and Green or Red and Blue channels of the image. Appleseed expects values encoded in the Red and Green channels.

-----

Refraction Parameters
^^^^^^^^^^^^^^^^^^^^^

*Refraction Amount*
    Intensity of the refraction, only taking place when *Fresnel* is *dielectric* :cite:`Walter2007`.

*Refraction Tint*
    Overall tinting factor, it affects the BTDF equally, unlike volumetric absorption.

Volumetric Absorption
~~~~~~~~~~~~~~~~~~~~~

*Absorption Depth*
    Sets the depth at which full absorption takes place. Low values result in dense absorbing materials, high values in transparent appearance ones.

*Absorption Color*
    The color used for the volumetric absorption.

-----

Coating Parameters
^^^^^^^^^^^^^^^^^^

*Coating Reflectivity*
    Intensity of specular highlights on the coating.

*Coating Roughness*
    Apparent surface roughness of the coating specular highlights.

*Coating IOR*
    Index of refraction of the coating layer, usually a dielectric, with values around 1.5.

Coating Absorption
~~~~~~~~~~~~~~~~~~

*Coating Thickness*
    Thickness of the coating layer, controlling the intensity of coating absorption, with 0 being no absorption, 1 being full absorption.

*Coating Absorption*
    Absorption color for the coating, white has no effect, black absorbs fully.

-----

Incandescence Parameters
^^^^^^^^^^^^^^^^^^^^^^^^

*Incandescence Amount*
    The overall intensity of the incandescence effect.

*Incandescence Type*
    Color choice for incandescence color, with *constant* taking as input the user-set value, and *blackbody* using a blackbody radiator. [#]_

*Incandescence Color*
    Incandescence color, ignored in *blackbody* mode.

*Temperature*
    Temperature in Kelvin degrees, ignored in *constant* mode.

Options
~~~~~~~

*Area Normalize EDF*
    Normalize by the object area, so that object deformations keep the incandescence energy. If unset, deforming the object will retain the incandescence color.

*Tonemap EDF*
    Tonemaps the potentially high energy result of the blackbody radiator into the [0,1] range. Disabled by default.

.. note:: The *tonemap EDF* option has effect **only** when *incandescence type* is set to *blackbody*.

-----

Transparency Parameters
^^^^^^^^^^^^^^^^^^^^^^^

*Transparency*
   Affects the *presence* of an object. When transparency is binary (full opaque or full transparent, with no in-between values), appleseed alpha masks should be used instead.

-----

Bump Parameters
^^^^^^^^^^^^^^^

*Coating Normal*
    The bump normal for the coating layer.

*Substrate Normal*
    The bump normal for the substrate.

-----

Matte Parameters
^^^^^^^^^^^^^^^^

*Enable Matte*
    Flag toggling matte holdouts on or off.

*Matte Opacity*
    Overall scaling factor for the matte, from solid black to normal.

*Matte Opacity Color*
    Color for the matte.

-----

Advanced Parameters
^^^^^^^^^^^^^^^^^^^

*SSS Ray Depth*
    Maximum number of ray bounces for the subsurface scattering term. 

*SSS Threshold*
    Defines the distance light has to travel within the medium to start the subsurface scattering effect. A low enough mean free path value will have a visually negligible difference from a diffuse term. This parameter sets the threshold at which the subsurface calculations start, instead of the ordinary diffuse term.

*Maximum Ray Depth*
    The maximum number of bounces a ray is allowed to travel.

-----

Outputs
-------

*Output Color*
    The final result color.

*Output Transparency*
    The final transparency color.

*Output Matte Opacity*
    The final matte opacity. Note that OSL *holdout* is unsupported at the moment.

-----

.. _label_standard_surface_screenshots:

Screenshots
-----------

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_aluminium.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Rough aluminium, specular set to *conductor* mode.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_coated_blue_absorption_rough_aluminium.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Rough aluminium, specular set to *conductor* mode, with a dielectric coating with blue colored absorption.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_coated_rough_aluminium.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Rough aluminium with high specular spread values, dielectric coating without absorption.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_coated_rough_aluminium_w_orange_absorption_high_spread.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Rough aluminium, specular set to *conductor* mode, full specular spread and high absorption value.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_diffuse_rough_coating_velvety.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Colored diffuse term with very rough dielectric coating giving a sheen like appearance.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_incandescence_blackbody_and_coating.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Blackbody radiator with tonemapped values, and a smooth dielectric coating.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_metal_chrome_anisotropy.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Slightly rough chrome with anisotropic highlights.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_refraction.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Smooth glass BTDF with minimal surface roughness.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_rough_glass_with_volume_absorption.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Rough glass with volumetric absorption.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_copper.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Moderately rough copper substrate, specular Fresnel set to *conductor*.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_rough_diffuse.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Diffuse term with diffuse roughness set to 1.0.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_subsurface1.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Subsurface scattering, with a global scale factor set to 0.5.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_rough_glass2.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Rough glass refraction with high specular spread.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_subsurface2.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Subsurface scattering with scale factor set to 0.1, rough specular, and sharp dielectric coating, creating the appearance of a jade like material.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_satin1.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Satin like material using a dielectric substrate with high roughness and anisotropy.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_velvet1.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Velvet like material using a dielectric substrate with high specular roughness, low specular spread, and a blueish specular tint.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_skinlike_specular.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Subsurface with a rough specular, creating the appearance of a skin like material.

.. thumbnail:: /_images/screenshots/standard_surface/stdsurface_gummybear.png
   :group: shots_standard_surface_group_A
   :width: 10%
   :title:

   Glass BSDF with roughness set to 0.6, absorption depth to 2.0, absorption color to red/orange, giving the appearance of a gummy bear like material.

-----

.. rubric:: Footnotes

.. [#] The diffuse BRDF used is the Oren-Nayar BRDF :cite:`Oren:1994:GLR:192161.192213`

.. [#] See also `Extending the Disney BRDF to a BSDF with Integrated Subsurface Scattering <http://blog.selfshadow.com/publications/s2015-shading-course/burley/s2015_pbs_disney_bsdf_notes.pdf>`_ for details.

.. [#] https://en.wikipedia.org/wiki/Black-body_radiation

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

