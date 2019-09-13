.. _label_as_subsurface:

.. fix_img_align::

|
 
.. image:: /_images/icons/asSubsurface.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Subsurface

asSubsurface
************

A raytraced, no precomputed required, subsurface-scattering material with a coupled specular term. This node capable of modelling a wide range of appearance thanks to built-in support for several diffusion profiles. 

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Subsurface Parameters
^^^^^^^^^^^^^^^^^^^^^

*Subsurface Profile*
    The diffusion profiles to use in the BSSDRF [#]_. This parameter can take the following values

    * Better Dipole
    * Directional Dipole :cite:`Frisvad:2014:DDM:2702692.2682629`
    * Gaussian :cite:`dEon:2007:ERH:2383847.2383869` :cite:`dEon:2011:QMR:1964921.1964951`
    * Normalized Diffusion :cite:`Christensen:2015:ARP:2775280.2792555`
    * Standard Dipole :cite:`Jensen:2001:PMS:383259.383319`, :cite:`Donner:2005:LDM:1186822.1073308`
    * Random Walk :cite:`Meng:2016:IDS:3071773.3071778`
    
*Reflectance*
    The surface reflectance.

*SSS Amount*
    A scaling factor for the overall contribution of the subsurface scattering term.

*Mean Free Path*
    Mean free path [#]_ controls how deep light can travel within the volume, scattering internally, until it's fully absorbed or exits the medium. Lower values in the limit would have a response similar to a diffuse term, while high values would allow light to travel almost unhindered, producing a translucent like appearance.

*MFP Scale*
    An overall scaling factor for the *Mean Free Path*.

SSS Advanced Parameters
"""""""""""""""""""""""

*SSS Ray Depth*
    The maximum ray depth allowed for a ray of type *subsurface*.

-----

Fresnel Parameters
^^^^^^^^^^^^^^^^^^

*Index of Refraction*
    The index of refraction of the material.

Fresnel Advanced Parameters
"""""""""""""""""""""""""""

*Fresnel Weight*
    A value of 1.0, will scale the subsurface contribution by the Fresnel transmittance, while a value of 0.0 will disable any scaling. When using the provided specular BRDF, in order to keep energy conservation, this scaling factor should be set to 1.0. If you're not using the provided specular term and/or want to compose one later in your workflow, you can disable this.
    You also have the freedom to use intermediary values.

-----

Specular Parameters
^^^^^^^^^^^^^^^^^^^

*Specular Weight*
    An overall scaling factor for the specular term. A value of 1.0 provides full intensity, 0.0 disabling it.

.. note::
   
   This shader allows the user to texture map the specular weight, to control the specular term intensity, but it does not provide a way to *tint* or color the specular term. That is intentional sually only dielectrics [#]_ have subsurface scattering, and dielectrics have no tinted specular highlights.

*Specular Roughness*
    The apparent surface roughness of the material. The distribution used is the *GGX* :cite:`Walter2007`, and energy conservation to take into account multiple scattering :cite:`Heitz:2016:MMB:2897824.2925943` is applied automatically.

Anisotropy Parameters
"""""""""""""""""""""

*Anisotropy Amount*
    The overall weight of the anisotropy, with a value of 0.0 producing isotropic specular highlights, and a value of 1.0 producing full anisotropic specular highlights.

*Anisotropy Angle*
    A rotation angle in [0,1] range, that is mapped internally to a full 360 degrees rotation and applied on top of the anisotropy value provided by the explicit anisotropy vector or anisotropy vector map.

*Anisotropy Mode*
    The anisotropy mode, which can either be a anisotropy vector map with the XY anisotropy encoded in the *red* and *green* channels of the image, or an explicit anisotropy vector, which can be provided via a :ref:`asAnisotropyVectorField <label_as_anisotropy_vector_field>` node. It can therefore take the values

        * Anisotropy Map
        * Explicit Vector

*Anisotropy Map*
    The anisotropy vector map to use when *Anisotropy Mode* is set to *Anisotropy Map*.

*Anisotropy Direction*
    An explicit anisotropy vector to use when the *Anisotropy Mode* parameter is set to *Explicit Vector*.

-----

Bump
^^^^

*Bump Normal*
    The unit length world space normal of the bumped surface.

-----

Matte Opacity
^^^^^^^^^^^^^

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
    The BSSRDF output with the optional added specular BRDF.

*Output Matte Opacity*
    The matte holdout.

-----

.. _label_as_subsurface_screenshots:

Screenshots
-----------

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_crackedrubber.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   A cracked vulcanized rubber like material.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_gaussian.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   A blue marble like material using the *Gaussian* diffusion profile.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_normalizeddiffusion.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   The same basic appearance using the *Normalized Diffusion* profile.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_randomwalk.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   Now using the *Random Walk* profile.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_standarddipole.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   The same basic appearance using now this *Standard Dipole* diffusion profile.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_directionaldipole.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   Now with the *Directional Dipole* profile.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_betterdipole.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   And finally with the *Better Dipole* profile.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_basketball.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   Modelling the appearance of a plastic ball, with bump mapping, medium roughness, small mean free path.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_snow.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   Modelling the appearance of snow, medium roughness, bump, high mean free path.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_crackedrubber2.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   A cracked vulcanized rubber like material.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_gaussian2.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   A blue marble like material using the *Gaussian* diffusion profile.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_normalizeddiffusion2.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   The same basic appearance using the *Normalized Diffusion* profile.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_randomwalk2.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   Now using the *Random Walk* profile.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_standarddipole2.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   The same basic appearance using now this *Standard Dipole* diffusion profile.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_directionaldipole2.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   Now with the *Directional Dipole* profile.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_betterdipole2.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   And finally with the *Better Dipole* profile.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_basketball2.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   Modelling the appearance of a plastic ball, with bump mapping, medium roughness, small mean free path.

.. thumbnail:: /_images/screenshots/subsurface/as_subsurface_snow2.png
   :group: shots_as_subsurface_group_A
   :width: 10%
   :title:

   Modelling the appearance of snow, medium roughness, bump, high mean free path.

-----

.. rubric:: Footnotes

.. [#] See also `bidirectional scattering distribution function <https://en.wikipedia.org/wiki/Bidirectional_scattering_distribution_function>`_.

.. [#] See `mean free path wikipedia page <https://en.wikipedia.org/wiki/Mean_free_path>`_ for more details.

.. [#] Dielectric is a material which is an electric insulator, the opposite of *conductors* which as the name says, conducts electricity. See `this page on dielectric materials <https://en.wikipedia.org/wiki/Dielectric>`_ for more details. In terms of look development an accepted simplification is that dielectrics have white or non-tinted specular highlights, while conductors have tinted or coloured specular highlights.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

