/*
 * Copyright (c) 2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* This file is auto-generated. !!! DO NOT EDIT !!! */

#include <asm.h>
#include <trusty_syscalls.h>

.macro  PROLOG
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
.endm

.macro  MOV_PARAMS
   /* Return Addr and stack*/
   leaq 1f(%rip), %rbx
   movq %rsp, %rbp
.endm

.macro EPILOG
1:  popq %r15
    popq %rbx
    popq %rbp
    popfq
.endm

FUNCTION(write)
    PROLOG
    movq $__NR_write, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(brk)
    PROLOG
    movq $__NR_brk, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(exit_group)
    PROLOG
    movq $__NR_exit_group, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(read)
    PROLOG
    movq $__NR_read, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(ioctl)
    PROLOG
    movq $__NR_ioctl, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

/*
 * Third parameter of nanosleep is uint64_t sleep_time,
 * while kernel level sys_nanosleep accept uint32_t sleep_time_l,
 * and uint32_t sleep_time_h. In 64-bit TA solution, need to divide
 * value of RDX(third parameter of nanosleep) into 2 part while
 * invoking syscall: set value of RCX as high 32 bit value of RDX,
 * keep RDX low 32 bit value of RDX, and zero high 32 bit of RDX.
 */
FUNCTION(nanosleep)
    pushq %rcx
    PROLOG
    movq %rdx, %rcx
    shr $32, %rcx
    /* clear high 32 bit of RDX */
    movl %edx, %edx
    movq $__NR_nanosleep, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    popq %rcx
    ret

FUNCTION(gettime)
    PROLOG
    movq $__NR_gettime, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(mmap)
    PROLOG
    movq $__NR_mmap, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(munmap)
    PROLOG
    movq $__NR_munmap, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(prepare_dma)
    PROLOG
    movq $__NR_prepare_dma, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(finish_dma)
    PROLOG
    movq $__NR_finish_dma, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(get_device_info)
    PROLOG
    movq $__NR_get_device_info, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(port_create)
    PROLOG
    movq $__NR_port_create, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(connect)
    PROLOG
    movq $__NR_connect, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(accept)
    PROLOG
    movq $__NR_accept, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(close)
    PROLOG
    movq $__NR_close, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(set_cookie)
    PROLOG
    movq $__NR_set_cookie, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(wait)
    PROLOG
    movq $__NR_wait, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(wait_any)
    PROLOG
    movq $__NR_wait_any, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(get_msg)
    PROLOG
    movq $__NR_get_msg, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(read_msg)
    PROLOG
    movq $__NR_read_msg, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(put_msg)
    PROLOG
    movq $__NR_put_msg, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(send_msg)
    PROLOG
    movq $__NR_send_msg, %rax
    MOV_PARAMS
    sysenter
    EPILOG
    ret
