# Do NOT modify segment
# Data segment declaration
.data
number: .word 10
s1_str: .asciiz "s1 = "
after_str: .asciiz "After program ends : \n"
before_str: .asciiz "Before program starts : \n"
memory_str: .asciiz "In the memory : "
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
	lw $s1, number
# Print the initial values inside the register and memory
	jal print_before
	jal print_s1
# end of do NOT modify segment
# Write your program here, before this line, $s1 = n

	andi $s2, $s2, 0	#set s2 to be 0, Fibeven
	ori  $s3, $s3, 1	#set s3 to be 1, Fibodd
	add  $s4, $zero, $s1	#set s4 = s1(t = n)
	addi $s5, $zero, 0	#sum of n numbers
loop:	
	blez $s4,exit_loop	#when t == 0, jump to exit_loop
	add  $s5, $s5, $s2	#add odd Fib to sum
	add  $s5, $s5, $s3	#add even Fib to sum
	add  $s6, $zero, $s3	#store previous s3
	add  $s2, $s2, $s3	#next Fib
	add  $s3, $s3, $s2	#next Fib
	addi $s4, $s4, -2	#t -= 2
	addi $t0, $zero, -1	#set t0 = -1
	beq  $s4, $t0, add_back #if s4 == -1, go to add_back
	j    loop
add_back:
	sub $s5, $s5, $s6		#add previous s3 to the sum
	j    exit_loop
exit_loop:
	add  $s1, $zero, $s5	#move sum to s1

# end of your prgram segment
# do NOT modify segment
# # Print the initial values inside the register
	jal print_after
	jal print_s1
	li $v0, 10
	syscall
# end of do NOT modify segment