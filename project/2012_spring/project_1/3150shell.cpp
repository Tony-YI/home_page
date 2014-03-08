#include <iostream>
#include <list>
#include <stdio.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

using namespace std;
/******************build in command "jobs"*******************/
struct job
{
	int pid[3];//since at most 3 child
	int num_of_process;
	char command_line[256];
	
	job()
		:num_of_process(0)
	{}
	
	job(int pid1, int pid2, int pid3, char cmdLine[256])
		:num_of_process(0)
	{
		if(pid1 != -1){num_of_process = 1;}
		if(pid2 != -1){num_of_process = 2;}
		if(pid3 != -1){num_of_process = 3;}
		pid[0] = pid1;
		pid[1] = pid2;
		pid[2] = pid3;
		strcpy(command_line, cmdLine);
	}
	
	void insert_contant(int pid1, int pid2, int pid3, char cmdLine[256])
	{
		if(pid1 != -1){num_of_process++;}
		if(pid2 != -1){num_of_process++;}
		if(pid3 != -1){num_of_process++;}
		pid[0] = pid1;
		pid[1] = pid2;
		pid[2] = pid3;
		strcpy(command_line, cmdLine);
	}	
};

class jobs
{
	private:
	int num_of_jobs;
	
	public:
	list<job> list_of_job;
	
	jobs()
		:num_of_jobs(0)
	{}
	
	void jobs_push(job j)
	{
		list_of_job.push_back( j );
		num_of_jobs++;
	}
	
	void jobs_removeAt( int position )//first is 1
	{
		num_of_jobs--;
		list<job>::iterator itr = list_of_job.begin();
		int count = 1;
		while( count != position )
		{
			itr++;
			count++;
		}
		list_of_job.erase( itr );
	}
	
	int get_num_of_jobs()
	{
		return num_of_jobs;
	}
};

/*********************Global variable**********************/
enum STATE { wait_for_command_line,  command_line_interpreter, built_in_command,
		process_creation, wait_for_termination };
	
enum STATE state = wait_for_command_line;

char cmdLine[256];//the original command line, may be valid or not
char tokens[512][256];//
int i = 0;//counting tokens in tokens[512][256]
jobs job_list;//global variable for jobs

/***********************Signal handle****************************/
void sig_hand(int sig)
{
	if(sig == SIGINT || sig == SIGTERM || sig == SIGQUIT || sig == SIGTSTP)
	{
		if( i != 0)
		{
			cout << endl;
		}
	}
}

void sig_ignore()
{
	signal( SIGINT, sig_hand );
	signal( SIGTERM, sig_hand );
	signal( SIGQUIT, sig_hand );
	signal( SIGTSTP, sig_hand );
}

void sig_reserve()
{
	signal( SIGINT, SIG_DFL );;
	signal( SIGTERM, SIG_DFL );
	signal( SIGQUIT, SIG_DFL );
	signal( SIGTSTP, SIG_DFL );
}

/************************Parser****************************/
void print_tokens_and_types( const char tokens[512][256], const int i )
{
	/*for( int temp = 0; temp < i; temp++ )
	{
		cout << "Token " << temp + 1 << ": \"" << tokens[temp] 
		<< "\" " << tokens[temp + 256] << endl;
	}*/
}

void print_tokens( const char tokens[512][256], const int i )
{
	/*for( int temp = 0; temp < i; temp++ )
	{
		cout << "Token " << temp + 1 << ": \"" << 
		tokens[temp] << "\" " << endl;
	}*/
}

bool valid_built_in_command( const char *t )
{
	return ( strcmp(t, "cd") == 0 || strcmp(t, "exit") == 0 
			|| strcmp(t, "jobs") == 0 || strcmp(t, "fg") == 0 );//
}

bool valid_file_name( const char *t )
{
	char keys[] = "> <	|*!\'\"";
	return !(strcspn( t, keys ) < strlen( t ));
}

bool valid_command_name( const char *t )
{
	if( !valid_built_in_command( t ) )
	{
		return valid_file_name( t );
	}

	return false;
}

bool valid_argument( const char *t )
{
	return valid_command_name( t );
}

void reset( int &i, char *retval, char cmdLine[256], 
				char tmp[256], char tokens[512][256], char buf[1024] )
{
	i = 0;
	retval = NULL;
	tmp = NULL;
	cmdLine = NULL;
	tokens = NULL;
	buf = NULL;
}

