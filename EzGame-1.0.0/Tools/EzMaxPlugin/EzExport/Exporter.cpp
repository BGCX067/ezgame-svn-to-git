#include "../EzMaxPluginPch.h"
#include "../Utils.h"
#include "Exporter.h"
#include "EzGenerateMesh.h"
#include <EzMatrix3>

#if VERSION_3DSMAX > ((5000<<16)+(15<<8)+0) // Version 5
#include <MeshNormalSpec.h> 
//#include "MNNormalSpec.h"
#endif
#ifdef USE_BIPED
#  include <cs/BipedApi.h>
#endif

int Exporter::m_Version=013;
static LPCTSTR EZFExportSection = TEXT("Export");

static bool isNodeOrParentSelected(INode *node) 
{
	if (node == NULL)
		return false;
	if (node->Selected())
		return true;
	return isNodeOrParentSelected(node->GetParentNode());
}

Exporter::Exporter(Interface *i)
:m_Interface(i)
{

}

bool Exporter::initialize(Interface *gi) 
{
	TCHAR iniName[MAX_PATH];

	if (getIniFileName(iniName))
	{
		//bool reparse = getIniValue<bool>("System", "Reparse", false, iniName);
		//if (reparse || TheAppSettings.empty()){
		//	TheAppSettings.clear();
		//}

		//string Applications = GetIniValue<string>("System", "KnownApplications", "", iniName);
		//stringlist apps = TokenizeString(Applications.c_str(), ";");
		//apps.push_back(string("User")); // always ensure that user is present
		//for (stringlist::iterator appstr=apps.begin(); appstr != apps.end(); ++appstr){
		//	AppSettings* setting = FindAppSetting(*appstr);
		//	if (NULL == setting){
		//		AppSettingsMap::iterator itr = TheAppSettings.insert(TheAppSettings.end(), AppSettings(*appstr));
		//		(*itr).ReadSettings(iniName);
		//	}
		//}

		return true;
	}

	return false;
}


bool  Exporter::readConfig(Interface *i)
{
	TCHAR iniName[MAX_PATH];


	if(getIniFileName(iniName))
	{
		m_CreatorName = getIniValue<EzStringc>(EZFExportSection, "Creator", "", iniName);
		m_TexPrefix = getIniValue<EzStringc>(EZFExportSection, "TexturePrefix", "", iniName);
		m_uiUserVersion = getIniValue<int>(EZFExportSection, "UserVersion", 1, iniName);
		m_TriStrips = getIniValue<int>(EZFExportSection, "GenerateStrips", 1, iniName);
		m_FlattenHierarchy = getIniValue(EZFExportSection, "FlattenHierarchy", 0, iniName);
		m_SkeletonOnly = getIniValue(EZFExportSection, "SkeletonOnly", 0, iniName);
		m_CollapseTransforms = getIniValue(EZFExportSection, "CollapseTransforms ", 0, iniName);
		m_VertexColors = getIniValue(EZFExportSection, "VertexColors", 0, iniName);
		m_ExportType = getIniValue(EZFExportSection, "ExportType", 0, iniName);

		return true;   
	}

	return false;
}

bool Exporter::writeConfig(Interface *i)
{
	TCHAR iniName[MAX_PATH];
	if(getIniFileName(iniName))
	{
		setIniValue(EZFExportSection, "GenerateStrips", m_TriStrips, iniName);    
		setIniValue(EZFExportSection, "TexturePrefix", CStr(m_TexPrefix.c_str()), iniName);
		setIniValue(EZFExportSection, "FlattenHierarchy", m_FlattenHierarchy, iniName);
		setIniValue(EZFExportSection, "SkeletonOnly", m_SkeletonOnly, iniName);
		setIniValue(EZFExportSection, "CollapseTransforms ", m_CollapseTransforms, iniName);
		setIniValue(EZFExportSection, "VertexColors ", m_VertexColors, iniName);
		setIniValue(EZFExportSection, "ExportType", m_ExportType, iniName);
		     

		return true;
	}

	return false;
}


bool Exporter::TMNegParity(const Matrix3 &m)
{
	return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?true:false;
}

EzPoint3 Exporter::getVertexNormal(Mesh* mesh, int faceNo, RVertex* rv)
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

	return EzPoint3(vertexNormal.x, vertexNormal.y, vertexNormal.z);
}

