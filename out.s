.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra

main:
  li $t1, 2
  move $t2, $t3
  move $t4, $t5
  li $t6, 3
  move $t7, $t8
  move $t9, $t10
  li $t11, 2
  li $t12, 3
  add $t13, $t14, $t15
  move $t16, $t17
  move $t18, $t19
