## Experimental [appleseed](http://appleseedhq.net/)-[maya](http://www.autodesk.com/products/maya/overview) integration plugin

#### Setup

1. Set environment variables:

        export MAYA_PLUG_IN_PATH=${BUILD_DIR}/src
        export MAYA_SCRIPT_PATH=${BUILD_DIR}/appleseed-maya/scripts
        export PYTHONPATH=$PYTHONPATH:${APPLESEED_DIR}/lib/python2.7:${BUILD_DIR}/scripts
        export XBMLANGPATH=${BUILD_DIR}/icons
        export APPLESEED_SEARCHPATH=${APPLESEED_DIR}/sandbox/shaders/maya

2. Launch Maya and load the plugin.

#### Planned features

- appleseed project export
- Command line render
- Batch render
- Render to RenderView
- IPR
- Motion blur
- IBL
- Maya shading nodes support
- OSL shaders
- Scripted access to translated scenes
- Bifrost
- XGen

#### Maya shading nodes status

List of Maya hypershade nodes and their status. Not all will be supported.

### Utilities

- [x] add double linear
- [x] add matrix
- [ ] angle between
- [ ] array mapper
- [x] blend colors
- [ ] blend two attr
- [ ] bump 2d
- [ ] bump 3d
- [ ] choice
- [ ] chooser
- [x] clamp
- [x] clearcoat
- [ ] color profile
- [ ] compose matrix
- [x] condition
- [x] contrast
- [ ] curve info
- [ ] decompose matrix
- [x] distance between
- [x] double switch
- [ ] euler to quat
- [x] four by four matrix
- [ ] frame cache
- [x] gamma correct
- [ ] height field
- [x] hsv to rgb
- [x] inverse matrix
- [ ] light info
- [x] luminance
- [x] mult double linear
- [x] mult matrix
- [x] multiply divide
- [ ] particle sampler
- [x] 2d placement
- [ ] 3d placement
- [x] plus minus average
- [ ] projection
- [x] quad switch
- [ ] quat add
- [ ] quat conjugate
- [ ] quat invert
- [ ] quat negate
- [ ] quat normalize
- [ ] quat prod
- [ ] quat sub
- [ ] quat to euler
- [ ] remap color
- [ ] remap hsv
- [ ] remap value
- [x] reverse
- [x] rgb to hsv
- [x] sampler info
- [x] set range
- [x] single switch
- [x] stencil
- [ ] surface info
- [ ] surf. luminance
- [x] transpose matrix
- [x] triple switch
- [ ] unit conversion
- [ ] uv chooser
- [x] vector product
- [x] weighted add matrix

### Image Planes

- [ ] image plane

### Surface

- [ ] GLSL shader (**unsupported**)
- [ ] shaderfx shader (**unsupported**)
- [ ] stingrayPBS (**unsupported**)
- [ ] anisotropic
- [ ] bifrost aero material
- [ ] bifrost foam material
- [ ] bifrost liquid material
- [ ] blinn
- [ ] cgfx shader (**unsupported**)
- [ ] hair tube shader
- [x] lambert
- [ ] layered shader
- [ ] ocean shader
- [ ] phong
- [ ] phongE
- [ ] ramp shader
- [ ] shading map
- [ ] surface shader
- [ ] use background

### Volumetric

- [ ] env fog
- [ ] fluid shape
- [ ] light fog
- [ ] particle cloud
- [ ] volume fog
- [ ] volume shader

### Displacement

- [ ] displacement:
- [ ] C muscle shader
- [ ] displacement

### 2D Textures

- [x] bulge
- [x] checker
- [ ] cloth
- [ ] file
- [ ] fluid texture 2d
- [ ] fractal
- [x] grid
- [ ] mandelbrot
- [ ] mountain
- [ ] movie
- [ ] noise
- [ ] ocean
- [ ] psd file
- [ ] ramp
- [ ] substance
- [ ] substance output
- [ ] water

### 3D Textures

- [ ] brownian
- [ ] cloud
- [ ] crater
- [ ] fluid texture 3d
- [ ] granite
- [ ] leather
- [ ] mandelbrot 3d
- [ ] marble
- [ ] rock
- [ ] snow
- [ ] solid fractal
- [ ] stucco
- [ ] volume noise
- [ ] wood

### Env Textures

- [ ] env ball
- [ ] env chrome
- [ ] env cube
- [ ] env sky
- [ ] env sphere

Other Textures:

- [ ] layered texture

### Lookdevkit nodes

- [ ] simplex noise


### Lights

- [ ] ambient light
- [ ] area light
- [x] directional light
- [x] point light
- [x] spot light
- [ ] volume light
