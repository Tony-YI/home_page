# Do NOT modify segment
# Data segment declaration
.data
x: .word 96, 265, 247
s1_str: .asciiz "s1 = "
s2_str: .asciiz "s2 = "
s3_str: .asciiz "s3 = "
after_str: .asciiz "After program ends : \n"
before_str: .asciiz "Before program starts : \n"
newline_str: .asciiz "\n"
space_str: .asciiz " "
# Text segment declaration
.text
print_s1:
	la $a0, s1_str
	li $v0, 4
	syscall
	add $a0, $zero, $s1	
	li $v0, 1
	syscall
	la $a0, newline_str
	li $v0, 4
	syscall
	jr $ra
print_s2:
	la $a0, s2_str
	li $v0, 4
	syscall
	add $a0, $zero, $s2
	li $v0, 1
	syscall
	la $a0, newline_str
	li $v0, 4
	syscall
	jr $ra
print_s3:
	la $a0, s3_str
	li $v0, 4
	syscall
	add $a0, $zero, $s3
	li $v0, 1
	syscall
	la $a0, newline_str
	li $v0, 4
	syscall
	jr $ra
print_after:
	la $a0, after_str
	li $v0, 4
	syscall
	jr $ra
print_before:
	la $a0, before_str
	li $v0, 4
	syscall
	jr $ra
main:
	la $s3, x
	lw $s1, 0($s3)
	lw $s2, 4($s3)
	lw $s3, 8($s3)
# Print the initial values inside the registers
	jal print_before
	jal print_s1
	jal print_s2
	jal print_s3
# end of do NOT modify segment
# Write your program here, before this line, $s1 = a, $s2, = b, $s3 = c









# end of your prgram segment
# do NOT modify segment
# # Print the initial values inside the register
	jal print_after
	jal print_s1
	li $v0, 10
	syscall
# end of do NOT modify segment