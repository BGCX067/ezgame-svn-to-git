#include "../EzMaxPluginPch.h"
#include "../Utils.h"
#include "EzGenerateMesh.h"
#include <functional>

#if VERSION_3DSMAX > ((5000<<16)+(15<<8)+0) // Version 5
#  include <MeshNormalSpec.h>
#endif
#ifdef USE_BIPED
#  include <cs/BipedApi.h>
#endif


static const Class_ID GNORMAL_CLASS_ID(0x243e22c6, 0x63f6a014);




struct VertexCompare
{
	typedef EzGenerateMesh::VertexGroup			VertexGroup;
	typedef EzGenerateMesh::FaceGroup			FaceGroup;
	typedef EzGenerateMesh::TexCoords			TexCoords;

	VertexCompare(FaceGroup& g, float pt, float nt, float vt) 
		: grp(g), thresh(pt), normthresh(nt), vthresh(vt) {
			if (normthresh > thresh)
				normthresh = thresh;
			if (vthresh > thresh)
				vthresh = thresh;
	}
	inline bool operator()(const EzPoint2& lhs, const EzPoint2& rhs, float thresh) const {
		return compare(lhs, rhs, thresh) < 0;
	}
	inline bool operator()(const VertexGroup& lhs, const VertexGroup& rhs) const {
		return compare(lhs, rhs) < 0;
	}
	inline bool operator()(int lhs, const VertexGroup& rhs) const {
		return compare(lhs, rhs) < 0;
	}
	inline bool operator()(const VertexGroup& lhs, int rhs) const {
		return compare(lhs, rhs) < 0;
	}
	inline bool operator()(int lhs, int rhs) const {
		return compare(lhs, rhs) < 0;
	}

	inline int compare(float a, float b, float thresh) const {
		if (equals(a, b, thresh)) return 0;
		return std::less<float>()(a, b) ? -1 : 1;
	}
	inline int compare(const Point3 &a, const Point3 &b, float thresh) const {
		int d;
		if ((d = compare(a.x,b.x,thresh)) != 0) return d;
		if ((d = compare(a.y,b.y,thresh)) != 0) return d;
		if ((d = compare(a.z,b.z,thresh)) != 0) return d;
		return 0;
	}
	inline int compare(const EzPoint3 &a, const Point3 &b, float thresh) const {
		int d;
		if ((d = compare(a.X,b.x,thresh)) != 0) return d;
		if ((d = compare(a.Y,b.y,thresh)) != 0) return d;
		if ((d = compare(a.Z,b.z,thresh)) != 0) return d;
		return 0;
	}
	inline int compare(const Point3 &a, const EzPoint3 &b, float thresh) const {
		int d;
		if ((d = compare(a.x,b.X,thresh)) != 0) return d;
		if ((d = compare(a.y,b.Y,thresh)) != 0) return d;
		if ((d = compare(a.z,b.Z,thresh)) != 0) return d;
		return 0;
	}
	inline int compare(const EzPoint3 &a, const EzPoint3 &b, float thresh) const {
		int d;
		if ((d = compare(a.X,b.X,thresh)) != 0) return d;
		if ((d = compare(a.Y,b.Y,thresh)) != 0) return d;
		if ((d = compare(a.Z,b.Z,thresh)) != 0) return d;
		return 0;
	}
	inline int compare(const VertexGroup& lhs, const VertexGroup& rhs) const {
		int d;
		if ((d = compare(lhs.pt,rhs.pt,thresh)) != 0) return d;
		if ((d = compare(lhs.norm,rhs.norm,normthresh)) != 0) return d;
		if ((d = compare(lhs.color,rhs.color,vthresh)) != 0) return d;
		if ((d = lhs.uvs.size() - rhs.uvs.size()) != 0) return d;
		for (int i=0; i<lhs.uvs.size(); ++i) {
			if ((d = compare(lhs.uvs[i],rhs.uvs[i],vthresh)) != 0) return d;
		}
		return 0;
	}
	inline int compare(const EzPoint2& lhs, const EzPoint2& rhs, float thresh) const {
		int d;
		if ((d = compare(lhs.X,rhs.Y,thresh)) != 0) return d;
		if ((d = compare(lhs.X,rhs.Y,thresh)) != 0) return d;
		return 0;
	}
	inline int compare(const EzColor &a, const EzColor &b, float thresh) const {
		int d;
		//if ((d = compare(a.r,b.r,thresh)) != 0) return d;
		//if ((d = compare(a.g,b.g,thresh)) != 0) return d;
		//if ((d = compare(a.b,b.b,thresh)) != 0) return d;
		return 0;
	}
	inline int compare(int lhs, const VertexGroup& rhs) const {
		return compare(grp.vgrp[lhs], rhs);
	}
	inline int compare(const VertexGroup& lhs, int rhs) const {
		return compare(lhs, grp.vgrp[rhs]);
	}
	inline int compare(int lhs, int rhs) const {
		return compare(grp.vgrp[lhs], grp.vgrp[rhs]);
	}
	FaceGroup& grp;
	float thresh, normthresh, vthresh;
};



