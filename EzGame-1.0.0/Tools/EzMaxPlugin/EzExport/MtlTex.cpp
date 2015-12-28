#include "../EzMaxPluginPch.h"
#include "Exporter.h"
#include "ObjectParams.h"
#include <shaders.h>
#include <EzMaterialProperty>
#include <EzShaderFactory>
#include "../Utils.h"


enum { BASE_MAP, C_DARK, C_DETAIL, C_GLOSS, C_GLOW, C_BUMP, C_NORMAL, C_UNK2, 
C_DECAL0, C_DECAL1, C_DECAL2, C_ENVMASK, C_ENV, C_HEIGHT, C_REFLECTION,
};


static bool GetTexFullName(Texmap *texMap, TSTR& fName)
{
	if (texMap && texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) 
	{
		TSTR fileName = ((BitmapTex*)texMap)->GetMapName();
	
		if (fileName.isNull()) 
		{
			fileName = ((BitmapTex*)texMap)->GetFullName();
		
			int idx = fileName.last('(');
			
			if (idx >= 0) 
			{
				fileName.remove(idx, fileName.length() - idx + 1);
				while ( --idx > 0 ) {
					if ( isspace(fileName[idx]) )
						fileName.remove(idx);
				}
			}
		}

		fName = fileName;
		return true;
	}
	return false;
}

static const Class_ID GNORMAL_CLASS_ID(0x243e22c6, 0x63f6a014);

