#include "../EzMaxPluginPch.h"
#include "EzMaxShader.h"
#include "EzMaxShaderDialog.h"


#include "IMtlEdit.h"


#define SHADER_VERS_CHUNK 0x6500

EzMaxShaderClassDesc EzShaderCD;
ClassDesc2 * GetEzShaderDesc(){ return &EzShaderCD; }

static ParamBlockDesc2 shader_param_blk 
( 
	shader_params, _T("shaderParameters"),  0, &EzShaderCD, P_AUTO_CONSTRUCT, 0, 

	// params
		sk_amb, _T("ambient"), TYPE_RGBA, P_ANIMATABLE, IDS_MAT_AMBIENT, 
			p_default, Color(0.0f, 0.0f, 0.0f), 
		end,
		sk_diff, _T("diffuse"), TYPE_RGBA, P_ANIMATABLE, IDS_MAT_DIFFUSE, 
			p_default, Color(0.0f, 1.0f, 0.0f), 
		end,
		sk_spec, _T("specular"), TYPE_RGBA, P_ANIMATABLE, IDS_MAT_SPECULAR, 
			p_default, Color(0.0f, 0.0f, 1.0f), 
		end,
		sk_emit, _T("emittance"), TYPE_RGBA, P_ANIMATABLE, IDS_MAT_EMITTANCE, 
			p_default, Color(0.0f, 0.0f, 0.0f), 
		end,
		sk_shininess, _T("shininess"), TYPE_FLOAT, P_ANIMATABLE, IDS_MAT_SHININESS, 
			p_default, 10.0f,
			p_range,    0.0f, 2500.0f,
			p_ui,       TYPE_SPINNER, EDITTYPE_FLOAT, 
			IDC_EDT_SHININESS, IDC_SPN_SHININESS, 1.0f,
		end,
		sk_alpha, _T("alpha"), TYPE_PCNT_FRAC, P_ANIMATABLE, IDS_MAT_ALPHA, 
		p_default, 1.0f,
		p_range,    0.0f, 1.0f,
		end,

		sk_specular_enable, _T("SpecularEnable"), TYPE_BOOL, P_ANIMATABLE, IDS_MAT_SPECULARENABLE,  
		p_default,      FALSE, 
		p_ui,           TYPE_SINGLECHEKBOX, IDC_CHK_SPECENABLE, 
		end,

		sk_applyMode, _T("ApplyMode"), TYPE_INT, 0, IDS_APPLY_MODE,  
		p_default,      2, 
		p_ui,           TYPE_INTLISTBOX, IDC_CBO_APPLY_MODE, 0, 
		end, 

		sk_shader_name, _T("CustomShader"), TYPE_STRING, 0, 
        IDS_SHADER_NAME,
        p_default,      EZ_DEFAULT_SHADER_NAME,
        end,
	end
 );



EzMaxShader::EzMaxShader(void)
{
	m_pblock = NULL; 
	m_pDlg = NULL; 
	m_curTime = 0;
	m_validInterval.SetEmpty(); 
	m_diffuse = m_decal = m_bump = m_gloss = m_glow = Color(0.0f, 0.0f, 0.0f);
	m_reflection = Color(1.0f, 1.0f, 1.0f);
	m_dark = m_detail = Color(1.0f, 1.0f, 1.0f);
}

EzMaxShader::~EzMaxShader(void)
{
}

//---------------------------------------------------------------------------
// Create the dialog panels themselves
#if MAX_RELEASE < 4000
ShaderParamDlg* EzMaxShader::CreateParamDialog(HWND hOldRollup, 
											   HWND hwMtlEdit, IMtlParams *imp, StdMat2* theMtl, int rollupOpen ) 
#else
ShaderParamDlg* EzMaxShader::CreateParamDialog(HWND hOldRollup, 
											   HWND hwMtlEdit, IMtlParams *imp, StdMat2* theMtl, int rollupOpen, int t) 
