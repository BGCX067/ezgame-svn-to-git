#ifndef __NERI3D_EDIRVERTYPES_H__
#define __NERI3D_EDIRVERTYPES_H__


#define ETA_SELECTMASK        0x0000000f  // mask for arg selector
#define ETA_DIFFUSE           0x00000000  // select diffuse color (read only)
#define ETA_CURRENT           0x00000001  // select stage destination register (read/write)
#define ETA_TEXTURE           0x00000002  // select texture color (read only)
#define ETA_TFACTOR           0x00000003  // select D3DRS_TEXTUREFACTOR (read only)
#define ETA_SPECULAR          0x00000004  // select specular color (read only)
#define ETA_TEMP              0x00000005  // select temporary register color (read/write)
#define ETA_CONSTANT          0x00000006  // select texture stage constant
#define ETA_COMPLEMENT        0x00000010  // take 1.0 - x (read modifier)
#define ETA_ALPHAREPLICATE    0x00000020  // replicate alpha to color components (read modifier)


enum eCullingType
{
	EAC_OFF = 0,
	EAC_BOX = 1,
	EAC_FRUSTUM_BOX = 2,
	EAC_FRUSTUM_SPHERE = 4

};


//! Texture coord clamp mode outside [0.0, 1.0]
enum ETEXTURE_CLAMP
{
	ETC_CLAMP_U_CLAMP_V = 0, /*!< Clamp in both directions. */
	ETC_CLAMP_U_WRAP_V, /*!< Clamp in the S(U) direction but wrap in the T(V) direction. */
	ETC_WRAP_U_CLAMP_V, /*!< Wrap in the S(U) direction but clamp in the T(V) direction. */
	ETC_WRAP_U_WRAP_V, /*!< Wrap in both directions. */
	ETC_CLAMP_MAX_MODES
};

enum ETEXTURE_FILTERMODE
{
	FILTER_NEAREST = 0, /*!< Simply uses the nearest pixel.  Very grainy. */
	FILTER_BILERP = 1, /*!< Uses bilinear filtering. */
	FILTER_TRILERP = 2, /*!< Uses trilinear filtering. */
	FILTER_NEAREST_MIPNEAREST = 3, /*!< Uses the nearest pixel from the mipmap that is closest to the display size. */
	FILTER_NEAREST_MIPLERP = 4, /*!< Blends the two mipmaps closest to the display size linearly, and then uses the nearest pixel from the result. */
	FILTER_BILERP_MIPNEAREST = 5, /*!< Uses the closest mipmap to the display size and then uses bilinear filtering on the pixels. */
	FILTER_MAX_MODES = 6
};

enum ETEXTUREFILTERTYPE
{
    ETEXF_NONE            = 0,    // filtering disabled (valid for mip filter only)
    ETEXF_POINT           = 1,    // nearest
    ETEXF_LINEAR          = 2,    // linear interpolation
    ETEXF_ANISOTROPIC     = 3,    // anisotropic
    ETEXF_PYRAMIDALQUAD   = 6,    // 4-sample tent
    ETEXF_GAUSSIANQUAD    = 7,    // 4-sample gaussian
    ETEXF_FORCE_DWORD     = 0x7fffffff,   // force 32-bit size enum
};


enum ETEXTUREADDRESS 
{
    ETADDRESS_WRAP            = 1,
    ETADDRESS_MIRROR          = 2,
    ETADDRESS_CLAMP           = 3,
    ETADDRESS_BORDER          = 4,
    ETADDRESS_MIRRORONCE      = 5,
    ETADDRESS_FORCE_DWORD     = 0x7fffffff, /* force 32-bit size enum */
};



enum ETEXTURE_APPLY_MODE
{
	// see table below for apply mode equations
	APPLY_REPLACE,
	APPLY_DECAL,
	APPLY_MODULATE,
	APPLY_MAX_MODES

};


enum ETEXTUREOP
{
	// Control
	ETOP_DISABLE              = 1,      // disables stage
	ETOP_SELECTARG1           = 2,      // the default
	ETOP_SELECTARG2           = 3,

