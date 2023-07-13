.globl  _start
_start:
  #to set current switch_boot_pcb
  #tips is pa4 chapter one user process
  move sp, a0
  move s0, zero
  jal call_main
