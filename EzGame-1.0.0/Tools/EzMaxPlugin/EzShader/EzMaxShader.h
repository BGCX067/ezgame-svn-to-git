#pragma once

#include "../EzMax.h"
#include "../Resource.h"

#define EZ_MAX_SHADER_VERSION 6


static Class_ID EzShaderClassID( 0x6c8667a2, 0x4cd6128f);




#define EZ_DEFAULT_SHADER_NAME "Default Shader"
#define EZ_DEFAULT_SHADER_TECHNIQUE_NAME "Best Available for Hardware"
#define EZ_DEFAULT_SHADER_TECHNIQUE_INDEX 0
#define EZ_SHADER_CUST_ATTRIB_NAME "NiShaderCustAttribs"


// paramblock2 block and parameter IDs.
enum { shader_params, };

// Paramblock2 parameter list
enum 
{ 
	sk_amb, sk_diff, sk_spec, sk_emit, sk_shininess, sk_alpha, sk_test_ref,
	sk_srcBlend, sk_destBlend, sk_testmode, sk_alphatest_enable, 
	sk_applyMode, sk_srcVertexMode, sk_lightingMode,
	sk_vertexColors_enable, sk_specular_enable, sk_no_sorter, sk_dither,
	// removing ps2 params would cause loading to fail for legacy files
	sk_l_base,   sk_l_dark,   sk_l_detail,   sk_l_decal1,   sk_l_decal2,   
	sk_l_glow, 
	// allowing sk_k to be 24 causes the .MAX file to be unloadable
	sk_k_base=100, sk_k_dark,   sk_k_detail,   sk_k_decal1,   sk_k_decal2,
	sk_k_glow, 
	sk_lon_base, sk_lon_dark, sk_lon_detail, sk_lon_decal1, sk_lon_decal2, 
	sk_lon_glow, 
	sk_kon_base, sk_kon_dark, sk_kon_detail, sk_kon_decal1, sk_kon_decal2,
	sk_kon_glow, 
	sk_alphaMode, sk_ps2_dark_rgb, sk_ps2_detail_rgb, sk_bump_magnitude, 
	sk_luma_offset, sk_luma_scale,
	sk_base_export, sk_dark_export, sk_detail_export, sk_decal1_export, 
	sk_decal2_export, sk_gloss_export, sk_glow_export, 
	sk_shader_name, sk_shader_viewer_technique, sk_shader_export_technique,
	sk_use_normal_maps, sk_normal_map_technique, sk_l_gloss, sk_lon_gloss,
	sk_k_gloss, sk_kon_gloss, sk_parallax_offset,
	sk_num_params
};



// channel names
static int texNameIDS[STD2_NMAX_TEXMAPS] = {
	IDS_CHAN_BASE,	IDS_CHAN_DARK, IDS_CHAN_DETAIL, IDS_CHAN_DECAL1, 
	IDS_CHAN_EMPTY, IDS_CHAN_EMPTY, IDS_CHAN_EMPTY, IDS_CHAN_EMPTY,
	IDS_CHAN_EMPTY, IDS_CHAN_EMPTY, IDS_CHAN_EMPTY, IDS_CHAN_EMPTY,
	IDS_CHAN_EMPTY, IDS_CHAN_EMPTY, IDS_CHAN_EMPTY, IDS_CHAN_EMPTY,
	IDS_CHAN_EMPTY, IDS_CHAN_EMPTY, IDS_CHAN_EMPTY, IDS_CHAN_EMPTY,
	IDS_CHAN_EMPTY, IDS_CHAN_EMPTY, IDS_CHAN_EMPTY, IDS_CHAN_EMPTY,
};	

// internal non-local parsable channel map names
static TCHAR* texInternalNames[STD2_NMAX_TEXMAPS] = {
	_T("base"),_T("dark"), _T("detailMap"), _T("decalMap1"), 	
	_T("filterMap"), _T(""), _T(""), _T(""), 
	_T(""), _T(""), _T(""), _T(""), 
	_T(""), _T(""), _T(""), _T(""),
	_T(""), _T(""), _T(""), _T(""), 
	_T(""), _T(""), _T(""), _T(""),
};

static int chanType[STD2_NMAX_TEXMAPS] = {
	CLR_CHANNEL, CLR_CHANNEL, CLR_CHANNEL, CLR_CHANNEL, 
	UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, 
	UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, 
	UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, 
	UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, 
	UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, 
};	


