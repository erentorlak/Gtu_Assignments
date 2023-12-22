.data 
row_prompt: .asciiz "Enter the number of rows: "
col_prompt: .asciiz "Enter the number of columns: "
stop_time_prompt: .asciiz "Enter time to stop: "
insert_prompt: .asciiz "Enter the number of insertions: "
row_input_prompt: .asciiz "Enter the row for insertion: "
col_input_prompt: .asciiz "Enter the column for insertion: "
path_exists_msg: .asciiz "Path exists!\n"
path_not_exists_msg: .asciiz "Path does not exist!\n"
.text

# Eren Torlak 10.11.2023
# v4( 130pt ) for Computer Organization CSE 331 Project 1 Bomberman

# $t8 matrix addres
# $s4, $s5 row and column size


main:
    # Prompt the user for the number of rows and columns
    li $v0, 4      		# Print string syscall
    la $a0, row_prompt 		# Load the address of the row_prompt string
    syscall

    # Read the user input for rows
    li $v0, 5      		# Read integer syscall
    syscall
    move $s4, $v0  		# Store the number of rows in $t4

    li $v0, 4      		# Print string syscall
    la $a0, col_prompt 		# Load the address of the col_prompt string
    syscall

    # Read the user input for columns
    li $v0, 5      		# Read integer syscall
    syscall
    move $s5, $v0  		# Store the number of columns in $t5
    
    # Prompt the user for the stop time input
    li $v0, 4
    la $a0, stop_time_prompt
    syscall
    
    # Read the user input for stop time
    li $v0, 5      		# Read integer syscall
    syscall
    move $s6, $v0  		# Store the number of rows in $s6
    
    mul $t6, $s4, $s5  		# Calculate the space needed for the matrix

    move $a0, $t6        	# Size in bytes
    li $v0, 9            	# sbrk syscall code
    syscall
    move $t8, $v0        	# Store the address of the allocated space in $t8

    # Fill the matrix with '0' 
    jal fill_matrix

    # Insert user input bombs to matrix
    jal insert_value
    
    # init time counter to zero 
    li  $t6, 0
    jal print_matrix	
 
gameloop:	
    
    beq $t6, $s6, exitgame	# exit when time is over
    jal decrease_time 		# decrease the bombs time numbers in matrix
       						
    jal print_matrix
    addi $t6, $t6, 1 		# increment time counter 
    beq $t6, $s6, exitgame   	# exit when time is over
    
    jal decrease_time		# decrease the bombs time numbers in matrix
    jal new_bombs 		# place new bombs 
      
    
    jal print_matrix 
    addi $t6, $t6, 1 		# increment time counter 
    beq $t6, $s6, exitgame    	# exit when time is over
      
    jal blow_bombs  		# when you see 1 second bombs blow them

    
    j gameloop
    
exitgame:			# Exit the program
   
    jal is_path_open	# Function to check if there is an open path from [0, 0] to [max_i, max_j]

    li $v0, 10  # Exit syscall code
    syscall

# Function to fill the matrix with 0 (Char)ascii = 48
fill_matrix:
    li $t0, 0       		# Initialize a row counter
    li $t1, 0       		# Initialize a column counter
    li $t2, 48     		# Initialize $t2 with the 0

fill_loop:
    beq $t0, $s4, exit_fill  	# If row counter is equal to the number of rows, exit fill
    li $t1, 0  			# Reset column counter

fill_column_loop:
    # Calculate the address of the element in the matrix
    mul $t3, $t0, $s5  		# Calculate the row offset
    add $t3, $t3, $t1  		# Add the column offset
    add $t3, $t3, $t8  		# Add addres of matrix ( $t8 )
    
    # Store the character 0 in the matrix
    sb $t2, 0($t3)

    # Increment the column counter
    addi $t1, $t1, 1

    # Check if the column counter is equal to the number of columns
    bne $t1, $s5, fill_column_loop

    # Increment the row counter
    addi $t0, $t0, 1

    # Continue filling rows
    j fill_loop

exit_fill:
    jr $ra  			# Return to the caller

# Function to insert values into the matrix
insert_value:
    li $v0, 4      		# Print string syscall
    la $a0, insert_prompt 	# Load the address of the insert_prompt string
    syscall

    # Read the number of insertions
    li $v0, 5      		# Read integer syscall
    syscall
    move $s7, $v0  		# Store the number of insertions in $t7

    li $t0, 0  			# Initialize a counter for insertions

