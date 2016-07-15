# Do NOT modify segment
# Data segment declaration
.data
number: .word 30
memory_data: .word 1350, 9648, 123458, 7568, 67867, 56415, 1234, 36879, 6452, 1231, 7810, 97899, 845645, 778921, 6689, 5123, 4456, 1373, 285214, 15689, 10456, 9646, 878456, 7456, 61237, 1895, 4984, 3546, 4892, 16871
s1_str: .asciiz "$s1 = "
s2_str: .asciiz "$s2 = "
before_str: .asciiz "Before program starts : \n"
after_str: .asciiz "After program ends : \n"
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
print_before:
	la $a0, before_str
	li $v0, 4
	syscall
	jr $ra
print_memory:
	la $a0, memory_str
	li $v0, 4
	syscall
	la $t0, memory_data
	lw $t1, number
print_memory_loop:
	lw $a0, ($t0)
	li $v0, 1
	syscall
	addi $t1, $t1, -1
	beq $t1, $zero, end_print_memory_loop
	addi $t0, $t0, 4
	la $a0, space_str
	li $v0, 4
	syscall
	j print_memory_loop
end_print_memory_loop:
	la $a0, newline_str
	li $v0, 4	
	syscall
	jr $ra
print_after:
	la $a0, after_str
	li $v0, 4
	syscall
	jr $ra
main:
	lw $s1, number
	la $s2, memory_data
# Print the initial values inside the register and memory
	jal print_before
	jal print_s1
	jal print_s2
	jal print_memory
# end of do NOT modify segment
# Write your program here, before this line, $s1 = n and $s2 = addr









# end of your prgram segment
# do NOT modify segment
# # Print the initial values inside the memory
	jal print_after
	jal print_memory
	li $v0, 10
	syscall
# end of do NOT modify segment