EzGenerateMesh::EzGenerateMesh(void)
:m_iNumVertexColors(0), m_pVertexColors(NULL), m_iNumTriangle(0), m_pTriangles(NULL), m_iNumMaps(0)
{
}

EzGenerateMesh::~EzGenerateMesh(void)
{
}

Mtl *EzGenerateMesh::getMaterial(INode *node, int subMtl)
{
	Mtl *nodeMtl = node->GetMtl();
	if (nodeMtl)
	{
		int numSub = nodeMtl->NumSubMtls();
		if (numSub > 0)
		{
			return nodeMtl->GetSubMtl(subMtl % numSub);
		}

		return nodeMtl;
	}
	return NULL;
}

EzPoint3 EzGenerateMesh::getVertexNormal(Mesh* mesh, int faceNo, RVertex* rv)
{
	Face* f = &mesh->faces[faceNo];
	DWORD smGroup = f->smGroup;
	int numNormals;
	Point3 vertexNormal;

	// Is normal specified
	// SPCIFIED is not currently used, but may be used in future versions.
	if (rv->rFlags & SPECIFIED_NORMAL)
	{
		vertexNormal = rv->rn.getNormal();
	} 
	else 
	{
		// If normal is not specified it's only available if the face belongs
		// to a smoothing group
		if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup) 
		{
			// If there is only one vertex is found in the rn member.
			if (numNormals == 1) 
			{
				vertexNormal = rv->rn.getNormal();
			} else 
			{
				// If two or more vertices are there you need to step through them
				// and find the vertex with the same smoothing group as the current face.
				// You will find multiple normals in the ern member.
				for (int i = 0; i < numNormals; i++) 
				{
					if (rv->ern[i].getSmGroup() & smGroup) 
						vertexNormal = rv->ern[i].getNormal();
				}
			}

		} 
		else 
		{
			// Get the normal from the Face if no smoothing groups are there
			vertexNormal = mesh->getFaceNormal(faceNo);
		}
	}

	return EZPOINT3(vertexNormal);
}


BitmapTex *EzGenerateMesh::getTexture(Mtl *mtl)
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

BitmapTex *EzGenerateMesh::getTexture(Mtl *mtl, int i)
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

void EzGenerateMesh::getTextureMatrix(Matrix3 &mat, Mtl *mtl)
{
	BitmapTex *tex = getTexture(mtl);
	if (tex)
		tex->GetUVTransform(mat);
	else
		mat.IdentityMatrix();
}

bool EzGenerateMesh::TMNegParity(const Matrix3 &m)
{
	return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?true:false;
}

void EzGenerateMesh::createVertexColors(Mesh *mesh)
{
	bool hasvc = false;

	// make Alpha color
	if (mesh->mapSupport(MAP_ALPHA))
	{
		mesh->setVCDisplayData(MAP_ALPHA);         
		m_iNumVertexColors = mesh->getNumVertCol();

		if(m_pVertexColors)
		{
			EzDelete m_pVertexColors;
			m_pVertexColors = NULL;
		}

		m_pVertexColors = EzNew EzColor [m_iNumVertexColors];

		VertColor *vertCol = mesh->vertColArray;

		if (vertCol) 
		{
			for (int i=0; i<m_iNumVertexColors; ++i) 
			{
				VertColor c = vertCol[ i ];
				float a = (c.x + c.y + c.z) / 3.0f;
				m_pVertexColors[i].a = a;
				hasvc |= (a != 1.0f);
			}
		}
	}

	// make RGB color
	if (mesh->mapSupport(0))
	{
		mesh->setVCDisplayData(0);
		VertColor *vertCol = mesh->vertColArray;
		int n = mesh->getNumVertCol();

		if(n > m_iNumVertexColors)
		{
			EzColor* pOldVertexColors = m_pVertexColors;

			m_pVertexColors = EzNew EzColor[n];
			CopyMemory(m_pVertexColors, pOldVertexColors, sizeof(EzColor) * m_iNumVertexColors);
			m_iNumVertexColors = n;
			EzDelete pOldVertexColors;
		}
	
		if (vertCol) 
		{
			for (int i=0; i<n; ++i) 
			{
				VertColor col = vertCol[ i ];
				m_pVertexColors[i] = EzColor(col.x, col.y, col.z, m_pVertexColors[i].a);
				hasvc |= (col.x != 1.0f || col.y != 1.0f || col.z != 1.0f);
			}
		}
	}

	if (!hasvc) 
	{
		m_iNumVertexColors = 0;
		EzDelete m_pVertexColors;
		m_pVertexColors = NULL;
	}
}


bool EzGenerateMesh::equal(const EzPoint3 &a, const EzPoint3 &b, float fThresh)
{
	return (fabsf(a.X-b.X) <= fThresh) &&
		   (fabsf(a.Y-b.Y) <= fThresh) &&
		   (fabsf(a.Z-b.Z) <= fThresh);
}



