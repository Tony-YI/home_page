# Do NOT modify segment
# Data segment declaration
.data
x: .word 4213956
s1_str: .asciiz "s1 = "
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
	lw $s1, x
# Print the initial values inside the register
	jal print_before
	jal print_s1
# end of do NOT modify segment
# Write your program here, before this line, $s1 = x









# end of your prgram segment
# do NOT modify segment
# # Print the initial values inside the register
	jal print_after
	jal print_s1
	li $v0, 10
	syscall
# end of do NOT modify segment