insert_loop:
    beq $t0, $s7, exit_insert  	# If insertion counter equals the desired insertions, exit insert

    # Ask for row and column positions
    li $v0, 4      		# Print string syscall
    la $a0, row_input_prompt 	# Load the address of the row_input_prompt string
    syscall

    # Read the row position
    li $v0, 5      		# Read integer syscall
    syscall
    move $t4, $v0  		# Store the row position in $t4

    li $v0, 4      		# Print string syscall
    la $a0, col_input_prompt 	# Load the address of the col_input_prompt string
    syscall

    # Read the column position
    li $v0, 5      		# Read integer syscall
    syscall
    move $t5, $v0  		# Store the column position in $t1

    # Calculate the address of the element in the matrix
    mul $t2, $t4, $s5  		# Calculate the row offset
    add $t2, $t2, $t5  		# Add the column offset
    add $t2, $t2, $t8 		# Add the addres of matrix
    
    # Store the character '3' in the matrix
    li $t3, 51  		# 51 is asci 3 char 
    sb $t3, 0($t2) 

    # Increment the insertion counter
    addi $t0, $t0, 1

    # Continue insertions
    j insert_loop

exit_insert:
    jr $ra  			# Return to the caller

# Function to place new bombs in the matrix
new_bombs:
    li $t0, 0  			# Initialize row counter

new_bombs_row_loop:
    beq $t0, $s4, exit_new_bombs  # If row counter is equal to the number of rows, exit new_bombs
    li $t1, 0  			# Initialize column counter

new_bombs_column_loop:
    # Calculate the address of the element in the matrix
    mul $t2, $t0, $s5  		# Calculate the row offset
    add $t2, $t2, $t1  		# Add the column offset
    add $t2, $t2, $t8  		# Add the addres of matrix (t8)
    # Load a character from the matrix
    lb $t3, 0($t2)

    # Check if the character is not zero . zero means not bomb
    li $t4, 48
    bne $t3, $t4, is_bomb  	# If it is bomb , go to is_bomb

    # if not bomb 
    # Store the character '3' in the matrix
    li $t3, 51
    sb $t3, 0($t2)

    # Increment the column counter
    addi $t1, $t1, 1

    # Check if the column counter is equal to the number of columns
    bne $t1, $s5, new_bombs_column_loop

    # Increment the row counter
    addi $t0, $t0, 1

    # Continue searching for '1's in the next row
    j new_bombs_row_loop

is_bomb:
    # Increment the column counter
    addi $t1, $t1, 1

    # Check if the column counter is equal to the number of columns
    bne $t1, $s5, new_bombs_column_loop

    # Increment the row counter
    addi $t0, $t0, 1

    # Continue searching for '1's in the next row
    j new_bombs_row_loop

exit_new_bombs:
    jr $ra  			# Return to the caller

# it blows the bombs that gonna blow in 1 second
blow_bombs:
    li $t0, 0  			# Initialize row counter

blow_bombs_row_loop:
    beq $t0, $s4, exit_blow_bombs  # If row counter is equal to the number of rows, exit new_bombs
    li $t1, 0  			# Initialize column counter

blow_bombs_column_loop:
    # Calculate the address of the element in the matrix
    mul $t2, $t0, $s5  		# Calculate the row offset
    add $t2, $t2, $t1  		# Add the column offset
    add $t2, $t2, $t8
    # Load a character from the matrix
    lb $t3, 0($t2)

    # Check if the character is '1'
    li $t4, 49
    beq $t3, $t4, blow_it  	# If '1', go to blow it 

    # Increment the column counter
    addi $t1, $t1, 1

    # Check if the column counter is equal to the number of columns
    bne $t1, $s5, blow_bombs_column_loop

    # Increment the row counter
    addi $t0, $t0, 1

    # Continue searching for '1's in the next row
    j blow_bombs_row_loop

blow_it:
    # around the current cell will be 0 unless there are 1(means bomb, they should blow at same time)
    li $t4, 48 # t4 is 0
    beq $t0, $zero, skip_up 	# control up 
	
    sub  $t9, $t2, $s5  	# Move to the up position and use a temporary register
    lb $t7, 0($t9) 
    li $t3, 49 			# ASCII value for '1'
        
    beq $t7, $t3, skip_up   	# is it bomb ('1')
        
    sb $t4, 0($t9)  		# Place a "0" in the up position
