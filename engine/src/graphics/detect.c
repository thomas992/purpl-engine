// GPU identification
//
// Copyright 2022 MobSlicer152
// This file is part of Purpl Engine
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

#include "purpl/graphics/detect.h"

PURPL_API bool purpl_check_igpu(bgfx_caps_t *info)
{
	if (!info)
		return false;

#ifdef PURPL_X86
#ifdef _MSC_VER
#else // _MSC_VER
	u32 eax;
	u32 ebx;
	u32 ecx;
	u32 edx;

	__get_cpuid(1, &eax, &ebx, &ecx, &edx);
	if (eax == 0x0306C3 && info->numGPUs == 1 && info->gpu[0].vendorId == BGFX_PCI_ID_INTEL)
		return true;
#endif // _MSC_VER
#endif // PURPL_X86

	return false;
}
