// Copyright 2022-2025 Nikita Fediuchin. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/***********************************************************************************************************************
 * @file
 * @brief Bindless graphics resource pool functions.
 */

#pragma once
#include "garden/graphics/pipeline/compute.hpp"
#include "garden/graphics/pipeline/graphics.hpp"
#include "garden/graphics/pipeline/ray-tracing.hpp"

namespace garden::graphics
{

/**
 * @brief Bindless graphics resource linear pool. 
 */
class BindlessPool final
{
	using FreeAllocs = vector<pair<uint32, uint64>>;

	struct UniformData final
	{
		FreeAllocs freeAllocs;
		uint32 occupancy = 0;
		uint32 updateBeginIndex = UINT32_MAX;
		uint32 updateEndIndex = 0;
	};
	
	tsl::robin_map<string, UniformData, SvHash, SvEqual> uniformData;
	ID<DescriptorSet> descriptorSet = {};

	uint32 allocate(string_view name, ID<Resource> resource, uint64 frameIndex);
public:
	/**
	 * @brief Creates a new bindless pool instance.
	 *
	 * @param pipeline target pipeline instance
	 * @param pipelineType pipeline instance type
	 * @param[in] uniforms shader uniform array
	 * @param[in] samplers dynamic sampler array (mutable uniforms)
	 * @param index index of descriptor set in the shader
	 */
	BindlessPool(ID<Pipeline> pipeline, PipelineType pipelineType, 
		DescriptorSet::Uniforms&& uniforms = {}, DescriptorSet::Samplers&& samplers = {}, uint8 index = 0);
	/**
	 * @brief Creates a new graphics bindless pool instance.
	 *
	 * @param graphicsPipeline target graphics pipeline
	 * @param[in] uniforms shader uniform array
	 * @param[in] samplers dynamic sampler array (mutable uniforms)
	 * @param index index of descriptor set in the shader
	 */
	BindlessPool(ID<GraphicsPipeline> graphicsPipeline, DescriptorSet::Uniforms&& uniforms = {}, 
		DescriptorSet::Samplers&& samplers = {}, uint8 index = 0) : BindlessPool(ID<Pipeline>(graphicsPipeline), 
		PipelineType::Graphics, std::move(uniforms), std::move(samplers), index) { }
	/**
	 * @brief Creates a new compute bindless pool instance.
	 *
	 * @param graphicsPipeline target compute pipeline
	 * @param[in] uniforms shader uniform array
	 * @param[in] samplers dynamic sampler array (mutable uniforms)
	 * @param index index of descriptor set in the shader
	 */
	BindlessPool(ID<ComputePipeline> computePipeline, DescriptorSet::Uniforms&& uniforms = {}, 
		DescriptorSet::Samplers&& samplers = {}, uint8 index = 0) : BindlessPool(ID<Pipeline>(computePipeline), 
		PipelineType::Compute, std::move(uniforms), std::move(samplers), index) { }
	/**
	 * @brief Creates a new ray tracing bindless pool instance.
	 *
	 * @param rayTracingPipeline target ray tracing pipeline
	 * @param[in] uniforms shader uniform array
	 * @param[in] samplers dynamic sampler array (mutable uniforms)
	 * @param index index of descriptor set in the shader
	 */
	BindlessPool(ID<RayTracingPipeline> rayTracingPipeline, DescriptorSet::Uniforms&& uniforms = {}, 
		DescriptorSet::Samplers&& samplers = {}, uint8 index = 0) : BindlessPool(ID<Pipeline>(rayTracingPipeline), 
		PipelineType::RayTracing, std::move(uniforms), std::move(samplers), index) { }

	/**
	 * @brief Creates a new empty bindless pool instance.
	 */
	BindlessPool() noexcept = default;

	/*******************************************************************************************************************
	 * @brief Returns bindless descriptor set instance.
	 */
	ID<DescriptorSet> getDescriptorSet() const noexcept { return descriptorSet; }

	/**
	 * @brief Allocates a new bindless descriptor set buffer.
	 *
	 * @param name target bindless uniform name
	 * @param buffer buffer to write into the descriptor set
	 * @param frameIndex current rendering frame index
	 */
	uint32 allocate(string_view name, ID<Buffer> buffer, uint64 frameIndex)
	{
		return allocate(name, ID<Resource>(buffer), frameIndex);
	}
	/**
	 * @brief Allocates a new bindless descriptor set image view.
	 *
	 * @param name target bindless uniform name
	 * @param imageView image view to write into the descriptor set
	 * @param frameIndex current rendering frame index
	 */
	uint32 allocate(string_view name, ID<ImageView> imageView, uint64 frameIndex)
	{
		return allocate(name, ID<Resource>(imageView), frameIndex);
	}
	/**
	 * @brief Allocates a new bindless descriptor set TLAS.
	 *
	 * @param name target bindless uniform name
	 * @param tlas TLAS to write into the descriptor set
	 * @param frameIndex current rendering frame index
	 */
	uint32 allocate(string_view name, ID<Tlas> tlas, uint64 frameIndex)
	{
		return allocate(name, ID<Resource>(tlas), frameIndex);
	}
	/**
	 * @brief Frees bindless descriptor set resource.
	 *
	 * @param name target bindless uniform name
	 * @param allocation allocated resource index
	 * @param frameIndex current rendering frame index
	 */
	void free(string_view name, uint32 allocation, uint64 frameIndex);

	/**
	 * @brief Updates pending bindless descriptor set writes.
	 */
	void update();
	/**
	 * @brief Destroys bindless descriptor set instance.
	 */
	void destroy();
};

} // namespace garden::graphics