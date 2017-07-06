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


time:
  addi $sp, $sp, -8
  sw $ra, 8($sp)
  sw $fp, 4($sp)
  move $fp, $sp
  addi $sp, $sp, -20
  move $t0, $a0
  sw $t0, 0($fp)
  move $t0, $a1
  sw $t0, -4($fp)
  lw $t1, 0($fp)
  move $t0, $t1
  sw $t0, -8($fp)
  sw $t1, 0($fp)
  lw $t1, -4($fp)
  move $t0, $t1
  sw $t0, -12($fp)
  sw $t1, -4($fp)
  lw $t1, -8($fp)
  lw $t2, -12($fp)
  mul $t0 $t1, $t2
  sw $t0, -16($fp)
  sw $t1, -8($fp)
  sw $t2, -12($fp)
  lw $t0, -16($fp)
  move $v0, $t0
  lw $ra, 8($fp)
  lw $fp, 4($fp)
  addi $sp, $sp, 28
  jr $ra

main:
  addi $sp, $sp, -8
  sw $ra, 8($sp)
  sw $fp, 4($sp)
  move $fp, $sp
  addi $sp, $sp, -184
  li $t1, 0
  sw $t1, -40($fp)
  lw $t2, -40($fp)
  move $t1, $t2
  sw $t1, -44($fp)
  sw $t2, -40($fp)
  lw $t2, -44($fp)
  move $t1, $t2
  sw $t1, -48($fp)
  sw $t2, -44($fp)
label1:
  lw $t2, -44($fp)
  move $t1, $t2
  sw $t1, -52($fp)
  sw $t2, -44($fp)
  li $t1, 4
  sw $t1, -56($fp)
  lw $t1, -52($fp)
  lw $t2, -56($fp)
  blt $t1, $t2, label2
  sw $t1, -52($fp)
  sw $t2, -56($fp)
  j label3
label2:
  lw $t2, -44($fp)
  move $t1, $t2
  sw $t1, -60($fp)
  sw $t2, -44($fp)
  lw $t2, -60($fp)
  li $t3, 4
  mul $t1 $t2, $t3
  sw $t1, -64($fp)
  sw $t2, -60($fp)
  la $t4, -16($fp)
  lw $t2, -64($fp)
  add $t1 $t4, $t2
  sw $t1, -68($fp)
  sw $t2, -64($fp)
  jal read
  move $t1, $v0
  sw $t1, -72($fp)
  lw $t1, -68($fp)
  lw $t2, -72($fp)
  sw $t2, 0($t1)
  lw $t2, -44($fp)
  move $t1, $t2
  sw $t1, -76($fp)
  sw $t2, -44($fp)
  lw $t2, -76($fp)
  li $t4, 4
  mul $t1 $t2, $t4
  sw $t1, -80($fp)
  sw $t2, -76($fp)
  la $t5, -36($fp)
  lw $t2, -80($fp)
  add $t1 $t5, $t2
  sw $t1, -84($fp)
  sw $t2, -80($fp)
  jal read
  move $t1, $v0
  sw $t1, -88($fp)
  lw $t1, -84($fp)
  lw $t2, -88($fp)
  sw $t2, 0($t1)
  lw $t2, -44($fp)
  move $t1, $t2
  sw $t1, -92($fp)
  sw $t2, -44($fp)
  li $t1, 1
  sw $t1, -96($fp)
  lw $t5, -92($fp)
  lw $t2, -96($fp)
  add $t1 $t5, $t2
  sw $t1, -100($fp)
  sw $t2, -96($fp)
  lw $t2, -100($fp)
  move $t1, $t2
  sw $t1, -44($fp)
  sw $t2, -100($fp)
  lw $t2, -44($fp)
  move $t1, $t2
  sw $t1, -104($fp)
  sw $t2, -44($fp)
  j label1
label3:
  li $t1, 0
  sw $t1, -108($fp)
  lw $t2, -108($fp)
  move $t1, $t2
  sw $t1, -44($fp)
  sw $t2, -108($fp)
  lw $t2, -44($fp)
  move $t1, $t2
  sw $t1, -112($fp)
  sw $t2, -44($fp)
label4:
  lw $t2, -44($fp)
  move $t1, $t2
  sw $t1, -116($fp)
  sw $t2, -44($fp)
  li $t1, 4
  sw $t1, -120($fp)
  lw $t1, -116($fp)
  lw $t2, -120($fp)
  blt $t1, $t2, label5
  sw $t1, -116($fp)
  sw $t2, -120($fp)
  j label6
label5:
  lw $t2, -44($fp)
  move $t1, $t2
  sw $t1, -124($fp)
  sw $t2, -44($fp)
  lw $t2, -124($fp)
  li $t5, 4
  mul $t1 $t2, $t5
  sw $t1, -128($fp)
  sw $t2, -124($fp)
  la $t6, -16($fp)
  lw $t2, -128($fp)
  add $t1 $t6, $t2
  sw $t1, -132($fp)
  sw $t2, -128($fp)
  lw $t2, -44($fp)
  move $t1, $t2
  sw $t1, -136($fp)
  sw $t2, -44($fp)
  lw $t2, -136($fp)
  li $t6, 4
  mul $t1 $t2, $t6
  sw $t1, -140($fp)
  sw $t2, -136($fp)
  la $t7, -36($fp)
  lw $t2, -140($fp)
  add $t1 $t7, $t2
  sw $t1, -144($fp)
  sw $t2, -140($fp)
  addi $sp, $sp, 0
  lw $t1, -144($fp)
  lw $t2, 0($t1)
  move $a1, $t2
  lw $t1, -132($fp)
  lw $t2, 0($t1)
  move $a0, $t2
  jal time
  move $t1, $v0
  sw $t1, -148($fp)
  lw $t2, -148($fp)
  move $t1, $t2
  sw $t1, -152($fp)
  sw $t2, -148($fp)
  lw $t2, -152($fp)
  move $t1, $t2
  sw $t1, -156($fp)
  sw $t2, -152($fp)
  lw $t2, -152($fp)
  move $t1, $t2
  sw $t1, -160($fp)
  sw $t2, -152($fp)
  lw $t1, -160($fp)
  move $a0, $t1
  jal write
  lw $t2, -44($fp)
  move $t1, $t2
  sw $t1, -164($fp)
  sw $t2, -44($fp)
  li $t1, 1
  sw $t1, -168($fp)
  lw $t7, -164($fp)
  lw $t2, -168($fp)
  add $t1 $t7, $t2
  sw $t1, -172($fp)
  sw $t2, -168($fp)
  lw $t2, -172($fp)
  move $t1, $t2
  sw $t1, -44($fp)
  sw $t2, -172($fp)
  lw $t2, -44($fp)
  move $t1, $t2
  sw $t1, -176($fp)
  sw $t2, -44($fp)
  j label4
label6:
  li $t1, 0
  sw $t1, -180($fp)
  lw $t1, -180($fp)
  move $v0, $t1
  lw $ra, 8($fp)
  lw $fp, 4($fp)
  addi $sp, $sp, 192
  jr $ra
