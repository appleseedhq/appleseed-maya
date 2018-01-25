.. _label_as_id_manifold:

.. fix_img_align::

|
 
.. image:: /_images/icons/asIdManifold.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: ID Manifold Icon

asIdManifold
************

A utility node that generates a integer hash, a color ID, and a greyscale ID from the input geometry, based on a number of user-set criteria. Coupled with color variation or randomization nodes, this allows the user to shade effortlessly large scenes with natural looking variation.

Parameters
----------

.. bogus directive to silence warnings::

-----

Manifold Parameters
^^^^^^^^^^^^^^^^^^^

*Manifold Type*
    The criteria to use when generating the outputs. It can create the randomization outputs based on the object, instance or assembly names or their IDs, and based on string matching via regular expressions [#]_.
    It takes the following values

        * Object Name
        * Object Instance Name
        * Assembly Name
        * Assembly Instance Name
        * Face ID
        * String Prefix
        * String Suffix
        * Find String [#]_

String Parameters
^^^^^^^^^^^^^^^^^

*Expression*
    The string expression to search for in the object or instance name. This string can be a regular expression.

*Domain*
    The domain of the input for the string matching with the *Expression* parameter. It can take the following values

        * Object Name
        * Object Instance Name
        * Assembly Name
        * Assembly Instance Name

*Seed*
    The seed [#]_ to use when generating the randomization outputs.

-----

Output Mode Parameters
^^^^^^^^^^^^^^^^^^^^^^

*Output Mode*
    The type of outputs created with the node. It can take the values

        * Hash Only
        * Hash & Greyscale Value
        * Hash, Greyscale & Color ID

-----

Outputs
-------

*Output Hash*
    The resulting integer hash.

*Output ID*
    The resulting color ID.

*Output Greyscale*
    The resulting greyscale ID.

-----

.. rubric:: Footnotes

.. [#] Regular expressions, `or regex <https://en.wikipedia.org/wiki/Regular_expression>`_. If you're unfamiliar with it, it allows the creation of complex patterns for string and substring matching. You can validate your expressions `here at regex101 <https://regex101.com/>`_.

.. [#] This mode tries to match the pattern anywhere in the string, while the two previous modes (*string suffix* and *string prefix*) try to match the beginning and the end of the string that was passed.

.. [#] A number used to initialize a pseudo random number generator, to allow some degree of determinism in a system. See `random seed <https://en.wikipedia.org/wiki/Random_seed>`_ for more information.
   