skip_up:
    
    subi  $t5, $s4, 1    	# decrease by 1 
    beq $t5, $t0, skip_down 	# if it is in bound, skip
	
    add  $t9, $t2, $s5  	# Move to the down position and use a temporary register
    lb $t7, 0($t9)
    li $t3, 49 			# ASCII value for '1'
        
    beq $t7, $t3, skip_down  	# is it bomb ('1')
        
    sb $t4, 0($t9)  		# Place a "0" in the up position
skip_down:

    beq $zero, $t1, skip_left 	# is it far left 

    subi $t9, $t2, 1  		# Move to the left position and use the temporary register
    lb $t7, 0($t9)
    li $t3, 49
        
    beq $t7, $t3, skip_left  	# is it bomb ('1')
        
    sb $t4, 0($t9)  		# Place a "0" in the left position
 skip_left:       
 
    subi  $t5, $s5, 1
    beq $t5, $t1, skip_right 	# is it far right
 
    addi $t9, $t2, 1  		# Move to the right position and use the temporary register
    lb $t7, 0($t9)
    li $t3, 49  		#asci 1 
        
    beq $t7, $t3, skip_right  	# is it bomb ('1')
        
    sb $t4, 0($t9)  		# Place a "0" in the left position  
 skip_right:
 	       
    sb $t4, 0($t2) 		# insert 0 to current place. 
        
    # Increment the column counter
    addi $t1, $t1, 1

    # Check if the column counter is equal to the number of columns
    bne $t1, $s5, blow_bombs_column_loop

    # Increment the row counter
    addi $t0, $t0, 1

    # Continue searching for '1's in the next row
    j blow_bombs_row_loop


exit_blow_bombs:
    jr $ra  			# Return to the caller

# Function to decrease time of bombs in matrix
decrease_time:
    li $t0, 0  			# Initialize row counter

decrease_time_row_loop:
    beq $t0, $s4, exit_decrease_time  # If row counter is equal to the number of rows, exit decrease_time
    li $t1, 0  			# Initialize column counter

decrease_time_column_loop:
    # Calculate the address of the element in the matrix
    mul $t2, $t0, $s5  		# Calculate the row offset
    add $t2, $t2, $t1  		# Add the column offset
    add $t2, $t2, $t8   	# Add the addres matrix offset
    
    # Load a character from the matrix
    lb $t3, 0($t2)

    # Check if the character is 0 (ASCII value 48)
    li $t4, 48
    beq $t3, $t4, not_bomb  	# If is '0', go to not_bomb

    # Decrease the time by 1 (example : decrement 3 to 2, 2 to 1, 1 to 0)
    subi $t3, $t3, 1

    # Store the updated character in the matrix
    sb $t3, 0($t2)

not_bomb:
    # Increment the column counter
    addi $t1, $t1, 1

    # Check if the column counter is equal to the number of columns
    bne $t1, $s5, decrease_time_column_loop

    # Increment the row counter
    addi $t0, $t0, 1

    # Continue decreasing time for the next row
    j decrease_time_row_loop

exit_decrease_time:
    jr $ra  			# Return to the caller
# it prints matrix by printing 0 for 0 otherwise X (means bomb)
print_matrix:
    li $t0, 0  			# Reset row counter

print_row_loop:
    beq $t0, $s4, exit_print  	# If row counter is equal to the number of rows, exit print
    li $t1, 0  			# Reset column counter

print_column_loop:
    # Calculate the address of the element in the matrix
    mul $t3, $t0, $s5  		# Calculate the row offset
    add $t3, $t3, $t1  		# Add the column offset
    add $t3, $t3, $t8   	# Add the base address of the matrix

    # if you want to print Actual matrix remove comment
    #lb  $a0, 0($t3)  
    #li $v0, 11  
    #syscall
    
    lb $t4, 0($t3) 		# load t4 the current cell 
    # Check if the character is equal to '0'
    beq $t4, 48, print_0

    # Check if the character is equal to 'X'
    bne $t4, 48, print_X
    