void get_tokens( int &i, char *retval, char cmdLine[256], char tmp[256],
					char tokens[512][256] )
{
	gets( cmdLine );
	strcpy( tmp, cmdLine );
	
	if( (retval = strtok( tmp, " " )) )
	{
		strcpy( tokens[i++], retval );
		while( (retval = strtok( NULL, " " )) )
		{
			strcpy( tokens[i++], retval );
		}
	}
}

const int parser( char tokens[512][256], const int i )
{
	//case 'enter'
	if ( i == 0 )
	{
		return 0;
	}
	//case 'built in command line' cd, exit, jobs
	else if ( valid_built_in_command(tokens[0]) )
	{
		if ( i == 2 && strcmp(tokens[0], "cd") == 0 )
		{
			strcpy( tokens[256], "(Built-in Command)" );
			strcpy( tokens[257], "(Argument)" );
			return 1;
		}
		else if( i == 1 && strcmp(tokens[0], "exit") == 0 )
		{
			strcpy( tokens[256], "(Built-in Command)" );
			return 2;
		}
		else if( i == 1 && strcmp(tokens[0], "jobs") == 0 )
		{
			strcpy( tokens[256], "(Build-in Command)" );
			return 4;
		}
		else if( strcmp(tokens[0], "fg") == 0 && ( i == 1 || i == 2) )
		{
			strcpy( tokens[256], "(Build-in Command)" );
			if( i == 1 ){ return 5; }
			if( i == 2 )
			{ 
				strcpy( tokens[256 + 1], "(Argument)" );
				return 6;
			}
		}
		else
		{
			return 3;
		}
	}
	//case 'command'
	else if ( valid_command_name(tokens[0]) )
	{
		int num = 1; 
		strcpy( tokens[256], "(Command Name)" );
		/////////////////////////////////////////////////////////////////////////////
		while ( strcmp(tokens[num], "<") != 0 && strcmp(tokens[num], "|") != 0
			&& strcmp(tokens[num], ">") != 0 && strcmp(tokens[num], ">>") != 0 && num < i )
		{
			if ( valid_argument(tokens[num]) )
			{
				strcpy( tokens[num + 256], "(Argument)" ); 
			}
			else{return -1;}
			num++;
		}
		//case '[command][enter]'
		if ( num == i )
		{
			return 7;
		}
		//case '[command]<[infile]'
		if ( strcmp(tokens[num], "<") == 0)
		{
			strcpy( tokens[num + 256], "(Redirect Input)" );
			num += 1;
			if ( valid_file_name(tokens[num]) )
			{
				strcpy( tokens[num + 256], "(Input Filename)" );
				num += 1;
				//case '[command]<[infile]|[command]' 
				if ( strcmp(tokens[num], "|") == 0 )
				{
					strcpy( tokens[num + 256], "(Pipe)" );
					num += 1;
					if ( valid_command_name(tokens[num]) )
					{
						strcpy( tokens[num + 256], "(Command Name)" );
						num += 1;

						while ( strcmp(tokens[num], "|") != 0 && strcmp(tokens[num], ">") != 0
							&& strcmp(tokens[num], ">>") != 0 && num < i )
						{
							if ( valid_argument(tokens[num]) )
							{
								strcpy( tokens[num + 256], "(Argument)" ); 
							}
							else{return -1;}
							num++;
						}
						if ( num == i )
						{
							return 8;
						}
						//case '[command]<[infile]|[command]>[outfile]' or //case '[command]<[infile]|[command]>>[outfile]'  
						if ( strcmp(tokens[num], ">") == 0 || strcmp(tokens[num], ">>") == 0 )
						{
							strcpy( tokens[num + 256], "(Redirect Output)" );
							num += 1;
							if ( valid_file_name(tokens[num]) && num == i - 1 )
							{
								strcpy( tokens[num + 256], "(Output Filename)" );
								return 9;
							}
							else{return -1;}
						}
						//case '[command]<[infile]|[command]|[command]' 
						if ( strcmp(tokens[num], "|") == 0 )
						{
							strcpy( tokens[num + 256], "(Pipe)" );
							num += 1;
							if ( valid_command_name(tokens[num]) )
							{
								strcpy( tokens[num + 256], "(Command Name)" );
								num += 1;
								
								while ( strcmp(tokens[num], ">") != 0 && strcmp(tokens[num], ">>") != 0 && num < i )
								{
									if ( valid_argument(tokens[num]) )
									{
										strcpy( tokens[num + 256], "(Argument)" );
									}
									else{return -1;}
									num++;
								}
								if ( num == i )
								{
									return 10;
								}
								//case '[command]<[infile]|[command]|[command]>or>>[outfile]' 
								if ( strcmp(tokens[num], ">") == 0 || strcmp(tokens[num], ">>") == 0 )
								{
									strcpy( tokens[num + 256], "(Redirect Output)" );
									num += 1;
									if ( valid_file_name(tokens[num]) && num == i - 1 )
									{
										strcpy( tokens[num + 256], "(Output Filename)" );
										return 11;
									}
									else{return -1;}
								}
							}
						}
					}
					else{return -1;}
				}
				//case '[command]<[infile]|[command]>,>>,enter' 
				else if ( strcmp(tokens[num], ">") == 0 || strcmp(tokens[num], ">>") == 0 )
				{
					strcpy( tokens[num + 256], "(Redirect Output)" );
					num += 1;
					if ( valid_file_name(tokens[num]) && num == i - 1 )
					{
						strcpy( tokens[num + 256], "(Output Filename)" );
						return 12;
					}
					else{return -1;}
				}
				else if ( num == i )
				{
					return 13;
				}
				else{return -1;}
			}
			else{return -1;}
		}
		//case '[command]|[command]' 
		if ( strcmp(tokens[num], "|") == 0 )
		{
			strcpy( tokens[num + 256], "(Pipe)" );
			num += 1;
			if ( valid_command_name(tokens[num]) )
			{
				strcpy( tokens[num + 256], "(Command Name)" );
				num += 1;
				
				while ( strcmp(tokens[num], "|") != 0 && strcmp(tokens[num], ">") != 0
					&& strcmp(tokens[num], ">>") != 0 && num < i )
				{
					if ( valid_argument(tokens[num]) )
					{
						strcpy( tokens[num + 256], "(Argument)" ); 
					}
					else{return -1;}
					num++;
				}
				//case '[command]|[command]enter' 
				if ( num == i )
				{
					return 14;
				}
				//case '[command]|[command]>or>>' 
				if ( strcmp(tokens[num], ">") == 0 || strcmp(tokens[num], ">>") == 0 )
				{
					strcpy( tokens[num + 256], "(Redirect Output)" );
					num += 1;
					if ( valid_file_name(tokens[num]) && num == i - 1 )
					{
						strcpy( tokens[num + 256], "(Output Filename)" );
						return 15;
					}
					else{return -1;}
				}
				//case '[command]|[command]|' 
				if ( strcmp(tokens[num], "|") == 0 )
				{
					strcpy( tokens[num + 256], "(Pipe)" );
					num += 1;
					if ( valid_command_name(tokens[num]) )
					{
						strcpy( tokens[num + 256], "(Command Name)" );
						num += 1;
						
						while ( strcmp(tokens[num], ">") != 0 && strcmp(tokens[num], ">>") != 0 && num < i )
						{
							if ( valid_argument(tokens[num]) )
							{
								strcpy( tokens[num + 256], "(Argument)" ); 
							}
							else{return -1;}
							num++;
						}
						if ( num == i )
						{
							return 16;
						}
						if ( strcmp(tokens[num], ">") == 0 || strcmp(tokens[num], ">>") == 0 )
						{
							strcpy( tokens[num + 256], "(Redirect Output)" );
							num += 1;
							if ( valid_file_name(tokens[num]) && num == i - 1 )
							{
								strcpy( tokens[num + 256], "(Output Filename)" );
								return 17;
							}
							else{return -1;}
						}
					}
				}
			}
			else{return -1;}
		}
		if ( strcmp(tokens[num], ">") == 0 || strcmp(tokens[num], ">>") == 0 )
		{
			strcpy( tokens[num + 256], "(Redirect Output)" );
			num += 1;
			if ( valid_file_name(tokens[num]) )
			{
				strcpy( tokens[num + 256], "(Output Filename)" );
				num += 1;
				if ( strcmp(tokens[num], "<") == 0 )
				{
					strcpy( tokens[num + 256], "(Redirect Input)" );
					num += 1;
					if ( valid_file_name(tokens[num]) && num == i - 1)
					{
						strcpy( tokens[num + 256], "(Input Filename)" );
						return 18;
					}
					else{return -1;}
				}
				else if (num == i)
				{
					return 19;
				}
				else{return -1;}
			}
			else{return -1;}
		}
	}
	//ivalid command line
	return -1;
}

