//  Unix-like RNG using RDSEED
//
//  Copyright 2022 MobSlicer152
//  This file is part of Purpl Engine
//
//  Licensed under the Apache License, Version 2.0 (the "License")//
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

.extern rand

.section .text

.global purpl_arch_random
purpl_arch_random:
	// Check for RDSEED
	movl $7, %eax
	xor %ecx, %ecx
	cpuid
	shrq $18, %rbx
	andl $1, %ebx
	jz .fail

	movl $100, %ecx // Number of retries
	.retry:
		rdseedq %rax
		jc .done // Carry flag set on success
		jmp .retry
	.fail:
		// Call rand
		call rand
	.done:
		ret