void Exporter::convertMatrix(EzMatrix3 &dst, const Matrix3 &src)
{
	Point3 r0 = src.GetRow(0);
	Point3 r1 = src.GetRow(2);
	Point3 r2 = src.GetRow(1);

	EzMatrix3 temp;

	dst[0] = r0.x;
	dst[1] = r0.z;
	dst[2] = r0.y;

	dst[3] = r1.x;
	dst[4] = r1.z;
	dst[5] = r1.y;

	dst[6] = r2.x;
	dst[7] = r2.z;
	dst[8] = r2.y;


}

Matrix3 Exporter::getNodeTransform(INode *node, TimeValue t, bool local)
{
	Matrix3 tm = node->GetNodeTM(t);
	if (local)
	{
		INode *parent = node->GetParentNode();
		if (parent != NULL) {
			Matrix3 pm = parent->GetNodeTM(t);
			pm.Invert();
			tm *= pm;
		}
	}
	return tm;
}

Matrix3 Exporter::getObjectTransform(INode *node, TimeValue t, bool local)
{
	Matrix3 tm = node->GetObjTMAfterWSM(t);
	if (local)
	{
		INode *parent = node->GetParentNode();
		if (parent != NULL) {
			Matrix3 pm = parent->GetNodeTM(t);
			pm.Invert();
			tm *= pm;
		}
	}
	return tm;
}

void Exporter::nodeTransform(EzTransform& transform, INode *node, TimeValue t, bool local)
{
	EzMatrix3 rot;

	Matrix3 tm = getNodeTransform(node, t, local);
	convertMatrix(rot, tm);
	transform.setRotation(rot);
	transform.setTranslation(EzPoint3(tm.GetTrans().x, tm.GetTrans().z, tm.GetTrans().y));

}

void Exporter::objectTransform(EzTransform& transform, INode *node, TimeValue t, bool local)
{
	EzMatrix3 rot;
	Matrix3 tm = getObjectTransform(node, t, local);
	convertMatrix(rot, tm);

	transform.setRotation(rot);
	transform.setTranslation(EzPoint3(tm.GetTrans().x, tm.GetTrans().z, tm.GetTrans().y));
}


int Exporter::countNodes(INode *node)
{
	int counter = 1;

	for (int i=0; i<node->NumberOfChildren(); i++) 
	{
		counter += countNodes(node->GetChildNode(i));
	}
	return counter;
}

EzNode* Exporter::findNode(INode* maxNode)
{
	NodeToNodeMap::Node* pNode = m_NodeMap.find(maxNode);

	if(pNode)
	{
		return pNode->getValue();
	}

	return NULL;
}

EzNode* Exporter::getNode(INode* maxNode)
{
	EzStringc name = maxNode->GetName();

	NodeToNodeMap::Node* pNode = m_NodeMap.find(maxNode);

	if(pNode)
	{
		return pNode->getValue();
	}

	EzNode* node = getNode(name);
	m_NodeMap[maxNode] = node;
	return node;
}
EzNode* Exporter::getNode(const EzStringc& name)
{
	NodeMap::Node *pNode = m_NameMap.find(name);
	if (pNode)
	{
		return pNode->getValue();
	}

	EzNode* node = EzStaticCast(EzNode, EzCreateObject("EzNode"));

	node->setName(name.c_str());
	m_NameMap[name] = node;
	return node;
}


EzNode* Exporter::makeNode(EzNode* parent, INode *maxNode, bool local)
{
	EzStringc name = (char*)maxNode->GetName();
	EzNode* node = getNode(maxNode);

	EzTransform transform;
	TimeValue t = 0;
	nodeTransform(transform, maxNode, t, local);

	node->setRotation(transform.getToMatrixRotation());
	node->setPosition(transform.getTranslation());
	
	exportUPB(node, maxNode);

	//// Normal Embedded Animation 
	//if (mExportType == NIF_WO_KF)
	//	CreateController(maxNode, Interval());

	parent->addChild(EzDynamicCast(EzAVObject, node));

	return node;
}

