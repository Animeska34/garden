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
 * @brief Common graphics resource functions.
 */

#pragma once
#include "garden/defines.hpp"

namespace garden::graphics
{

class ResourceExt;

/***********************************************************************************************************************
 * @brief Graphics resource type.
 */
enum class ResourceType : uint8
{
	Buffer,  Image, ImageView, Framebuffer, Sampler, Blas, Tlas, 
	GraphicsPipeline, ComputePipeline, RayTracingPipeline, DescriptorSet, Count
};

/**
 * @brief Graphics resource base class.
 * 
 * @details 
 * Various types of objects that represent GPU data used in rendering and computation tasks. Resources allow 
 * applications to define, store, and manipulate the data necessary for graphics rendering and compute operations.
 */
class Resource
{
protected:
	void* instance = nullptr;
	uint32 busyLock = 0;
	
	#if GARDEN_DEBUG || GARDEN_EDITOR
	#define UNNAMED_RESOURCE "unnamed"
	string debugName = UNNAMED_RESOURCE;
	#endif

	virtual bool destroy() = 0;
	friend class ResourceExt;
public:
	/*******************************************************************************************************************
	 * @brief Creates a new empty resource data container.
	 * @note Use @ref GraphicsSystem to create, destroy and access graphics resources.
	 */
	Resource() = default;

	/**
	 * @brief Is resource fully ready for graphics rendering.
	 * @details Graphics resource is loaded and transferred.
	 */
	bool isReady() const noexcept { return instance && busyLock < 1; }
	/**
	 * @brief Is resource loaded. (Does not include ongoing transfers!)
	 * @warning This doesn't takes into account if resource is fully transferred!
	 */
	bool isLoaded() const noexcept { return instance; }

	#if GARDEN_DEBUG || GARDEN_EDITOR
	/**
	 * @brief Returns resource debug name. (Debug Only)
	 * @details Also visible inside GPU profilers. (RenderDoc, Nsight, Xcode...)
	 */
	const string& getDebugName() const noexcept { return debugName; }
	/**
	 * @brief Sets resource debug name. (Debug Only)
	 * @param[in] name debug resource name
	 */
	virtual void setDebugName(const string& name)
	{
		GARDEN_ASSERT(!name.empty());
		debugName = name;
	}
	#endif
};

/**
 * @brief Graphics resource extension mechanism.
 * @warning Use only if you know what you are doing!
 */
class ResourceExt final
{
public:
	/**
	 * @brief Returns resource native instance.
	 * @warning In most cases you should use @ref Resource functions.
	 * @param[in] resource target resource instance
	 */
	static void*& getInstance(Resource& resource) noexcept { return resource.instance; }
	/**
	 * @brief Returns resource busy lock.
	 * @warning In most cases you should use @ref Resource functions.
	 * @param[in] resource target resource instance
	 */
	static uint32& getBusyLock(Resource& resource) noexcept { return resource.busyLock; }

	#if GARDEN_DEBUG || GARDEN_EDITOR
	/**
	 * @brief Returns resource debug name.
	 * @warning In most cases you should use @ref Resource functions.
	 * @param[in] resource target resource instance
	 */
	static string& getDebugName(Resource& resource) noexcept { return resource.debugName; }
	#endif
};

} // namespace garden::graphics