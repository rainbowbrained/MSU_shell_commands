# MSU_shell_commands

This project implements a few functions from the SHELL language:
1. mv
2. cmp
3. tail

***

 OPERATING INSTRUCTION 
 =====================

    The programm gets one of three names of functions as the first arguement: mv, cmp or 
    tail. The second and the third arguements are arguements for the corresponding 
    function.

    This archive also includes test files: "short_file.txt" and "another_short_file.txt"
    with the same content (50 lines), "very_long_file.txt" and 
    "another_very_long_file.txt" with the same content (5000 lines, first 50 lines are
    the same as in "short_file.txt"), "file.txt" and "another_file.txt" with the same 
    content (first 3 lines are the same as in "short_file.txt"). Those tests may be used
    by user to check the correctness if program's work.
    
    It is recommended to create directories in the same directory with the programm and 
    try to rename it or move some file int it by calling
    ./p04 mv dir file.txt
    ./p04 mv dir new_dir_name

***

my_mv
=====================
    
#### DESCRIPTION

    The function gets two arguements and operates depending on their types:
    
    1. If the first arguement is a name of a file and the second arguement is a directory
    or vice versa (the first arguement is a directory of a file and the second arguement 
    is a file), then the function rewrites the given file into the given directory.
    
    2. If both arguements are files (or directories), the function determines which of 
    arguements is a name of existing file or directory and which is a new name, and 
    renames the file or directory.
    
    The result of function does not depend on the files' sizes.
    
#### RETURN VALUE

    In case of succsess the function returns 0. In case of a system error the function 
    returns errno. In case of special errors (see ERRORS below) the function returns -1. 
    
#### ERRORS

    1. The function writes in the stderr stream about all system errors and returns 
    errno. This types of errors include:
    - unable to open an existing file or a directory;
    - unable to create a file while moving a file to another directory;
    - unable to read or write data from the file;
    - unable to delete old file from the previouse directory while moving a file to
    another directory;
    
    2. If the function gets two arguements, an there is no file or directory with the 
    same name as arguements.
    
    3. If the function gets two files' names or directories' names, both of which already
    exist in the working directory.
    
    4. If the function gets two same arguements.
    
#### NOTES

    Files and directories given as aguements must have appropriate access rights: 
    ability to rename and open the directory, ability to read from file, rename it, write
    to the file and delete it.
    

#### EXAMPLE

    By default, the working directory consists of:
    - object file p04
    - files with names file1, file2
    - directory with name dir
    
    1. ./p04 mv file1 file
    renames file1 to file
    
    2. ./p04 mv file1 file
    renames file back to file1
    
    
    3. ./p04 mv dir file1
    moves file1 to the directory dir
    
    4. ./p04 mv dir file1
    renames dir to file1 (because the current working directory does not contains files
    named "file1")
    
    5. ./p04 mv file2 file1
    renames file2 to file1 (because the current working directory does not contains files
    named "file1")
    
***

my_cmp
=====================
 
#### DESCRIPTION

    The function gets two names of files as arguements and compares their content. If 
    they differ, the function prints the order number of the first line and char, in 
    which they differ. If they do not differ the function prints according message.
    
    If the second arguement is missing, the function compares the first file with the
    data in the standart input stream by rewriting it to the new created file with the
    name stdin_temporary_file($*#&@^!%$@(}:> (PRAYING THE GOD IT IS NOT OCCUPIED), 
    operating with it as if it is the second arguement and deleting it in the end.
    
    The result of function does not depend on the files' sizes.
    
#### RETURN VALUE

    In case of succsess the function returns 0. In case of a system error the function 
    returns errno. In case of special errors (see ERRORS below) the function returns -1. 

#### NOTES

    Both files must have appropriate access rights, at least ability to read and seek
    inside the file.
    
#### ERRORS

    1. The function writes in the stderr stream about all system errors and returns 
    errno. This types of errors include:
    - unable to open files;
    - unable to read data from files;
    - unable to write messages into the output stream.
    - unable to create a temporary file
    
    2. If the function gets two same arguements.

***

my_tail
=====================

#### DESCRIPTION

    The function gets filename as the first arguement and optionaly an integer number
    starting with '-' or '+' as a second arguement and prints lines from the given file
    into the output stream depending on the second arguement.
    
    1. If the second arguement equals '+n' where n is an integer, the function prints 
    all lines from the given file after the n'th line (including the n'th line). If 
    the function containes less than n strings the function does not print anything.
    
    2. If the second arguement equals '-n' where n is an integer, the function prints 
    last n lines from the given file (including the n'th string counting from the end).
    
    3. If the second arguement is missing, the function operates is if the second 
    argeuement equals '-10'.
    
    4. If the first arguement is missing, the function works with the data in the 
    standart input stream by rewriting it to the new created file with the name 
    stdin_temporary_file($*#&@^!%$@(}:> (PRAYING THE GOD IT IS NOT OCCUPIED ONE MORE 
    TIME), operating with it as if it is the first arguement and deleting it in the end.
    The second arguement is automatically considered to be '-10'.
    
    The result of function does not depend on the files' sizes.
    
#### RETURN VALUE

    In case of succsess the function returns 0. In case of a system error the function 
    returns errno. In case of special errors (see ERRORS below) the function returns -1. 
    
#### ERRORS

    1. The function writes in the stderr stream about all system errors and returns 
    errno. This types of errors include:
    - unable to open a file;
    - unable to read data from the file;
    - unable to write in the output stream;
    - unable to seek inside the file.
    - unable to create a temporary file
    
    2. If the second arguement has an inappropriate format.
    
    3. If a number in the second arguement exceeds maximum long long integer value.
    
#### NOTES

    1. The number in the second arguement must fit into the long long integer.
    2. The file must have appropriate access rights, at least ability to read and seek
    inside the file.
    
#### EXAMPLE

    1. ./p04 tail file
    prints last 10 lines from the file
    
    2. ./p04 tail file +5
    prints all lines from the given file after the 5'th line (including the 5'th line)
    
    3. ./p04 mv tail -3
    prints last 3 lines from the given file
