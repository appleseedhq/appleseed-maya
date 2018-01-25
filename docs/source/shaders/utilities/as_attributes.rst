.. _label_as_attributes:

.. fix_img_align::

|
 
.. image:: /_images/icons/asAttributes.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Attributes Icon

asAttributes
************

A node that exposes to the user the general attributes specified in OSL [#]_ and the appleseed renderer specific attributes.

|

Parameters
----------

.. bogus directive to silence warnings::

This node has no input parameters.

-----

Outputs
-------

*Assembly Instance ID*
    An integer value containing the ID of an assembly [#]_ instance.

*Assembly Instance Name*
    A string containing the name of the assembly instance.

*Assembly Name*
    A string containing the name of the assembly.

*Camera Clip Far*
    The farthest camera clipping plane.

*Camera Clip Near*
    The nearest camera clipping plane.

*Camera FOV*
    The camera Field Of View [#]_.

*Camera Pixel Aspect*
    The camera pixel aspect.

*Camera Projection*
    The camera projection.

*Camera Resolution X*
    The camera X resolution.

*Camera Resolution Y*
    The camera Y resolution.

*Screen Window X Max*
    The screen window *x* maximum value.

*Screen Window X Min*
    The screen window *x* minimum value.

*Screen Window Y Max*
    The screen window *y* maximum value.

*Screen Window Y Min*
    The screen window *y* minimum value.

*Shutter Close Time*
    The shutter closing time.

*Shutter Open Time*
    The shutter opening time.

*Object Instance ID*
    An integer containing an object instance's ID.

*Object Instance Index*
    An integer containing an object instance's index.

*Object Instance Name*
    A string containing the object instance's name.

*Object Name*
    A string containing an object's name.

*Ray Differentials*
    An boolean denoting if the path being traced has ray differentials.

*Ray Depth*
    A integer denoting the present ray depth.

*Ray IOR*
    The absolute index of refraction of the medium the present ray is travelling in. This is affected by nested dielectrics :cite:`ND.2002.10487555`.

*Ray Length*
    The current path's length.

-----

.. rubric:: Footnotes

.. [#] `OSL or Open Shading Language <https://github.com/imageworks/OpenShadingLanguage>`_.

.. [#] See appleseed's wiki entry for `assemblies concepts <https://github.com/appleseedhq/appleseed/wiki/Project-File-Format#conventions>`_ and `procedural assemblies <https://github.com/appleseedhq/appleseed/wiki/Procedural-Assemblies-Design-Document>`_.

.. [#] See `field of view wikipedia page <https://en.wikipedia.org/wiki/Field_of_view>` for more details.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

