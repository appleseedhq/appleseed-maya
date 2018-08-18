.. _label_as_fresnel:

.. fix_img_align::

|
 
.. image:: /_images/icons/asFresnel.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Fresnel Icon

asFresnel
*********

A node that gives the user the reflection amount due to a viewer Fresnel term for dielectrics or conductors.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Fresnel Parameters
^^^^^^^^^^^^^^^^^^

*Fresnel Type*
    The type of Fresnel function to use. It can be one for dielectric [#]_ materials, or for conductors [#]_, with a physically based or an artist friendly :cite:`Gulbrandsen2014Fresnel` parameterization.
    The values it can take are therefore

        * Simple Dielectric
        * Artist Friendly
        * Physically Based

.. hint::

   You can find physically based values for the complex index of refraction [#]_ in literature or in online resources such as this `RefractiveIndex.Info <https://refractiveindex.info/>`_ or `LuxPop.com <http://www.luxpop.com/HU_v172.cgi?OpCode=73>`_.

*Index Of Refraction*
    The (monochromatic) absolute index of refraction for a dielectric, considered only when *Fresnel Type* is set to *Simple Dielectric*.

*Facing Tint*
    The R,G,B reflectance at normal or facing incidence for a conductor Fresnel. Considered only when the *Fresnel Type* parameter is set to *Artist Friendly*.

*Edge Tint*
    The R,G,B reflectance at edge or grazing incidence for Fresnel. Considered only when the *Fresnel Type* parameter is set to *Artist Friendly*.

*Complex IOR*
    The R,G,B index of refraction for the conductor Fresnel [#]_. Considered only when in *Physically Based* mode.

*Extinction Coefficient*
    The R,G,B extinction coefficient for the conductor Fresnel [#]_. Considered only when in *Physically Based* mode.

.. warning::
   
   In order to be physically correct, the Fresnel term would need to provide the amount of light reflected off the surface of the object, but the object's surface might be described (and typically is) by a statistical distribution of normal vectors. Therefore, the correct Fresnel term would depend on this distribution's microfacet normal, and subsequently on the surface roughness.

   This node however does **not** provide the Fresnel reflection amount off a microfacet normal, but from the true surface normal, a *viewer Fresnel term*.
   This is provided for creative freedom (i.e: creative blending of materials with :ref:`asBlendShader <label_as_blend_shader>` node.

-----

Globals Parameters
^^^^^^^^^^^^^^^^^^

*Surface Normal*
    The unit length, world space shading normal. You can use the bump normal here as well, as long as it's normalized and in *world* space.

*Viewer Vector*
    The unit length, world space vector pointing from the eye position to the surface point P being shaded.

-----

Outputs
-------

*Output Color*
    The output RGB Fresnel reflection amount.

*Output Alpha*
    The output  dielectric Fresnel amount when *Fresnel Type* is set to *Simple Dielectric*, or the luminance [#]_ of the *Output Color* when set to the other modes.

-----

.. rubric:: Footnotes

.. [#] See `dielectric definition <https://en.wikipedia.org/wiki/Dielectric>`_ and `Fresnel Equations <https://en.wikipedia.org/wiki/Fresnel_equations>`_ for details.

.. [#] See `conductor definition <https://en.wikipedia.org/wiki/Electrical_conductor>`_ for details.

.. [#] Complex index of refraction, where the real part :math:`\eta` is the index of refraction and describes the phase velocity of the wave, and the imaginary part :math:`\kappa` is the extinction coefficient and indicates the amount of attenuation when the electro-magnetic wave propagates through the material.
   See `Complex refractive index <https://en.wikipedia.org/wiki/Refractive_index#Complex_refractive_index>`_ for more details.

.. [#] More precisely the real part of the complex index of refraction of the conductor.

.. [#]  More precisely, the imaginary part of the complex index of refraction for a conductor.

.. [#] For now, the luminance is set to use the `ITU-R BT.709/Rec.709 <https://www.itu.int/rec/R-REC-BT.709/>`_ Y coefficients, since that is the working space appleseed is using. In the future, this will automatically reflect the choice of working or rendering space chosen by the user.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

