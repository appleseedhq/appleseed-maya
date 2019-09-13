.. _label_as_plastic:

.. fix_img_align::

|

.. image:: /_images/icons/asPlastic.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Plastic BSDF

asPlastic
*********

A physically correct plastic shader, stacking a specular term on top of a diffuse substrate.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Diffuse Parameters
^^^^^^^^^^^^^^^^^^

*Diffuse Color*
    The diffuse color.

*Diffuse Weight*
    A scaling factor for the diffuse BRDF.

*Scattering*
    The scattering amount for the diffuse term. Full scattering will take into account all the events in the specular term, while a value of 0 will disable them.

Specular Parameters
^^^^^^^^^^^^^^^^^^^

*Specular Color*
    The specular color.

*Specular Weight*
    A scaling factor for the specular BRDF.

*Index of Refraction*
    The index of refraction for the (dielectric [#]_) Fresnel term.

*Specular Roughness*
    The apparent surface roughness, with a value near 0 producing sharp highlights, and a value of 1.0 producing soft diffuse like highlights.

.. note::

    As specular roughness values increase, there is energy loss due to the lack of multiple scattering :cite:`Heitz:2016:MMB:2897824.2925943`. Appleseed however does compensate for this energy loss.

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
    The plastic BRDF output color.

*Output Matte Opacity*
    The matte holdout.

-----

.. _label_as_plastic_screenshots:

Screenshots
-----------

.. thumbnail:: /_images/screenshots/plastic/as_plastic_dirtyplastic.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   A dirty plastic material, using Substance maps.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_dirtyplastic3.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   Yet another dirty plastic material.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_dirty_rubber.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   A dirty black rubber material, with a dust layer, using the Student's t-MDF with a moderate roughness.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_fiberglass.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   A fiber glass like material, using the Student's t-MDF, moderate specular spread.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_yellowball1.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   A painted plastic material.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_painted_wall.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   A painted wall like material, with plastic paint.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_painted_wall3.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   Yet another painted wall material.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_painted_wall5.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   Painted concrete, with flaking stucco.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_ball.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   A plastic beach ball.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_dirtyplastic2.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   A dirty plastic material, using Substance maps.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_dirtyplastic4.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   Yet another dirty plastic material.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_dirty_rubber2.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   A dirty black rubber material, with a dust layer, using the Student's t-MDF with a moderate roughness.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_fiberglass2.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   A fiber glass like material, using the Student's t-MDF, moderate specular spread.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_yellowball2.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   A painted plastic material.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_painted_wall2.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   A painted wall like material, with plastic paint.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_painted_wall4.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   Yet another painted wall material.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_painted_wall6.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   Painted concrete, with flaking stucco.

.. thumbnail:: /_images/screenshots/plastic/as_plastic_ball2.png
   :group: shots_as_plastic_group_A
   :width: 10%
   :title:

   A plastic beach ball.

-----

.. rubric:: Footnotes

.. [#]  Dielectric is a material which is an electric insulator, the opposite of *conductors* which as the name says, conducts electricity. See `this page on dielectric materials <https://en.wikipedia.org/wiki/Dielectric>`_ for more details. In terms of look development an accepted simplification is that dielectrics have white or non-tinted specular highlights, while conductors have tinted or coloured specular highlights.

.. [#] The microfacet distribution function is a function that describes statistically the microscopic shape of the surface's as a distribution of microfacet orientations. See the `this page on the normal distribution function (NDF) <http://www.reedbeta.com/blog/hows-the-ndf-really-defined/>`_, and this page on `specular highlights <https://en.wikipedia.org/wiki/Specular_highlight>`_ for more details.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

