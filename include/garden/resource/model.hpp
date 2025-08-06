#pragma once

#include <assimp/scene.h>

#include <vector>
#include <filesystem>
#include "garden/graphics/buffer.hpp"

using namespace ecsm;
using namespace garden::graphics;

namespace garden
{
	struct Vertex
	{
		public:
		float3 position;
		float3 normal;
		float2 uv;
	};

	struct Mesh
	{
		public:
		ID<Buffer> vertices;
		ID<Buffer> indices;
	};

	struct Model
	{
	public:
		vector<Mesh> lods;
		//TODO Add materials

		Mesh*lod(int &level);
		void addLod(std::filesystem::path &path);
		void addLods(std::filesystem::path *path, int size);
		void destroy();
	private:
		static Mesh loadMesh(std::filesystem::path &path);
		static const aiScene *loadScene(std::filesystem::path &path);
	};
}

