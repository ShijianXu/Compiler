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
  addi $sp, $sp, -8
  sw $ra, 8($sp)
  sw $fp, 4($sp)
  move $fp, $sp
  addi $sp, $sp, -100
  jal read
  move $t0, $v0
  sw $t0, -8($fp)
read
  lw $t1, -8($fp)
  move $t0, $t1
  sw $t0, -16($fp)
  sw $t1, -8($fp)
ass
  lw $t1, -16($fp)
  move $t0, $t1
  sw $t0, -12($fp)
  sw $t1, -16($fp)
ass
  jal read
  move $t0, $v0
  sw $t0, -28($fp)
read
  lw $t1, -28($fp)
  move $t0, $t1
  sw $t0, -36($fp)
  sw $t1, -28($fp)
ass
  lw $t1, -36($fp)
  move $t0, $t1
  sw $t0, -32($fp)
  sw $t1, -36($fp)
ass
  lw $t1, -44($fp)
  move $t0, $t1
  sw $t0, -56($fp)
  sw $t1, -44($fp)
ass
  li $t0, 3
  sw $t0, -60($fp)
ass
  lw $t1, -56($fp)
  lw $t2, -60($fp)
  add $t0 $t1, $t2
  sw $t0, -68($fp)
  sw $t1, -56($fp)
  sw $t2, -60($fp)
add
  lw $t1, -68($fp)
  move $t0, $t1
  sw $t0, -76($fp)
  sw $t1, -68($fp)
ass
  lw $t1, -76($fp)
  move $t0, $t1
  sw $t0, -72($fp)
  sw $t1, -76($fp)
ass
  lw $t1, -84($fp)
  move $t0, $t1
  sw $t0, -88($fp)
  sw $t1, -84($fp)
ass
  lw $t0, -88($fp)
  move $a0, $t0
  jal write
write
  li $t1, 0
  sw $t1, -96($fp)
ass
  lw $t1, -96($fp)
  move $v0, $t1
  lw $ra, 8($fp)
  lw $fp, 4($fp)
  addi $sp, $sp, 100
  jr $ra
ret
