// Definitions of the functions in purpl/util/string.h
//
// Copyright 2021 MobSlicer152
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

#include "purpl/util/string.h"

PURPL_API char *purpl_strrplc(char *dst, const char *src, const char *old, const char *new)
{
	return purpl_strnrplc(dst, src, strlen(src) + 1, old, new);
}

PURPL_API char *purpl_strnrplc(char *dst, const char *src, size_t n, const char *old, const char *new)
{
	char *tmp;
	const char *p;

	tmp = calloc(n, sizeof(char));
	if (!tmp) {
		strncpy(dst, src, n);
		return dst;
	}

	strncpy(dst, src, n);
	p = dst;

	while (p = strstr(p, old)) {
		strncpy(tmp, dst, p - dst);
		tmp[p - dst] = 0;
		strncat(tmp, new, n);
		strncat(tmp, p + strlen(old), n);
		strncpy(dst, tmp, n);
	}

	free(tmp);

	return dst;
}
