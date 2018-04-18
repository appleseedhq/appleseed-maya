.. _label_as_globals:

.. fix_img_align::

|
 
.. image:: /_images/icons/asGlobals.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Globals Icon

asGlobals
*********

A node that exposes the OSL [#]_ global variables to the user, **and** the appleseed specific global variables as well.

|

Parameters
----------

.. bogus directive to silence warnings::

This node has no input parameters.

-----

Outputs
-------

*Surface Position*
    The surface point *P* in world space.

*Reference Position*
    The surface point *Pref* of a reference or pose mesh, in world space.

*Light Point Position*
    The position in the light source, *Ps*, in world space.

*Shading Normal*
    The surface normal *N* in world space, not necessarily normalized.

*Reference Normal*
    The surface normal *Nref* of a reference or pose mesh, in world space.

*Geometric Normal*
    The true geometric normal *Ng* in world space, not necessarily normalized.

*Viewer Vector*
    The vector *I* pointing from the eye position into the surface point being shaded, unit length.

*Bitangent Vector*
    The bitangent or binormal vector *Bn*, unit length.

*Tangent Vector*
    The tangent vector *Tn*, unit length.

*Shutter Time*
    The *time* global variable, denoting shutter time.

*Time Amount*
    The amount of time *dtime* covered by the shading sample.

*dP/dtime*
    Derivative of *P* in regard to *time*, or how *P* changes per unit *time*.

*U Coordinate*
    Texture coordinate *u*.

*V Coordinate*
    Texture coordinate *v*.

*UV Coordinates*
    A float array containing both the *u* and *v* coordinates, sometimes required for some applications that expect this specific data type.

*dN/du*
    The global variable *dNdu* is a appleseed specific global variable, and denotes the change of *N* in regard to *u*.

*dN/dv*
    The global variable *dNdv* is a appleseed specific global variable, and denotes the change of *N* in regard to *v*.

*dP/du*
    The global variable *dPdu*, denoting the rate of change or partial derivative of *P* in regard to *u*. 

*dP/dv*
    The global variable *dPdv*, denoting the rate of change or partial derivative of *P* in regard to *v*.

*dP/dx*
    The rate of change or partial derivative of *P* in regard to *x*, or ``Dx(P)``.

*dP/dy*
    The rate of change or partial derivative of *P* in regard to *y*, or ``Dy(P)``.

*dP/dz*
    The rate of change or partial derivative of *P* in regard to *z*, or ``Dz(P)``.

.. attention:: Some of the global variables exposed are specific to appleseed, hence shaders using these variables will not be entirely portable, unless the target renderers have some measure of equivalency.

-----

.. rubric:: Footnotes

.. [#] `OSL or Open Shading Language <https://github.com/imageworks/OpenShadingLanguage>`_.