#endif
{
	Interval v;
	Update(imp->GetTime(),v);

	EzMaxShaderDialog *pDlg = new EzMaxShaderDialog(hwMtlEdit, imp);

	pDlg->SetThings( theMtl, this  );

	TCHAR* dlgTemplate;

	dlgTemplate = MAKEINTRESOURCE(IDD_EZSHADER);

	// If it already exists, replace it
	if ( hOldRollup ) 
	{
		pDlg->m_hRollup = imp->ReplaceRollupPage( 
			hOldRollup,
			hInstance,
			dlgTemplate,
			EzShaderDlgProc, 
			GetString(IDS_SHADER_CLASS_NAME),
			(LPARAM)pDlg , 
			rollupOpen);
	} 
	else
	{
		// otherwise, add one
		pDlg->m_hRollup = imp->AddRollupPage( 
			hInstance,
			dlgTemplate,
			EzShaderDlgProc, 
			GetString(IDS_SHADER_CLASS_NAME), 
			(LPARAM)pDlg , 
			rollupOpen);
	}

	return (ShaderParamDlg*) pDlg;   
}


StdMat2* EzMaxShader::GetMtlContainingThisShader(Mtl* pkMaterial)
{
	
	if (pkMaterial == NULL)
		return NULL;

	if (pkMaterial->ClassID() == Class_ID(CMTL_CLASS_ID, 0) || 
		pkMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
	{
		return (StdMat2 *)pkMaterial;
	}

	if (pkMaterial->IsMultiMtl())
	{
		Mtl *pSub;
		for (int i = 0; i < pkMaterial->NumSubMtls(); i++)
		{
			pSub = pkMaterial->GetSubMtl(i);
			StdMat2* pkMtl = GetMtlContainingThisShader(pSub);
			if (pkMtl != NULL)
				return pkMtl;
		}
	}

	return NULL;
}

//NiString EzMaxShader::GetMtlName()
//{
//	IMtlEditInterface* pkInterface = GetMtlEditInterface();
//	if (pkInterface)
//	{
//		MtlBase * pkMtl = pkInterface->GetCurMtl();
//		if (pkMtl && pkMtl->SuperClassID() == MATERIAL_CLASS_ID)
//		{
//			return GetMtlNameContainingThisShader((Mtl*)pkMtl);
//		}
//		else
//		{
//			return NULL;
//		}
//	}
//	else
//	{
//		return NULL;
//	}
//}

StdMat2 * EzMaxShader::GetMtl()
{
	IMtlEditInterface* pkInterface = GetMtlEditInterface();
	if (pkInterface)
	{
		MtlBase * pkMtl = pkInterface->GetCurMtl();
		if (pkMtl && pkMtl->SuperClassID() == MATERIAL_CLASS_ID)
		{
			return GetMtlContainingThisShader((Mtl*)pkMtl);
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

void EzMaxShader::Update(TimeValue t, Interval &valid) 
{
	float fTemp;
	Point3 p;
	int iTemp;

	if (!m_validInterval.InInterval(t)) 
	{
		m_validInterval.SetInfinite();

		m_pblock->GetValue( sk_amb, t, p, m_validInterval );
		Color kCol = Color(p.x,p.y,p.z);
		m_ambient = Bound(kCol);

		m_pblock->GetValue( sk_diff, t, p, m_validInterval );
		kCol = Color(p.x,p.y,p.z);
		m_diffuse = Bound(kCol);

		m_pblock->GetValue( sk_spec, t, p, m_validInterval );
		kCol = Color(p.x,p.y,p.z);
		m_specular = Bound(kCol);

		m_pblock->GetValue( sk_emit, t, p, m_validInterval );
		kCol = Color(p.x,p.y,p.z);
		m_emittance = Bound(kCol);

		m_pblock->GetValue( sk_shininess, t, fTemp, m_validInterval );
		m_fShininess = Bound(fTemp, 0.0f, 2500.0f);

		m_pblock->GetValue( sk_alpha, t, fTemp, m_validInterval );
		m_fAlpha = Bound(fTemp);

		//m_pblock->GetValue( sk_bump_magnitude, t, fTemp, m_validInterval );
		//m_fBumpMagnitude = Bound(fTemp, -2.0f, 2.0f);
		//m_pblock->GetValue( sk_luma_offset, t, fTemp, m_validInterval) ;
		//m_fLumaOffset = Bound(fTemp, 0.0f, 1.0f);
		//m_pblock->GetValue( sk_luma_scale, t, fTemp, m_validInterval );
		//m_fLumaScale = Bound(fTemp, 0.0f, 1.0f);
		//m_pblock->GetValue( sk_parallax_offset, t, fTemp, m_validInterval) ;
		//m_fParallaxOffset = Bound(fTemp, 0.0f, 10.0f);

		//m_pblock->GetValue( sk_test_ref, t, iTemp, m_validInterval);
		//m_ucTestRef = iTemp;

		//m_iSrcBlend = m_pblock->GetInt(sk_srcBlend);

		//m_iDestBlend = m_pblock->GetInt(sk_destBlend);

		//m_iAlphaMode = m_pblock->GetInt(sk_alphaMode);

		//m_pblock->GetValue( sk_testmode, t, iTemp, m_validInterval);
		//m_iAlphaTestMode = iTemp;

		//m_bAlphaTestOn = m_pblock->GetInt(sk_alphatest_enable) ? 
		//	true : false;

		//m_pblock->GetValue( sk_applyMode, t, iTemp, m_validInterval);
		//m_iApplyMode = iTemp;

		//m_pblock->GetValue( sk_srcVertexMode, t, iTemp, m_validInterval);
		//m_iSrcVertexMode = iTemp;

		//m_pblock->GetValue( sk_lightingMode, t, iTemp, m_validInterval);
		//m_iLightingMode = iTemp;

		//m_bVertexColorsOn = 
		//	m_pblock->GetInt(sk_vertexColors_enable) ? true : false;
		m_bSpecularOn = m_pblock->GetInt(sk_specular_enable) ? true : false;
		//m_bNoSorter = m_pblock->GetInt(sk_no_sorter) ? true : false;
		//m_bDither = m_pblock->GetInt(sk_dither) ? true : false;

		m_curTime = t;
	}

	valid &= m_validInterval;
}

void EzMaxShader::CopyStdParams( Shader* pFrom )
{
	// We don't want to see this parameter copying in macrorecorder
	macroRecorder->Disable(); 

	SetAmbientClr( pFrom->GetAmbientClr(0,0), m_curTime );
	SetDiffuseClr( pFrom->GetDiffuseClr(0,0), m_curTime );

	macroRecorder->Enable();
	m_validInterval.SetEmpty();  
}

void EzMaxShader::Reset()
{
	// Create and intialize paramblock2
	EzShaderCD.MakeAutoParamBlocks(this);   

	Color defaultColor = Color(0.588235f, 0.588235f, 0.588235f);

	m_validInterval.SetEmpty();
	SetAmbientClr( defaultColor, 0 );
	SetDiffuseClr( Color(1, 1, 1), 0 );
	StdMat * pkMat = GetMtl();
	if (pkMat)
	{
		pkMat->SetDiffuse(defaultColor, 0);
		pkMat->SetAmbient(Color(1, 1, 1), 0);
	}

	SetSpecularClr( Color(0.0f,0.0f,1.0f), 0 );
	SetEmittance( Color(0.0f,0.0f,0.0f), 0 );
	SetShininess( 10.0f,0);   
	SetAlpha( 1.0f,0);   
	//SetTestRef( 0, 0);
	//SetSrcBlend( NiAlphaProperty::ALPHA_SRCALPHA, 0);
	//SetDestBlend( NiAlphaProperty::ALPHA_INVSRCALPHA, 0); 
	//SetAlphaMode(0, 0);         // automatic
	//SetAlphaTestMode( 4, 0);    // greater
	//SetAlphaTestOn(false, 0);
	SetTextureApplyMode(2, 0);  // modulate
	//SetSourceVertexMode(2, 0);  // amb_diff
	//SetLightingMode(1, 0);      // e_a_d
	//SetVertexColorsOn(true, 0);
	SetSpecularOn(false, 0);
	//SetNoSorter(false, 0);
	//SetDither(false, 0);

}



RefTargetHandle EzMaxShader::Clone( RemapDir &remap )
{
	EzMaxShader* pNew = new EzMaxShader();

	pNew->ReplaceReference(0, remap.CloneRef(m_pblock));
	pNew->m_validInterval.SetEmpty();    
	pNew->m_ambient = m_ambient;
	pNew->m_diffuse = m_diffuse;
	pNew->m_specular = m_specular;
	pNew->m_emittance = m_emittance;
	pNew->m_fShininess = m_fShininess;
	pNew->m_fBumpMagnitude = m_fBumpMagnitude;
	pNew->m_fLumaOffset = m_fLumaOffset;
	pNew->m_fLumaScale = m_fLumaScale;
	pNew->m_fParallaxOffset = m_fParallaxOffset;
	pNew->m_fAlpha = m_fAlpha;
	pNew->m_ucTestRef = m_ucTestRef;
	pNew->m_iSrcBlend = m_iSrcBlend;
	pNew->m_iDestBlend = m_iDestBlend;
	pNew->m_iAlphaTestMode = m_iAlphaTestMode;
	pNew->m_bAlphaTestOn = m_bAlphaTestOn;
	pNew->m_iApplyMode = m_iApplyMode;
	pNew->m_iSrcVertexMode = m_iSrcVertexMode;
	pNew->m_iLightingMode = m_iLightingMode;
	pNew->m_bVertexColorsOn = m_bVertexColorsOn;
	pNew->m_bSpecularOn = m_bSpecularOn;
	pNew->m_bNoSorter = m_bNoSorter;
	pNew->m_bDither = m_bDither;
	pNew->m_iAlphaMode = m_iAlphaMode;

	return (RefTargetHandle) pNew;
}

void EzMaxShader::GetIllumParams( ShadeContext &sc, IllumParams &ip )
{
	ip.stdParams = SupportStdParams();
	ip.channels[S_BASE] = m_diffuse;
	ip.channels[S_DARK] = m_dark;
	ip.channels[S_DETAIL] = m_detail;
	ip.channels[S_DECAL] = m_decal;
	ip.channels[S_BUMP] = m_bump;
	ip.channels[S_GLOSS] = m_gloss;
	ip.channels[S_GLOW] = m_glow;
	ip.channels[S_REFLECTION] = m_reflection;
}

void EzMaxShader::Illum(ShadeContext &sc, IllumParams &ip)
{
	LightDesc *pLight;
	Color lightCol;

	// Get our parameters our of the channels
	Color base      = ip.channels[ S_BASE ];
	Color dark      = ip.channels[ S_DARK ];
	Color detail    = ip.channels[ S_DETAIL ];
	Color decal     = ip.channels[ S_DECAL ];
	Color bump      = ip.channels[ S_BUMP ];
	Color gloss     = ip.channels[ S_GLOSS ];
	Color glow      = ip.channels[ S_GLOW ];
	Color reflection = ip.channels[ S_REFLECTION];

	ip.specIllumOut.Black();

	if (m_iApplyMode)
	{
		for (int i = 0; i < sc.nLights; i++) 
		{
			register float fNdotL, fDiffCoef;
			Point3 L;

			pLight = sc.Light(i);
#if MAX_RELEASE < 4000            
			if (pLight->Illuminate(sc, ip.N, lightCol, L, fNdotL, fDiffCoef)) 
#else
			if (pLight->Illuminate(sc, sc.Normal(), lightCol, L, fNdotL,
				fDiffCoef)) 
#endif
			{
				if (pLight->ambientOnly) 
				{
					ip.ambIllumOut += lightCol;
					continue;
				}

				if (fNdotL <= 0.0f) 
					continue;

				if (pLight->affectDiffuse)
					ip.diffIllumOut += fDiffCoef * lightCol;

				if (m_bSpecularOn && pLight->affectSpecular) 
				{
#if MAX_RELEASE < 4000
					Point3 H = Normalize(L - ip.V);
					float c = DotProd(ip.N, H);  
#else
					Point3 H = Normalize(L - sc.V());
					float c = DotProd(sc.Normal(), H);  
#endif
					if (c > 0.0f) 
					{
						c = (float)pow(c, m_fShininess); 
						// c * bright * lightCol;
						ip.specIllumOut += c * lightCol; 
					}
				}
			}
		}
	}
	else    
	{
		ip.ambIllumOut.Black();
		ip.diffIllumOut.White();
	}

	ip.ambIllumOut *= m_ambient;
	ip.diffIllumOut *= dark * (base * m_diffuse * detail);  // + decal;
	ip.selfIllumOut = m_emittance + glow + decal;
	ip.specIllumOut *= m_specular;



#if MAX_RELEASE >= 4000
	//  ShadeTransmission(sc, ip, ip.channels[ID_RR], ip.refractAmt);
	//  ShadeReflection( sc, ip, ip.channels[ID_RL] ); 
	CombineComponents( sc, ip ); 
#endif

}


void EzMaxShader::AffectReflection(ShadeContext &sc, 
								   IllumParams &ip, Color &rcol) 
{ 
	rcol *= m_specular; 
}

void EzMaxShader::CombineComponents( ShadeContext &sc, IllumParams& ip )
{ 
	//  ip.finalC = ip.finalOpac * 
	//    (ip.ambIllumOut + ip.diffIllumOut  + ip.selfIllumOut)
	//          + ip.specIllumOut + ip.reflIllumOut + ip.transIllumOut; 

	ip.finalC = (ip.ambIllumOut + ip.diffIllumOut +
		ip.selfIllumOut) + ip.specIllumOut;
}

BOOL EzMaxShader::IsMetal() 
{ 
	return FALSE; 
}


IOResult EzMaxShader::Save(ISave *isave) 
{ 
	ULONG nb;

	isave->BeginChunk(SHADER_VERS_CHUNK);
	int version = EZ_MAX_SHADER_VERSION;
	isave->Write(&version,sizeof(version),&nb);         
	isave->EndChunk();

	return IO_OK;
}    


RefResult EzMaxShader::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
										  PartID& partID, RefMessage message ) 
{
	switch (message) {
		case REFMSG_CHANGE:
			m_validInterval.SetEmpty();
			if (hTarget == m_pblock)
			{
				// update UI if paramblock changed, possibly from scripter
				ParamID changingParam = m_pblock->LastNotifyParamID();
				// reload the dialog if present
				if (m_pDlg)
				{
					m_pDlg->UpdateDialog( changingParam );
				}
			}
			break;
	}
	return(REF_SUCCEED);
}


class EzMaxShaderCB: public PostLoadCallback 
{
public:
	EzMaxShader *m_pks;
	int loadVersion;
	EzMaxShaderCB(EzMaxShader *newS, int loadVers) { m_pks = newS; loadVersion = loadVers; }
	void proc(ILoad *iload) 
	{
		//m_pks->ReplaceReference(0,UpdateParameterBlock2(StraussShaderPB, STRAUSS_SHADER_NPARAMS, (IParamBlock*)m_pks->m_pblock, &shader_param_blk));
	}
};


IOResult EzMaxShader::Load(ILoad *iload) 
{ 
	ULONG nb;
	int id;
	int version = 0;

	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) 
	{
		switch(id = iload->CurChunkID())  
		{
		case SHADER_VERS_CHUNK:
			res = iload->Read(&version,sizeof(version), &nb);
			break;
		}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
	}
	
	iload->RegisterPostLoadCallback(new EzMaxShaderCB(this, version));
	if(version < EZ_MAX_SHADER_VERSION)
	{
		iload->SetObsolete();
	}
	return IO_OK;
}