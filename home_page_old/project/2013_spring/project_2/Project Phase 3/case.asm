addi $s1, $zero, 2
add $s2, $s1, $s1
slt $s2, $s1, $zero
slt $s2, $zero, $s1
mult $s1, $s1
div $lo, $s1
lw $s2, 0($zero)
sw $s1, 4($zero)