	// Modulate
	ETOP_MODULATE             = 4,      // multiply args together
	ETOP_MODULATE2X           = 5,      // multiply and  1 bit
	ETOP_MODULATE4X           = 6,      // multiply and  2 bits

	// Add
	ETOP_ADD                  =  7,   // add arguments together
	ETOP_ADDSIGNED            =  8,   // add with -0.5 bias
	ETOP_ADDSIGNED2X          =  9,   // as above but left  1 bit
	ETOP_SUBTRACT             = 10,   // Arg1 - Arg2, with no saturation
	ETOP_ADDSMOOTH            = 11,   // add 2 args, subtract product
	// Arg1 + Arg2 - Arg1*Arg2
	// = Arg1 + (1-Arg1)*Arg2

	// Linear alpha blend: Arg1*(Alpha) + Arg2*(1-Alpha)
	ETOP_BLENDDIFFUSEALPHA    = 12, // iterated alpha
	ETOP_BLENDTEXTUREALPHA    = 13, // texture alpha
	ETOP_BLENDFACTORALPHA     = 14, // alpha from D3DRS_TEXTUREFACTOR

	// Linear alpha blend with pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)
	ETOP_BLENDTEXTUREALPHAPM  = 15, // texture alpha
	ETOP_BLENDCURRENTALPHA    = 16, // by alpha of current color

	// Specular mapping
	ETOP_PREMODULATE            = 17,     // modulate with next texture before use
	ETOP_MODULATEALPHA_ADDCOLOR = 18,     // Arg1.RGB + Arg1.A*Arg2.RGB
	// COLOROP only
	ETOP_MODULATECOLOR_ADDALPHA = 19,     // Arg1.RGB*Arg2.RGB + Arg1.A
	// COLOROP only
	ETOP_MODULATEINVALPHA_ADDCOLOR = 20,  // (1-Arg1.A)*Arg2.RGB + Arg1.RGB
	// COLOROP only
	ETOP_MODULATEINVCOLOR_ADDALPHA = 21,  // (1-Arg1.RGB)*Arg2.RGB + Arg1.A
	// COLOROP only

	// Bump mapping
	ETOP_BUMPENVMAP           = 22, // per pixel env map perturbation
	ETOP_BUMPENVMAPLUMINANCE  = 23, // with luminance channel

	// This can do either diffuse or specular bump mapping with correct input.
	// Performs the function (Arg1.R*Arg2.R + Arg1.G*Arg2.G + Arg1.B*Arg2.B)
	// where each component has been scaled and offset to make it signed.
	// The result is replicated into all four (including alpha) channels.
	// This is a valid COLOROP only.
	ETOP_DOTPRODUCT3          = 24,

	// Triadic ops
	ETOP_MULTIPLYADD          = 25, // Arg0 + Arg1*Arg2
	ETOP_LERP                 = 26, // (Arg0)*Arg1 + (1-Arg0)*Arg2

	ETOP_FORCE_DWORD = 0x7fffffff,
};

enum ETEXTURESTAGESTATETYPE
{
	ETS_COLOROP        =  1, /* D3DTEXTUREOP - per-stage blending controls for color channels */
	ETS_COLORARG1      =  2, /* D3DTA_* (texture arg) */
	ETS_COLORARG2      =  3, /* D3DTA_* (texture arg) */
	ETS_ALPHAOP        =  4, /* D3DTEXTUREOP - per-stage blending controls for alpha channel */
	ETS_ALPHAARG1      =  5, /* D3DTA_* (texture arg) */
	ETS_ALPHAARG2      =  6, /* D3DTA_* (texture arg) */
	ETS_BUMPENVMAT00   =  7, /* float (bump mapping matrix) */
	ETS_BUMPENVMAT01   =  8, /* float (bump mapping matrix) */
	ETS_BUMPENVMAT10   =  9, /* float (bump mapping matrix) */
	ETS_BUMPENVMAT11   = 10, /* float (bump mapping matrix) */
	ETS_TEXCOORDINDEX  = 11, /* identifies which set of texture coordinates index this texture */
	ETS_BUMPENVLSCALE  = 22, /* float scale for bump map luminance */
	ETS_BUMPENVLOFFSET = 23, /* float offset for bump map luminance */
	ETS_TEXTURETRANSFORMFLAGS = 24, /* D3DTEXTURETRANSFORMFLAGS controls texture transform */
	ETS_COLORARG0      = 26, /* D3DTA_* third arg for triadic ops */
	ETS_ALPHAARG0      = 27, /* D3DTA_* third arg for triadic ops */
	ETS_RESULTARG      = 28, /* D3DTA_* arg for result (CURRENT or TEMP) */
	ETS_CONSTANT       = 32 /* Per-stage constant D3DTA_CONSTANT */
};

