/*******************************************************************************
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
*******************************************************************************/

#include "vmm_defs.h"
#include "vmm_dbg.h"
#include "vmm_objects.h"
#include "modules/local_apic.h"
#include "modules/ipc.h"
#include "modules/vmcall.h"
#include "modules/trusty_guest.h"
#include "guest.h"
#include "guest_cpu.h"
#include "gpm_api.h"
#include "scheduler.h"
#include "host_memory_manager.h"
#include "host_cpu.h"
#include "event_mgr.h"
#include "trusty_startup.h"
#include "modules/msr_isolation.h"

#ifdef MODULE_VTD
#include "modules/vtd.h"
#endif

#ifdef MODULE_DEADLOOP
#include "modules/deadloop.h"
#endif

#ifdef MODULE_DEVICE_PROT
#include "modules/device_prot.h"
#endif


#define PAGE_4K_SIZE	4096

/* 0x31 is not used in Android (in CHT, BXT, GSD simics) */
#define LK_TIMER_INTR	0x31

/* Trusted OS calls internal to secure monitor */
#define	SMC_ENTITY_SECURE_MONITOR	60

/* Used in SMC_STDCALL_NR */
#define SMC_NR(entity, fn, fastcall, smc64) ((((fastcall) & 0x1) << 31) | \
					     (((smc64) & 0x1) << 30) | \
					     (((entity) & 0x3F) << 24) | \
					     ((fn) & 0xFFFF) \
					    )

/* Used in SMC_SC_NOP */
#define SMC_STDCALL_NR(entity, fn)	SMC_NR((entity), (fn), 0, 0)

/*
 * SMC_SC_NOP - origin defination is in smcall.h in lk
 * need use this defination to identify the timer interrupt
 * from Android to LK
 */
#define SMC_SC_NOP	SMC_STDCALL_NR  (SMC_ENTITY_SECURE_MONITOR, 3)

uint32_t lapic_lint0 = 0;

static boolean_t first_smc = TRUE;

static void smc_vmcall_exit(guest_cpu_handle_t gcpu)
{
	guest_cpu_handle_t next_gcpu;
	uint64_t rdi, rsi, rdx, rbx;

	vmcs_read(gcpu->vmcs, VMCS_GUEST_RIP);// update cache
	vmcs_read(gcpu->vmcs, VMCS_EXIT_INSTR_LEN);// update cache
	next_gcpu = scheduler_select_next_gcpu();

	if (gcpu->guest_handle->id != 0) {

		if (!first_smc) {
			rdi = gcpu_get_gp_reg(gcpu, IA32_REG_RDI);
			gcpu_set_gp_reg(next_gcpu, IA32_REG_RDI, rdi);
		}

		if((TRUE == first_smc) && (0 == host_cpu_id())) {
			/* unmask lint0, so it can receive external interrupt */
			lapic_write_reg(LAPIC_LVT_LINT0_REG, lapic_lint0);

			/*
			 * CR8 set to be 0x7F when SG is booting,
			 * reset to 0 to unblock any interrupt.
                         */
			gcpu_set_control_reg(gcpu, IA32_CTRL_CR8, 0);
			first_smc = FALSE;
			print_info("VMM: Launch Android\n");
		}
	}
	else {
		rdi = gcpu_get_gp_reg(gcpu, IA32_REG_RDI);
		rsi = gcpu_get_gp_reg(gcpu, IA32_REG_RSI);
		rdx = gcpu_get_gp_reg(gcpu, IA32_REG_RDX);
		rbx = gcpu_get_gp_reg(gcpu, IA32_REG_RBX);
		gcpu_set_gp_reg(next_gcpu, IA32_REG_RDI, rdi);
		gcpu_set_gp_reg(next_gcpu, IA32_REG_RSI, rsi);
		gcpu_set_gp_reg(next_gcpu, IA32_REG_RDX, rdx);
		gcpu_set_gp_reg(next_gcpu, IA32_REG_RBX, rbx);

		/* send timer interrupt to LK */
		if(SMC_SC_NOP == rdi && LK_TIMER_INTR == rsi)
			gcpu_set_pending_intr(next_gcpu, LK_TIMER_INTR);
	}
}

#define SGUEST_ID               1

guest_id_t get_sguest_id(void)
{
	return ((guest_id_t) SGUEST_ID);
}

