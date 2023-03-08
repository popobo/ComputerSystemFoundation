
#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define MAP(c, f) c(f)

#define REGS(f) \
      f( 1)       f( 3) f( 4) f( 5) f( 6) f( 7) f( 8) f( 9) \
f(10) f(11) f(12) f(13) f(14) f(15) f(16) f(17) f(18) f(19) \
f(20) f(21) f(22) f(23) f(24) f(25) f(26) f(27) f(28) f(29) \
f(30) f(31)

#define PUSH(n) sw concat(x, n), (n * 4)(sp);
#define POP(n)  lw concat(x, n), (n * 4)(sp);

#define CONTEXT_SIZE ((32 + 4) * 4)
#define OFFSET_SP     ( 2 * 4)
#define OFFSET_CAUSE  (32 * 4)
#define OFFSET_STATUS (33 * 4)
#define OFFSET_EPC    (34 * 4)
#define OFFSET_NP     (35 * 4)

.globl __am_asm_trap
__am_asm_trap:
  // ---- c->sp = $sp, 
  // if pp is kernel, sp will be the same as original sp
  // so after save context, if mscratch is 0(kernel), save sp directly

  // ---- if (ksp != 0) { $sp = ksp;}
  // exchange sp and mscratch, mscratch stores the ksp value
  csrrw sp, mscratch, sp
  // now sp stores the ksp value, mscratch stores the original sp
  // if sp(ksp) is 0, which means pp is kernel, 
  // the original sp points kernel stack, so go to 29, restore sp from mscratch

  // if sp(ksp) is not 0, which means pp is user,
  // we should use kernel stack to store context, and sp is ksp, so jump to save context directly
  // mscratch is original sp, which points to user stack, not zero and useful
  bnez sp, save_context
  // sp(ksp) is 0, pp is kernel
  // mscratch is original sp, which points to kernel stack, not zero but useless
  // so we can set mscratch 0 to means that np is kernel
  csrr sp, mscratch
  
  // set mscratch 0
  csrrw t0, mscratch, t0
  li t0, 0
  csrrw t0, mscratch, t0
  
save_context:
  // now sp is pointing to the kernel stack
  addi sp, sp, -CONTEXT_SIZE
  
  MAP(REGS, PUSH)
  
  mv t0, sp
  addi t0, t0, CONTEXT_SIZE
  sw t0, OFFSET_SP(sp)
  
  csrr t0, mscratch
  beqz t0, skip_set_mscratch_c_sp
  // mscratch is not 0, c->sp = mscratch (set c->sp $sp)
  sw t0, OFFSET_SP(sp)

skip_set_mscratch_c_sp:
  csrr t0, scause
  csrr t1, sstatus
  csrr t2, sepc

  sw t0, OFFSET_CAUSE(sp)
  sw t1, OFFSET_STATUS(sp)
  sw t2, OFFSET_EPC(sp)

  // ---- c->np = (ksp == ? KERNEL : USER);
  csrr t0, mscratch
  // if t3 is not zero, np is user
  li t1, 1
  bnez t0, am_irq_handle
  // if t3 is zero, np is kernel(0)
  li t1, 0
am_irq_handle:
  sw t1, OFFSET_NP(sp)
  
  // ---- ksp = 0;
  li t0, 0
  csrw mscratch, t0

  mv a0, sp
  jal __am_irq_handle
  mv sp, a0

restore_context:
  lw t1, OFFSET_STATUS(sp)
  lw t2, OFFSET_EPC(sp)
  csrw sstatus, t1
  csrw sepc, t2
 
  // if (cp->np == USER) { ksp = $sp }
  lw t3, OFFSET_NP(sp)
  beqz t3, pop_gprs
  // t3 != 0, c->np is user
  // set ksp = $sp + CONTEXT_SIZE
  addi t4, sp, CONTEXT_SIZE
  csrw mscratch, t4
pop_gprs:
  MAP(REGS, POP)
  
  lw sp, OFFSET_SP(sp)

  sret
