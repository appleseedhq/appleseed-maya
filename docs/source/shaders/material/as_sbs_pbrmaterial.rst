.. _label_as_sbs_pbrmaterial:

.. fix_img_align::

|
 
.. image:: /_images/icons/asSbsPbrMaterial.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: SBS PBR Material

asSbsPBRMaterial
****************

A shader created to give a good visual match with `Allegorithmic <https://www.allegorithmic.com/>`_ `Substance Painter's <https://www.allegorithmic.com/products/substance-painter>`_ PBR material (*roughness* based workflow).
It also has dedicated input slots for many of Substance Painter's exported auxiliary textures.
For convenience the parameters were labelled and grouped as much as possible matching the conventions used in Substance Painter.

|

.. seealso::

   The `Substance Painter PBR Guide <https://www.allegorithmic.com/pbr-guide>`_.

|

Parameters
----------

.. bogus directive to silence warning::

-----

Ambient Occlusion
^^^^^^^^^^^^^^^^^

*Ambient Occlusion*
    This parameter does **not** compute any ambient occlusion itself. Rather it expects the exported ambient occlusion auxiliary texture from Substance Painter if such channel was exported (which is usually exported with scene-linear encoding).

-----

Surface Color
^^^^^^^^^^^^^

*Base Color*
    The surface color channel, which usually is set by default to use the sRGB OETF [#]_.

.. seealso::

   See `The importance of terminology and sRGB uncertainty <https://www.colour-science.org/posts/the-importance-of-terminology-and-srgb-uncertainty/>`_ for more details.

-----

Emissive
^^^^^^^^

*Emissive Color*
    The color to use for the incandescence effect, usually encoded with a sRGB OETF by default.

*Emissive Intensity*
    A scaling factor for the incandescence effect.

-----

Height
^^^^^^

*Height*
    The value to use for scalar bump mapping, usually a grayscale texture (and *raw* quantity).

*Scale*
    An overall scaling factor for the scalar bump mapping quantity *height*.

-----

Metallic
^^^^^^^^

*Metallic*
    The *metallicness* or *metallic* texture slot. With low values the specular appearance is that of a dielectric such as plastic or glass, while higher values produce the appearance of metal.

-----

Normal
^^^^^^

*Normal*
    The slot for normal mapping. In Maya this automatically creates a *bump2d* node whose *bump mode* must be set to *tangent space* bump mapping.
    With other DCC applications, you should create a :ref:`asBump <label_as_bump>` node instead, and set its :ref:`mode parameters <label_bump_mode_parameters>` to *Normal Map* while having in mind that exporting a map in OpenGL or DirectX convention will flip the G channel of the exported map of one in regard to the other.
    See the :ref:`asBump documentation <label_as_bump>` for more details.

-----

Roughness
^^^^^^^^^

*Roughness*
    The roughness texture channel. A *raw* quantity usually exported as a grayscale texture.

-----

Opacity
^^^^^^^

*Opacity*
    The opacity texture channel, with a value of 0.0 denoting a fully transparent surface, and a value of 1.0 denoting a fully opaque surface. A *raw* quantity usually exported as a grayscale texture.

-----

Specular
^^^^^^^^

*Specular Level*
    The slot for the specular level texture. By default this quantity is not usually exposed to the user and is set to a fixed value.

.. note::

   The *Specular Level* is not exposed by default, but it can be exposed. Its default value of 0.5 will be scaled internally and used to determine the reflectance at facing or normal incidence. This in turn will drive the Fresnel reflectance which is used to control the appearance of the dielectric specular term.

-----

Anisotropy
^^^^^^^^^^

*Anisotropy Level*
    The slot for the amount of anisotropy for the specular highlights, if the user created and exported such a channel. A *raw* quantity usually saved as a grayscale texture.

*Anisotropy Angle*
    The slot or value for an anisotropy rotation angle, where the value 1.0 maps to 360 degrees rotation. A *raw* quantity usually saved as a grayscale texture.

-----

Refraction
^^^^^^^^^^

*Refraction*
    The amount of refraction, where a value of 0 is a diffuse BRDF [#]_, and a value of 1 is a pure smooth specular BTDF [#]_, and in-between values blending between the diffuse and refractive term.

*Refraction IOR*
    The index of refraction [#]_ of the material. This parameter affects **only** the refraction term and has no effect whatsoever on the specular term.

*Scattering*
    Controls how much light is scattered through the surface. Unused at the moment.

*Absorption*
    Controls how much light is absorbed through the surface, with a value of 1.0 leading to the light being fully absorbed by the medium.

*Absorption Color*
    Controls how light shifts when light traverses the medium's volume.

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

Notes
^^^^^

.. attention::

   When using texture atlas such as UDIMs, have in mind that you're probably going to have a large number of channels and textures to access. This will have a cost in performance. One way to maximize performance and mitigate this cost is to pre-process the textures with `OpenImageIO's maketx <https://github.com/OpenImageIO/oiio>`_ utility.

-----

.. _label_sbs_pbrmaterial_screenshots:

Screenshots
-----------

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat1_direct.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Flaked yellow paint over metal, with oxidation and dirt.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat2_direct.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Scratched dark steel Substance smart material, with edge wear.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat3_direct.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Greenish paint layers over smooth steel, with mesh curvature controlling the exposure of the underlying metallic substrate.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat4_direct.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Flaked orange paint over aluminum, with a layer of accumulated dirt on top. Curvature and grunge maps controlled the flakiness and erosion of the paint layer.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat5_direct.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Dark walnut wood material.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat6_direct.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Another variation of layered paint over a dark metallic substrate.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat7_direct.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Scratched bronze with edge wear and curvature controlling a small patine effect.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat8_direct.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Marble material.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat9_direct.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Dark plastic material, with accumulation of dirt.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat1_evening.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Flaked yellow paint over metal, with oxidation and dirt.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat2_evening.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Scratched dark steel Substance smart material, with edge wear.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat3_evening.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Greenish paint layers over smooth steel, with mesh curvature controlling the exposure of the underlying metallic substrate.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat4_evening.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Flaked orange paint over aluminum, with a layer of accumulated dirt on top. Curvature and grunge maps controlled the flakiness and erosion of the paint layer.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat5_evening.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Dark walnut wood material.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat6_evening.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Another variation of layered paint over a dark metallic substrate.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat7_evening.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Scratched bronze with edge wear and curvature controlling a small patine effect.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat8_evening.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Marble material.

.. thumbnail:: /_images/screenshots/sbs_pbrmaterial/sbs_pbr_mat9_evening.jpg
   :group: shots_sbs_pbrmaterial_group_A
   :width: 10%
   :title:

   Dark plastic material, with accumulation of dirt.

-----

.. rubric:: Footnotes

.. [#] By default, the color channel, and other color quantities are set to be encoded with the sRGB OETF. But note that you cannot specify RGB primaries or white point in Substance Painter, so you should assume these to be the sRGB/Rec.709 RGB primaries and D65 whitepoint if you intend to convert to other color spaces.

.. [#] Which in this case is the `Lambert BRDF <https://en.wikipedia.org/wiki/Lambertian>`_.
   
.. [#] See also `the definition of BSDF <https://en.wikipedia.org/wiki/Bidirectional_scattering_distribution_function>`_ for more details.

.. [#] The *real* and absolute index of refraction of the material. Since dielectrics have a very small extinction coefficient, this is assumed to be 0, and monochromatic. Absolute since it's assumed to be the ratio of the wave speed in the vacuum and wave speed in the medium. Appleseed is **not** querying the exterior medium with *nested dielectrics* :cite:`ND.2002.10487555` to compute the correct index of refraction. The priority was given to have as much as possible a matching appearance with Substance Painter's PBR material.
   
-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