bool Exporter::isNodeTracked(INode *node)
{
	//if (Exporter::mUseTimeTags) {
	//	// Assume only one top level node has animation
	//	if (mI->GetRootNode() == node->GetParentNode() && isNodeKeyed(node)) {
	//		return true;
	//	}
	//}
	//else if (node->HasNoteTracks()) {
	//	for (int i=0, n=node->NumNoteTracks(); i<n; ++i) {
	//		if ( NoteTrack *nt = node->GetNoteTrack(i) ) {
	//			if ( nt->ClassID() == Class_ID(NOTETRACK_CLASS_ID,0) ) {
	//				DefNoteTrack *defNT = (DefNoteTrack *)nt;
	//				if ( defNT->NumKeys() > 0 ) {
	//					for (int j=0, m=defNT->keys.Count(); j<m; ++j) {
	//						NoteKey* key = defNT->keys[j];
	//						// Versions less than 20.0.0.4 will always export
	//						if (wildmatch("*start*", key->note) ) {
	//							return true;
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	return false;
}

bool Exporter::isMeshGroup(INode *maxNode, bool root)
{
	if (root)
	{
		if (!maxNode->IsGroupHead())
			return false;
	} 
	else
	{
		//if (!isCollision(maxNode))
		//{
			TimeValue t = 0;
			ObjectState os = maxNode->EvalWorldState(t); 
			if (os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID)
				return true;
		//}
	}

	for (int i=0; i<maxNode->NumberOfChildren(); i++) 
	{
		if (isMeshGroup(maxNode->GetChildNode(i), false))
			return true;
	}

	return false;
}

bool Exporter::exportUPB(EzNode *root, INode *node)
{
	bool ok = false;
	//if (!mUserPropBuffer)
	//	return ok;

	// Write the actual UPB sans any np_ prefixed strings
	TSTR upb;
	node->GetUserPropBuffer(upb);
	if (!upb.isNull())
	{
		//string line;
		//istringstream istr(string(upb), ios_base::out);
		//ostringstream ostr;
		//while (!istr.eof()) 
		//{
		//	std::getline(istr, line);
		//	if (!line.empty() && 0 != line.compare(0, 3, "np_"))
		//		ostr << line << endl;
		//}
		//if (!ostr.str().empty())
		//{
			//NiStringExtraDataRef strings = CreateNiObject<NiStringExtraData>();	
			//strings->SetName("UPB");
			//strings->SetData(ostr.str());
			//root->AddExtraData(DynamicCast<NiExtraData>(strings));
			//ok = true;
		//}
	}
	return ok;
}

void Exporter::progressUpdate(EProgressSection section, const TCHAR *s)
{
	//if (mSuppressPrompts)
	//	return;

	if (m_Interface->GetCancel())
	{
		return;
	}

	int total = 1;
	int percent = 1;
	for (int i=0; i< int(ProgressSectionCount); i++)
	{
		total += m_iprogressMax[i];
	
		if (i < section) 
		{
			percent += m_iprogressMax[i];
		} 
		else if (i == section )
		{
			percent += (++m_iprogressCounters[i]);
		}
	}
	m_Interface->ProgressUpdate( (percent * 100) / total , s == NULL ? TRUE : FALSE,  const_cast<TCHAR*>(s));
}

