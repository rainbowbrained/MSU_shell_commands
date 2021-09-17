/* Specifications:
 * 1) If the input string has fewer then 4 arguements, the programm prints notification 
 * and quits.
 * 2) If a problem while opening, reading or writing in the file occurs, the programm
 * prints notification about error and returns errno.
 * 3) Even if the original file has fewer than 100 strings the program creates empty 
 * second file (or empties an existing one), prints notification about it and quits (not 
 * an error).
 * 
 * The first file is filled element-wisely by reading each character and analyzing if it
 * is a shift to a new string. It is filled while the total number of shifts is fewer 
 * than 100.
 * 
 * ReadMe! how to use
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>	
#include <dirent.h>	
#include <string.h>	
#include <ctype.h>	
#include <limits.h>	

int my_mv (char * file1, char * file2) //./prog, p, f
{
	#define CLOSEDIRS if (dirfd1 != NULL) { \
					closedir(dirfd1); \
				} \
                if (dirfd2 != NULL) { \
					closedir(dirfd2); \
				} \
	
	int save_errno;
	char dir[1024];
	struct __dirstream * dirfd1;
	struct __dirstream * dirfd2;
    getcwd(dir, 1024); //saving current working directory
    int fd, fd_new;
    struct stat s;
    if (((dirfd1 = opendir(file1)) == NULL)&&((errno == ENOTDIR)||(errno == ENOENT))) {
		//file1 is not a directory
	    if ((fd = open (file1, O_RDONLY, 0644)) == -1) {
			//file1 is a name
			if (((fd = open (file2, O_RDONLY, 0644)) == -1)&&(opendir(file2) == NULL)) {
				save_errno = errno;
                char buf[] = "Can not open both files or directories\n";
                write (STDERR_FILENO, buf, sizeof(buf));
	            return save_errno;
			}
        }
		if ((dirfd2 = opendir(file2)) == NULL) {
			//two files or new filename + file
			if ((fd = open (file1, O_RDONLY, 0644)) == -1) {
				//first is a new filename
				if ((fd = open (file2, O_RDONLY, 0644)) == -1) {
				    char buf[] = "Non of arguements is not a file\n";
                    write (STDERR_FILENO, buf, sizeof(buf));
	                return -1;
	            } else {
					rename (file2, file1);
			        return 0;
				}
            } else {
				//first is a file
				if ((fd = open (file2, O_RDONLY, 0644)) == -1) {
					rename (file1, file2);
			        return 0;
	            } else {
					char buf[] = "Both file names are already used\n";
                    write (STDERR_FILENO, buf, sizeof(buf));
					close (fd);
	                return -1;
				}
			}
		} else {
			//second is a directory, first is a file or name, copy file or rename
my_mv_copy_files: stat (file1, &s);
			if ((fd = open (file1, O_RDONLY, 0644)) == -1) {
                rename (file2, file1);
                CLOSEDIRS;
			    return 0;
            }
            chdir (file2); //going to the second directory
            if ((fd_new = open (file1, O_RDONLY, 0644)) != -1) {
			    char buf[] = "A file with the same name exists in this directory\n";
                write (STDERR_FILENO, buf, sizeof(buf));
	            CLOSEDIRS;
	            chdir (dir);
	            return -1;
			}
            
			if ((fd_new = open (file1, O_WRONLY | O_APPEND | O_CREAT, s.st_mode)) == -1) {
                save_errno = errno;
                close (fd);
                char buf[] = "Can not create the second file\n";
                write (STDERR_FILENO, buf, sizeof(buf));
	            CLOSEDIRS;
	            chdir (dir);
	            return save_errno;
            }
            char buf[1024];
            ssize_t n;
            while ((n = read(fd, buf, sizeof(buf))) != 0) {
				if (n == -1) {
					save_errno = errno;
					char buf1[] = "Error while reading file\n";
                    write (STDERR_FILENO, buf1, sizeof(buf1));
					CLOSEDIRS;
					chdir (dir);
					close (fd);
					return save_errno;
				}
				char *p = buf;
				while (n) {
					ssize_t nw = write (fd_new, p, n);
					if (nw == -1) {
						save_errno = errno;
						char buf1[] = "Error while rewriting the file\n";
                        write (STDERR_FILENO, buf1, sizeof(buf1));
					    CLOSEDIRS;
					    chdir (dir);
					    close (fd);
					    return save_errno;
					}
				    p += nw;
				    n -= nw;
				}
			}
			chdir (dir);
			if (remove(file1) == -1) {
				save_errno = errno;
				char buf1[] = "Error while deleting the previous file\n";
                write (STDERR_FILENO, buf1, sizeof(buf1));
				CLOSEDIRS;
				close (fd);
				return save_errno;
			}
			CLOSEDIRS;
			close (fd);
			return 0;
		}
	} else if ((dirfd1 == NULL)&&(errno != ENOTDIR)&&(errno != ENOENT))  {
		save_errno = errno;
		char buf[] = "Can not open a directory\n";
        write (STDERR_FILENO, buf, sizeof(buf));
		return save_errno;
	} else {
		//first is a directory
	    if (((dirfd2 = opendir(file2)) == NULL)&&((errno == ENOTDIR)||(errno == ENOENT))){
			if ((fd = open (file2, O_RDONLY, 0644)) == -1) {
				//second is a new directory name
                rename (file1, file2);
                CLOSEDIRS;
                chdir (dir);
			    return 0;
            }
			//second is a filename, first is a directory
			char * tmp = file1;
			file1 = file2;
			file2 = tmp;
			struct __dirstream * tmpfd = dirfd1;
			dirfd1 = dirfd2;
			dirfd2 = tmpfd;
			goto my_mv_copy_files;
		} else {
			//second maybe a directory name
			if (dirfd2 != NULL) {
				char buf[] = "Both directory names are already used\n";
                write (STDERR_FILENO, buf, sizeof(buf));
	            CLOSEDIRS;
	            chdir (dir);
	            return -1;
			} else {
                char buf[] = "Can not open the second directory\n";
                write (STDERR_FILENO, buf, sizeof(buf));
	            CLOSEDIRS;
	            chdir (dir);
	            return -1;
		    }
		}
    }
    CLOSEDIRS;
    return 0;
}

#define DELETETMP if (file[0] == '\0') {\
			if (remove(tmpfile) == -1) {\
					save_errno = errno;\
					char buftmpdel[] = "Error while deleting temporary file with stdin\n";\
					write (STDERR_FILENO, buftmpdel, sizeof(buftmpdel));\
					return save_errno;\
				}\
		} \

int my_cmp (char * file, char * file2)
{
	int fd1, fd2, n1, n2, save_errno;
	unsigned long long int num_line = 1, num_char = 1;
	
	char tmpfile[] = "stdin_temporary_file($*#&@^!%$@(}:>";
	if (file[0] == '\0') {
		if ((fd1 = open (tmpfile, O_RDWR | O_APPEND | O_CREAT , 0664)) == -1) {
            save_errno = errno;
            close (fd1);
            char bufcr[] = "Can not create the temporary file\n";
            write (STDERR_FILENO, bufcr, sizeof(bufcr));
	        return save_errno;
        }
		//dup2 (STDIN_FILENO, fd);
		char c[4096];
		int another_n;
		while ((another_n = read (STDIN_FILENO, c, sizeof(c))) != 0) {
			if (write (fd1, c, another_n) == -1) {
			    save_errno = errno;
		        char bufwr[] = "2. Error while writing in the output stream\n";
                write (STDERR_FILENO, bufwr, sizeof(bufwr));
                DELETETMP;
				close (fd1);
	            return save_errno;
			}
		//if (fork() == 0) {execlp("cat", "cat", "stdin_temporary_file($*#&@^!%$@(}:>", NULL); }
		}
		lseek(fd1, 0, SEEK_SET);
		
	} else if ((fd1 = open (file, O_RDONLY, 0644)) == -1) {
		save_errno = errno;
		char buf[] = "Can not open the first file\n";
        write (STDERR_FILENO, buf, sizeof(buf));
        DELETETMP;
		close (fd1);
	    return save_errno;
    }
    
    if ((fd2 = open (file2, O_RDONLY, 0644)) == -1) {
	    save_errno = errno;
		char buf[] = "Can not open the second file\n";
        write (STDERR_FILENO, buf, sizeof(buf));
        DELETETMP;
		close (fd1);
	    return save_errno;
    }
    char buf1[1024], buf2[1024];
    while ((n1 = read(fd1, buf1, sizeof(buf1))) != 0) {
	    if (n1 == -1) {
			save_errno = errno;
			char buf[] = "Error while reading from the first file\n";
            write (STDERR_FILENO, buf, sizeof(buf));
            DELETETMP;
		    close (fd1);
	        return save_errno;
		}
		n2 = read(fd2, buf2, sizeof(buf2));
		if (n2 == -1) {
			save_errno = errno;
			char buf[] = "Error while reading from the second file\n";
            write (STDERR_FILENO, buf, sizeof(buf));
            DELETETMP;
			close (fd1);
			return save_errno;
		}
		int i = 0;
		while ((i < n1)&&(i < n2)&&(buf1[i] == buf2[i])) {
		    if (buf1[i] == '\n') {
				num_line++;
				num_char = 1;
			} else {
			    num_char ++;
		    }
		    i++;
		}
		if (((i != n1)||(i != n2))&&(buf1[i] != buf2[i])) {
		    char num_line_str[256], num_char_str[256];
		    sprintf(num_line_str, "%lld", num_line);
		    sprintf(num_char_str, "%lld", num_char);
		    char buf[4096] = "\0";
		    if (file[0] == '\0') {
				char one_more_buffer[] = "Standart input stream";
				strcat(buf, one_more_buffer);
			} else strcat(buf, file);
		    strcat(buf, " differs from ");
		    strcat(buf, file2);
		    strcat(buf, ": line ");
		    strcat(buf, num_line_str);
		    strcat(buf, " char ");
		    strcat(buf, num_char_str);
		    if (write (STDOUT_FILENO, buf, strlen(buf)) == -1) {
				save_errno = errno;
                DELETETMP;
		        close (fd1);
		        close (fd2);
                return save_errno;
            }
		    DELETETMP;
		    close (fd1);
		    close (fd2);
		    return 0;
		}
	}
	if ((n2 = read(fd2, buf2, sizeof(buf2))) == 0) {
		char buf[4096] = "\0";
		if (file[0] == '\0') {
            char one_more_buffer1[] = "Standart input stream";
			strcat(buf, one_more_buffer1);
	    } else strcat(buf, file);
		strcat(buf, " does not differ from ");
		strcat(buf, file2);
		if (write (STDOUT_FILENO, buf, strlen(buf)) == -1) {
            save_errno = errno;
            DELETETMP;
		    close (fd1);
		    close (fd2);
            return save_errno;
        }
        DELETETMP;
		close (fd1);
        close (fd2);
		return 0;
	} else {
        char num_line_str[256], num_char_str[256];
		sprintf(num_line_str, "%lld", num_line);
        sprintf(num_char_str, "%lld", num_char);
        char buf[4096] = "\0";
        if (file[0] == '\0') {
			char one_more_buffer2[] = "Standart input stream";
			strcat(buf, one_more_buffer2);
		} else strcat(buf, file);
        strcat(buf, " differs from ");
        strcat(buf, file2);
        strcat(buf, ": line ");
        strcat(buf, num_line_str);
        strcat(buf, " char ");
        strcat(buf, num_char_str);
        if (write (STDOUT_FILENO, buf, strlen(buf)) == -1) {
            save_errno = errno;
            DELETETMP;
		    close (fd1);
		    close (fd2);
            return save_errno;
        }
	    DELETETMP;
	    close (fd1);
	    close (fd2);
	    return 0;
	}
	DELETETMP;
    close (fd1);
    close (fd2);
	return 0;
}

int my_tail (char * file, char * opt)
{
	int fd, n1, save_errno;
	char tmpfile[] = "stdin_temporary_file($*#&@^!%$@(}:>";
	if (file[0] == '\0') {
		
		if ((fd = open (tmpfile, O_RDWR | O_APPEND | O_CREAT , 0664)) == -1) {
            save_errno = errno;
            close (fd);
            char bufcr[] = "Can not create the temporary file\n";
            write (STDERR_FILENO, bufcr, sizeof(bufcr));
	        return save_errno;
        }
		//dup2 (STDIN_FILENO, fd);
		char c[4096];
		int another_n;
		while ((another_n = read (STDIN_FILENO, c, sizeof(c))) != 0) {
			if (write (fd, c, another_n) == -1) {
			    save_errno = errno;
		        char bufwr[] = "2. Error while writing in the output stream\n";
                write (STDERR_FILENO, bufwr, sizeof(bufwr));
                DELETETMP;
				close (fd);
	            return save_errno;
			}
		}
		
	} else if ((fd = open (file, O_RDONLY, 0644)) == -1) {
		save_errno = errno;
	    char buf[] = "Can not open the file\n";
        write (STDERR_FILENO, buf, sizeof(buf));
	    return save_errno;
    }
    
    if ((opt[0] != '+')&&(opt[0] != '-')) {
		char buf[] = "Second arguement must start with '+' or '-'\n";
        write (STDERR_FILENO, buf, sizeof(buf));
        DELETETMP;
        close(fd);
	    return -1;
	}
	int i = 1;
	while (i < strlen(opt)) {
		if ((opt[i] > '9')||(opt[i] < '0')) {
			char buf[] = "Second arguement must be numeric and start with '+' or '-'\n";
            write (STDERR_FILENO, buf, sizeof(buf));
            DELETETMP;
            close(fd);
	        return -1;
		}
		i++;
	}
    
    char buf[1024];
	long long int n = atoll(opt+1), cur_line = 1;

	if (n == LLONG_MAX) {
		char buf1[] = "The number in the second arguement exceeds max possible value\n";
        write (STDERR_FILENO, buf1, sizeof(buf1));
        DELETETMP;
        close(fd);
	    return -1;
	}
	n = atoll(opt);
	
	if (n >= 0) {
		//debugged
		if (n == 0) n = 1;
		while ((n1 = read(fd, buf, sizeof(buf))) != 0) {
            if (n1 == -1) {
				save_errno = errno;
			    char buf1[] = "Error while reading from the file\n";
                write (STDERR_FILENO, buf1, sizeof(buf1));
                DELETETMP;
                close (fd);
			    return save_errno;
		    }
		    if (cur_line >= n) {
			    ssize_t nw = write (STDOUT_FILENO, buf, n1);
				if (nw == -1) {
					save_errno = errno;
					char buf1[] = "1. Error while writing in the output stream\n";
                    write (STDERR_FILENO, buf1, sizeof(buf1));
                    DELETETMP;
                    close (fd);
		            return save_errno;
			    }
		    } else {
		        int i1 = 0;
		        while (i1 < n1) {
                    if (buf[i1] == '\n') {
				        cur_line++;
                    }
                    i1++;
			        if ((cur_line >= n)&&(i1 < n1)) {
						ssize_t nw = write (STDOUT_FILENO, buf + i1, 1);
				        if (nw == -1) {
							save_errno = errno;
					        char buf1[] = "2. Error while writing in the output stream\n";
                            write (STDERR_FILENO, buf1, sizeof(buf1));
                            DELETETMP;
				            close (fd);
				            return save_errno;
				        }
				    }
		        }
	        }
	    }
	    DELETETMP;
	    close(fd);
	    return 0;
	} else if (lseek(fd, -sizeof(buf), SEEK_END) != -1) {
		n = -n + 2;
		cur_line = 1;
	    //reading from the end of file	
	    while ((n1 = read(fd, buf, sizeof(buf))) != 0) {
			if (n1 == -1) {
				save_errno = errno;
		        char buf1[] = "Error while reading file\n";
                write (STDERR_FILENO, buf1, sizeof(buf1));
                DELETETMP;
	            close (fd);
	            return save_errno;
            }
			int i1 = n1 - 1;
			while ((i1 >= 0)&&(cur_line < n)) {
				if (buf[i1] == '\n') cur_line++;
				i1--;
			}
			if (cur_line >= n) {
				//fprintf (stderr, "buf = %s, %i", buf + i1 + 2, n1-i1-2);
				ssize_t nw = write (STDOUT_FILENO, buf + i1 + 2, n1 - i1 - 2);
				if (nw == -1) {
					save_errno = errno;
                    char buff[] = "1. Error while rewriting the file\n";
                    write (STDERR_FILENO, buff, sizeof(buff));
                    DELETETMP;
                    close (fd);
					return save_errno;
	            }
                ssize_t n2;
my_tail_write:  n2 = 0;
                char buf1[1024];
                while ((n2 = read(fd, buf1, sizeof(buf))) != 0) {
					//puts(buf);
					if (n2 == -1) {
						save_errno = errno;
                        char buff[] = "Error while reading file\n";
                        write (STDERR_FILENO, buff, sizeof(buff));
                        DELETETMP;
		                close (fd);
		                return save_errno;
	                }
	                ssize_t nw1 = write (STDOUT_FILENO, buf1, n2);
				    if (nw1 == -1) {
						save_errno = errno;
                        char buff[] = "2. Error while rewriting the file\n";
                        write (STDERR_FILENO, buff, sizeof(buff));
                        DELETETMP;
                        close (fd);
                        return save_errno;
	                }
	            }
	            DELETETMP;
	            close(fd);
	            return 0;
	        }
	        if (lseek(fd, -2*sizeof(buf), SEEK_CUR) == -1) {
				lseek(fd, 0, SEEK_SET);
				char buf1[1024];
				n1 = read(fd, buf1, sizeof(buf1));
				int flag = 1, j = 0, k = 0;
				while (flag) {
					while ((buf1[j] != buf[k])&&(j < sizeof(buf1))) j++;
					while ((j < sizeof(buf1))&&(buf1[j] == buf[k])) {
						j++;
						k++;
					}
					if (j == sizeof(buf1)) {
						j = j - k;
						flag = 0;
					} else {
					    j = j - k + 2;
					    k = 0;
				    }
				}
				i1 = j;
				n--;
				while ((i1 > 0)&&(cur_line <= n)) {
			        if (buf1[i1] == '\n') cur_line++;
			        i--;
		        }
		        if ((cur_line >= n)||(i1 == 0)) {
						//write the whole file in the stdin from the beginning
					lseek(fd, 0, SEEK_SET);
					goto my_tail_write;
				} else {
						//write the whole file in the stdin from i
					ssize_t nw = write (STDOUT_FILENO, buf1 + i1 + 2, n1 - i1 - 2);
					if (nw == -1) {
						save_errno = errno;
                        char buff[] = "3. Error while writing in the output stream\n";
                        write (STDERR_FILENO, buff, sizeof(buff));
                        DELETETMP;
                        close (fd);
                        return save_errno;
                    }
                    goto my_tail_write;
                }
			}
		}
		
	} else {
		if (errno != ESPIPE) {
			n = -n + 1;
			//file is < 1024 bytes long
			lseek(fd, 0, SEEK_SET);
			n1 = read(fd, buf, sizeof(buf));
			int j = n1 - 1;
			while ((j >= 0)&&(cur_line <= n)) {
				if (buf[j] == '\n') cur_line++;
				j--;
			}
			if (cur_line <= n) {
				ssize_t nw = write (STDOUT_FILENO, buf, n1);
				if (nw == -1) {
					save_errno = errno;
                    char buff[] = "5. Error while writing in the output stream\n";
                    write (STDERR_FILENO, buff, sizeof(buff));
                    DELETETMP;
                    close (fd);
                    return save_errno;
				}
				DELETETMP;
                close (fd);
				return 0;
			} else {
				ssize_t nw = write (STDOUT_FILENO, buf + j + 2, n1 - j - 2);
				if (nw == -1) {
					save_errno = errno;
                    char buff[] = "6. Error while writing in the output stream\n";
                    write (STDERR_FILENO, buff, sizeof(buff));
                    DELETETMP;
                    close (fd);
                    return save_errno;
				}
				DELETETMP;
                close (fd);
				return 0;
			}
		} else {
			save_errno = errno;
		    char buff[] = "Can not seek the end of file\n";
            write (STDERR_FILENO, buff, sizeof(buff));
            DELETETMP;
		    close(fd);
            return save_errno;
        }
    }
    DELETETMP;
    close (fd);
    return 0;
}

int main (int argc, char ** argv) //[prog, f, f1, f2]
{
	if (argc < 2) {
		char buf[] = "Not enough arguements\n";
        write (STDERR_FILENO, buf, sizeof(buf));
		return -1;
	}
	if ((!strcmp(argv[1], "mv"))&&(argc >= 4 )) { //p04 mv file1 file2
	    my_mv(argv[2], argv[3]);
	    return 0;
	} else if ((!strcmp(argv[1], "mv"))&&(argc < 4 )) {
	    char buf[] = "mv command must have 2 arguements\n";
        write (STDERR_FILENO, buf, sizeof(buf));
        return -1;    
    } else if (!strcmp(argv[1], "cmp")) {
		if (argc >= 4){
			if (!strcmp(argv[2], argv[3])) {
				char buf[] = "Same files' names\n";
				write (STDERR_FILENO, buf, sizeof(buf));
				return -1;
		    }
	        my_cmp(argv[2], argv[3]);
	    } else if (argc == 3) {
			char my_argv1[] = "\0";
			my_cmp(my_argv1, argv[2]);
		}
	    return 0;
    } else if (!strcmp(argv[1], "tail")) {
		if (argc >= 4) { //p04 tail file -n
	        my_tail(argv[2], argv[3]);
	    } else if (argc == 3) {
			my_tail(argv[2], "-10\0");
		} else if (argc == 2) {
			char my_argv[] = "\0";
			my_tail(my_argv, "-10\0");
		}
	    return 0;
    } else {
		char buf[] = "Unknown name of a function\n";
        write (STDERR_FILENO, buf, sizeof(buf));
	    return -1;
    } 
	return 0;
	
}