void get_comand_and_argument(char* argv[256], int which_command )//0 for first command, 1 for second command, 2 for third command
{
	int flag = -1;
	int argv_count = 0;
	int tokens_count = 0;
	
	while( flag != which_command )
	{
		if( strcmp(tokens[tokens_count + 256], "(Command Name)") == 0 )
		{
			flag++;
		}
		tokens_count++;
	}
	tokens_count--;
	while( (strcmp(tokens[tokens_count + 256], "(Command Name)") == 0
			|| strcmp(tokens[tokens_count + 256], "(Argument)") == 0) && tokens_count < i )
	{
		argv[argv_count] = (char*) tokens[tokens_count];//spent a long time in char*
		argv_count++;
		tokens_count++;
	}
	argv[argv_count] = '\0';//Three days gone by forgot this line//
							//funny bugs without '\0';
}

void exec(char *argv[256])
{
	//setenv("PATH","/bin:/bin/usr:.",1);
	if( execvp(argv[0], argv) )
	{
		if( errno == ENOENT )
		{
			cout << "[" << argv[0] << "]: No such file or directory" << endl;
			exit(-1);
			//others
		}
		else
		{
			cout << "[" << argv[0] << "]: unknown error" << errno << endl;
			exit(-1);
			//others
		}
	}
}