enum ETEXTURETRANSFORMFLAGS
{
	ETTFF_DISABLE         = 0,    // texture coordinates are passed directly
	ETTFF_COUNT1          = 1,    // rasterizer should expect 1-D texture coords
	ETTFF_COUNT2          = 2,    // rasterizer should expect 2-D texture coords
	ETTFF_COUNT3          = 3,    // rasterizer should expect 3-D texture coords
	ETTFF_COUNT4          = 4,    // rasterizer should expect 4-D texture coords
	ETTFF_PROJECTED       = 256,  // texcoords to be divided by COUNTth element
	ETTFF_FORCE_DWORD     = 0x7fffffff
};



enum EMULTISAMPLE_TYPE
{
	EMULTISAMPLE_NONE            =  0,
	EMULTISAMPLE_NONMASKABLE     =  1,
	EMULTISAMPLE_2_SAMPLES       =  2,
	EMULTISAMPLE_3_SAMPLES       =  3,
	EMULTISAMPLE_4_SAMPLES       =  4,
	EMULTISAMPLE_5_SAMPLES       =  5,
	EMULTISAMPLE_6_SAMPLES       =  6,
	EMULTISAMPLE_7_SAMPLES       =  7,
	EMULTISAMPLE_8_SAMPLES       =  8,
	EMULTISAMPLE_9_SAMPLES       =  9,
	EMULTISAMPLE_10_SAMPLES      = 10,
	EMULTISAMPLE_11_SAMPLES      = 11,
	EMULTISAMPLE_12_SAMPLES      = 12,
	EMULTISAMPLE_13_SAMPLES      = 13,
	EMULTISAMPLE_14_SAMPLES      = 14,
	EMULTISAMPLE_15_SAMPLES      = 15,
	EMULTISAMPLE_16_SAMPLES      = 16,
	EMAX_MULTISAMPLES		    = 17
};

enum SAMPLERSTATETYPE
{
    ESAMP_ADDRESSU       = 1,  /* D3DTEXTUREADDRESS for U coordinate */
    ESAMP_ADDRESSV       = 2,  /* D3DTEXTUREADDRESS for V coordinate */
    ESAMP_ADDRESSW       = 3,  /* D3DTEXTUREADDRESS for W coordinate */
    ESAMP_BORDERCOLOR    = 4,  /* D3DCOLOR */
    ESAMP_MAGFILTER      = 5,  /* D3DTEXTUREFILTER filter to use for magnification */
    ESAMP_MINFILTER      = 6,  /* D3DTEXTUREFILTER filter to use for minification */
    ESAMP_MIPFILTER      = 7,  /* D3DTEXTUREFILTER filter to use between mipmaps during minification */
    ESAMP_MIPMAPLODBIAS  = 8,  /* float Mipmap LOD bias */
    ESAMP_MAXMIPLEVEL    = 9,  /* DWORD 0..(n-1) LOD index of largest map to use (0 == largest) */
    ESAMP_MAXANISOTROPY  = 10, /* DWORD maximum anisotropy */
    ESAMP_SRGBTEXTURE    = 11, /* Default = 0 (which means Gamma 1.0,
                                   no correction required.) else correct for
                                   Gamma = 2.2 */
    ESAMP_ELEMENTINDEX   = 12, /* When multi-element texture is assigned to sampler, this
                                    indicates which element index to use.  Default = 0.  */
    ESAMP_DMAPOFFSET     = 13, /* Offset in vertices in the pre-sampled displacement map.
                                    Only valid for D3DDMAPSAMPLER sampler  */
    ESAMP_FORCE_DWORD   = 0x7fffffff, /* force 32-bit size enum */
};


