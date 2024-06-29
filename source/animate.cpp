// Copyright 2022-2024 Nikita Fediuchin. All rights reserved.
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

#include "garden/animate.hpp"

using namespace garden;

bool Animation::destroy()
{
	for (const auto& keyframe : keyframes)
	{
		const auto& animatables = keyframe.second;
		for (const auto& pair : animatables)
		{
			auto animatableSystem = dynamic_cast<IAnimatable*>(pair.first);
			animatableSystem->destroyAnimation(pair.second);
		}
	}
	return true;
}