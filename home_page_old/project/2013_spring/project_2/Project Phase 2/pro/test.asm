	addi $s10, $zero, 1
	addi $s1, $zero, 4
	addi $s2, $zero, 16
	add $fp, $s10, $s2
	sub $s5, $s1, $s2
	sub $s6, $s5, $s5
	and $s7, $fp, $s2
	or $s8, $s2, $s1
	sllv $s9, $s1, $s10
	srlv $s10, $s8, $s1
	lw $s11, 4($s9)
	lw $s12, -4($s9)
	sw $s7, 0($zero)
	slt $s14, $s10, $s8
	slt $s15, $s8, $s10
	slt $s16, $s10, $s5
	mult $s7, $s8
	mult $lo, $fp
	mult $lo, $lo
	add $s19, $lo, $fp
	mult $s19, $s19
	add $s21, $hi, $zero
	div $s21, $s1
	div $lo, $fp
