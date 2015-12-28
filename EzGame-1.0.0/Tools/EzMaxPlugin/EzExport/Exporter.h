#ifndef __NIFEXPORT__H
#define __NIFEXPORT__H

#include <EzNode>
#include <EzMap>
#include <EzTriangle>
#include <EzTexturingProperty>
#include "EzGenerateMesh.h"

class BitmapTex;

#define	EZF_WITHOUT_ANI						 0
#define	EZF_WO_KF							 1
#define	SINGLE_KF_WITH_EZF					 2
#define	SINGLE_KF_WO_EZF					 3
#define	MULTI_KF_WITH_EZF					 4
#define	MULTI_KF_WO_EZF						 5
#define	EZF_WITH_MGR						 6



class Exporter
{

public:

	enum Result
	{
		Error,
		Ok,
		Abort,
		Skip
	};

	// Callback for post-processing instructions
	struct EzCallback
	{
		EzCallback() {};
		virtual ~EzCallback() {};
		virtual Result execute() = 0;
	};

	Exporter(Interface *i);

	Result doExport(EzNode *pRoot, INode *node);

	bool initialize(Interface *gi);
	bool readConfig(Interface *i);
	bool writeConfig(Interface *i);


	bool			m_SelectedOnly;
	EzStringc		m_CreatorName;
	EzStringc		m_TexPrefix;
	int				m_TriStrips;
	int				m_uiUserVersion; 	
	int				m_FlattenHierarchy;
	int				m_SkeletonOnly;
	int				m_CollapseTransforms;	
	int				m_VertexColors;
	int				m_ExportCollision;

	int				m_ExportType;



protected:
	typedef EzMap<EzStringc, EzNode*>	NodeMap;	
	typedef EzMap<INode*,	EzNode*>	NodeToNodeMap;

					
	EzTriBasedGeom* makeMesh(EzNode* parent, Mtl *mtl, EzGenerateMesh::FaceGroup* pFG, bool exportStrips);
	Result exportMesh(EzNode* eznode, INode *node, TimeValue t);
	bool exportToolsShader(EzAVObject* parent, Mtl* mtl);


	// Texture
	BitmapTex*	getTexture(Mtl *mtl);
	BitmapTex*	getTexture(Mtl *mtl, int i);
	void getTextureMatrix(Matrix3 &mat, Mtl *mtl);
	void makeTexture(EzAVObject* parent, Mtl *mtl);
//	bool makeTextureDesc(BitmapTex *bmTex, EzTexturingProperty* pTextureProb, EzGeometryData* shape);

	//Mtl *getMaterial(INode *node, int subMtl);
	void makeMaterial(EzAVObject* parent, Mtl *mtl);

	
	Result exportNodes(EzNode* parent, INode *node);
	int countNodes(INode *node);
	bool isMeshGroup(INode *maxNode, bool root = true);
	bool exportUPB(EzNode *root, INode *node);

	EzNode* makeNode(EzNode* parent, INode *maxNode, bool local);

	// util
	bool TMNegParity(const Matrix3 &m);
	EzPoint3 getVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);
	EzNode* findNode(INode* maxNode);
	EzNode* getNode(INode* maxNode);
	EzNode* getNode(const EzStringc& name);

	bool isNodeTracked(INode *node);


	void convertMatrix(EzMatrix3 &dst, const Matrix3 &src);
	Matrix3 getNodeTransform(INode *node, TimeValue t, bool local);
	Matrix3 getObjectTransform(INode *node, TimeValue t, bool local);
	void nodeTransform(EzTransform& transform, INode *node, TimeValue t, bool local);
	void objectTransform(EzTransform& transform, INode *node, TimeValue t, bool local);


	
	Interface				*m_Interface;
	EzNode*					m_pEzRoot;

	NodeMap					m_NameMap;
	NodeToNodeMap			m_NodeMap;

	


	/* exporter version */
	static int				m_Version;

	enum EProgressSection
	{
		Geometry,
		Animation,
		Collision,
		Skin,
		ProgressSectionCount
	};
	int m_iprogressCounters[ProgressSectionCount];
	int m_iprogressMax[ProgressSectionCount];

	void progressUpdate( EProgressSection section, const TCHAR *s );

};


extern HINSTANCE hInstance;

#endif