//---------------------------------------------------------------------------
// Definition of our UI and map parameters for the shader
//---------------------------------------------------------------------------

// Number of Map Buttons on our UI and number of texmaps
#define NMBUTS 1
#define SHADER_NTEXMAPS 10
// Channels used by this shader
#define S_BASE      0
#define S_DARK      1
#define S_DETAIL    2
#define S_DECAL     3
#define S_NORMALMAP 4
#define S_PARALLAX  5
#define S_BUMP      6
#define S_GLOSS     7
#define S_GLOW      8
#define S_PARTICLE_COLOR 9
#define S_PARTICLE_OPACITY 10
#define S_REFLECTION 11
#define S_DECAL2    12


#define SHADER_PARAMS (STD_PARAM_SELFILLUM | STD_PARAM_SELFILLUM_CLR | \
	STD_PARAM_DIFFUSE_CLR | STD_PARAM_SPECULAR_CLR | \
	STD_PARAM_SELFILLUM_CLR_ON)

static int stdIDToChannel[N_ID_CHANNELS] = 
{
	-1,     // 1, // base
	0,      // 5, // dark
	-1,     //-1, // detail
	-1,     //-1, // decals
	4,      // 8, // bump
	1,      // 4, // gloss
	-1,     //-1, // glow
	-1,     //-1, // particle color
	-1,     //-1, // particle opacity
	-1,     // 9, // reflection
	-1,     //-1,
	-1      //-1
};

class EzMaxShaderDialog;




class EzMaxShader : public Shader 
{
	friend class StraussShaderCB;
	friend class EzMaxShaderDialog;


protected:

	// The parameter block
	IParamBlock2    *m_pblock; 

	Interval         m_validInterval;
	TimeValue        m_curTime;


	// Pointer to the dialog handler
	EzMaxShaderDialog *m_pDlg;


	// Storage for our parameters
	Color           m_ambient;
	Color           m_diffuse;
	Color           m_specular;
	Color           m_emittance;
	Color           m_reflection;

	float           m_fShininess;
	float           m_fBumpMagnitude;
	float           m_fLumaOffset;
	float           m_fLumaScale;
	float           m_fParallaxOffset;

	float           m_fAlpha;
	unsigned char   m_ucTestRef;
	int             m_iAlphaMode;
	int             m_iSrcBlend;
	int             m_iDestBlend;
	int             m_iAlphaTestMode;
	bool            m_bAlphaTestOn;

	int             m_iApplyMode;
	int             m_iSrcVertexMode;
	int             m_iLightingMode;

	bool            m_bVertexColorsOn;
	bool            m_bSpecularOn;
	bool            m_bNoSorter;
	bool            m_bDither;

	Color           m_dark;
	Color           m_detail;
	Color           m_decal;
	Color           m_bump;
	Color           m_gloss;
	Color           m_glow;
	Color           m_particle_color;
	Color           m_particle_opacity;


public:
	EzMaxShader();
	~EzMaxShader(void);
	void DeleteThis(){ delete this; }		
	ULONG SupportStdParams(){ return SHADER_PARAMS; }

	// copy std params, for switching shaders
	void CopyStdParams( Shader* pFrom );

	// texture maps
	long nTexChannelsSupported(){ return SHADER_NTEXMAPS; }
	TSTR GetTexChannelName( long nChan ){ return GetString( texNameIDS[ nChan ] ); }
	TSTR GetTexChannelInternalName( long nChan ) { return texInternalNames[ nChan ]; }
	long ChannelType( long nChan ) { return chanType[nChan]; }
	long StdIDToChannel( long stdID ){ return stdIDToChannel[stdID]; }

	BOOL KeyAtTime(int id,TimeValue t) { return m_pblock->KeyFrameAtTime(id,t); }
	ULONG GetRequirements( int subMtlNum ){ return MTLREQ_PHONG; }

	ShaderParamDlg* CreateParamDialog(HWND hOldRollup, HWND hwMtlEdit, IMtlParams *imp, StdMat2* theMtl, int rollupOpen, int );
	ShaderParamDlg* GetParamDlg(int){ return (ShaderParamDlg*) m_pDlg; }
	void SetParamDlg( ShaderParamDlg* newDlg, int n=0 )
	{ 
		m_pDlg = (EzMaxShaderDialog*)newDlg; 
	}

	Class_ID ClassID() { return EzShaderClassID; }
	SClass_ID SuperClassID() { return SHADER_CLASS_ID; }
	TSTR GetName() { return GetString( IDS_SHADER_NAME ); }
	void GetClassName(TSTR& s) { s = GetName(); }  

