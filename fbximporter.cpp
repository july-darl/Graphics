#include "fbximporter.h"
#include <QDebug>

CFbxImporter::CFbxImporter()
{
    InitSdk();
}

bool CFbxImporter::InitSdk()
{
    pManager = FbxManager::Create();
    if( !pManager )
    {
        return false;
    }

    FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
    pManager->SetIOSettings(ios);

    FbxString lPath = FbxGetApplicationDirectory();
    pManager->LoadPluginsDirectory(lPath.Buffer());

    pScene = FbxScene::Create(pManager, "My Scene");
    if( !pScene )
    {
        return false;
    }
    qDebug() << "success init sdk " ;
    return true;
}

void CFbxImporter::ExtractBone(FbxNode* pNode)
{;
    FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
    if (pNodeAttribute && pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
    {
        const string& strBone = pNodeAttribute->GetName();
        mapBones[strBone] = pNode;
        qDebug() << QString::fromStdString(strBone);
    }
    for(auto i = 0; i < pNode->GetChildCount(); i++)
    {
        FbxNode* pChildNode = pNode->GetChild(i);
        ExtractBone(pChildNode);
    }
}

bool CFbxImporter::LoadFbx(const string& pFilename)
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;

    // Get the file version number generate by the FBX SDK.
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    qDebug() << lSDKMajor << " " << lSDKMinor << " " << lSDKRevision;
    // Create an importer.
    FbxImporter* lImporter = FbxImporter::Create(pManager,"");

    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename.c_str(), -1, pManager->GetIOSettings());
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if( !lImportStatus )
    {
        FbxString error = lImporter->GetStatus().GetErrorString();
        qDebug() << "load false";
        return false;
    }

    // Import the scene.
    if(!lImporter->Import(pScene))
    {
        lImporter->Destroy();
        return false;
    }


    lImporter->Destroy();

    ExtractBone(pScene->GetRootNode());

    FbxAnimStack* pAnimStack = pScene->GetCurrentAnimationStack();
  //  FbxAnimLayer* pLayer = pAnimStack->GetMember<FbxAnimLayer>(0);

    for (auto& bones : mapBones)
    {
        FbxNode* pNode = bones.second;
       // unsigned int frameCount = 0;
        FbxTimeSpan interval;   //每帧时间间隔
        bool bResult = pNode->GetAnimationInterval(interval);
        if (!bResult)
        {
            return false;
        }
        FbxTime timePerFrame;
        timePerFrame.SetTime(0, 0, 0, 1, 0, pScene->GetGlobalSettings().GetTimeMode());

        const FbxTimeSpan animTimeSpan = pAnimStack->GetLocalTimeSpan();
        const FbxTime startTime = animTimeSpan.GetStart();
        const FbxTime endTime = animTimeSpan.GetStop();
        char lTimeString[1000];
        int numFrames = 0;
        for (FbxTime time = startTime; time <= endTime; time += timePerFrame, ++numFrames)
        {
            FbxAMatrix kMatGlobal = pNode->EvaluateGlobalTransform(time);//transform of bone in world space at time t
            auto currentTime = atoi(time.GetTimeString(lTimeString, FbxUShort(256)));

         //   m_pAnimation->AddFrame(boneIndex, pNode->GetName(), currentTime, d3dMat);
        }

    }

    return true;
}