#ifdef ENABLE_SGUEST_SMP
static void ipc_sg_handler(guest_cpu_handle_t gcpu, void *arg)
{
	cpu_id_t guest_id = (cpu_id_t)(size_t)arg;

	if (guest_get_id(gcpu_guest_handle(gcpu)) == guest_id)
	{
		gcpu_set_pending_intr(gcpu, 0x80);
	}
}

static void reschedule_vmcall_exit(guest_cpu_handle_t gcpu)
{
	guest_id_t guest_id = get_sguest_id();

	ipc_exec_on_all_other_cpus(ipc_sg_handler,
			 (void *)(size_t)guest_id);
}
#endif

// set pending interrupt to next gcpu
static void trusty_vmcall_set_pending_intr(guest_cpu_handle_t gcpu)
{
	guest_cpu_handle_t next_gcpu = gcpu->next_same_host_cpu;
	uint8_t vector = (uint8_t)gcpu_get_gp_reg(gcpu, IA32_REG_RBX);
	gcpu_set_pending_intr(next_gcpu, vector);
}

#ifdef ENABLE_SECOND_GUEST_DEBUG_BUFFER
extern void initialize_debug_buffer(guest_cpu_handle_t gcpu);
#endif /* #ifdef ENABLE_SECOND_GUEST_DEBUG_BUFFER */

/* Lint0 of LVT in Local APIC has been set to receive ExtINT,
 * but during Second Guest booting stage, ExtINT is unexpected,
 * and Second Guset cannot handle ExtINT.
 * Set lint0 of LVT in Local APIC to receive Fixxed interrupt,
 * and mask it */
 #define IA32_LOCAL_APIC_LVT_MASK_BIT 17
void mask_local_apic_lint0()
{
	uint32_t value = 0;

	lapic_lint0 = lapic_read_reg(LAPIC_LVT_LINT0_REG);

	BIT_SET(value, IA32_LOCAL_APIC_LVT_MASK_BIT);
	lapic_write_reg(LAPIC_LVT_LINT0_REG, value);
}

#define MSG_SIZE            128
static uint64_t g_sg_debug_buffer = 0;	/* Debug buffer GVA */
static uint32_t g_sg_debug_buffer_size = 4096;	/* Default Debug Buffer Size
												* set to 4K */
static uint64_t g_message_num = 0;	/* Debug messages sequence number */
static uint64_t g_current_debug_msg_addr = 0;	/* Location of the latest
												* message in the debug buffer */
void log_message_to_buffer(const char *format, ...);

boolean_t vmm_copy_to_guest_phy_addr(guest_cpu_handle_t gcpu, void *gpa,
				     uint32_t size, void *hva)
{
	uint64_t gpa_dst = (uint64_t)gpa;
	uint64_t hva_dst = 0;
	uint8_t *hva_src = (uint8_t *)hva;
	guest_handle_t guest;

	VMM_ASSERT(gcpu);
	/* Use Primary Guest GPM */
	guest = guest_handle(0);

	if (!gpm_gpa_to_hva(guest_get_gpm(guest), gpa_dst, &hva_dst)) {
		print_panic(
			"%s: Failed to convert gpa=%P to hva\n",
			__FUNCTION__,
			gpa_dst);
		return FALSE;
	}

	vmm_memcpy((void *)hva_dst, hva_src, size);

	return TRUE;
}

#define MAX_VMM_DEBUG_BUFFER  1024
static char *g_tmp_debug_buffer = NULL;
static int g_tmp_debug_buffer_cnt = 0;
void util_mstr_add_helper(char *pmszMultiSz, char *pszStr)
{
	if(NULL == pmszMultiSz) return;
	if(NULL == pszStr) return;

	while (pmszMultiSz[0] != '\0') {
		pmszMultiSz += vmm_strlen(pmszMultiSz) + 1;
	}

	vmm_strcpy(pmszMultiSz, pszStr);

	pmszMultiSz += vmm_strlen(pmszMultiSz) + 1;

	*pmszMultiSz = '\0';
}

void log_message_to_debug_buffer(char *pszStr)
{
	size_t len;

	len = vmm_strlen(pszStr);

	if (len < 1)
		return;

	if (NULL == g_tmp_debug_buffer) {
		g_tmp_debug_buffer = (char *)mem_alloc(MAX_VMM_DEBUG_BUFFER + 16);
	}

	if (NULL == g_tmp_debug_buffer)
		return;

	if ((g_tmp_debug_buffer_cnt + len) >= MAX_VMM_DEBUG_BUFFER)
		return;

	g_tmp_debug_buffer_cnt += len;

	util_mstr_add_helper(g_tmp_debug_buffer, pszStr);
}

