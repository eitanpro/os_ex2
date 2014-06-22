/******************************************
* Eitan Kondratovsky
* 315872937
* 8923103
* ex2
******************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>

#define PARAMETER_NUMBER_ERROR_MESSAGE \
"expect to get one parameter to main function\n"
#define PARAMETER_NUMBER_ERROR 1
#define SIGACTION_ERROR_MESSAGE "sigaction error\n"
#define SIGACTION_ERROR 2
#define ALARM_ERROR_MESSAGE "interapt alarm waiting\n"
#define ALARM_ERROR 3
#define FORK_ERROR_MESSAGE "fork error\n"
#define FORK_ERROR 4
#define EXECUTE_ERROR_MESSAGE "execute error\n"
#define EXECUTE_ERROR 5
#define PIPE_ERROR_MESSAGE "pipe error\n"
#define PIPE_ERROR 6
#define WRITE_ERROR_MESSAGE "write error\n"
#define WRITE_ERROR 7
#define DUP_ERROR_MESSAGE "dup error\n"
#define DUP_ERROR 8
#define CLOSE_ERROR_MESSAGE "close error\n"
#define CLOSE_ERROR 9
#define SIGNAL_SEND_ERROR_MESSAGE "send signal error\n"
#define SIGNAL_SEND_ERROR 10
#define BYE_MESSAGE "Big Daddy Bye Bye\n"

void ExitSignalHandler(int signalNumber);
void InitActions();
int ConvertStringToInt(char *numberString);
void ConvertIntToString(int number, char *numberString);

int pid1, pid2;
int fda[2];

int main(int argc, char *argv[]) {
	int time;
	char buffer[20];

	if (argc != 2) { // if get only one parameter extra
		write(2, PARAMETER_NUMBER_ERROR_MESSAGE, 
				strlen(PARAMETER_NUMBER_ERROR_MESSAGE));
		exit(PARAMETER_NUMBER_ERROR);
	}

	if (pipe(fda) != 0) {
		write(2, PIPE_ERROR_MESSAGE, strlen(PIPE_ERROR_MESSAGE));
		exit(PIPE_ERROR);
	}

	if ((pid1 = fork()) < 0) {
		write(2, FORK_ERROR_MESSAGE, strlen(FORK_ERROR_MESSAGE));
		exit(FORK_ERROR);
	}
	else if (pid1 == 0) { // inside son process 1
		if (close(0) < 0) {
			write(2, CLOSE_ERROR_MESSAGE, strlen(CLOSE_ERROR_MESSAGE));
			exit(CLOSE_ERROR);
		}
		if (dup(fda[0]) < 0) {
			write(2, DUP_ERROR_MESSAGE, strlen(DUP_ERROR_MESSAGE));
			exit(DUP_ERROR);
		}
		if (close(fda[0]) < 0) {
			write(2, CLOSE_ERROR_MESSAGE, strlen(CLOSE_ERROR_MESSAGE));
			exit(CLOSE_ERROR);
		}
		if (close(fda[1]) < 0) {
			write(2, CLOSE_ERROR_MESSAGE, strlen(CLOSE_ERROR_MESSAGE));
			exit(CLOSE_ERROR);
		}
		if (execv("ex2_inp.out", NULL) < 0) {
			write(2, EXECUTE_ERROR_MESSAGE,
					strlen(EXECUTE_ERROR_MESSAGE));
			exit(EXECUTE_ERROR);
		}
		write(2, EXECUTE_ERROR_MESSAGE,	strlen(EXECUTE_ERROR_MESSAGE));
		exit(0);
	}
	// inside parent process
	if ((pid2  = fork()) < 0) {
		write(2, FORK_ERROR_MESSAGE, strlen(FORK_ERROR_MESSAGE));
		exit(FORK_ERROR);
	}
	else if (pid2 == 0) { // inside son process 2
		// change standard write fd
		if (dup2(fda[1], 1) < 0) {
			write(2, DUP_ERROR_MESSAGE, strlen(DUP_ERROR_MESSAGE));
			exit(DUP_ERROR);
		}
		if (close(fda[0]) < 0) {
			write(2, CLOSE_ERROR_MESSAGE, strlen(CLOSE_ERROR_MESSAGE));
			exit(CLOSE_ERROR);
		}
		if (close(fda[1]) < 0) {
			write(2, CLOSE_ERROR_MESSAGE, strlen(CLOSE_ERROR_MESSAGE));
			exit(CLOSE_ERROR);
		}
		ConvertIntToString(pid1, buffer);
		if (execlp("ex2_upd.out", "ex2_upd.out", buffer, NULL) < 0) {
			write(2, EXECUTE_ERROR_MESSAGE,
					strlen(EXECUTE_ERROR_MESSAGE));
			exit(EXECUTE_ERROR);
		}
		write(2, EXECUTE_ERROR_MESSAGE,	strlen(EXECUTE_ERROR_MESSAGE));
		exit(0);
	}
	// parent process:
	InitActions();
	time = ConvertStringToInt(argv[1]);
	if (alarm(time) < 0) {
		write(2, ALARM_ERROR_MESSAGE, strlen(ALARM_ERROR_MESSAGE));
		exit(ALARM_ERROR);
	}

	while(1) {
		pause();
	}
	return 0;
}

/***********************************************
* Function name: ExitSignalHandler
* The input: number of signal
* The output: none
* The Function operation: send exit signal to
*		son processes and exit
***********************************************/
void ExitSignalHandler(int signalNumber) {
	if (kill(pid2, SIGINT) < 0) {
		write(2, SIGNAL_SEND_ERROR_MESSAGE,
				strlen(SIGNAL_SEND_ERROR_MESSAGE));
		exit(SIGNAL_SEND_ERROR);
	}
	if (kill(pid1, SIGINT) < 0) {
		write(2, SIGNAL_SEND_ERROR_MESSAGE,
				strlen(SIGNAL_SEND_ERROR_MESSAGE));
		exit(SIGNAL_SEND_ERROR);
	}
	sleep(1);
	if (close(fda[0]) < 0) {
		write(2, CLOSE_ERROR_MESSAGE,
				strlen(CLOSE_ERROR_MESSAGE));
		exit(CLOSE_ERROR);
	}
	if (close(fda[1]) < 0) {
		write(2, CLOSE_ERROR_MESSAGE,
				strlen(CLOSE_ERROR_MESSAGE));
		exit(CLOSE_ERROR);
	}
	if (write(1, BYE_MESSAGE, strlen(BYE_MESSAGE)) < 0) {
		write(2, WRITE_ERROR_MESSAGE, strlen(WRITE_ERROR_MESSAGE));
		exit(WRITE_ERROR);
	}
	exit(0);
}

