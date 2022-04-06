;  Windows RNG using RDSEED
;
;  Copyright 2022 MobSlicer152
;  This file is part of Purpl Engine
;
;  Licensed under the Apache License, Version 2.0 (the "License");
;  you may not use this file except in compliance with the License.
;  You may obtain a copy of the License at
;
;      http://www.apache.org/licenses/LICENSE-2.0
;
;  Unless required by applicable law or agreed to in writing, software
;  distributed under the License is distributed on an "AS IS" BASIS,
;  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
;  See the License for the specific language governing permissions and
;  limitations under the License.


OPTION PROLOGUE:NONE

EXTERN rand

.code

PUBLIC purpl_arch_random
purpl_arch_random PROC
	; Check for RDSEED
	mov eax, 7
	mov ecx, 0
	cpuid
	shr rbx, 18
	and ebx, 1
	jz fail

	mov ecx, 100
	retry:
		rdseed rax
		jc done
		jmp retry
	fail:
		call rand
		ret
	done:
		ret
purpl_arch_random ENDP

END