void initialize_debug_buffer(guest_cpu_handle_t gcpu)
{
	uint64_t r_edx = 0, r_esi = 0;
	char *tmp = g_tmp_debug_buffer;

	r_edx = gcpu_get_gp_reg(gcpu, IA32_REG_RDX);
	r_esi = gcpu_get_gp_reg(gcpu, IA32_REG_RSI);

	g_sg_debug_buffer = r_edx;
	g_message_num = 0;			/* reset the message_num */
	g_current_debug_msg_addr = 0;	/* reset current debug pointer */

	if (r_esi != 0)				/* Use the default of 4K otherwise */
		g_sg_debug_buffer_size = (uint32_t) r_esi;

	log_message_to_buffer("EVMM debug v1.2 info -- built @ %s - %s\n", __DATE__, __TIME__);

	if (tmp != NULL) {
		while(tmp[0] != '\0') {
			log_message_to_buffer("%s\n", tmp);
			tmp += vmm_strlen(tmp) + 1;
		}
	}

	if (g_tmp_debug_buffer) {
		mem_free((void *)g_tmp_debug_buffer);
		g_tmp_debug_buffer = NULL;
	}

	print_trace(
		"CPU%d: Initializing Debug Buffer: g_sg_debug_buffer=0X%lX buf_size = %d\n",
		host_cpu_id(), g_sg_debug_buffer, g_sg_debug_buffer_size);
}

boolean_t is_debug_buffer_initialized()
{
	return (g_sg_debug_buffer != 0);
}

void log_message_to_buffer(const char *format, ...)
{
	guest_cpu_handle_t gcpu;
	va_list args;
	char msg[MSG_SIZE];
	char temp_msg[MSG_SIZE];
	static uint64_t g_sg_debug_buffer_tail = 0;	/* Holds the tail of the ring
												* buffer */
	int size;

	va_start(args, format);
	/* Form a message string for logging */
	size = vmm_vsprintf_s(msg, MSG_SIZE, format, args);

	gcpu = scheduler_current_gcpu();

	/* Add Message number to the begining of each message */
	size =
		vmm_sprintf_s(temp_msg, MSG_SIZE, "%s%d%s%s", "[", g_message_num++, "]",
		msg);
	/* reset the buffer tail when the debug buffer is reinitialized via vmcall.
	*/
	if (g_current_debug_msg_addr == 0)
		g_sg_debug_buffer_tail = 0;

	if (g_sg_debug_buffer) {
		if (g_current_debug_msg_addr == 0
			|| (g_current_debug_msg_addr + size) >
			(g_sg_debug_buffer + g_sg_debug_buffer_size)) {
				g_current_debug_msg_addr = g_sg_debug_buffer + 64;	/* First 64
																	* Bytes
																	* contain the
																	* tail address
																	*/
				/* buffer is rolled over first time, then set the buffer tail to
				* the current message address */
				if (g_current_debug_msg_addr != 0)
					g_sg_debug_buffer_tail = (uint64_t) g_current_debug_msg_addr;
		}
		if (!vmm_copy_to_guest_phy_addr(gcpu,
			(void *)(g_current_debug_msg_addr),
			(unsigned int)vmm_strlen(temp_msg),
			(void *)temp_msg)) {
				print_panic(
					"%s: Failed to copy message to buffer from Second Guest\n",
					__FUNCTION__);
		}
		/* Update the buffer head */
		g_current_debug_msg_addr += size;
		/* Update the buffer tail address at the first 64 bytes of the debug
		* buffer */
		if (g_sg_debug_buffer_tail != 0) {
			g_sg_debug_buffer_tail = (uint64_t) g_current_debug_msg_addr;
			size =
				vmm_sprintf_s(temp_msg, MSG_SIZE, "%s%lx%s", "***:",
				g_sg_debug_buffer_tail, ":***");
			if (!vmm_copy_to_guest_phy_addr
				(gcpu, (void *)(g_sg_debug_buffer),
				(unsigned int)vmm_strlen(temp_msg), (void *)temp_msg)) {
					print_panic(
						"%s: Failed to copy message to buffer from Second Guest\n",
						__FUNCTION__);
			}
		}
	} else {
		print_panic("%s: g_sg_debug_buffer is NULL\n",
			__FUNCTION__);
	}
}


