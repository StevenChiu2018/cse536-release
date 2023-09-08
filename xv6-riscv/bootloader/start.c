/* These files have been taken from the open-source xv6 Operating System codebase (MIT License).  */

#include "types.h"
#include "param.h"
#include "layout.h"
#include "riscv.h"
#include "defs.h"
#include "buf.h"
#include "measurements.h"
#include <stdbool.h>

void main();
void timerinit();

/* entry.S needs one stack per CPU */
__attribute__ ((aligned (16))) char bl_stack[STSIZE * NCPU];

/* Context (SHA-256) for secure boot */
SHA256_CTX sha256_ctx;

/* Structure to collects system information */
struct sys_info {
  /* Bootloader binary addresses */
  uint64 bl_start;
  uint64 bl_end;
  /* Accessible DRAM addresses (excluding bootloader) */
  uint64 dr_start;
  uint64 dr_end;
  /* Kernel SHA-256 hashes */
  BYTE expected_kernel_measurement[32];
  BYTE observed_kernel_measurement[32];
};
struct sys_info* sys_info_ptr;

extern void _entry(void);
void panic(char *s)
{
  for(;;)
    ;
}

void copy_kernel_to(uint64 destination, uint64 size, enum kernel ktype)
{
  int blockno = 0;
  int blockno_offset = 4;
  int copied_size = 0;
  struct buf buffer;
  while(copied_size < size)
  {
    buffer.blockno = blockno + blockno_offset;
    kernel_copy(ktype, &buffer);
    uint64 copy_to_addr = destination + (blockno * BSIZE);
    memmove((char *)copy_to_addr, buffer.data, BSIZE);

    blockno++;
    copied_size += BSIZE;
  }
}

/* CSE 536: Boot into the RECOVERY kernel instead of NORMAL kernel
 * when hash verification fails. */
void setup_kernel(enum kernel ktype)
{
  uint64 kernel_load_addr = find_kernel_load_addr(ktype);
  uint64 kernel_binary_size = find_kernel_size(ktype);
  copy_kernel_to(kernel_load_addr, kernel_binary_size, ktype);
  uint64 kernel_entry = find_kernel_entry_addr(ktype);

  /* CSE 536: Write the correct kernel entry point */
  w_mepc((uint64) kernel_entry);
}

bool verified_kernel_hash()
{
  for(int i=0;i<32;i++)
    if(sys_info_ptr -> expected_kernel_measurement[i] != sys_info_ptr -> observed_kernel_measurement[i])
      return false;

  return true;
}

void load_expected_kernel_measurement()
{
  for(int i=0;i<32;i++)
    sys_info_ptr -> expected_kernel_measurement[i] = trusted_kernel_hash[i];
}

/* CSE 536: Function verifies if NORMAL kernel is expected or tampered. */
bool is_secure_boot(void) {
  /* Read the binary and update the observed measurement
   * (simplified template provided below)
   * Three more tasks required below:
   *  1. Compare observed measurement with expected hash
   *  2. Setup the recovery kernel if comparison fails
   *  3. Copy expected kernel hash to the system information table */

  sha256_init(&sha256_ctx);
  uint binary_size = find_kernel_size(NORMAL);
  sha256_update(&sha256_ctx, (uchar*) RAMDISK, binary_size);
  sha256_final(&sha256_ctx, sys_info_ptr->observed_kernel_measurement);
  load_expected_kernel_measurement();

  return verified_kernel_hash();
}

// entry.S jumps here in machine mode on stack0.
void start()
{
  /* CSE 536: Define the system information table's location. */
  sys_info_ptr = (struct sys_info*) 0x80080000;

  // keep each CPU's hartid in its tp register, for cpuid().
  int id = r_mhartid();
  w_tp(id);

  // set M Previous Privilege mode to Supervisor, for mret.
  unsigned long x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  x |= MSTATUS_MPP_S;
  w_mstatus(x);

  // disable paging
  w_satp(0);

  /* CSE 536: Unless kernelpmp[1-2] booted, allow all memory
   * regions to be accessed in S-mode. */
  #if !defined(KERNELPMP1) || !defined(KERNELPMP2)
    w_pmpaddr0(0x3fffffffffffffull);
    w_pmpcfg0(0xf);
  #endif

  /* CSE 536: With kernelpmp1, isolate upper 10MBs using TOR */
  #if defined(KERNELPMP1)
    w_pmpaddr0(0x21d40000);
    w_pmpcfg0(0xf);
  #endif

  /* CSE 536: With kernelpmp2, isolate 118-120 MB and 122-126 MB using NAPOT */
  #if defined(KERNELPMP2)
    w_pmpaddr0(0x21d80000);
    w_pmpaddr1(0x21dbffff);
    w_pmpaddr2(0x21e3ffff);
    w_pmpaddr3(0x21effffe);
    w_pmpaddr4(0x21fbffff);
    w_pmpcfg0(0x1f181f180f);
  #endif

  /* CSE 536: Verify if the kernel is untampered for secure boot */
  if (is_secure_boot()) setup_kernel(NORMAL);
  else setup_kernel(RECOVERY);

  /* CSE 536: Provide system information to the kernel. */

  /* CSE 536: Send the observed hash value to the kernel (using sys_info_ptr) */

  sys_info_ptr -> bl_start = 0x80000000;
  sys_info_ptr -> bl_end = 0x80065848;
  sys_info_ptr -> dr_start = 0x80065849;
  sys_info_ptr -> dr_end = 0x88000000;

  // delegate all interrupts and exceptions to supervisor mode.
  w_medeleg(0xffff);
  w_mideleg(0xffff);
  w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);

  // return address fix
  uint64 addr = (uint64) panic;
  asm volatile("mv ra, %0" : : "r" (addr));

  // switch to supervisor mode and jump to main().
  asm volatile("mret");
}