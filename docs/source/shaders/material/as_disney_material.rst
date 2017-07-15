.. _label_as_disney_material:
.. image:: /_images/icons/as_disney_material.png
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
^^^^^^^

*Output Color*
    The final result color.

*Output Transparency*
    **Unused** presently.

*Output Matte Opacity*
    **Unused** presently

-----

.. _label_disney_material_screenshots:

Screenshots
-----------

.. thumbnail:: /_images/screenshots/voronoi2d/voronoi2d_euclidian_f1.png
   :group: shots_voronoi2d_group_A
   :width: 10%
   :title:

   Euclidian metric, with the first feature nearest to the evaluated cell.

-----

.. rubric:: Footnotes

.. [#] https://en.wikipedia.org/wiki/Bidirectional_scattering_distribution_function

-----

.. rubric:: References

.. _DisneyBSDF:  `Physically Based Shading at Disney (pdf)<http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_slides_v2.pdf>`

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