/***********************************************
* Function name: InitActions
* The input: none
* The output: none
* The Function operation: initialize the actions
*		of the process
***********************************************/
void InitActions() {
	struct sigaction finishAction;
	sigset_t blockMask;

	// block whole signals except of SIGINT signal
	sigfillset(&blockMask);
	sigdelset(&blockMask, SIGINT);

	// initialize board action
	finishAction.sa_handler = ExitSignalHandler;
	finishAction.sa_mask = blockMask;
	finishAction.sa_flags = 0;
	if (sigaction(SIGALRM, &finishAction, NULL) != 0) {
		write(2, SIGACTION_ERROR_MESSAGE,
				strlen(SIGACTION_ERROR_MESSAGE));
		exit(SIGACTION_ERROR);
	}
	if (sigaction(SIGINT, &finishAction, NULL) != 0) {
		write(2, SIGACTION_ERROR_MESSAGE,
				strlen(SIGACTION_ERROR_MESSAGE));
		exit(SIGACTION_ERROR);
	}
}

/***********************************************
* Function name: ConvertStringToInt
* The input: string
* The output: integer
* The Function operation: returns the integer
*		that found in the given string
***********************************************/
int ConvertStringToInt(char *numberString) {
	int result = 0, isNegative = 0;
	int i = 0, length = strlen(numberString);
	if (numberString[0] == '-') {
		isNegative = 1;
		i++;
	}
	for (; i < length; i++) {
		if (numberString[i] >= '0' && numberString[i] <= '9') {
			result = result * 10 + (numberString[i] - '0');
		} else { // means we got not a number character
			break; // will return the number we got before the character
		}
	}
	if (isNegative) {
		return result * -1;
	}
	return result;
}

/***********************************************
* Function name: ConvertIntToString
* The input: number and string
* The output: none
* The Function operation: fill the string with
*		the number's digits. We assume there
*		are maximum 4 digits.
***********************************************/
void ConvertIntToString(int number, char *numberString) {
	int num, digitsNumber = 0, i;
	char temp;
	// put all digits in reverse order
	if (number != 0) {
		for (num = number; num != 0; num = num / 10) {
			numberString[digitsNumber++] = (num % 10) + '0';
		}
	}
	else {
		numberString[0] = '0';
		digitsNumber = 1;
	}
	// reverse string
	for (i = 0; i <= digitsNumber / 2 - 1; i++) {
		temp = numberString[i];
		numberString[i] = numberString[digitsNumber - 1 - i];
		numberString[digitsNumber - 1 - i] = temp;
	}
	numberString[digitsNumber] = '\0';
}
