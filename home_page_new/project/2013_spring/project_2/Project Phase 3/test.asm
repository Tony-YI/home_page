	addi $s1, $zero, 3
	addi $s2, $zero, 4
	lw $s1, 0($s2)
	sub $s4, $s1, $s2
	sub $s4, $s1, $s2
	j loop
	add $s2, $s2, $s2
loop:	bne $s1, $s2, bran
	add $s2, $s2, $s2
bran:	add $s2, $s2, $s2
	sub $s2, $s2, $s2
	addi $s2, $s2, 3
	mult $s2, $s2
	div $s2, $s2
	and $s2, $s2, $s2
	or $s2, $s2, $s2
	sllv $s2, $s2, $s2
	srlv $s2, $s2, $s2
	slt $s2, $s2, $s2