//! Enumeration flags telling the video driver in which format textures
//! should be created.
enum eTEXTURE_CREATION_FLAG
{
	//! Forces the driver to create 16 bit textures always, independent of
	//! which format the file on disk has. When choosing this you may loose
	//! some color detail, but gain much speed and memory. 16 bit textures
	//! can be transferred twice as fast as 32 bit textures and only use
	//! half of the space in memory.
	//! When using this flag, it does not make sense to use the flags
	//! ETCF_ALWAYS_32_BIT, ETCF_OPTIMIZED_FOR_QUALITY,
	//! or ETCF_OPTIMIZED_FOR_SPEED at the same time.
	ETCF_ALWAYS_16_BIT = 0x00000001,

	//! Forces the driver to create 32 bit textures always, independent of
	//! which format the file on disk has. Please note that some drivers
	//! (like the software device) will ignore this, because they are only
	//! able to create and use 16 bit textures.
	//! When using this flag, it does not make sense to use the flags
	//! ETCF_ALWAYS_16_BIT, ETCF_OPTIMIZED_FOR_QUALITY,
	//! or ETCF_OPTIMIZED_FOR_SPEED at the same time.
	ETCF_ALWAYS_32_BIT = 0x00000002,

	//! Lets the driver decide in which format the textures are created and
	//! tries to make the textures look as good as possible.
	//! Usually it simply chooses the format in which the texture was stored on disk.
	//! When using this flag, it does not make sense to use the flags
	//! ETCF_ALWAYS_16_BIT, ETCF_ALWAYS_32_BIT,
	//! or ETCF_OPTIMIZED_FOR_SPEED at the same time.
	ETCF_OPTIMIZED_FOR_QUALITY = 0x00000004,

	//! Lets the driver decide in which format the textures are created and
	//! tries to create them maximizing render speed.
	//! When using this flag, it does not make sense to use the flags
	//! ETCF_ALWAYS_16_BIT, ETCF_ALWAYS_32_BIT, or ETCF_OPTIMIZED_FOR_QUALITY,
	//! at the same time.
	ETCF_OPTIMIZED_FOR_SPEED = 0x00000008,

	//! Automatically creates mip EzMap levels for the textures.
	ETCF_CREATE_MIP_MAPS = 0x00000010,

	//! Discard any alpha layer and use non-alpha color format.
	ETCF_NO_ALPHA_CHANNEL = 0x00000020,

	//! This flag is never used, it only forces the compiler to
	//! compile these enumeration values to 32 bit.
	ETCF_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum eAVOBJECT_RENDER_PASS
{
	//! In this pass, lights are transformed into camera space and added to the driver
	EAVO_LIGHT,

	//! This is used for sky boxes.
	EAVO_SKY_BOX,

	EAVO_AUTOMATIC,

	//! Solid scene nodes or special scene nodes without materials.
	EAVO_SOLID,

	//! Drawn after the transparent nodes, the time for drawing shadow volumes
	EAVO_SHADOW,

	//! Transparent scene nodes, drawn after shadow nodes. They are sorted from back to front and drawn in that order.
	EAVO_TRANSPARENT,

	//! Never used, value specifing how much parameters there are.
	EAVO_COUNT
};


enum ECOLOR_FORMAT
{
	//! 16 bit color format. There are 5 bits for every color component, and a 
	//! single bit is left for alpha information.
	ECF_A1R5G5B5 = 0,	

	//! Standard 16 bit color format.
	ECF_R5G6B5,

