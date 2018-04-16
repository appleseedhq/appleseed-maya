.. _label_as_bump:

.. fix_img_align::

|
 
.. image:: /_images/icons/asBump.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Bump Icon

asBump
******

A node that allows the user to apply scalar bump mapping, or normal mapping.

|

Parameters
----------

.. bogus directive to silence warnings::

-----

.. _label_bump_mode_parameters:

Mode Parameters
^^^^^^^^^^^^^^^

*Mode*
    The choice of bump mapping algorithm to use. It can be one of:

        * Bump :cite:`Blinn:1978:SWS:965139.507101`
        * Normal Map :cite:`Cohen:1998:AS:280814.280832`, :cite:`Cignoni:1998:GMP:288216.288224`


.. _label_bump_parameters:

Bump Parameters
^^^^^^^^^^^^^^^

*Bump Value*
    A scalar value controlling the magnitude of the bump effect.

*Bump Depth*
    A scalar value controlling the bump depth of the bump effect. Unlike the *bump value* which expects a value in [0,1] range, the *bump depth* can be a positive or negative value, in which case it will apply the bump effect outwards or inwards respectively.

Normal Map
^^^^^^^^^^

*Normal Map Weight*
    A scaling factor that defines the contribution weight of the normal map. With a value of 0.0, no contribution takes place and the regular surface normal is used. A value of 1.0 defines full contribution of the normal map input.

*Normal Map*
    The input normal map color.

*Map Coordinate System*
    The coordinate system of the input normal map used [#]_. It can be one of:
    
        * *Tangent Space*
        * *Object Space*
        * *World Space*

Advanced Parameters
"""""""""""""""""""

*Map Signedness*
    The signedness of the input normal map. If your map is in [-1,1] range, use *Signed*. If your map is in [0,1] range, used *Unsigned*.

*Flip R*
    Flip the *red* channel of the input tangent space normal map.

*Flip G*
    Flip the *green* channel of the input tangent space normal map.

*Swap RG*
    Swap the *red* and *green* channels of the input tangent space normal map.

.. note:: The channel flipping and swapping options **only** have effect on the tangent space normal maps. They are ignored when the *Map Coordinate System* is *Object Space* or *World Space*.

Surface Parameters
^^^^^^^^^^^^^^^^^^

*Surface Normal*
    The base surface normal to use. It can be the result of a previous bump node, or the global variable *N* from the :ref:`asAttributes node <label_as_attributes>`. 
    If not set, it defaults to the (world space) surface normal N.

-----

Outputs
-------

*Result*
    The unit length world space bumped normal.

-----

.. rubric:: Footnotes

.. [#] Usually one uses *tangent space* normal maps, but the option is provided here to use *object* and *world* space normal maps, which sometimes can be exported from other applications.  

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