	int NumSubs() { return 1; }  
	Animatable* SubAnim(int i){ return (i==0)? m_pblock : NULL; }
	TSTR SubAnimName(int i){ return TSTR(GetString( IDS_SHADER_PARAMETERS_NAME )); };
	int SubNumToRefNum(int subNum) { return subNum;	}

	// add direct ParamBlock2 access
	int	NumParamBlocks() { return 1; }
	IParamBlock2* GetParamBlock(int i) { return m_pblock; }
	IParamBlock2* GetParamBlockByID(BlockID id) { return (m_pblock->ID() == id) ? m_pblock : NULL; } 

	int NumRefs() { return 1; }
	RefTargetHandle GetReference(int i){ return (i==0)? m_pblock : NULL; }
	void SetReference(int i, RefTargetHandle rtarg) 
	{ if (i==0) m_pblock = (IParamBlock2*)rtarg; else assert(0); }
	void NotifyChanged(){ NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE); }

	void Update(TimeValue t, Interval& valid);
	void Reset();

	float Bound( float fX, float fMin = 0.0f, float fMax = 1.0f );
	Color Bound( Color& c );

	StdMat2 * GetMtl();

#if MAX_RELEASE <= 8000
	RefTargetHandle Clone(RemapDir &remap = NoRemap());
#else
	RefTargetHandle Clone(RemapDir &remap = DefaultRemapDir());
#endif

	RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
		PartID& partID, RefMessage message );
	// IO
	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);

	void GetIllumParams( ShadeContext &sc, IllumParams &ip );

	// Shader specific implimentations
	void Illum(ShadeContext &sc, IllumParams &ip);
	void AffectReflection(ShadeContext &sc, IllumParams &ip, Color &rcol);

	void CombineComponents( ShadeContext &sc, IllumParams& ip );

	// Standard locks not supported (Diffuse+Specular)/(Ambient+Diffuse)
	void SetLockDS(BOOL lock);
	BOOL GetLockDS();
	void SetLockAD(BOOL lock);
	BOOL GetLockAD();
	void SetLockADTex(BOOL lock);
	BOOL GetLockADTex();

	// Metal support
	BOOL IsMetal();


	void SetSpecularLevel(float v, TimeValue t);        
	float GetSpecularLevel(int mtlNum, BOOL backFace);
	float GetSpecularLevel(TimeValue t);


	inline void SetGlossiness(float v, TimeValue t);
	inline float GetGlossiness(int mtlNum, BOOL backFace);
	inline float GetGlossiness( TimeValue t);

	// ----------------------------------------------------------
	void SetAmbientClr(Color c, TimeValue t);
	Color GetAmbientClr(int mtlNum, BOOL backFace);     
	Color GetAmbientClr(TimeValue t);       
	
	// ----------------------------------------------------------
	void SetDiffuseClr(Color c, TimeValue t);
	Color GetDiffuseClr(int mtlNum, BOOL backFace);     
	Color GetDiffuseClr(TimeValue t);       

	// ----------------------------------------------------------
	void SetSpecularClr(Color c, TimeValue t);
	Color GetSpecularClr(int mtlNum, BOOL backFace);
	Color GetSpecularClr(TimeValue t);

	// ----------------------------------------------------------
	void SetEmittance(Color c, TimeValue t);
	Color GetEmittance(int mtlNum, BOOL backFace);      
	Color GetEmittance(TimeValue t);        

	// ----------------------------------------------------------
	void SetShininess(float fShininess, TimeValue t);
	float GetShininess(int mtlNum, BOOL backFace);      
	float GetShininess(TimeValue t);      


	// ALPHA
	void SetAlpha(float fAlpha, TimeValue t);
	float GetAlpha(int mtlNum, BOOL backFace);      
	float GetAlpha(TimeValue t);     


	// SELFILLUM
	void SetSelfIllum(float v, TimeValue t);
	float GetSelfIllum(int mtlNum=0, BOOL backFace=FALSE);
	float GetSelfIllum(TimeValue t);    

	void SetSelfIllumClrOn( BOOL on );
	BOOL IsSelfIllumClrOn(int mtlNum, BOOL backFace);
	BOOL IsSelfIllumClrOn();

	void SetSelfIllumClr(Color c, TimeValue t);
	Color GetSelfIllumClr(int mtlNum=0, BOOL backFace=FALSE);
	Color GetSelfIllumClr(TimeValue t);     

	// SOFTEN
	void SetSoftenLevel(float v, TimeValue t);
	float GetSoftenLevel(int mtlNum=0, BOOL backFace=FALSE);
	float GetSoftenLevel(TimeValue t);

	// Vertex Colors Enable
	void SetVertexColorsOn(int i, TimeValue t);
	int GetVertexColorsOn(int mtlNum, BOOL backFace);
	int GetVertexColorsOn(TimeValue t);

	// Specular Enable
	void SetSpecularOn(int i, TimeValue t);
	int GetSpecularOn(int mtlNum, BOOL backFace);
	int GetSpecularOn(TimeValue t);


	// APPLY_MODE
	void SetTextureApplyMode(int iVal, TimeValue t);
	int GetTextureApplyMode(int mtlNum, BOOL backFace);
	int GetTextureApplyMode(TimeValue t);