	//! 16 bit, 4 bits for each color,
	ECF_A4R4G4B4,

	//! 24 bit color, no alpha channel, but 8 bit for red, green and blue.
	ECF_R8G8B8,

	//! Default 32 bit color format. 8 bits are used for every component:
	//! red, green, blue and alpha.
	ECF_A8R8G8B8,

	//! 32 bit color, no alpha.  8 bits for red, green and blue components.
	ECF_X8R8G8B8,

	//! Compressed DXT1 color format.
	ECF_DXT1,

	//! Compressed DXT2 color format.
	ECF_DXT2,

	//! Compressed DXT3 color format.
	ECF_DXT3,

	//! Compressed DXT4 color format.
	ECF_DXT4,

	//! Compressed DXT5 color format.
	ECF_DXT5,

	//! 8 bit Luminance format.
	ECF_L8,

	//! 8 bit integer data
	ECF_I8,

	//! 16 bit integer data
	ECF_I16,

	//! 32 bit integer data
	ECF_I32,

	//! 32 bit floating point data
	ECF_F32,

	//! 64-bit integer format using 16 bits for each component.
	ECF_A16B16G16R16,

	//! 32-bit float format using 16 bits for the red channel and 16 bits for the green channel.
	ECF_G16R16F,

	//! 64-bit float format using 16 bits for the each channel (alpha, blue, green, red).
	ECF_A16B16G16R16F,

	//! 32-bit float format using 32 bits for the red channel.
	ECF_R32F,

	//! 64-bit float format using 32 bits for the red channel and 32 bits for the green channel.
	ECF_G32R32F,

	//! 128-bit float format using 32 bits for the each channel (alpha, blue, green, red).
	ECF_A32B32G32R32F,

	//! DX10 in it's infinite stupidity, seems to only support 32-bit color data for texture in 
	//! A8B8G8R8 format....go figure, the one format which was never used anywhere before.
	ECF_A8B8G8R8,

	//! Unknown Color Format
	ECF_UNKNOWN,
};



//! enumeration for geometry transformation states
enum ETRANSFORMATION_STATE
{
	//! View transformation
	ETS_VIEW = 0,
	//! World transformation
	ETS_WORLD,
	//! Projection transformation
	ETS_PROJECTION,
	//! Texture transformation
	ETS_TEXTURE_0,
	//! Texture transformation
	ETS_TEXTURE_1,
	//! Texture transformation
	ETS_TEXTURE_2,
	//! Texture transformation
	ETS_TEXTURE_3,
	//! Not used
	ETS_COUNT

};


//! Enumeration for all vertex types there are.
enum EVERTEX_TYPE
{
	//! Standard vertex type used by the Irrlicht engine, video::S3DVertex.
	EVT_VERTEX = 0,


	EVT_STANDARD,

	//! Vertex with two texture coordinates, video::S3DVertex2TCoords.
	/** Usually used for geometry with lightmaps or other special materials. */
	EVT_2TCOORDS,

	//! Vertex with a tangent and binormal vector, video::S3DVertexTangents.
	/** Usually used for tangent space normal mapping. */
	EVT_TANGENTS
};

//! Enumeration for all primitive types there are.
enum EPRIMITIVE_TYPE
{
	//! All vertices are non-connected points.
	EPT_POINTS=0,

	//! All vertices form a single connected line.
	EPT_LINE_STRIP,

	//! Every two vertices are connected creating n/2 lines.
	EPT_LINES,

	//! After the first two vertices each vertex defines a new EzTriangle.
	//! Always the two last and the new one form a new EzTriangle.
	EPT_TRIANGLE_STRIP,

	//! After the first two vertices each vertex defines a new EzTriangle.
	//! All around the common first vertex.
	EPT_TRIANGLE_FAN,

	//! Explicitly set all vertices for each EzTriangle.
	EPT_TRIANGLE_LIST,

};


enum EVIDEO_DRIVER_FEATURE
{
	//! Is driver able to render to a surface?
	EVDF_RENDER_TO_TARGET = 0,