BitmapTex *Exporter::getTexture(Mtl *mtl)
{
	if (!mtl)
		return NULL;
	int texMaps = mtl->NumSubTexmaps();
	if (!texMaps)
		return NULL;
	for (int i=0; i<texMaps; i++)
	{
		Texmap *texMap = mtl->GetSubTexmap(i);
		if (!texMap)
			continue;

		if (texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
		{
			return (BitmapTex*)texMap;
		} else if (texMap && texMap->ClassID() == GNORMAL_CLASS_ID) 
		{
			texMap = texMap->GetSubTexmap(0);

			if (texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) 
			{
				return ((BitmapTex*)texMap);
			}
		}
	}
	return NULL;
}

BitmapTex *Exporter::getTexture(Mtl *mtl, int i)
{
	if (mtl) {
		int texMaps = mtl->NumSubTexmaps();
		if (i < texMaps) {
			if (Texmap *texMap = mtl->GetSubTexmap(i)) 
			{
				if (texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) 
				{
					return (BitmapTex*)texMap;
				} 
				else if (texMap && texMap->ClassID() == GNORMAL_CLASS_ID) 
				{
					texMap = texMap->GetSubTexmap(0);

					if (texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) 
					{
						return ((BitmapTex*)texMap);
					}
				}
			}
		}
	}
	return NULL;
}

void Exporter::getTextureMatrix(Matrix3 &mat, Mtl *mtl)
{
	BitmapTex *tex = getTexture(mtl);
	if (tex)
		tex->GetUVTransform(mat);
	else
		mat.IdentityMatrix();
}

void Exporter::makeTexture(EzAVObject* parent, Mtl *mtl)
{
	BitmapTex *bmTex = getTexture(mtl);
	if (!bmTex)
		return;

	EzTexturingProperty* texProp = EzDynamicCast(EzTexturingProperty, EzCreateObject("EzTexturingProperty"));

	//EzTextureDesc td;

	//EzTriBasedGeom *shape = EzDynamicCast(EzTriBasedGeom, parent);

	//if (makeTextureDesc(bmTex, td, shape->getData()))
	//{
	//	texProp->setTexture(EzTexturingProperty::BASE_MAP, td);
	//}

	EzProperty* prop = EzDynamicCast(EzProperty, texProp);
	parent->addProperty(prop);
}

//bool Exporter::makeTextureDesc(BitmapTex *bmTex, EzTexturingProperty* pTextureProb, EzGeometryData* shape)
//{
//
//	td.source = EzNew EzTexture();
//
//	td.eApplymode = APPLY_MODULATE;
//	// Filtering
//	switch (bmTex->GetFilterType())
//	{
//	case FILTER_PYR:  td.eFilterMode = FILTER_TRILERP; break;
//	case FILTER_SAT:  td.eFilterMode = FILTER_BILERP; break;
//	case FILTER_NADA: td.eFilterMode = FILTER_NEAREST; break;
//	}
//
//	td.eClampMode = ETC_WRAP_U_WRAP_V;
//	switch (bmTex->GetTextureTiling())
//	{
//	case 3: td.eClampMode = ETC_WRAP_U_WRAP_V; break;
//	case 1: td.eClampMode = ETC_WRAP_U_CLAMP_V; break;
//	case 2: td.eClampMode = ETC_CLAMP_U_WRAP_V; break;
//	case 0: td.eClampMode = ETC_CLAMP_U_CLAMP_V; break;
//	}
//
//	if (UVGen *uvGen = bmTex->GetTheUVGen())
//	{
//		if (uvGen && uvGen->IsStdUVGen()) 
//		{
//			StdUVGen *uvg = (StdUVGen*)uvGen;
//			td.uvSet = shape->getUVSetIndex(uvg->GetMapChannel());
//			if (td.uvSet == -1) td.uvSet = 0;
//		}
//
//		if (RefTargetHandle ref = uvGen->GetReference(0))
//		{
//			EzPoint2 trans, tiling;
//			float wangle;
//			bool ok = true;
//			if(ok) ok &= getMAXScriptValue(ref, "U_Offset", 0, trans.X);
//			if(ok) ok &= getMAXScriptValue(ref, "V_Offset", 0, trans.Y);
//			if(ok) ok &= getMAXScriptValue(ref, "U_Tiling", 0, tiling.X);
//			if(ok) ok &= getMAXScriptValue(ref, "V_Tiling", 0, tiling.Y);
//			if(ok) ok &= getMAXScriptValue(ref, "W_Angle", 0, wangle);
//			if (ok) 
//			{
//				if (trans.X != 0.0f || trans.Y != 0.0f || tiling.X != 1.0f || tiling.Y != 1.0f || wangle != 0.0f) 
//				{
//					td.hasTextureTransform = true;
//					td.translation = trans;
//					td.tiling = tiling;
//					td.usRotation = degToRad(wangle);
//					td.usTransformType = 1;
//					td.centerOffset = EzPoint2(0.5, 0.5);
//				}
//			}
//		}
//	}
//
//
//
//
//
//
//
//////	if (Exporter::mNifVersionInt >= VER_20_0_0_4)
//////	{
//////		td.source->SetPixelLayout( PIX_LAY_DEFAULT );
//////		td.source->SetMipMapFormat( MIP_FMT_DEFAULT );
//////		td.source->SetAlphaFormat( ALPHA_DEFAULT );
//////	}
//////
//		// Get file name and check if it matches the "app" settings in the ini file
//	TSTR mapPath;
//	mapPath = bmTex->GetMapName();
//
//	TSTR p, f;
//	SplitPathFile(mapPath, &p, &f);
//	TSTR newPath;
//
//	if (m_TexPrefix != "")
//		newPath = TSTR(m_TexPrefix.c_str()) + _T("\\") + f;
//	else
//		newPath = f;
//
//	td.source->setExternalTexture(newPath.data());
//
//	return true;
//}



bool Exporter::exportToolsShader(EzAVObject* parent, Mtl* mtl)
{
	if (!mtl) 
		return false;

	RefTargetHandle ref = mtl->GetReference(2/*shader*/);
	if (!ref)
		return false;

	TSTR shaderByName; 
	Class_ID shaderID = Class_ID(0, 0);

	if(mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0) )
	{
		StdMat2 * smtl = (StdMat2*)mtl;
		if (smtl->SupportsShaders()) 
		{
			if (Shader *s = smtl->GetShader()) 
			{
				s->GetClassName(shaderByName);
				shaderID = s->ClassID();
			}
		}
	}

	//if (shaderID != EZSHADER_CLASS_ID)
	//{
	//		return false;
	//}

	Color ambient = Color(0.0f,0.0f,0.0f), diffuse = Color(0.0f,0.0f,0.0f), specular = Color(0.0f,0.0f,0.0f), emittance = Color(0.0f,0.0f,0.0f);
	float glossiness = 0.0f, alpha = 0.0f, Magnitude = 0.0f, LumaScale = 0.0f, LumaOffset = 0.0f;
	int TestRef = 0, srcBlend = 0, destBlend = 0, TestMode = 0;
	bool AlphaTestEnable = false;
	int ApplyMode = 0, SrcVertexMode = 0, LightingMode = 0;
	bool VertexColorsEnable = false, SpecularEnable = false, NoSorter = false, Dither = false;
	int alphaMode = 0, BaseTextureExport=0, DarkTextureExport=0, DetailTextureExport=0;
	int Decal1TextureExport=0, Decal2TextureExport=0, GlossTextureExport=0, GlowTextureExport=0;
	LPTSTR CustomShader = NULL;
	int ShaderViewerTechnique=0, ShaderExportTechnique=0;
	bool UseNormalMaps = false;
	int NormalMapTechnique=0;

	bool ok = true;

	if(ok) ok &= getMAXScriptValue(ref, "ambient", 0, ambient );
	if(ok) ok &= getMAXScriptValue(ref, "diffuse", 0, diffuse );
	if(ok) ok &= getMAXScriptValue(ref, "specular", 0, specular );
	if(ok) ok &= getMAXScriptValue(ref, "emittance", 0, emittance );
	if(ok) ok &= getMAXScriptValue(ref, "shininess", 0, glossiness );
	//if(ok) ok &= getMAXScriptValue(ref, "alpha", 0, alpha );
	//if(ok) ok &= getMAXScriptValue(ref, "Bump_Map_Magnitude", 0, Magnitude);
	//if(ok) ok &= getMAXScriptValue(ref, "Bump_Map_Luma_Scale", 0, LumaScale);
	//if(ok) ok &= getMAXScriptValue(ref, "Bump_Map_Luma_offset", 0, LumaOffset);
	//if(ok) ok &= getMAXScriptValue(ref, "TestRef", 0, TestRef );
	//if(ok) ok &= getMAXScriptValue(ref, "AlphaTestEnable", 0, AlphaTestEnable );
	//if(ok) ok &= getMAXScriptValue(ref, "Vertex_Color_Enable", 0, VertexColorsEnable);
	//if(ok) ok &= getMAXScriptValue(ref, "SpecularEnable", 0, SpecularEnable);
	//if(ok) ok &= getMAXScriptValue(ref, "NoSorter", 0, NoSorter);
	//if(ok) ok &= getMAXScriptValue(ref, "Dither", 0, Dither );
	//if(ok) ok &= getMAXScriptValue(ref, "srcBlend", 0, srcBlend);
	//if(ok) ok &= getMAXScriptValue(ref, "destBlend", 0, destBlend);
	//if(ok) ok &= getMAXScriptValue(ref, "TestMode", 0, TestMode );
	//if(ok) ok &= getMAXScriptValue(ref, "ApplyMode", 0, ApplyMode);
	//if(ok) ok &= getMAXScriptValue(ref, "SourceVertexMode", 0, SrcVertexMode);
	//if(ok) ok &= getMAXScriptValue(ref, "LightingMode", 0, LightingMode);
	//if(ok) ok &= getMAXScriptValue(ref, "alphaMode", 0, alphaMode);
	if(ok) ok &= getMAXScriptValue(ref, "CustomShader", 0, CustomShader );



	EzShaderLibrary* pShaderLib = EzShaderFactory::getShaderLib(CustomShader);

	if(pShaderLib)
	{
		EzGeometry* pGeometry = EzDynamicCast(EzGeometry, parent);

		EZASSERT(pGeometry);

		pGeometry->setShaderLibrary(pShaderLib);
		

	}

	
	EzMaterialProperty* pEzMatProperty = EzDynamicCast(EzMaterialProperty,EzCreateObject("EzMaterialProperty"));


	if(ok)
	{
		pEzMatProperty->setAmbientColor(EZCOLOR(ambient));
		pEzMatProperty->setDiffuseColor(EZCOLOR(diffuse));
		pEzMatProperty->setSpecularColor(EZCOLOR(specular));
		pEzMatProperty->setEmissiveColor(EZCOLOR(emittance));
		pEzMatProperty->setGlossiness(glossiness / 100.0f);
		pEzMatProperty->setTransparency(alpha);


	}
	

	parent->addProperty(pEzMatProperty);

	StdMat2 *m2 = NULL;
	if(mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) m2 = (StdMat2*)mtl;

	int ntex = mtl->NumSubTexmaps();
	if (ntex > 0)
	{
		int maxTexture = 7;
		EzTexturingProperty* pTexProp = NULL;
		for (int i = 0; i < ntex; ++i) 
		{
			BitmapTex *bmTex = getTexture(mtl, i);
			if (!bmTex || (m2 && (m2->GetMapState(i)) != 2))
				continue;

		
			if (pTexProp == NULL)
			{
				pTexProp = new EzTexturingProperty();       
				pTexProp->setTextureCount(3);
				//pTexProp->setApplyMode(Niflib::ApplyMode(ApplyMode));
//					pTexProp->sSetTextureCount(7);
			}

		//	EzTextureDesc td;

		//	EzTriBasedGeom *shape = EzDynamicCast(EzTriBasedGeom, parent);

		//	if (makeTextureDesc(bmTex, td, shape->getData())) 
		//	{
		//		//if (textype == BUMP_MAP) {
		//		//		td.source->SetPixelLayout(PIX_LAY_BUMPMAP);
		//		//		texProp->SetLumaOffset(LumaOffset);
		//		//		texProp->SetLumaScale(LumaScale);

		//		//		Matrix22 m2;
		//		//		m2[0][0] = m2[1][1] = Magnitude;
		//		//		m2[0][1] = m2[1][0] = 0.0f;
		//		//	texProp->SetBumpMapMatrix(m2);
		//		//}
		//		pTexProp->setTexture((EzTexturingProperty::EMAP)i, td);

		//		//// kludge for setting decal maps without messing up the file sizes
		//		//if ( Exporter::mNifVersionInt <= 0x14010003)
		//		//{
		//		//	if (textype == C_DECAL0)
		//		//		texProp->SetTextureCount(7);
		//		//	else if (textype == C_DECAL1)
		//		//		texProp->SetTextureCount(8);
		//		//}

		//		//if ( textype > maxTexture )
		//		//	maxTexture = textype;
		//	}
		}
		
			
		if (pTexProp != NULL)
		{
			parent->addProperty(pTexProp);
		}
	}
		
	return true;
}

