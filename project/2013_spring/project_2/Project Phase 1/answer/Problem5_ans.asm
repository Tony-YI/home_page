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

	and  $s3, $s3, $zero		#set s3 = 0, sum
	addi $s4, $zero, 10		#set s4 = 10(deximal)
loop:
	div  $s1, $s4			#get the last digit first
	mflo $s1			#div
	mfhi $s2			#mod
	add  $s3, $s3, $s2		#add sum
	beq  $s2, $zero, exit_loop	#if s2 == 0, means no more remainder
	j    loop
exit_loop:
	add $s1, $zero, $s3		#sum up

# end of your prgram segment
# do NOT modify segment
# # Print the initial values inside the register
	jal print_after
	jal print_s1
	li $v0, 10
	syscall
# end of do NOT modify segment