// Functions for using NTDLL (DbgPrint and some other ones)
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

#include "purpl/util/private/win/ntdll.h"

void *ntdll_base;

u32 (*DbgPrint_l)(const char *fmt, ...) = 0;

void purpl_load_ntdll(void)
{
	PEB *peb;
	PEB_LDR_DATA *ldr;
	LIST_ENTRY *head;
	LIST_ENTRY *cur;
	LDR_DATA_TABLE_ENTRY *cur_dll = NULL;

	// NTDLL is _always_ loaded in normal processes, find it
	peb = NtCurrentPeb();
	ldr = peb->Ldr;
	head = &ldr->InLoadOrderModuleList;
	for (cur = head->Flink; cur != head; cur = cur->Flink) {
		cur_dll = CONTAINING_RECORD(cur, LDR_DATA_TABLE_ENTRY,
					    InLoadOrderLinks);
		if (purpl_wcscasecmp(cur_dll->FullDllName.Buffer,
				     L"C:\\Windows\\System32\\ntdll.dll") == 0)
			break;
	}

	// Should be impossible
	if (!cur_dll)
		return;

	ntdll_base = cur_dll->DllBase;

	DbgPrint_l = GetProcAddress(ntdll_base, "DbgPrint");
}

void *purpl_get_pe_symbol(void *base, const char *name)
{
	void *sym = NULL;

	if (!base)
		return NULL;



	return sym;
}
