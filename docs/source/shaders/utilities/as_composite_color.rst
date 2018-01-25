.. _label_as_composite_color:

.. fix_img_align::

|
 
.. image:: /_images/icons/asCompositeColor.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Composite Color Icon

asCompositeColor
****************

A node to `composite <https://en.wikipedia.org/wiki/Alpha_compositing>`_ two RGBA inputs using `Porter-Duff <http://ssp.impulsetrain.com/porterduff.html>`_ :cite:`Porter:1984:CDI:800031.808606` compositing operators.

Parameters
----------

.. bogus directive to silence warnings::

-----

Color Parameters
^^^^^^^^^^^^^^^^

*Source Color*
    The source color.

*Source Alpha*
    The source alpha channel.

*Destination Color*
    The destination color.

*Destination Alpha*
    The destination alpha channel.

*Composite Mode*
    The composite operator [#]_ to use. It can take the following values

        * Source
        * Destination
        * Over
        * Under
        * In
        * Mask
        * Out
        * Stencil
        * Atop
        * Dst-Atop
        * Xor
        * Matte

.. seealso::
   
   *Merging and Transformation of Raster Images for Cartoon Animation* :cite:`Wallace:1981:MTR:965161.806813` and the `W3.org webpage <https://www.w3.org/TR/compositing-1/#advancedcompositing>`_ for a detailed view on compositing algebra in general.

*Clamp Output*
    Checking this checkbox will clamp the output into the [0,1] range.

-----

Outputs
-------

*Output Color*
    The composited color.

*Output Alpha*
    The alpha with the result of the compositing operation.

-----

.. rubric:: Footnotes

.. [#] The original `Porter-Duff paper (PDF file) can be found here <https://keithp.com/~keithp/porterduff/>`_.

-----

.. rubric:: References

.. bibliography:: /bibtex/references.bib
    :filter: docname in docnames

