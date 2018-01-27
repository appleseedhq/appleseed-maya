.. _label_as_texture_info:

.. fix_img_align::

|
 
.. image:: /_images/icons/asTextureInfo.png
   :width: 128px
   :align: left
   :height: 128px
   :alt: Texture Info Icon

asTextureInfo
*************

A node providing the user with a number of potentially useful information for the input texture. Notice that the input texture can be any texture, a 2D or 3D texture. The list of information queried might be extended to include appleseed specific metadata.

Parameters
----------

.. bogus directive to silence warnings::

-----

Texture Parameters
^^^^^^^^^^^^^^^^^^

*Texture File*
    The input texture file.

-----

Outputs
-------

*Average Color*
    The average color of the first 3 channels of the texture file.

*Average Alpha*
    The average value of the channel with the *A* name in the texture file, if it exists.

*Resolution*
    A vector containing the X and Y resolution in its first two components, or the X, Y and Z resolution in the case of input 3D texture file.

*Channels*
    The total number of channels in the input texture file.

*Subimages*
    The number of subimages in the texture file. In the EXR case, this refers to the number of additional channels.

*Texture Type*
    Returns the type of the texture file, with the following strings according to the input type

        * *Plain Texture*
        * *Shadow*
        * *Environment*
        * *Volume Texture*

*Texture Format*
    The texture format of the input file, differentiating between the format of the fundamental texture types. The return strings can take the following values

        * *Plain Texture*
        * *Shadow*
        * *CubeFace Shadow*
        * *Volume Shadow*
        * *CubeFace Environment*
        * *LatLong Environment*
        * *Volume Texture*

*Data Window Minimum*
    A vector containing the minimum X and Y values of the pixel data window of the input texture, in the case of a 2D texture file. And the minimum X, Y and Z values of the pixel data window in the case of a 3D texture file.

*Data Window Maximum*
    A vector containing the maximum X and Y values of the pixel data window of the input texture, in the case of a 2D texture file. And the maximum X, Y and Z values of the pixel data window in the case of a 3D texture file.

*Display Window Minimum*
    A vector containing the minimum X and Y values of the full window of the image, in the case of a 2D texture file, and the minimum X, Y and Z values in the case of a 3D texture file.

*Display Window Maximum*
    A vector containing the maximum X and Y values of the full window of the image, in the case of a 2D texture file, and the maximum X, Y and Z values in the case of a 3D texture file.

*World To Camera Matrix*
    When the input texture is a rendered image, this parameter provides the world to camera transformation matrix used.

*World To Screen Matrix*
    When the input texture is a rendered image, this parameter provides the matrix that transforms points from *world* space into *screen* space - a 2D coordinate system where the *x* and *y* values are in [-1,1] range.

-----

.. seealso::

   The `Open Shading Language documentation <https://github.com/imageworks/OpenShadingLanguage/blob/master/src/doc/osl-languagespec.pdf>`_ at github.