#ifdef ENABLE_SECOND_GUEST_DEBUG_BUFFER
extern void log_message_to_buffer(const char *format, ...);
extern void log_message_to_debug_buffer(char *pszStr);
extern boolean_t is_debug_buffer_initialized();
static uint32_t sg_debug_buf_lock = 0;	/* Used to guard the debug log
										 * function. */
											 /* 0 : not locked */
											 /* 1 or more : locked */
#endif							/* #ifdef ENABLE_SECOND_GUEST_DEBUG_BUFFER */

static
void raw_lock(volatile uint32_t *p_lock_var)
{
	uint32_t old_value;

	for (;; ) {
		/* Loop until the successfully incremented the lock variable */
		/* from 0 to 1 (i.e., we are the only lockers */

		old_value = asm_lock_cmpxchg32(
			(uint32_t *)p_lock_var,
			1, /* New */
			0); /* Expected */
		if (0 == old_value) {
			break;
		}
		asm_pause();
	}
}

static
void raw_unlock(volatile uint32_t *p_lock_var)
{
	*p_lock_var = 0;
}

static
void vmm_io_vmcall_puts_handler(guest_cpu_handle_t gcpu)
{
	hva_t hva;
	hpa_t hpa;
	guest_handle_t guest_handle;
	gpm_handle_t gpm_handle;
	address_t arg1;

	/*
	 * Since Linux in the second guest uses zero-base 1:1 identity mapping,
	 * we need to convert the guest physical address (also is the guest
	 * virtual address to host physical address to make this
	 * vmm_io_vmcall_puts_handler works properly
	 */
	arg1 = gcpu_get_gp_reg(gcpu, IA32_REG_RDX);
	guest_handle = gcpu_guest_handle(gcpu);
	gpm_handle = guest_get_gpm(guest_handle);

	if (!gpm_gpa_to_hva(gpm_handle, (gpa_t) arg1, &hva)) {
		print_panic(
				"%s: Failed to convert gpa=%P to hva\n", __FUNCTION__, arg1);
	}

	hmm_hva_to_hpa(hva, &hpa);
#ifdef ENABLE_SECOND_GUEST_DEBUG_BUFFER
	/* Log Second Guest messages to the debug buffer */
	/* Skip newline in front of the message */
	if (*(char *)hva != '\n') {
		raw_lock(&sg_debug_buf_lock);
		if (is_debug_buffer_initialized()) {
			log_message_to_buffer("%s\n", (char *)hva);
		} else {
			log_message_to_debug_buffer((char *)hva);
		}
		raw_unlock(&sg_debug_buf_lock);
	}
#endif							/* #ifdef ENABLE_SECOND_GUEST_DEBUG_BUFFER */
	print_trace("%s\n", (char *)hva);
}

static void trusty_vmcall_dump_init(guest_cpu_handle_t gcpu)
{
	gva_t dump_gva;

	D(VMM_ASSERT(gcpu->guest_handle->id == 0);)

	/* RDI stored the pointer of deadloop_dump_t which allocated by the guest */
	dump_gva = gcpu_get_gp_reg(gcpu, IA32_REG_RDI);

	/* register event deadloop */
	if(!deadloop_setup(gcpu, dump_gva))
		return;
	/* set the return value */
	gcpu_set_gp_reg(gcpu, IA32_REG_RAX, 0);

	/* TODO: save the initial vmcs */
	//vmcs_store_initial(gcpu, host_cpu_id());
}

void guest_register_vmcall_services(guest_id_t guest_id)
{
	/* vmcalls for both OS */
	vmcall_register(guest_id, TRUSTY_VMCALL_SMC, smc_vmcall_exit);
	vmcall_register(guest_id, TRUSTY_VMCALL_PENDING_INTR, trusty_vmcall_set_pending_intr);

	/* only for the second guest : LK */
	if (guest_id != 0) {
		vmcall_register(guest_id, TRUSTY_VMCALL_PUTS, vmm_io_vmcall_puts_handler);

#ifdef ENABLE_SGUEST_SMP
		vmcall_register(guest_id, TRUSTY_VMCALL_RESCHEDULE, reschedule_vmcall_exit);
#endif
	}
	else /* only for guest_id=0, the primary guest: Android*/
	{
#ifdef ENABLE_SECOND_GUEST_DEBUG_BUFFER
		vmcall_register(guest_id, TRUSTY_VMCALL_DEBUG_BUFFER, initialize_debug_buffer);
#endif

#ifdef MODULE_DEADLOOP
		vmcall_register(guest_id, TRUSTY_VMCALL_DUMP_INIT, trusty_vmcall_dump_init);
#endif
	}
}