void in_redirection( int fd, const char tokens[512][256] )
{
	int in_fd;
	int mm = 0;
	// first step, open the input file
	while( strcmp(tokens[mm], "(Input Filename)") != 0 )
	{
		mm++;
	}

	in_fd = open(tokens[mm - 256], O_RDONLY);
	if (in_fd == -1) 
	{
		//fprintf(stderr, "Cannot open file\n");
		cout << "[" << tokens[mm - 256] << "]: Cannot open file\n";
		exit(-1);
	}

	// second step, redirect standard input stream to the opened file
	if ( dup2(in_fd , fd) == -1) 
	{
		//fprintf(stderr, "Cannot redirect input stream\n");
		cout << "[" << tokens[mm - 256] << "]: Cannot redirect input stream\n";
		exit(-1);
	}

	// third step, close the in_fd
	if ( close(in_fd) == -1 ) 
	{
		//fprintf(stderr, "Cannot close file\n");
		cout << "[" << tokens[mm - 256] << "]: Cannot close file\n";
		exit(-1);
	}
 }

void out_redirection( int fd, const char tokens[512][256] )/* outMode = 0 for truncate , 1 for append */
{
	int out_mode;
	int out_fd;
	int mm;
	//find out the out mode
	for( mm = 0; mm < i ;mm++ )
	{
		if( strcmp(tokens[mm], ">") == 0 )
		{
			out_mode = 0;
		}
		if( strcmp(tokens[mm], ">>") == 0 )
		{
			out_mode = 1;
		}
	} 
	// first step, open the output file
	mm = 256;
	while( strcmp(tokens[mm], "(Output Filename)") != 0 )
	{
		mm++;
	}

	/* outMode = 0 for truncate , 1 for append */
	if ( out_mode )
	{
		out_fd = open( tokens[mm - 256], O_WRONLY | O_CREAT | O_APPEND , S_IRWXU ) ;
	}
	else
	{
		out_fd = open( tokens[mm - 256], O_WRONLY | O_CREAT | O_TRUNC , S_IRWXU ) ;
	}
	if ( out_fd == -1 ) 
	{ 
		cout << "[" << tokens[mm - 256] << "]: Cannot open file\n";
		exit(-1); 
	}
	if ( dup2 ( out_fd , 1) == -1 )
	{
		cout << "[" << tokens[mm - 256] << "]: Cannot redirect output stream\n";
		exit(-1);
	}
	if ( close ( out_fd ) == -1 )
	{
		cout << "[" << tokens[mm - 256] << "]: Cannot close file\n";
		exit(-1);
	}	
}	 

