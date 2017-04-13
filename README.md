## appleseed-maya

appleseed-maya is an appleseed plugin for [Autodesk® Maya®](http://www.autodesk.com/products/maya/overview).

### Setup

1. Set environment variables:

        export MAYA_PLUG_IN_PATH=${BUILD_DIR}/src/appleseedmaya
        export MAYA_SCRIPT_PATH=${THIS_DIR}/scripts
        export PYTHONPATH=$PYTHONPATH:${APPLESEED_DIR}/lib/python2.7:${THIS_DIR}/scripts
        export XBMLANGPATH=${THIS_DIR}/icons
        export MAYA_PRESET_PATH=${THIS_DIR}/presets:$MAYA_PRESET_PATH
        export APPLESEED_SEARCHPATH=${APPLESEED_DIR}/sandbox/shaders/maya

2. Launch Maya and load the plugin.

### Planned features

- [x] appleseed project export
- [ ] Command line render
- [ ] Batch render
- [x] Render to RenderView
- [ ] IPR
- [ ] Motion blur
- [x] IBL
- [x] Maya shading nodes support
- [ ] OSL shaders
- [ ] Scripted access to translated scenes
- [ ] Bifrost
- [ ] XGen

### Maya shading nodes status

List of Maya hypershade nodes and their status. Not all will be supported.

#### Utilities

- [x] add double linear
- [x] add matrix
- [ ] angle between
- [ ] array mapper
- [x] blend colors
- [ ] blend two attr
- [x] bump 2d
- [x] bump 3d
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
- [ ] frame cache (Unsupported)
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
- [x] 3d placement
- [x] plus minus average
- [ ] projection (**unsupported**, see AS Projection instead)
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

#### Image planes

- [ ] image plane

#### Surface

- [ ] GLSL shader (**unsupported**)
- [ ] shaderfx shader (**unsupported**)
- [ ] stingrayPBS (**unsupported**)
- [x] anisotropic
- [ ] bifrost aero material
- [ ] bifrost foam material
- [ ] bifrost liquid material
- [x] blinn
- [ ] cgfx shader (**unsupported**)
- [ ] hair tube shader
- [x] lambert
- [ ] layered shader
- [ ] ocean shader
- [x] phong
- [x] phongE
- [ ] ramp shader
- [ ] shading map (**unsupported**)
- [ ] surface shader
- [ ] use background

#### Volumetric

- [ ] env fog
- [ ] fluid shape
- [ ] light fog
- [ ] particle cloud
- [ ] volume fog
- [ ] volume shader

#### Displacement

- [ ] displacement
- [ ] C muscle shader

#### 2D textures

- [x] bulge
- [x] checker
- [x] cloth
- [x] file
- [ ] fluid texture 2d
- [x] fractal
- [x] grid
- [ ] mandelbrot
- [x] mountain
- [x] movie
- [x] noise
- [ ] ocean
- [x] psd file
- [x] ramp
- [x] substance (Use node image file output)
- [ ] substance output
- [x] water

#### 3D textures

- [ ] brownian
- [x] cloud
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

#### Env textures

- [ ] env ball
- [x] env chrome
- [ ] env cube
- [ ] env sky
- [ ] env sphere

Other Textures:

- [x] layered texture

#### Lookdevkit nodes

- [ ] simplex noise

#### Lights

- [ ] ambient light
- [x] area light
- [x] directional light
- [x] point light
- [x] spot light
- [ ] volume light

#### Custom Nodes

##### Surface

- [x] AS Disney Material
- [x] AS Glass
- [ ] AS Car Paint

##### Textures

- [x] AS Voronoi3D
- [x] AS Voronoi2D
- [ ] AS Noise3D
- [ ] AS Noise2D

##### Manifolds

- [ ] AS Projection

##### Utility

- [ ] AS Color Transform
- [ ] AS Color Balance

