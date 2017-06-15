.. _label_as_glass:
.. image:: ../images/as_glass.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Glass BSDF

*******
asGlass
*******

A Glass BSDF :cite:`Walter:2007:MMR:2383847.2383874`, with volumetric absorption.

Parameters
==========

.. bogus directive to silence warnings::

-----

Surface Transmittance
---------------------

*Transmittance Color*:
    Transmittance color.

*Transmittance Amount*:
    Transmittance amount

-----

Specular Parameters
-------------------

*Reflection Tint*:
    Reflection Tint

*Refraction Tint*:
    Refraction Tint

*Index of Refraction*:
    Index of Refraction

*Distribution*:
    Microfacet distribution function, it can be

    1. *Beckmann* :cite:`Cook:1982:RMC:357290.357293`
    2. *GGX* :cite:`heitz:hal-00996995` 

*Roughness*:
    Apparent surface roughness

*Anisotropy Amount*:
    Surface anisotropy amount

*Anisotropy Angle*:
    The anisotropy angle, in [0,1] range, maps to [0,360] degrees.

*Anisotropy Vector Map*:
    A color anisotropy vector tangent field, with the X and Y anisotropy encoded in the R and G channels.

-----

Volume Material Parameters
--------------------------

*Volume Transmittance*:
    Volume transmittance.

*Transmittance Distance*:
    Volume transmittance distance.

-----

Bump Parameters
---------------

*Bump Normal*:
    The unit length world space normal of the bumped surface.

.....

Advanced Parameters
-------------------

*Ray Depth*:
    The maximum ray depth a ray is allowed to bounce before being terminated.

-----

Outputs
=======

*Output Color*:
    The BSDF output color.

*Output Transparency*:
    The output transparency.

-----

.. note:: The output transparency is unused at the moment.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