void exec_one_command( int mode )//7 for no redirection, 13 for in redirection only, 19 for out redirection only,
								 //12 for in-out redirections, 18 for out-in redirection
{
	int pid;
	if( (pid = fork()) )
	{
		if( pid == -1 )
		{
			fprintf(stderr, "cannot create child 1\n");
			exit(-1);
		}
		int status;
		if ( waitpid(pid, &status, WUNTRACED) != pid )
		{//There is something wrong in the child, handle the errors here
			exit(-1);
		}
		else
		{
			if ( WIFSTOPPED ( status ) )
			{
				job temp;//this must define here, else there are some funny bugs
				temp.insert_contant(pid, -1, -1, cmdLine);
				job_list.jobs_push( temp );
				cout << "job suspended" << endl;
			}
			else
			{
				/*...*/
			}
		}
	}
	else
	{
		//reserve signal handler//
		sig_reserve();
		//redirects input first if any
		if( mode == 13 || mode == 12 || mode == 18)
		{
			in_redirection( STDIN_FILENO, tokens );
		}
		//redirects output if any
		if( mode == 19 || mode == 12 || mode == 18 )
		{
			out_redirection( STDOUT_FILENO , tokens );
		}	
		//execute//
		char* argv[256];
		get_comand_and_argument(argv, 0);
		exec( argv );	
	}
}
void pipe_two_command( int pipe_mode )// 14 for no redirection, 8 for input redirection,
									//15 for output redirection, 9 for inout redirection
{
	int pid1, pid2;
	// create a pipe, referenced by two handlers (file descriptors)
	// fd[0] and fd[1]
	int fd[2];
	if ( pipe(fd)== -1 ) 
	{
		fprintf(stderr, "cannot pipe\n");
		exit(-1);
	}

	// create the first child
	if ( (pid1=fork()) ) 
	{
		if ( pid1 == -1 )
		{
			fprintf(stderr, "cannot create child 1\n");
			exit(-1);
		}

		// create the second child
		if ( (pid2 = fork()) ) 
		{// this is parent process
			// close the two file descriptors in the parent process
			if ( close(fd[0])==-1 || close(fd[1])==-1 ) 
			{
				fprintf(stderr, "cannot close fds in the parent\n");
				exit(-1);
			}

			if ( pid2 == -1 ) 
			{
				fprintf(stderr, "cannot create child 2\n");
				exit(-1);
			}
			int status1, status2;//record the status of children
			// wait for the two child processes to stop or terminate
			if ( waitpid(pid1, &status1, WUNTRACED) != pid1 || waitpid(pid2, &status2, WUNTRACED) != pid2 )
			{// There is something wrong in the child, handle the errors here
				exit(-1);
			}
			else 
			{
				if ( (WIFSTOPPED(status1) || status1 == 256) &&  
					(WIFSTOPPED(status2) || status2 == 256) )
				{
					job temp;
					temp.insert_contant(pid1, pid2, -1, cmdLine);
					job_list.jobs_push( temp );
					cout << "job suspended" << endl;
				}
				else //if( status1 == 65280 || status2 == 65280 )//in exec function, someone exit(-1)
				{
					kill ( pid1, SIGKILL );
					kill ( pid2, SIGKILL );
					waitpid(pid1, &status1, WUNTRACED);//otherwise zombie
					waitpid(pid2, &status2, WUNTRACED);
				}
			}
		}
		else 
		{// this is child 2
			//reserve signal handler//
			sig_reserve();
			//redirects output file//
			if( pipe_mode == 15 || pipe_mode == 9 )
			{
				out_redirection( STDOUT_FILENO, tokens );
			}
			// redirect the standard input stream to one end of the pipe
			if ( dup2(fd[0], STDIN_FILENO) == -1 )
			{
				fprintf(stderr, "cannot redirect the input stream of child 2\n");
				exit(-1);
			}
			// close the two file descriptors
			if ( close(fd[0]) == -1 || close(fd[1]) == -1)
			{
				fprintf(stderr, "cannot close fds in the child 2\n");
				exit(-1);
			}

			//execute argv2
			char *argv2[256];//must define here, else funny bugs
			get_comand_and_argument( argv2, 1 );
			exec( argv2 );
		}
	}
	else 
	{// this is child 1
		//reserve signal handler//
		sig_reserve();
		//redirect the input file when pipe_mode = 1
		if( pipe_mode == 8 || pipe_mode == 9 )
		{
			in_redirection( STDIN_FILENO, tokens );
		}
		// redirect the standard output to the other end of the pipe
		if ( dup2(fd[1], STDOUT_FILENO) == -1 )
		{
			fprintf(stderr, "cannot redirect the output stream of child 1\n");
			exit(-1);
		}
		// close the two file descriptors
		if ( close(fd[0]) == -1 || close(fd[1]) == -1)
		{
			fprintf(stderr, "cannot close fds in the child 1\n");
			exit(-1);
		}

		//execute argv1
		char *argv1[256];//must define here, else funny bugs
		get_comand_and_argument( argv1, 0 );
		exec( argv1 );
	}
}

