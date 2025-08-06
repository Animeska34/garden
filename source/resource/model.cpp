#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/vector3.h>

#include "garden/resource/model.hpp"

#include <filesystem>
#include <vector>
#include "garden/graphics/buffer.hpp"
#include "garden/system/graphics.hpp"

using namespace ecsm;
using namespace garden;
using namespace garden::graphics;


const aiScene *Model::loadScene(std::filesystem::path &path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error(importer.GetErrorString());
    }
    return scene;
}

Mesh Model::loadMesh(std::filesystem::path &path)
{

    const aiScene *scene = Model::loadScene(path);

    unsigned int vCount = 0;
    unsigned int iCount = 0;

    // Gettings size of buffers
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh *aiMesh = scene->mMeshes[i];

        vCount += aiMesh->mNumVertices;

        // Increasing size of indices buffer
        //  Because aiProcess_Triangulate is used during import, all faces are triangles, so there is no need to process each face separately.
        iCount += aiMesh->mNumFaces * 3;

        // This is indices buffer size calculation for non triangulated mesh.
        // for (unsigned int f = 0; f < aiMesh->mNumFaces; ++f)
        // {
        // 	const aiFace &face = aiMesh->mFaces[f];
        // 	indicesCount += face.mNumIndices;
        // }
    }

    auto gs = GraphicsSystem::Instance::get();

    unsigned int vSize = sizeof(Vertex) * vCount;
    unsigned int iSize = sizeof(unsigned int) * iCount;

    ID<Buffer> vStage = gs->createBuffer(Buffer::Usage::TransferSrc, Buffer::CpuAccess::SequentialWrite, vSize, Buffer::Location::Auto, Buffer::Strategy::Speed);
    ID<Buffer> iStage = gs->createBuffer(Buffer::Usage::TransferSrc, Buffer::CpuAccess::SequentialWrite, iSize, Buffer::Location::Auto, Buffer::Strategy::Speed);

    View<Buffer> vView = gs->get(vStage);
    View<Buffer> iView = gs->get(iStage);

    auto vMap = (Vertex *)vView->getMap();
    auto iMap = (unsigned int *)iView->getMap();

    unsigned int vOffset = 0;
    unsigned int fOffset = 0;

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
    {
        aiMesh *aiMesh = scene->mMeshes[m];

        for (unsigned int v = 0; v < aiMesh->mNumVertices; ++v)
        {
            unsigned int pos = vOffset + v;
            Vertex vertex;

            // Writing vertices
            auto av = aiMesh->mVertices[v];
            vertex.position = float3(av.x, av.y, av.z);

            // TODO Impl multiple UV Channel support

            // Writing UV0
            auto uv = aiMesh->HasTextureCoords(0) ? aiMesh->mTextureCoords[0][v] : aiVector3d(0, 0, 0);
            vertex.uv = float2(uv.x, uv.y);

            auto norm = aiMesh->HasNormals() ? aiMesh->mNormals[v] : aiVector3D(0, 0, 0);
            vertex.normal = float3(norm.x, norm.y, norm.z);

            vMap[pos] = vertex;
        }
        vOffset += aiMesh->mNumVertices;

        // Writing indices (Triangulated Mesh)
        for (unsigned int f = 0; f < aiMesh->mNumFaces; ++f)
        {
            unsigned int pos = fOffset + f;
            auto indexPos = pos * 3;
            auto face = aiMesh->mFaces[f];
            iMap[indexPos] = face.mIndices[0];
            iMap[indexPos + 1] = face.mIndices[1];
            iMap[indexPos + 2] = face.mIndices[2];
        }
        fOffset += aiMesh->mNumFaces;
    }

    Mesh mesh;

    mesh.vertices = gs->createBuffer(Buffer::Usage::Vertex, Buffer::CpuAccess::None, vSize, Buffer::Location::PreferGPU, Buffer::Strategy::Size);
    mesh.indices = gs->createBuffer(Buffer::Usage::Index, Buffer::CpuAccess::None, iSize, Buffer::Location::PreferGPU, Buffer::Strategy::Size);

    Buffer::copy(vStage, mesh.vertices);
    Buffer::copy(iStage, mesh.indices);

    gs->destroy(iStage);
    gs->destroy(vStage);

    return mesh;
}

Mesh* Model::lod(int&level)
{
    if (level < lods.size())
        return &lods[level];
    return &lods[0];
}

void Model::addLod(std::filesystem::path &path)
{
    Mesh mesh = Model::loadMesh(path);
    this->lods.push_back(mesh);
}

void Model::addLods(std::filesystem::path *path, int size)
{
    for (int i = 0; i < size; ++i)
    {
        Mesh mesh = Model::loadMesh(path[i]);
        this->lods.push_back(mesh);
    }
}