extern guest_handle_t init_single_guest(uint32_t number_of_host_processors, const module_file_info_t *evmm_file);

static boolean_t trusty_gcpu_init(guest_cpu_handle_t gcpu, void *gcpu_state)
{
	if (gcpu->guest_handle->id == 1) {
		if (gcpu_state)
			gcpu_initialize(gcpu, (gcpu_state_t *)gcpu_state);
		else
			gcpu_set_reset_state(gcpu);
	}
	return TRUE;
}

void init_trusty_guest(const evmm_desc_t *evmm_desc)
{
	trusty_desc_t *trusty_desc = (trusty_desc_t *)evmm_desc->extra_info;

	guest_handle_t trusty_guest;
	guest_gcpu_econtext_t context;
	guest_cpu_handle_t gcpu;
	cpu_id_t hcpu_id;
	gcpu_state_t *cpus_arr = NULL;
	trusty_startup_info_t *trusty_para;

	cpus_arr = &trusty_desc->gcpu0_state;

	trusty_para = (trusty_startup_info_t *)cpus_arr[0].gp_reg[IA32_REG_RDI];
	if(NULL == trusty_para) {
		print_panic("Invalid Trusty startup info address.\n");
		VMM_DEADLOOP();
	} else {
		/* Used to check structure in both sides are same */
		trusty_para->size_of_this_struct    = sizeof(trusty_startup_info_t);
		/* Used to set heap of LK */
		trusty_para->mem_size               =
			trusty_desc->lk_file.runtime_total_size;
		trusty_para->calibrate_tsc_per_ms   = evmm_desc->tsc_per_ms;
		trusty_para->trusty_mem_base        =
			trusty_desc->lk_file.runtime_addr;
	}

	/* LK will use parameter in RDI */
	cpus_arr[0].gp_reg[IA32_REG_RDI] = (uint64_t)(void *)trusty_para;

	print_trace("Init trusty guest\n");

	/* TODO: refine it later */
	trusty_guest = init_single_guest(1, &(evmm_desc->evmm_file));
	if (!trusty_guest) {
		print_trace(
			"initialize_all_guests: Cannot init secondary guest\n");
	}
	trusty_gcpu_init(trusty_guest->gcpu_list, &trusty_desc->gcpu0_state);

	gpm_remove_mapping(guest_get_gpm(guest_handle(0)),
				trusty_desc->lk_file.runtime_addr,
				trusty_desc->lk_file.runtime_total_size);
	print_trace(
			"Primary guest GPM: remove sguest image base %llx size 0x%x\r\n",
			trusty_desc->lk_file.runtime_addr,
			trusty_desc->lk_file.runtime_total_size);

	guest_register_vmcall_services(0);
	guest_register_vmcall_services(1);

	/* Isolate below MSRs between guests and set initial value to 0 */
	add_to_msr_isolation_list(IA32_MSR_STAR, 0, GUESTS_ISOLATION);
	add_to_msr_isolation_list(IA32_MSR_LSTAR, 0, GUESTS_ISOLATION);
	add_to_msr_isolation_list(IA32_MSR_CSTAR, 0, GUESTS_ISOLATION);
	add_to_msr_isolation_list(IA32_MSR_SYSCALL_MASK, 0, GUESTS_ISOLATION);
	add_to_msr_isolation_list(IA32_MSR_KERNEL_GS_BASE, 0, GUESTS_ISOLATION);

	mask_local_apic_lint0();

#ifdef MODULE_VTD
	vtd_remove_range(trusty_desc->lk_file.runtime_addr,
		trusty_desc->lk_file.runtime_total_size);
#endif

#ifdef MODULE_DEVICE_PROT
	/*
	 * Sample for enable memory protection
	 * #define TEST_START          0x783fe000
	 * #define TEST_SIZE           0x00001000
	 * remove_device_access(1, TEST_START, TEST_SIZE);
	 */
#endif
}