void pipe_three_command( int pipe_mode )// 16 for no redirection, 10 for input redirection,
									//17 for output redirection, 11 for inout redirection
{
	int pid1, pid2, pid3;
	int status1, status2, status3;
	// create a pipe, referenced by two handlers (file descriptors)
	int fd1[2];//for command 1 and 2
	int fd2[2];//for command 2 and 3
	if ( pipe(fd1) == -1 || pipe(fd2) == -1 ) 
	{
		fprintf(stderr, "cannot pipe\n");
		exit(-1);
	}

	// create the first child
	if ( (pid1=fork()) ) 
	{
		if ( pid1 == -1 )
		{
			fprintf(stderr, "cannot create child 1\n");
			exit(-1);
		}

		// create the second child
		if ( (pid2 = fork()) ) 
		{// this is parent process
			if ( pid2 == -1 ) 
			{
				fprintf(stderr, "cannot create child 2\n");
				exit(-1);
			}
			//create the third child
			if( (pid3 = fork()) )
			{
				// close the four file descriptors in the parent process
				if ( close(fd1[0])==-1 || close(fd1[1])==-1 
					|| close(fd2[0]) == -1 || close(fd2[1]) == -1 ) 
				{
					fprintf(stderr, "cannot close fds in the parent\n");
					exit(-1);
				}
				if ( pid3 == -1 ) 
				{
					fprintf(stderr, "cannot create child 2\n");
					exit(-1);
				}
				
				// wait for the three child processes to stop or terminate
				if ( waitpid(pid1, &status1, WUNTRACED) != pid1 || waitpid(pid2, &status2, WUNTRACED) != pid2 
						|| waitpid(pid3, &status3, WUNTRACED) != pid3 )
				{//There is something wrong in the child, handle the errors here
					exit(-1);
				}
				else 
				{
					if ( (WIFSTOPPED ( status1 ) || status1 == 256) &&  (WIFSTOPPED ( status2 ) || status2 == 256) 
						&& (WIFSTOPPED ( status3 ) || status3 == 256) )//stopped or can not find the file
					{
						job temp;
						temp.insert_contant(pid1, pid2, pid3, cmdLine);
						job_list.jobs_push( temp );
						cout << "job suspended" << endl;
					}
					else if( status1 == 65280 || status2 == 65280 || status3 == 65280 )//in exec function, someone exit(-1)
					{
						kill ( pid1, SIGKILL );
						kill ( pid2, SIGKILL );
						kill ( pid3, SIGKILL );
						waitpid(pid1, NULL, 0);//otherwise zombie
						waitpid(pid2, NULL, 0);
						waitpid(pid3, NULL, 0);
					}
				}
			}
			else
			{// this is child 3
				//reserve signal handler//
				sig_reserve();
				//redirects output file//
				if( pipe_mode == 17 || pipe_mode == 11 )
				{
					out_redirection( STDOUT_FILENO, tokens );
				}
				// redirect the standard input stream to one end of the pipe
				if ( dup2(fd2[0], STDIN_FILENO) == -1 )
				{
					fprintf(stderr, "cannot redirect the input stream of child 3\n");
					exit(-1);
				}
				// close the four file descriptors
				if ( close(fd2[0]) == -1 || close(fd2[1]) == -1
					|| close(fd1[0]) == -1 || close(fd1[1]) == -1 )
				{
					fprintf(stderr, "cannot close fds in the child 3\n");
					exit(-1);
				}

				// execute argv3
				char *argv3[256];
				get_comand_and_argument(argv3, 2);
				exec( argv3 );
			}	
		}
		else 
		{// this is child 2
			//reserve signal handler//
			sig_reserve();
			// redirect the standard input stream to one end of the pipe
			// the standard output stream to the other end of the pipe
			if ( dup2(fd1[0], STDIN_FILENO) == -1 )
			{
				fprintf(stderr, "cannot redirect the input stream of child 2\n");
				exit(-1);
			}
			if ( dup2(fd2[1], STDOUT_FILENO) == -1 )
			{
				fprintf(stderr, "cannot redirect the output stream of child 2\n");
				exit(-1);
			}
			// close the two file descriptors
			if ( close(fd2[0]) == -1 || close(fd2[1]) == -1
					|| close(fd1[0]) == -1 || close(fd1[1]) == -1 )
			{
				fprintf(stderr, "cannot close fds in the child 2\n");
				exit(-1);
			}

			// setup arguments and execute argv2
			char *argv2[256];
			get_comand_and_argument(argv2, 1);
			exec( argv2 );
		}
	}
	else 
	{// this is child 1
		//reserve signal handler//
		sig_reserve();
		//redirect the input file when pipe_mode = 1
		if( pipe_mode == 10 || pipe_mode == 11 )
		{
			in_redirection( STDIN_FILENO, tokens );
		}
		// redirect the standard output to the other end of the pipe
		if ( dup2(fd1[1], STDOUT_FILENO) == -1 )
		{
			fprintf(stderr, "cannot redirect the output stream of child 1\n");
			exit(-1);
		}
		// close the two file descriptors
		if ( close(fd2[0]) == -1 || close(fd2[1]) == -1
				|| close(fd1[0]) == -1 || close(fd1[1]) == -1 )
		{
			fprintf(stderr, "cannot close fds in the child 1\n");
			exit(-1);
		}

		// setup arguments and execute argv1
		char *argv1[256];
		get_comand_and_argument(argv1, 0);
		exec( argv1 );
	}
}

