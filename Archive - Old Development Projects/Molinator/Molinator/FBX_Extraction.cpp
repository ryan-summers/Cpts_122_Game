#include "Molinator.h"

HRESULT loadFBX (vector<Vertex> *pVertexVector, char *pFileName) {
	FbxManager *manager = NULL;

	/*Initialize the FBXManager*/
	if (manager == nullptr) { //create the new manager
		manager = FbxManager::Create();
		FbxIOSettings* pIOsettings = FbxIOSettings::Create(manager, IOSROOT );
		manager->SetIOSettings(pIOsettings);
	}

	FbxImporter* pImporter = FbxImporter::Create(manager,""); //create a new importer
    FbxScene* pFbxScene = FbxScene::Create(manager,""); //create a new scene

	bool bSuccess = pImporter->Initialize(pFileName, -1, manager->GetIOSettings() ); //initialize the importer with the FBX file
	if(!bSuccess) return E_FAIL;

	bSuccess = pImporter->Import(pFbxScene); //import the fbx file into the fbx scene
	if(!bSuccess) return E_FAIL;

	pImporter->Destroy(); //destroy importer

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode(); //get the root node for the object and start gathering info

   if(pFbxRootNode) //if a nod was found, we can then start reading in data for all nodes
   {
      for(int i = 0; i < pFbxRootNode->GetChildCount(); i++) //loop through each node contained in the FBX file
      {
         FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i); //loop through all of the objects within the scene
         if(pFbxChildNode->GetNodeAttribute() == NULL)
            continue;

         FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

         if(AttributeType != FbxNodeAttribute::eMesh) //make sure the data we are reading is mesh data
            continue;

         FbxMesh* pMesh = (FbxMesh*) pFbxChildNode->GetNodeAttribute();

         FbxVector4* pVertices = pMesh->GetControlPoints();

         for (int j = 0; j < pMesh->GetPolygonCount(); j++) //loop through each polygon within the object
         {
			 //make sure we are using triangles here
            int iNumVertices = pMesh->GetPolygonSize(j);
            if (iNumVertices != 3) //ensure we are dealing with triangulated objects! we can only draw triangles
				return -1;

            for (int k = 0; k < iNumVertices; k++) //loop through each vertice of the polygon
            {
               int iControlPointIndex = pMesh->GetPolygonVertex(j, k);

               Vertex vertex;
               vertex.x = (float)pVertices[iControlPointIndex].mData[0];
               vertex.y = (float)pVertices[iControlPointIndex].mData[1];
               vertex.z = (float)pVertices[iControlPointIndex].mData[2];
			   vertex.tx = (float)j/pMesh->GetPolygonCount();
			   vertex.ty = (float)j/pMesh->GetPolygonCount();
               pVertexVector->push_back( vertex );
            }
         }

      }

   }

   return S_OK;
}