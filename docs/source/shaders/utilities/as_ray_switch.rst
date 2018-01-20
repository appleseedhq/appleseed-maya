.. _label_as_ray_switch:

.. fix_img_align::

|
 
.. image:: /_images/icons/asRaySwitch.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Ray Switch Icon

asRaySwitch
***********

A node that returns a specific input color according to the ray type being evaluated by the renderer :cite:`Heckbert:1990:ART:97880.97895`.

Parameters
----------

.. bogus directive to silence warnings::

-----

Color Attributes
^^^^^^^^^^^^^^^^

*Camera Ray Color*
    The color that passes through for primary visibility rays, also known as *camera* rays.

*Light Ray Color*
    The color that passes through *light* rays, involved in light emitting surfaces and light sources (via EDF or *emittance distribution functions*). This is also used in Bi-Directional Path Tracing [#]_ and in Stochastic Progressive Photon Mapping [#]_.

*Shadow Ray Color*
    The color that passes through for *shadow* rays, that compute the visibility between two points.

*Transparency Ray Color*
    The color that passes through for rays of type *transparency", for matte holdouts, and when the transparency closure is evaluated.

*Diffuse Ray Color*
    The color that passes through for rays of type *diffuse*, typically involved in indirect diffuse lighting.

*Glossy Ray Color*
    The color that passes through for *glossy* rays, typically involved in glossy indirect specular lighting, such as blurry/soft reflections or refractions.

*Specular Ray Color*
    The color that passes through for *specular* rays, typically involved in the calculation of perfect mirror reflective or refractive surfaces.

*Subsurface Ray Color*
    The color that passes through for rays of type *subsurface*, typically involved in lighting calculations of `BSSRDFs <https://en.wikipedia.org/wiki/Bidirectional_scattering_distribution_function>`_.

-----

Outputs
^^^^^^^

*Output Color*
    The color that passed passed through for the specific ray type.

-----

.. _label_as_ray_switch_screenshots:

Screenshots
-----------

.. thumbnail:: /_images/screenshots/ray_switch/ray_switch_colorramp_workingspace_rec709.png
   :group: shots_ray_switch_group_A
   :width: 10%
   :title:

   Original color ramp, synColor render/working space set to (scene-linear) sRGB/Rec.709 primaries and D65 white point.

-----

.. rubric:: Footnotes

.. [#] BDPT for short, see :cite:`Veach:PhD`

.. [#] SPPM for short, see :cite:`Hachisuka:2009:SPP:1618452.1618487`


-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

