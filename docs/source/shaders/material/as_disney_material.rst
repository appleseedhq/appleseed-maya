.. _label_as_disney_material:

.. fix_img_align::

|
 
.. image:: /_images/icons/asDisneyMaterial.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Disney BSDF

asDisneyMaterial
****************

The Disney *principled* BRDF :cite:`McAuley:2012:PPS:2343483.2343493`, a physically based material.

|

Parameters
----------

.. bogus directive to silence warning::

-----

Common Material Parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^

*Surface Color*
    The surface color

*Subsurface Amount*
    The amount of sub-surface scattering [#]_

-----

Specular Parameters
^^^^^^^^^^^^^^^^^^^

*Specular Amount*
    The amount of specular highlights

*Specular Roughness*
    The apparent surface roughness affecting the specular highlights

*Specular Tint*
    The tinting amount of the specular highlights, with a value 0 having achromatic highlights, and a value of 1.0 inheriting the surface color.

*Metallicness*
    A low value is suitable for the appearance of dielectrics such as plastic, with a higher value producing more intensity specular reflections, and more suitable for conductors.

*Anisotropy Amount*
    The amount of anisotropy affecting the specular highlights, with a value of 0.0 producing isotropic highlights, and a value of 1.0 producing full anisotropic highlights.

*Anisotropy Angle*
    The angle affecting anisotropic highlights, which maps the values from [0,1] range into [0,360] range.

*Anisotropy Vector Map*
    A vector tangent field (color) map, with the *X* and *Y* anisotropy directions encoded in the *R* and *G* channels.

-----

Sheen Parameters
^^^^^^^^^^^^^^^^

*Sheen Amount*
    The amount of sheen, see Westin velvet.

*Sheen Tint*
    The tinting (color) affecting the sheen effect.

-----

Coating Parameters
^^^^^^^^^^^^^^^^^^

*Coating Amount*
    The amount of (clear) coating on the material, producing an extra layer of (usually) sharp highlights.

*Coating Glossyness*
    The coating layer glossyness, with low values producing a dull like highlight, similar to a high surface roughness value, and higher values producing sharper highlights.

-----

Bump Parameters
^^^^^^^^^^^^^^^

*Bump Normal*
    The unit length (bump) normal, usually passed from a *bump2d* or a *bump3d* node.

-----

Advanced Parameters
^^^^^^^^^^^^^^^^^^^

*Ray Depth*
    The maximum number of bounces a ray is allowed to travel.

-----

Outputs
-------

*Output Color*
    The final result color.

*Output Transparency*
    The resulting transparency (unused presently).

*Output Matte Opacity*
    The resulting matte opacity.

-----

.. _label_disney_material_screenshots:

Screenshots
-----------

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_velvety.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   A velvety like appearance, with sheen.

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_anisotropy_satin.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Satin like material with anisotropy, sheen, moderate to low roughness.

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_hammered_copper.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Hammered copper like material, with high metalicness, moderate roughness, and a cellnoise based tangent space normal map.

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_metal.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Blue metallic alloy, with moderate metalicness, high roughness.

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_carbonfiber.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Tiled carbon fiber anisotropy vector map showing a use case of anisotropy, no metalicness, moderate roughness, and coating on top.


.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_metal_scratch.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Metalic surface with scratches encoded in a anisotropy vector map. No coating, moderate roughness and high metalicness.

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_coated_alloy.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Coated alloy like surface with high roughness and sharp coating.

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_blue_plastic.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Plastic like surface with mapped (single scattering) subsurface, specular roughness.

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_painted_metal.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Painted metal surface, showing transition of *metalicness* values, roughness, bump.

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_velvety2.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   A velvety like appearance, with sheen.

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_anisotropy_satin2.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Satin like material with anisotropy, sheen, moderate to low roughness.  

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_hammered_copper2.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Hammered copper like material, with high metalicness, moderate roughness, and a cellnoise based tangent space normal map.

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_metal2.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Blue metallic alloy, with moderate metalicness, high roughness.

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_carbonfiber2.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Tiled carbon fiber anisotropy vector map showing a use case of anisotropy, no metalicness, moderate roughness, and coating on top.


.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_metal_scratch2.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Metalic surface with scratches encoded in a anisotropy vector map. No coating, moderate roughness and high metalicness.

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_coated_alloy2.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Coated alloy like surface with high roughness and sharp coating.

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_blue_plastic2.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Plastic like surface with mapped (single scattering) subsurface, specular roughness.

.. thumbnail:: /_images/screenshots/disney/as_disneymaterial_painted_metal2.png
   :group: shots_disney_material_group_A
   :width: 10%
   :title:

   Painted metal surface, showing transition of metalicness values, roughness, bump.

-----

.. rubric:: Footnotes

.. [#] In the shader, it's not a `full BSSRDF <https://en.wikipedia.org/wiki/Subsurface_scattering>`_, but an approximation using only single scattering.

-----

.. rubric:: References

.. _DisneyBSDF:  `Physically Based Shading at Disney (pdf)<http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_slides_v2.pdf>`

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