void fg( int mode )// 5 for no integer, 6 for one integer, this is no completed job conrol
{
	int job_num;
	if( mode == 5 ){ job_num = 1; }
	else if( mode == 6)
	{
		job_num = atoi((char*)tokens[1]);
	}
	
	list<job>::iterator itr = job_list.list_of_job.begin();
	int count = 1;
	while( count != job_num )
	{
		count++;
		itr++;
	}
	
	//send SIGCONT to all the processes in this job
	int status1 = -1, status2 = -1, status3 = -1;
	cout << itr->command_line << endl;
	for( count = 0; count < itr->num_of_process; count++ )
	{
		if( kill(itr->pid[count], SIGCONT) == -1 )
		{
			cout << "error: cant not continue process" << endl;
			state = wait_for_command_line;
		}
	}
	
	//remove job
	if( itr->num_of_process == 1 )
	{
		if(waitpid(itr->pid[0], &status1, WUNTRACED) == -1)
		{//There is something wrong in the child, handle the errors here
			exit(-1);
		}
		else
		{
			if(WIFSIGNALED(status1) || WIFEXITED(status1))
			{
				job_list.jobs_removeAt( job_num );
				cout << "[" << job_num << "]	job wake up" << endl;
			}
		}
	}
	else if( itr->num_of_process == 2 )
	{
		if(waitpid(itr->pid[0], &status1, WUNTRACED) == -1 || waitpid(itr->pid[1], &status2, WUNTRACED) == -1)
		{//There is something wrong in the child, handle the errors here
			exit(-1);
		}
		else
		{
			if( (WIFSIGNALED(status1) || WIFEXITED(status1)) && (WIFSIGNALED(status2) || WIFEXITED(status2)) )
			{
				job_list.jobs_removeAt( job_num );
				cout << "[" << job_num << "]	job wake up " << endl;
			}
		}
	}
	else if( itr->num_of_process == 3 )
	{
		if(waitpid(itr->pid[0], &status1, WUNTRACED) == -1 || waitpid(itr->pid[1], &status2, WUNTRACED) == -1
				|| waitpid(itr->pid[2], &status3, WUNTRACED) == -1)
		{//There is something wrong in the child, handle the errors here
			exit(-1);
		}
		else
		{
			if( (WIFSIGNALED(status1) || WIFEXITED(status1)) && (WIFSIGNALED(status2) || WIFEXITED(status2))
					&& (WIFSIGNALED(status3) || WIFEXITED(status3)) )
			{
				job_list.jobs_removeAt( job_num );
				cout << "[" << job_num << "]	job wake up" << endl;
			}
		}
	}
}

