.. _label_as_metal:

.. fix_img_align::

|
 
.. image:: /_images/icons/asMetal.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Metal BSDF

asMetal
*******

A metal shader, with anisotropy and a user-friendly :cite:`Gulbrandsen2014Fresnel` complex index of refraction Fresnel, with some presets for common metal types derived from real-world measurements [#]_.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Fresnel Parameters
^^^^^^^^^^^^^^^^^^

*Face Reflectance*
    RGB reflectance at normal or facing incidence.

*Edge Reflectance*
    RGB reflectance at edge or grazing incidence.

-----

Specular Parameters
^^^^^^^^^^^^^^^^^^^ 

*Roughness*
    The apparent surface roughness.

*Energy Compensation*
    Microfacet models typically fail to take into account multiple scattering :cite:`Heitz:2016:MMB:2897824.2925943`, and as such, with high roughness values there is a substancial energy loss. To what extent it depends mostly on the MDF used, with *GGX* exhibiting considerable energy loss.

    In order to negate the impact of this energy loss a separate compensation term is applied. This parameter scales the contribution of this compensation term, with a value of 1.0 trying to compensate for all the energy lost, and a value of 0.0 essentially disabling any compensation.

.. seealso::

   This is covered in SIGGRAPH 2017 physically based shading course `Revisiting Physically Based Shading at Imageworks (Christopher Kulla and Alejandro Conty) <http://blog.selfshadow.com/publications/s2017-shading-course/>`_.

Anisotropy
^^^^^^^^^^

*Anisotropy Amount*
    Overall intensity of the anisotropy effect, with a value of 0.0 representing a isotropic specular highlight.

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
    The metal BRDF output color.

*Output Matte Opacity*
    The matte holdout.

-----

.. _label_as_metal_screenshots:

Screenshots
-----------

.. thumbnail:: /_images/screenshots/metal/as_metal_copper.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Textured copper, with tangent space normal mapping for a slight hammered look.

.. thumbnail:: /_images/screenshots/metal/as_metal_silver.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Textured silver.

.. thumbnail:: /_images/screenshots/metal/as_metal_aniso_steel.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Textured steel, with an anisotropy vector field providing the anisotropy directions, and moderate anisotropy weight.

.. thumbnail:: /_images/screenshots/metal/as_metal_gold.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Gold with some scratches in the tangent space normal map.

.. thumbnail:: /_images/screenshots/metal/as_metal_copper2.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Another textured copper.

.. thumbnail:: /_images/screenshots/metal/as_metal_steel2.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Another textured steel.

.. thumbnail:: /_images/screenshots/metal/as_metal_measured_copper.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Setup using measured complex ior for copper.

.. thumbnail:: /_images/screenshots/metal/as_metal_measured_brass.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Setup using measured complex ior for brass.

.. thumbnail:: /_images/screenshots/metal/as_metal_measured_aluminium3.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Measured aluminium with textured radial anisotropy.

.. thumbnail:: /_images/screenshots/metal/as_metal_copper3.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Textured copper with a different IBL.

.. thumbnail:: /_images/screenshots/metal/as_metal_silver2.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Textured silver with a different IBL.

.. thumbnail:: /_images/screenshots/metal/as_metal_aniso_steel2.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Textured anisotropic steel with another different IBL.

.. thumbnail:: /_images/screenshots/metal/as_metal_gold2.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Gold with another IBL setup.

.. thumbnail:: /_images/screenshots/metal/as_metal_copper4.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Darker copper with another IBL setup.

.. thumbnail:: /_images/screenshots/metal/as_metal_steel3.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Dirty isotropic steel in another IBL setup.

.. thumbnail:: /_images/screenshots/metal/as_metal_measured_copper2.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Setup using measured complex ior for copper.

.. thumbnail:: /_images/screenshots/metal/as_metal_measured_brass2.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Setup using measured complex ior for brass.

.. thumbnail:: /_images/screenshots/metal/as_metal_measured_aluminium4.png
   :group: shots_as_metal_group_A
   :width: 10%
   :title:

   Measured aluminium with textured radial anisotropy in a different IBL setup.

-----

.. rubric:: Footnotes

.. [#] From *.nk* files, containing the data for several metals, alloys, semi-conductors in several wavelenght ranges (not exclusively in the visible light range). Maya attribute presets are provided for aluminium, brass, chromium, copper, gold, iron, lead, mercury, nickel, osmium, platinum, aluminium-gold intermetallic (*purple plague*), silver, titanium, titanium nitride, tungsten and zinc. See `the LuxPop database <http://www.luxpop.com/HU_v172.cgi?OpCode=73>`_ and `refractive index database <https://refractiveindex.info/>`_ for more measured complex ior data.

.. [#] Such as the :ref:`anisotropy vector field node <label_as_anisotropy_vector_field>`.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

