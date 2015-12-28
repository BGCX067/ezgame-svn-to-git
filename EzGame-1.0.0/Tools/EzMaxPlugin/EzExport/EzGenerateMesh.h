#pragma once



class BitmapTex;



class EzGenerateMesh
{
public:

	typedef EzArray<EzPoint2>	TexCoords;

	struct VertexGroup
	{
		int				idx;
		EzPoint3		pt;
		EzPoint3		norm;
		TexCoords		uvs;
		EzColor			color;
	};

	struct FaceGroup
	{
		EzArray<VertexGroup>	vgrp;
		EzArray<int>	        vmap;
		EzArray<EzPoint3>		verts;
		EzArray<EzPoint3>		vnorms;
		EzArray<EzTriangle16>	Triangles;
		EzArray<TexCoords>		uvs;
		EzArray<int>			uvMapping;
	};


	typedef EzMap<int, FaceGroup*>   MeshObjects;
	MeshObjects			m_MeshGroup;

	EzGenerateMesh(void);
	~EzGenerateMesh(void);

	void createVertexColors(Mesh *mesh);
	void createMesh(INode *node, Mesh& mesh, TimeValue t, bool bNoSplit);
	const int& getNumMesh() const;
	const int& getNumVectorColors() const;

protected:

	bool equal(const EzPoint3 &a, const EzPoint3 &b, float fThresh);
	int addVertex(FaceGroup& fg, int face, int vi, Mesh *mesh, const Matrix3 &texm);
	
	bool TMNegParity(const Matrix3 &m);
	BitmapTex *getTexture(Mtl *mtl);
	BitmapTex *getTexture(Mtl *mtl, int i);
	void getTextureMatrix(Matrix3 &mat, Mtl *mtl);
	EzPoint3 getVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);
	Mtl *getMaterial(INode *node, int subMtl);

	int					m_iNumVertexColors;
	EzColor*			m_pVertexColors;

	int					m_iNumTriangle;
	EzTriangle16*		m_pTriangles;

	int					m_iNumMaps;
	VertexGroup*		m_pVertexGrp;
};

inline const int& EzGenerateMesh::getNumVectorColors() const
{
	return m_iNumVertexColors;
}

inline const int& EzGenerateMesh::getNumMesh() const
{
	return m_MeshGroup.size();
}