void reaction( const char tokens[512][256], const int i, const int cases )
{
	if ( cases == -1 )
	{
		print_tokens( tokens, i );
		cout << "Error: invalid input command line" << endl;
	}
	if ( cases == 0 ){}
	if ( cases == 1 )
	{
		print_tokens_and_types( tokens, i );
		//change directory//
		if( chdir(tokens[1]) == 0 ){}
		else
		{
			cout << tokens[1] << ": cannot change directory." << endl;
		}
	}
	if ( cases == 2 )
	{
		print_tokens_and_types( tokens, i );
		if( job_list.get_num_of_jobs() == 0 )
		{
			exit( 0 );
		}
		else
		{
			cout << "There is at least one suspended job" << endl;
		} 
	}
	if ( cases == 3 )
	{
		print_tokens( tokens, i );
		cout << "\"" << tokens[0] 
			<< "\" : wrong number of arguments" << endl;
	}
	if ( cases == 4 )
	{
		if( job_list.get_num_of_jobs() == 0 )
		{
			cout << "No suspended job" << endl;
		}
		else
		{
			list<job>::iterator itr = job_list.list_of_job.begin();
			int count = 1;
			while( count < (job_list.get_num_of_jobs() + 1) )
			{
				cout << "[" << count << "]		suspended		"
						<< itr->command_line << endl;
				count++;
				itr++;
			}
		}
	}
	if ( cases == 5 || cases == 6 )
	{
		print_tokens_and_types( tokens, i );
		if( (i == 1 && job_list.get_num_of_jobs() != 0)
			|| (i == 2 && atoi((char*)tokens[1]) >= 1 && atoi((char*)tokens[1]) <= job_list.get_num_of_jobs()) )
		{
			fg( cases );
		}
		else
		{
			cout << "no such job" << endl;
		}
	}
	if ( cases == 7 || cases == 12 || cases == 13 || cases == 18 || cases == 19 )
	{
		print_tokens_and_types( tokens, i );
		exec_one_command( cases );
	}
	if ( cases == 8 || cases == 9 || cases == 14 || cases == 15 )
	{
		print_tokens_and_types( tokens, i );
		pipe_two_command( cases );
	}
	if ( cases == 10 || cases == 11 || cases == 16 || cases == 17 )
	{
		print_tokens_and_types( tokens, i );
		pipe_three_command( cases );
	}
}

/****************************Shell**************************/
void shell()
{	
	//signal handler//
	sig_ignore();
	
	int cases;
	char tmp[256];
	char *retval = NULL;
	int ini = 1024;//buffer for print direction
	char buf[1024];
	
	while(1)
	{
		switch( state )
		{
		case (wait_for_command_line):
			reset( i, retval, cmdLine, tmp, tokens, buf );//reset
			//////print directory//////////////////
			cout << "[3150 shell: " << getcwd(buf, ini) << "]$ ";
			///////////get tokens////////////////////////////////
			get_tokens( i, retval, cmdLine, tmp, tokens );
			state = command_line_interpreter;
			//////////////////////////////////////
			break;
			
		case (command_line_interpreter):
			cases = parser(tokens, i);
			if ( cases == -1 || cases == 0 || cases == 3 )
			{
				reaction( tokens, i, cases );
				state = wait_for_command_line;
			}
			if ( cases == 1 || cases == 2 || cases == 4 || cases == 5 || cases == 6 )
			{
				state = built_in_command;
			}
			if ( cases >= 7 && cases <= 19 )
			{
				state = process_creation;
			}
			break;
			
		case (built_in_command):
			reaction( tokens, i, cases );
			state = wait_for_command_line;
			break;
			
		case (process_creation):
			reaction( tokens, i, cases );
			state = wait_for_termination;
			break;
			
		case (wait_for_termination):
			state = wait_for_command_line;
			break;
		}		
	}
}

/******************************Main*****************************/
int main()
{
	shell();
	return 0;
}