void Exporter::makeMaterial(EzAVObject* parent, Mtl *mtl)
{
	// Fill-in using the Civ4 Shader if available
	bool done = exportToolsShader(parent, mtl);
	if (done)
	{
		return;
	}

	EzStringc name;
	EzMaterialProperty* mtlProp = EzDynamicCast(EzMaterialProperty,EzCreateObject("EzMaterialProperty"));

	if (mtl)
	{
		Color c;

		c = mtl->GetAmbient();
		mtlProp->setAmbientColor(EzColor(c.r, c.g, c.b));

		c = mtl->GetDiffuse();
		mtlProp->setDiffuseColor(EzColor(c.r, c.g, c.b));

		c = mtl->GetSpecular();
		mtlProp->setSpecularColor(EzColor(c.r, c.g, c.b));

		c = (mtl->GetSelfIllumColorOn()) ? mtl->GetSelfIllumColor() : Color(0,0,0);
		mtlProp->setEmissiveColor(EzColor(c.r, c.g, c.b));

		mtlProp->setTransparency(1);
		mtlProp->setGlossiness(mtl->GetShininess() * 100.0f);
		name = (char*)mtl->GetName();

		if(mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0) )
		{
			StdMat2 * smtl = (StdMat2*)mtl;
			mtlProp->setTransparency(smtl->GetOpacity(0));

			if (smtl->SupportsShaders())
			{
				if (Shader *s = smtl->GetShader())
				{
					//if (smtl->GetWire())
					//{
					//	NiWireframePropertyRef wireProp = new NiWireframeProperty();
					//	wireProp->SetFlags(1);
					//	parent->AddProperty(wireProp);
					//}
					//if (smtl->GetTwoSided())
					//{
					//	NiStencilPropertyRef stencil = new NiStencilProperty();
					//	stencil->SetStencilFunction(TEST_GREATER);
					//	stencil->SetStencilState(false);
					//	stencil->SetPassAction(ACTION_INCREMENT);
					//	stencil->SetFaceDrawMode(DRAW_BOTH);
					//	stencil->SetFlags(19840);
					//	parent->AddProperty(stencil);
					//}
					//if (smtl->IsFaceted())
					//{
					//	NiShadePropertyRef shade = CreateNiObject<NiShadeProperty>();
					//	shade->SetFlags(0);
					//	parent->AddProperty(shade);
					//}
				}
			}
		}
	} else
	{
		mtlProp->setAmbientColor(EzColor(0.588f, 0.588f, 0.588f));
		mtlProp->setDiffuseColor(EzColor(1, 1, 1));
		mtlProp->setSpecularColor(EzColor(0.9f, 0.9f, 0.9f));
		mtlProp->setEmissiveColor(EzColor(0, 0, 0));
		mtlProp->setTransparency(1);
		mtlProp->setGlossiness(10);
		name = "default";
	}

	mtlProp->setName(name.c_str());

	EzProperty* prop = EzDynamicCast(EzProperty, mtlProp);

	parent->addProperty(prop);

	//makeTexture(parent, mtl);
}
//
//
//Mtl *Exporter::getMaterial(INode *node, int subMtl)
//{
//	Mtl *nodeMtl = node->GetMtl();
//	if (nodeMtl)
//	{
//		int numSub = nodeMtl->NumSubMtls();
//		if (numSub > 0)
//		{
//			return nodeMtl->GetSubMtl(subMtl % numSub);
//		}
//
//		return nodeMtl;
//	}
//	return NULL;
//}
//