EzTriBasedGeom* Exporter::makeMesh(EzNode* parent, Mtl *mtl, EzGenerateMesh::FaceGroup* pFG, bool exportStrips)
{
	EzTriBasedGeom* shape;
	EzTriBasedGeomData* data;


	if (exportStrips) 
	{
	//	shape = new EzTriStrips();
////		data = new NiTriStripsData(grp.faces, !mUseAlternateStripper);
	} 
	else
	{
		shape = EzNew EzTriShape();
		data = EzNew EzTriShapeData();
	}

	int iNumVertex = pFG->vgrp.size();

	EzPoint3* pVertices = EzNew EzPoint3[iNumVertex];
	EzPoint3* pNorms = EzNew EzPoint3[iNumVertex];

	for(unsigned int n = 0; n < iNumVertex; ++n)
	{
		EzMemoryCopy(&pVertices[n], &pFG->vgrp[n].pt, sizeof(EzPoint3));
		EzMemoryCopy(&pNorms[n], &pFG->vgrp[n].norm, sizeof(EzPoint3));
	}

	data->setNumVertices(iNumVertex);
	data->setVertices(pVertices);
	data->setNormals(pNorms);


	unsigned short usNumTri = pFG->Triangles.size();

	EzTriangle16* pTri = EzNew EzTriangle16[usNumTri];


	for(unsigned int n = 0; n < usNumTri; ++n)
	{
		EzMemoryCopy(&pTri[n], &pFG->Triangles[n], sizeof(EzTriangle16));
	}


	data->setNumTrianglePoints(usNumTri);
	data->setTriangle(pTri);
	data->updateBound();

	for(int idx = 0; idx < pFG->uvMapping.size(); ++idx)
	{
		EzPoint2* pUV = EzNew EzPoint2[iNumVertex];

		for(unsigned int n = 0; n < iNumVertex; ++n)
		{
			EzMemoryCopy(&pUV[n], &pFG->vgrp[n].uvs[idx], sizeof(EzPoint2));
		}
	
		data->addUVSet(pUV, pFG->uvMapping[idx]);

		EzDelete pUV;
	}


	if (m_VertexColors)
	{
		bool allWhite = true;
		EzColor white(1.0f, 1.0f, 1.0f, 1.0f);

		EzColor* pVColor = new EzColor[iNumVertex];

		for (int i = 0; i < iNumVertex; ++i) 
		{
			if (white != pFG->vgrp[i].color) 
			{
				allWhite = false; 
				*pVColor = pFG->vgrp[i].color;
				break;
			}
		}

		if (!allWhite)
		{
			data->setVertexColors(pVColor);
		}
		else
		{
			EzDelete pVColor;
		}
	}

	data->setHasVertexColors(m_VertexColors ? true : false);

	shape->setData(data);
//
//	//if (Exporter::mTangentAndBinormalExtraData && (Exporter::mNifVersionInt > VER_4_2_2_0))
//	//{
//	//	// enable traditional tangents and binormals for non-oblivion meshes
//	//	if ( !IsOblivion() && (Exporter::mNifVersionInt >= VER_10_0_1_0) )
//	//		data->SetTspaceFlag( 0x10 );
//	//	shape->UpdateTangentSpace(Exporter::mTangentAndBinormalMethod);
//	//}
//

	EzAVObject* pAVObject = EzDynamicCast(EzAVObject, shape);

	parent->addChild(pAVObject);

	makeMaterial(pAVObject, mtl);

	return shape;
}

Exporter::Result Exporter::exportMesh(EzNode* eznode, INode *node, TimeValue t)
{
	ObjectState os = node->EvalWorldState(t);

	bool local = !m_FlattenHierarchy;

	TriObject *tri = (TriObject *)os.obj->ConvertToType(t, Class_ID(TRIOBJ_CLASS_ID, 0));
	if (!tri)
		return Skip;

	Mesh *copymesh = NULL;
	Mesh *mesh = &tri->GetMesh();

	Matrix3 mtx(true), rtx(true);

	if (m_CollapseTransforms)
	{
		mtx = getNodeLocalTM(node, t);
		mtx.NoTrans();
		Quat q(mtx);
		q.MakeMatrix(rtx);
		mesh = copymesh = new Mesh(*mesh);
		{
			int n = mesh->getNumVerts();

			for ( int i = 0; i < n; ++i ) 
			{
				Point3& vert = mesh->getVert(i);
				vert = mtx * vert;
			}
			mesh->checkNormals(TRUE);

#if VERSION_3DSMAX > ((5100<<16)+(8<<8)+0) // Version 6+
			MeshNormalSpec *specNorms = mesh->GetSpecifiedNormals ();
			if (NULL != specNorms) {
				specNorms->CheckNormals();
				for ( unsigned int i = 0; i < specNorms->GetNumNormals(); ++i )
				{
					Point3& norm = specNorms->Normal(i);
					norm = (rtx * norm).Normalize();
				}
			}
#endif
		}
	}

	// Note that calling setVCDisplayData will clear things like normals so we set this up first
	EzGenerateMesh* pEzMesh = EzNew EzGenerateMesh;

	if (m_VertexColors)
	{
		pEzMesh->createVertexColors(mesh);
	}

#if VERSION_3DSMAX <= ((5100<<16)+(8<<8)+0) // Version 5
	mesh->checkNormals(TRUE);
#else
	MeshNormalSpec *specNorms = mesh->GetSpecifiedNormals ();
	if (NULL != specNorms) 
	{
		specNorms->CheckNormals();
		if (specNorms->GetNumNormals() == 0)
		{
			mesh->checkNormals(TRUE);
		}
	} 
	else
	{
		mesh->checkNormals(TRUE);
	}
#endif

	Result result = Ok;

	//Modifier* geomMorpherMod = GetMorpherModifier(node);
	bool noSplit = FALSE;
	//	bool noSplit = (NULL != geomMorpherMod);

	pEzMesh->createMesh(node, *mesh, t, noSplit);


	bool exportStrips = m_TriStrips ? true : false;

	EzTransform trans;

	//Matrix44 tm = Matrix44::IDENTITY;

	//if ( mExportExtraNodes || (mExportType != NIF_WO_ANIM && isNodeKeyed(node) ) ) 
	//{
	//	tm = TOMATRIX4(getObjectTransform(node, t, false) * Inverse(getNodeTransform(node, t, false)));
	//} else 
	//{
		objectTransform(trans, node, t, local);
	//}
	//tm = TOMATRIX4(Inverse(mtx)) * tm;

	TSTR basename = node->NodeName();
	TSTR format = (!basename.isNull() && pEzMesh->getNumMesh() > 1) ? "%s:%d" : "%s";

	EzGenerateMesh::MeshObjects::Iterator  it =	pEzMesh->m_MeshGroup.getIterator();
	int i = 1;

	while(!it.atEnd())
	{
		EzStringc name = FormatString(format, basename.data(), i);
		EzTriBasedGeom* shape = makeMesh(eznode, getMaterial(node, it.getNode()->getKey()), it.getNode()->getValue(), exportStrips);

		if (shape == NULL)
		{
			result = Error;
			break;
		}

		if (node->IsHidden())
		{
			shape->setVisibility(false);
		}

		shape->setName(name.c_str());
		shape->setPosition(trans.getTranslation());
		shape->setRotation(trans.getToMatrixRotation());

		//if (Exporter::mZeroTransforms)
		//{
		//	shape->ApplyTransforms();
		//}

		//makeSkin(shape, node, grp->second, t);

		//if (geomMorpherMod) {
		//	vector<Vector3> verts = shape->GetData()->GetVertices();
		//	exportGeomMorpherControl(geomMorpherMod, verts, shape->GetData()->GetVertexIndices(), shape);
		//	shape->GetData()->SetConsistencyFlags(CT_VOLATILE);
		//}


		it++;
		i++;
	}

	if (tri != os.obj)
		tri->DeleteMe();

	if (copymesh)
		delete copymesh;

	return result;
}


