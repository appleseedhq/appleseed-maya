.. _label_as_anisotropy_vector_field:

.. fix_img_align::

|
 
.. image:: /_images/icons/asAnisotropyVectorField.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Anisotropy Vector Field Icon

asAnisotropyVectorField
***********************

A node that manipulates anisotropy vector maps and outputs a anisotropy vector for use in BxDFs that support anisotropy.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

Field Parameters
^^^^^^^^^^^^^^^^

*Field Color*
    The anisotropy vector map color.

*Rotation Angle*
    A rotation angle, in degrees, from -360 to 360 degrees, to rotate the input anisotropy vector map. This is an absolute value to re-orient the initial anisotropy vector map, non-texturable.

*Field Mode*
    The input anisotropy vector map can have the anisotropy directions along X and Y, encoded in the Red and Green channels, or in the Red and Blue channels.
    Typically, RG encoding is used in applications such as RenderMan [#]_, while RB encoding is used in applications such as Modo [#]_ or Softimage [#]_.
    The values allowed are therefore

        * Red/Green as XY
        * Red/Blue as XY

-----

Rotation Parameters
^^^^^^^^^^^^^^^^^^^

*Rotation Value*
    This parameter rotates the RG or RB vector field, and unlike the *Rotation Angle* parameter, this parameter takes as inputs values between 0.0 and 1.0, and is texturable. Internally this is mapped to [0,360] degrees.

*Rotation Mode*
    The rotation computed earlier in *Rotation Value* can be added to the vector field, or the rotation can be applied to both directions along a central axis. That is, the rotation can be mapped from the input [0,1] values to [0,360] degrees (Absolute), or to [-360,360] degrees (Centered) - where a value of 0.5 means no rotation. Values below 0.5 means counter-clockwise rotation, and values above 0.5 imply clockwise rotation.
    The allowed modes are therefore

        * Centered
        * Absolute

----

Output Parameters
^^^^^^^^^^^^^^^^^

*Normalize Output*
    Parameter that forces a normalization of the final anisotropy vector. The default is to enable it.

-----

Surface Normal
^^^^^^^^^^^^^^

*Surface Normal*
    The world space, unit length shading normal, bump mapped. **Not** a tangent space nor a object space normal.

-----

Outputs
-------

*Anisotropy Vector*
    The final re-oriented, RG or RB encoded anisotropy vector.

-----

.. rubric:: Footnotes

.. [#] See RenderMan's `tangent field <https://rmanwiki.pixar.com/display/REN/PxrTangentField>`_ for details on PRman's implementation.

.. [#] See `Modo anisotropy help <http://modo.docs.thefoundry.co.uk/modo/501/help/pages/shaderendering/ShaderItems/AnisotropicDirection.html>`_ for details on the Modo implementation. Unlike PRman's, the XY data is encoded in R and B channels, not R and G.

.. [#] See `Softimage anisotropy patterns <http://www.softimageblog.com/archives/197>`_. Like Modo, the anisotropy data is encoded in the R and B channels.

