.. _label_as_switch_texture:

.. fix_img_align::

|
 
.. image:: /_images/icons/asSwitchTexture.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Switch Texture Icon

asSwitchTexture
***************

A node that switches the output texture from a list of 8 inputs, based on object or instance IDs, names, or string patterns, facilitating the creation of automatic variation in large scenes.

Parameters
----------

.. bogus directive to silence warnings::

-----

Color Parameters
^^^^^^^^^^^^^^^^

*Input Color 0*
    The first color in the list of colors or textures to evaluate.

*(...)*

.. note::

   The subsequent inputs follow exactly the same structure and parameterization.

*Input Color 7*
    The last color in the list of colors or textures to evaluate.

*Cycle Mode*
    When cycling through the list of colors or textures, if the index goes outside the list bounds, then one has the choice to cycle back to the beginning of the list, or if to clamp to the list size.

-----

Manifold Parameters
^^^^^^^^^^^^^^^^^^^

*Manifold Type*
    The type of manifold to use in order to determine which lookup to do in the list of colors or textures.
    It can be one of the following

        * Object Name
        * Object Instance Name
        * Assembly Name
        * Assembly Instance Name
        * Face ID
        * String Prefix
        * String Suffix
        * Find String

String
""""""

*Expression*
    The expression [#]_ to search in the expression domain. 

*Domain*
    The domain used to search an expression for.

*Seed*
    The seed [#]_ to use for the manifold.
    
-----

Outputs
-------

*Output Color*
    The resulting color.

-----

.. rubric:: Footnotes

.. [#] Regular expressions, `or regex <https://en.wikipedia.org/wiki/Regular_expression>`_. If you're unfamiliar with it, it allows the creation of complex patterns for string and substring matching. You can validate your expressions `here at regex101 <https://regex101.com/>`_.

.. [#] A number used to initialize a pseudo random number generator, to allow some degree of determinism in a system. See `random seed <https://en.wikipedia.org/wiki/Random_seed>`_ for more information.