continue_print_space:
    # Print a space character
    li $v0, 11  		# Print character syscall code
    li $a0, ' '  		# Space character
    syscall

    # Increment the column counter
    addi $t1, $t1, 1

    # Check if the column counter is equal to the number of columns
    bne $t1, $s5, print_column_loop

    # Print a newline character
    li $v0, 11  		# Print character syscall code
    li $a0, '\n'  		# Newline character
    syscall

    # Increment the row counter
    addi $t0, $t0, 1

    # Continue printing rows
    j print_row_loop

exit_print:
    addi $t6, $t6, 1		# add 1 because it hold index
    li $v0, 1    		# Print  second we are in
    move $a0, $t6  		# Load the value from $t6 into $a0
    syscall 			# Print  second we are in
    subi $t6, $t6, 1 		# sub 1 because we added 1 for temporary
	
    li $v0, 11  		# Print character syscall code
    li $a0, '\n'  		# Newline character
    syscall
    
    li $v0, 11  		# Print character syscall code
    li $a0, '\n'  		# Newline character
    syscall
    
    jr $ra  			# Return to the caller

print_X:
    li $v0, 11  		# Print character syscall code
    li $a0, 'X'
    syscall
    j continue_print_space

print_0:
    li $v0, 11  		# Print character syscall code
    li $a0, '0'
    syscall
    j continue_print_space

# Function to check if there is an open path from [0, 0] to [max_i, max_j]
#   $t8: Address of the matrix
#   $s4: Number of rows
#   $s5: Number of columns
is_path_open:
    # Check if [0, 0] or [max_i, max_j] is a bomb
    lb $t0, 0($t8)  		# Load value at [0, 0]
    
    # Check if [0, 0] is a bomb
    li $s3, 48
    bne $t0, $s3, path_not_exist  # If it's a bomb, path does not exist

    # Check if [max_i, max_j] is a bomb
    mul $t0, $s4, $s5
    subi $t0, $t0, 1
    add $t0, $t0, $t8	
    lb $t1, 0($t0)
    
    # Check if [max_i, max_j] is a bomb
    bne $t1, $s3, path_not_exist  # If it's a bomb, path does not exist

    # Initialize counters
    li $t0, 0  # Row counter
    li $t1, 0  # Column counter

    # Loop for moving right and down
move_right_down_loop:
    # is path exist
    add $t9, $s4, $s5 		#  t9 = row + col
    subi $t9, $t9, 2  		# t9 = t9 - 2 
    
    add $t3, $t0, $t1  		# t3 = row counter + col counter 
    
    beq $t3, $t9, path_exist  	# if it is last cell in matrix
        
    # bound check right
    add $t4, $t1, 1  		# Move to the right position
    blt $t4, $s5, check_right 	# If within bounds, check right

    j check_down

check_right:
    # Load the value in the right position
    mul $t6, $t0, $s5  		# Calculate the row offset
    add $t6, $t6, $t1  		# Add the column offset
    add $t6, $t6, $t8  		# Add the matrix address
    lb $t2, 1($t6) 		# one right

    # Check if there no bomb on right 
    beq $t2, $s3, move_right  	# If it's not a bomb, move right

    j check_down 		# if right has bomb

move_right:
    # Increment column counter
    addi $t1, $t1, 1

    j move_right_down_loop

check_down:

    # Load the value in the down position
    mul $t6, $t0, $s5  		# Calculate the row offset
    add $t6, $t6, $t1  		# Add the column offset
    add $t6, $t6, $s5  		# Add the move down
    add $t6, $t6, $t8  		# Add the matrix address
    lb $t2, 0($t6) 

    bne $t2, $s3, path_not_exist # If it's a bomb game over
    
    # Check down boundary
    add $t5, $t0, 1  		# Move down
    blt $t5, $s4, move_down  	# If within bounds, move down

    j path_not_exist 		# if we cant go right and down it is not feasible

move_down:
    # Increment row counter
    addi $t0, $t0, 1

    j move_right_down_loop     	# return to loop

path_not_exist:
    # Print path does not exist
    li $v0, 4
    la $a0, path_not_exists_msg
    syscall
    
    jr $ra   # return gameloop
path_exist:
    # Print path  exist
    li $v0, 4
    la $a0, path_exists_msg
    syscall
    
    jr $ra   # return gameloop  