int EzGenerateMesh::addVertex(FaceGroup& fg, int face, int vi, Mesh *mesh, const Matrix3 &texm)
{
	VertexGroup vg;
	int vidx;
	vidx = vg.idx = mesh->faces[ face ].v[ vi ];
	EzPoint3 vertex;
	EzPoint3 norm;

	vg.pt = EZPOINT3(mesh->verts[ vidx ]);

#if VERSION_3DSMAX <= ((5100<<16)+(8<<8)+0) // Version 5
	vg.norm = getVertexNormal(mesh, face, mesh->getRVertPtr(vidx));
#else
	MeshNormalSpec *specNorms = mesh->GetSpecifiedNormals ();
	if (NULL != specNorms && specNorms->GetNumNormals() != 0)
		vg.norm = EZPOINT3(specNorms->GetNormal(face, vi));
	else
		vg.norm = getVertexNormal(mesh, face, mesh->getRVertPtr(vidx));
#endif

	int nmaps = fg.uvMapping.size();
	vg.uvs.set_used(nmaps > 0 ? nmaps : 1);

	if (nmaps > 0) 
	{
		for (int n = 0; n < fg.uvMapping.size(); ++n)
		{
			int maxIDX = fg.uvMapping[n];
			UVVert *uv = mesh->mapVerts(maxIDX); 
			TVFace *tv = mesh->mapFaces(maxIDX);

			EzPoint2 uvs;

			if (uv && tv) 
			{
				Point3 uvw = uv[ tv[ face ].t[ vi ]] * texm;
				uvs.X = uvw[0];
				uvs.Y = uvw[1] + 1.0f;
				vg.uvs[n] = uvs;
			}
		}
	} 
	else 
	{
		if (mesh->tVerts && mesh->tvFace) 
		{
			EzPoint2 uvs;

			Point3 uvw = mesh->tVerts[ mesh->tvFace[ face ].t[ vi ]] * texm;
			uvs.X = uvw[0];
			uvs.Y = uvw[1] + 1.0f;
			vg.uvs[0] = uvs;
		}
	}

	if (m_iNumVertexColors)
	{
		TVFace *vcFace = mesh->vcFaceData ? mesh->vcFaceData : mesh->vcFace;
		if (vcFace) 
		{
			int vidx = vcFace[ face ].t[ vi ];
			vg.color = m_pVertexColors[vidx];
		}
	}


	VertexCompare vc(fg, 0.0f, 0.0f, 0.0f);

	for (int i=0; i<fg.vgrp.size(); i++)
	{
		if ( vc.compare(vg, i) == 0 )
			return i;
	}

	fg.vgrp.push_back(vg);
	
	return fg.vgrp.size() - 1;
}

void EzGenerateMesh::createMesh(INode *node, Mesh& mesh, TimeValue t, bool bNoSplit)
{
	Mtl* nodeMtl = node->GetMtl();
	Matrix3 tm = node->GetObjTMAfterWSM(t);

	// Order of the vertices. Get 'em counter clockwise if the objects is
	// negatively scaled.
	int vi[3];

	if (TMNegParity(tm)) 
	{
		vi[0] = 0; vi[1] = 1; vi[2] = 2;
	} 
	else 
	{
		vi[0] = 2; vi[1] = 1; vi[2] = 0;
	}


	int nv = mesh.getNumVerts();
	int nf = mesh.getNumFaces();
	Matrix3 flip;
	flip.IdentityMatrix();
	flip.Scale(Point3(1, -1, 1));

	int face, numSubMtls = nodeMtl ? nodeMtl->NumSubMtls() : 0;

	for (face=0; face<mesh.getNumFaces(); face++) 
	{
		int mtlID = (numSubMtls!=0) ? (mesh.faces[face].getMatID() % numSubMtls) : 0;
		Mtl *mtl = getMaterial(node, mtlID);
		Matrix3 texm;
		getTextureMatrix(texm, mtl);
		texm *= flip;

		FaceGroup* pFG = NULL;
		MeshObjects::Node * pMeshNode = m_MeshGroup.find(mtlID);
			
		if(!pMeshNode)
		{
			pFG = EzNew FaceGroup;
			m_MeshGroup.insert(mtlID, pFG);
		}
		else
		{
			pFG = pMeshNode->getValue();
		}
		if (pFG->uvMapping.size() == 0) // Only needs to be done once per face group
		{
			int nmaps = 0;

			BOOL e = mesh.mapSupport(0);
			int nmapsStart = max(1, mesh.getNumMaps() - (mesh.mapSupport(0) ? 1 : 0)); // Omit vertex color map.
			for (int ii = 1; ii <= nmapsStart; ii++) // Winnow out the unsupported maps.
			{
				if (!mesh.mapSupport(ii)) 
					continue;

				pFG->uvMapping.push_back(ii);
				nmaps++;
			}

			if(nmaps > 0)
			{
				pFG->uvs.set_used(nmaps == 0 ? 1 : nmaps);
			}
		}

		EzTriangle16 tri;
		for (int i=0; i<3; i++)
		{
			tri[i] = addVertex(*pFG, face, vi[i], &mesh, flip);
		}

		pFG->Triangles.push_back(tri);
	}

}