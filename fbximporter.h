#ifndef FBXIMPORTER_H
#define FBXIMPORTER_H

#include <fbxsdk.h>
#include <string>
#include <unordered_map>

using namespace std;
class CFbxImporter
{
private:
    FbxManager*                     pManager = nullptr;
    FbxScene*                       pScene = nullptr;
    unordered_map<string, FbxNode*> mapBones;
    void                            ExtractBone(FbxNode* pNode);
public:
    CFbxImporter();
    bool                            InitSdk();
    bool                            LoadFbx(const string& pFilename);
};

#endif // FBXIMPORTER_H