	//! Is hardeware transform and lighting supported?
	EVDF_HARDWARE_TL,

	//! Are multiple textures per material possible?
	EVDF_MULTITEXTURE,

	//! Is driver able to render with a bilinear filter applied?
	EVDF_BILINEAR_FILTER,

	//! Can the driver handle mip maps?
	EVDF_MIP_MAP,

	//! Can the driver update mip maps automatically?
	EVDF_MIP_MAP_AUTO_UPDATE,

	//! Are stencilbuffers switched on and does the device support stencil buffers?
	EVDF_STENCIL_BUFFER,

	//! Is Vertex Shader 1.1 supported?
	EVDF_VERTEX_SHADER_1_1,

	//! Is Vertex Shader 2.0 supported?
	EVDF_VERTEX_SHADER_2_0,

	//! Is Vertex Shader 3.0 supported?
	EVDF_VERTEX_SHADER_3_0,

	//! Is Pixel Shader 1.1 supported?
	EVDF_PIXEL_SHADER_1_1,

	//! Is Pixel Shader 1.2 supported?
	EVDF_PIXEL_SHADER_1_2,

	//! Is Pixel Shader 1.3 supported?
	EVDF_PIXEL_SHADER_1_3,

	//! Is Pixel Shader 1.4 supported?
	EVDF_PIXEL_SHADER_1_4,

	//! Is Pixel Shader 2.0 supported?
	EVDF_PIXEL_SHADER_2_0,

	//! Is Pixel Shader 3.0 supported?
	EVDF_PIXEL_SHADER_3_0,

	//! Are ARB vertex programs v1.0 supported?
	EVDF_ARB_VERTEX_PROGRAM_1,

	//! Are ARB fragment programs v1.0 supported?
	EVDF_ARB_FRAGMENT_PROGRAM_1,

	//! Is GLSL supported?
	EVDF_ARB_GLSL,

	//! Is HLSL supported?
	EVDF_HLSL,

	//! Are non-square textures supported?
	EVDF_TEXTURE_NSQUARE,

	//! Are non-power-of-two textures supported?
	EVDF_TEXTURE_NPOT,

	//! Covers all DXT1 - DXT5 formats
	EVDF_COMPRESSED_TEXTURES,

	//! A1R5G5B5
	EVDF_A1R5G5G5_TEXTURES,

	//! R5G6B5
	EVDF_R5G6B5_TEXTURES,

	//! A4R4G4B4
	EVDF_A4R4G4B4_TEXTURES,

	//! X8R8G8B8
	EVDF_X8R8G8B8_TEXTURES,

	//! A8R8G8B8
	EVDF_A8R8G8B8_TEXTURES,

	//! L8
	EVDF_L8_TEXTURES,

	//! Are framebuffer objects supported?
	EVDF_FRAMEBUFFER_OBJECT,

	//! Are vertex buffer objects supported?
	EVDF_VERTEX_BUFFER_OBJECT,

	//! Only used for counting the elements of this enum
	EVDF_COUNT
};



enum EANI_KEYFREAMTYPE
{
	NOINTERP = 0,
	LINEAR_KEY = 1, /*!< Use linear interpolation. */
	QUADRATIC_KEY = 2, /*!< Use quadratic interpolation.  Forward and back tangents will be stored. */
	TBC_KEY = 3, /*!< Use Tension Bias Continuity interpolation.  Tension, bias, and continuity will be stored. */
	XYZ_ROTATION_KEY = 4, /*!< For use only with rotation data.  Separate X, Y, and Z keys will be stored instead of using quaternions. */
	SETP_KEY = 5, /*!< Unknown.  Step function? */
	MAX_ANI_KEYFREAMTYPE = 6,
};

enum EANI_KEYCONTENT
{
	FLOATKEY,
	POSKEY,
	ROTKEY,
	COLORKEY,
	TEXTKEY,
	BOOLKEY,
	NUMKEYCONTENTS
};





#endif