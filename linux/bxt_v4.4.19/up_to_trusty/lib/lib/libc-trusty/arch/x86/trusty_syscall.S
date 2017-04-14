/*
 * Copyright (c) 2015 Intel Corporation
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
    push %ebp
    mov %esp, %ebp
    pushf
    push %ebp
    push %esi
    push %edi
    push %ebx
.endm

.macro  MOV_PARAMS
   mov 0x08(%ebp),%edi
   mov 0x0c(%ebp),%esi
   mov 0x10(%ebp),%edx
   mov 0x14(%ebp),%ecx
   /* Return Addr and stack*/
   mov $1f, %ebx
   mov %esp, %ebp
.endm

.macro EPILOG
1:  pop %ebx
    pop %edi
    pop %esi
    pop %ebp
    popf
    pop %ebp
.endm

FUNCTION(write)
    PROLOG
    mov $__NR_write, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(brk)
    PROLOG
    mov $__NR_brk, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(exit_group)
    PROLOG
    mov $__NR_exit_group, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(read)
    PROLOG
    mov $__NR_read, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(ioctl)
    PROLOG
    mov $__NR_ioctl, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(nanosleep)
    PROLOG
    mov $__NR_nanosleep, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(gettime)
    PROLOG
    mov $__NR_gettime, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(mmap)
    PROLOG
    mov $__NR_mmap, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(munmap)
    PROLOG
    mov $__NR_munmap, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(prepare_dma)
    PROLOG
    mov $__NR_prepare_dma, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(finish_dma)
    PROLOG
    mov $__NR_finish_dma, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(get_device_info)
    PROLOG
    mov $__NR_get_device_info, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(port_create)
    PROLOG
    mov $__NR_port_create, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(connect)
    PROLOG
    mov $__NR_connect, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(accept)
    PROLOG
    mov $__NR_accept, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(close)
    PROLOG
    mov $__NR_close, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(set_cookie)
    PROLOG
    mov $__NR_set_cookie, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(wait)
    PROLOG
    mov $__NR_wait, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(wait_any)
    PROLOG
    mov $__NR_wait_any, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(get_msg)
    PROLOG
    mov $__NR_get_msg, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(read_msg)
    PROLOG
    mov $__NR_read_msg, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(put_msg)
    PROLOG
    mov $__NR_put_msg, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret

FUNCTION(send_msg)
    PROLOG
    mov $__NR_send_msg, %eax
    MOV_PARAMS
    sysenter
    EPILOG
    ret