// Primary recursive decent routine
Exporter::Result Exporter::exportNodes(EzNode* parent, INode *node)
{
	TSTR nodeName = node->GetName();
	progressUpdate(Geometry, FormatText("'%s' Geometry", nodeName.data()));

	//bool coll = isCollision(node);
	//// Abort if is a collision node or is hidden and we are not exporting hidden
	//if (coll ||	(node->IsHidden() && !mExportHidden) || isHandled(node))
	//	return Skip;

	int local = !m_FlattenHierarchy;
	EzNode* nodeParent = local ? m_pEzRoot : parent;

	EzNode* pNewParent;
	TimeValue t = 0;
	ObjectState os = node->EvalWorldState(t);

	// Always skip bones and bipeds
	SClass_ID scid = node->SuperClassID();
	Class_ID ncid = node->ClassID();
	TSTR nodeClass; node->GetClassName(nodeClass);
	
	if (node->IsBoneShowing())
	{
	//	pNewParent = exportBone(nodeParent, node);
	}
	else if (os.obj && os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID)
	{
		//TSTR objClass;
		//os.obj->GetClassName(objClass);
		SClass_ID oscid = os.obj->SuperClassID();
		Class_ID oncid = os.obj->ClassID();
		if (wildmatch("Bip?? Footsteps", nodeName))
		{
			// ignore footsteps
		}
		else if (  os.obj			
			&& (  os.obj->ClassID() == BONE_OBJ_CLASSID 
			|| os.obj->ClassID() == Class_ID(BONE_CLASS_ID,0)
			|| os.obj->ClassID() == Class_ID(0x00009125,0) /* Biped Twist Helpers */
			)
			) 
		{
	//		newParent = exportBone(nodeParent, node);
		} 
		else if (!m_SkeletonOnly) // Export mesh
		{
			//if (mExportType != NIF_WO_ANIM && isNodeTracked(node)) 
			//{
			//	// Create Node + Accum if has Start Track
			//	newParent = createAccumNode( makeNode(nodeParent, node, local) , node);
			//} else if ( mExportExtraNodes || (mExportType != NIF_WO_ANIM && isNodeKeyed(node) ) ) {
			//	// Create node if using Extra Nodes or if exporting with anim and node has key values
			//	newParent = makeNode(nodeParent, node, local);
			//} else {
			//	// Else don't create a node
				pNewParent = nodeParent;
			//}
			//// No need to export meshes when NIF is not exported.
			//if (mExportType != SINGLE_KF_WO_NIF && mExportType != MULTI_KF_WO_NIF)
			//{
				Result result = exportMesh(pNewParent, node, t);
				if (result != Ok)
					return result;
			//}
		}
	}
	//else if (mExportCameras && os.obj && os.obj->SuperClassID()==CAMERA_CLASS_ID)
	//{
	//	newParent = makeNode(nodeParent, node, local);
	//}
	//else if (mExportLights && os.obj && os.obj->SuperClassID()==LIGHT_CLASS_ID)
	//{
	//	return exportLight(nodeParent, node, (GenLight*)os.obj);
	//}
	//else if (isMeshGroup(node) && local && !mSkeletonOnly) // only create node if local
	//{
	else if (isMeshGroup(node) && local && !m_SkeletonOnly) // only create node if local
	{
		pNewParent = makeNode(parent, node, local);
	} 
	else
		pNewParent = parent;

	for (int i=0; i<node->NumberOfChildren(); i++) 
	{
		Result result = exportNodes(pNewParent, node->GetChildNode(i));
		if (result!=Ok && result!=Skip)
			return result;
	}
	return Ok;
}

