#pragma once

#include <vector>
#include <filesystem>
#include "garden/graphics/buffer.hpp"

#define WIN32_LEAN_AND_MEAN

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

		void destroy();
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
	};
}