private:
	StdMat2 * GetMtlContainingThisShader(Mtl* pkMtl);



};


///////////// Class Descriptor ////////////////////////
class EzMaxShaderClassDesc:public ClassDesc2 
{
public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { 	return new EzMaxShader(); }
	const TCHAR *	ClassName() { return GetString(IDS_SHADER_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SHADER_CLASS_ID; }
	Class_ID 		ClassID() { return EzShaderClassID; }
	const TCHAR* 	Category() { return _T("");  }
	const TCHAR*	InternalName() { return _T("2232Strauss"); } // returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			 // returns owning module handle
};

inline void EzMaxShader::SetGlossiness(float v, TimeValue t)
{ 
}

inline float EzMaxShader::GetGlossiness(int mtlNum, BOOL backFace)
{ 
	return 40.0f; 
}   

inline float EzMaxShader::GetGlossiness( TimeValue t)
{
	return 40.0f;  
}       

inline void EzMaxShader::SetDiffuseClr(Color c, TimeValue t)        
{ 
	m_diffuse = c; 
	m_pblock->SetValue( sk_diff, t, c); 
}

inline Color EzMaxShader::GetDiffuseClr(int mtlNum, BOOL backFace)
{ 
	return m_diffuse;
}

inline Color EzMaxShader::GetDiffuseClr(TimeValue t)
{ 
	return m_pblock->GetColor(sk_diff, t); 
}

inline void EzMaxShader::SetAmbientClr(Color c, TimeValue t)
{
	m_ambient = c; 
	m_pblock->SetValue( sk_amb, t, c); 
}

inline Color EzMaxShader::GetAmbientClr(int mtlNum, BOOL backFace)
{ 
	return m_ambient;
}

inline Color EzMaxShader::GetAmbientClr(TimeValue t)
{ 
	return m_pblock->GetColor(sk_amb, t);
}

inline void EzMaxShader::SetSpecularClr(Color c, TimeValue t)
{
	m_specular = c; 
	m_pblock->SetValue( sk_spec, t, c); 
}

inline Color EzMaxShader::GetSpecularClr(int mtlNum, BOOL backFace)
{ 
	return m_specular;
}

inline Color EzMaxShader::GetSpecularClr(TimeValue t)
{ 
	return m_pblock->GetColor(sk_spec, t);
}

inline void EzMaxShader::SetEmittance(Color c, TimeValue t)     
{ 
	m_emittance = c; 
	m_pblock->SetValue( sk_emit, t, c); 
}

inline Color EzMaxShader::GetEmittance(int mtlNum, BOOL backFace)
{ 
	return m_emittance;
}

inline Color EzMaxShader::GetEmittance(TimeValue t)
{ 
	return m_pblock->GetColor(sk_emit, t); 
}

inline void EzMaxShader::SetShininess(float fVal, TimeValue t)
{ 
	m_fShininess = fVal;
	m_pblock->SetValue( sk_shininess, t, fVal);
}

inline float EzMaxShader::GetShininess(int mtlNum, BOOL backFace)
{ 
	return m_fShininess;
}   

inline float EzMaxShader::GetShininess( TimeValue t)
{
	return m_pblock->GetFloat(sk_shininess, t);
}       


inline void EzMaxShader::SetSelfIllum(float v, TimeValue t) 
{
}


inline float EzMaxShader::GetSelfIllum(int mtlNum, BOOL backFace)
{ 
	return 0.0f; 
}


inline void EzMaxShader::SetSelfIllumClrOn( BOOL on )
{
}

inline BOOL EzMaxShader::IsSelfIllumClrOn()
{ 
	return FALSE; 
}

inline BOOL EzMaxShader::IsSelfIllumClrOn(int mtlNum, BOOL backFace)
{ 
	return FALSE; 
}

inline void EzMaxShader::SetLockDS(BOOL lock)
{ 
}

inline BOOL EzMaxShader::GetLockDS()
{ 
	return FALSE; 
}

inline void EzMaxShader::SetLockAD(BOOL lock)
{
}

inline BOOL EzMaxShader::GetLockAD()
{ 
	return FALSE; 
}

inline void EzMaxShader::SetLockADTex(BOOL lock)
{ 
}

inline BOOL EzMaxShader::GetLockADTex()
{ 
	return FALSE; 
}

inline void EzMaxShader::SetAlpha(float fVal, TimeValue t)
{ 
	m_fAlpha = fVal;
	m_pblock->SetValue( sk_alpha, t, fVal);
}

//---------------------------------------------------------------------------
inline float EzMaxShader::GetAlpha(int mtlNum, BOOL backFace)
{ 
	return m_fAlpha;
}   

//---------------------------------------------------------------------------
inline float EzMaxShader::GetAlpha( TimeValue t)
{
	return m_pblock->GetFloat(sk_alpha, t);
}  

inline void EzMaxShader::SetSelfIllumClr(Color c, TimeValue t)
{
}

inline Color EzMaxShader::GetSelfIllumClr(int mtlNum, BOOL backFace)
{ 
	return Color(0,0,0); 
}

inline float EzMaxShader::GetSelfIllum(TimeValue t)
{ 
	return 0.0f;
}       

inline Color EzMaxShader::GetSelfIllumClr(TimeValue t)
{ 
	return Color(0,0,0);
}       

inline void EzMaxShader::SetSoftenLevel(float v, TimeValue t)
{
}

inline float EzMaxShader::GetSoftenLevel(int mtlNum, BOOL backFace)
{ 
	return 0.0f; 
}

inline float EzMaxShader::GetSoftenLevel(TimeValue t)
{ 
	return  0.0f; 
}

inline void EzMaxShader::SetSpecularLevel(float v, TimeValue t)
{
}


inline float EzMaxShader::GetSpecularLevel(int mtlNum, BOOL backFace)
{ 
	return 1.0f; 
}

inline float EzMaxShader::GetSpecularLevel(TimeValue t)
{ 
	return 1.0f; 
}


inline void EzMaxShader::SetSpecularOn(int iVal, TimeValue t)
{
	m_bSpecularOn = iVal ? true : false;
	m_pblock->SetValue( sk_specular_enable, t, iVal);
}

inline int EzMaxShader::GetSpecularOn(int mtlNum, BOOL backFace)
{ 
	return m_bSpecularOn;
}   
//---------------------------------------------------------------------------
inline int EzMaxShader::GetSpecularOn(TimeValue t)
{
	return m_pblock->GetInt(sk_specular_enable, t);
}   

inline float EzMaxShader::Bound(float fX, float fMin, float fMax)
{ 
	return fX < fMin? fMin:( fX > fMax? fMax : fX); 
}

inline Color EzMaxShader::Bound( Color& c )
{ 
	return Color( Bound(c.r), Bound(c.g), Bound(c.b) ); 
}


//---------------------------------------------------------------------------
inline void EzMaxShader::SetVertexColorsOn(int iVal, TimeValue t)
{
	m_bVertexColorsOn = iVal ? true : false;
	m_pblock->SetValue( sk_vertexColors_enable, t, iVal);
}
//---------------------------------------------------------------------------
inline int EzMaxShader::GetVertexColorsOn(int mtlNum, BOOL backFace)
{ 
	return m_bVertexColorsOn;
}   
//---------------------------------------------------------------------------
inline int EzMaxShader::GetVertexColorsOn(TimeValue t)
{
	return m_pblock->GetInt(sk_vertexColors_enable, t);
} 

inline void EzMaxShader::SetTextureApplyMode(int iVal, TimeValue t)
{
	m_iApplyMode = iVal;
	m_pblock->SetValue( sk_applyMode, t, iVal);
}

inline int EzMaxShader::GetTextureApplyMode(int mtlNum, BOOL backFace)
{ 
	return m_iApplyMode;
}   


inline int EzMaxShader::GetTextureApplyMode(TimeValue t)
{
	return m_pblock->GetInt(sk_applyMode, t);
}