Exporter::Result Exporter::doExport(EzNode *pRoot, INode *node)
{

	pRoot->setName("Scene Root");

	if (!m_SelectedOnly)
	{

	}

	m_pEzRoot = pRoot;

	if (m_SelectedOnly)
	{
		int count = 0;
		int n = m_Interface->GetSelNodeCount();
		EzArray<INode*> selectedRoots;

		for (int i=0; i<n; ++i) 
		{
			INode * selNode = m_Interface->GetSelNode(i);
			
			if (!isNodeOrParentSelected(selNode->GetParentNode())) 
			{
				selectedRoots.push_back(selNode);
				count += countNodes(selNode);
			}
		}
	
		if (selectedRoots.size() == 0) 
		{
			return Error;
		}

		m_iprogressMax[Geometry] = m_iprogressMax[Skin] = count;
		
		if (m_ExportCollision)
		{
			m_iprogressMax[Collision] = m_iprogressMax[Geometry];
		}
	
	//	//if (mExportType == NIF_WO_ANIM)
	//	//	m_iprogressMax[Animation] = m_iprogressMax[Geometry];

		for (size_t i=0; i<selectedRoots.size(); i++)
		{
			Result result = exportNodes(pRoot, selectedRoots[i]);
			
			if (result != Ok && result != Skip)
				return result;

			if (m_ExportCollision) 
			{
				//result = exportCollision(root, selectedRoots[i]);
				//if (result != Ok)
				//	return result;
			}
		}
		// Always Zero out root transforms
		EzList<EzAVObjectPtr> children = pRoot->getChildren();
		
		//for (size_t i=0; i<children.size(); ++i)
		//{
		//	children[i]->SetLocalTransform(Matrix44::IDENTITY);
		//}

		//// Fix Used Nodes that were never properly initialized.  Happens normally during select export
		//for (NodeMap::iterator itr = mNameMap.begin(); itr != mNameMap.end(); ++itr) {
		//	NiNodeRef bone = (*itr).second;
		//	if (bone->GetParent() == NULL) {
		//		if (INode* boneNode = mI->GetINodeByName((*itr).first.c_str())) {
		//			makeNode(root, boneNode, false);
		//		}
		//	}
		//}

		//// Special case when exporting a single branch, use first child as scene root
		//if (selectedRoots.size() == 1 ) {
		//	vector<NiNodeRef> childnodes = DynamicCast<NiNode>(root->GetChildren());
		//	if (childnodes.size() == 1) {
		//		NiNodeRef child = childnodes[0];
		//		root->RemoveChild(child);
		//		root = child;
		//		mNiRoot = root;
		//		exportPrn(root, selectedRoots[0]);
		//	}
		//}
	}
	else
	{
		int count = countNodes(node);
		
		m_iprogressMax[Geometry] = m_iprogressMax[Skin] = count;

		if (m_ExportCollision)
			m_iprogressMax[Collision] = m_iprogressMax[Geometry];
		
		//if (mExportType == NIF_WO_ANIM)
		//	m_iprogressMax[Animation] = m_iprogressMax[Geometry];


		// Normal export
		Result result = exportNodes(pRoot, node);
		if (result != Ok)
			return result;


	}



	return Ok;


}
