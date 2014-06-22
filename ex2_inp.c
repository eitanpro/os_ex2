/***********************************************
* @eitanpro
* ex2
***********************************************/

#include <string.h> // using strlen
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMBER_OF_DIGITS 4
#define BOARD_SIZE 16
#define BYE_MESSAGE "BYE BYE\n"

#define SIGACTION_ERROR_MESSAGE "sigaction error\n"
#define SIGACTION_ERROR 2
#define WRITE_ERROR_MESSAGE "write error\n"
#define WRITE_ERROR 3
#define READ_ERROR_MESSAGE "read error\n"
#define READ_ERROR 4

void ExitSignalHandler(int signalNumber);
void DoNothingHandler(int signalNumber);
void BoardSignalHandler(int signalNumber);
void InputBoard(int *board);
void PrintBoard(int *board);
void ConvertIntToString(int number, char *numberString);

int finishRunFlag = 0;

int main() {
	struct sigaction boardAction, exitAction, doNothingAction;
	sigset_t boardBlockMask, blockAllMask;

	// block whole signals except of SIGINT signal
	sigfillset(&boardBlockMask);
	sigdelset(&boardBlockMask, SIGINT);

	// initialize board action
	boardAction.sa_handler = BoardSignalHandler;
	boardAction.sa_mask = boardBlockMask;
	boardAction.sa_flags = 0;
	if (sigaction(SIGUSR1, &boardAction, NULL) != 0) {
		write(2, SIGACTION_ERROR_MESSAGE,
				strlen(SIGACTION_ERROR_MESSAGE));
		exit(SIGACTION_ERROR);
	}

	// block whole signals
	sigfillset(&blockAllMask);

	// initialize exit action
	exitAction.sa_handler = ExitSignalHandler;
	exitAction.sa_mask = blockAllMask;
	exitAction.sa_flags = 0;
	if (sigaction(SIGINT, &exitAction, NULL) != 0) {
		write(2, SIGACTION_ERROR_MESSAGE,
				strlen(SIGACTION_ERROR_MESSAGE));
		exit(SIGACTION_ERROR);
	}

	doNothingAction.sa_handler = DoNothingHandler;
	doNothingAction.sa_mask = blockAllMask;
	doNothingAction.sa_flags = 0;
	if (sigaction(SIGUSR2, &doNothingAction, NULL) != 0) {
		write(2, SIGACTION_ERROR_MESSAGE,
				strlen(SIGACTION_ERROR_MESSAGE));
		exit(SIGACTION_ERROR);
	}

	while(!finishRunFlag) {
		sleep(1);
	}

	return 0;
}

/***********************************************
* Function name: ExitSignalHandler
* The input: number of signal
* The output: none
* The Function operation: write bye message
*		and finish the program
***********************************************/
void ExitSignalHandler(int signalNumber) {
	if (write(1, BYE_MESSAGE, strlen(BYE_MESSAGE)) < 0) {
		write(2, WRITE_ERROR_MESSAGE, strlen(WRITE_ERROR_MESSAGE));
		exit(WRITE_ERROR);
	}
	finishRunFlag = 1;
	exit(0);
}

/***********************************************
* Function name: DoNothingHandler
* The input: number of signal
* The output: none
* The Function operation: do nothing
***********************************************/
void DoNothingHandler(int signalNumber) {
	// do nothing
}

/***********************************************
* Function name: BoardSignalHandler
* The input: number of signal
* The output: none
* The Function operation: inputs board from user
*		and prints it
***********************************************/
void BoardSignalHandler(int signalNumber) {
	int board[BOARD_SIZE];
	InputBoard(board);
	PrintBoard(board);
}

/***********************************************
* Function name: InputBoard
* The input: array of integers
* The output: none
* The Function operation: fill the array by
*		the user's input
***********************************************/
void InputBoard(int *board) {
	int index = 0, size, i, info = 0;
	char ch;
	do {
		if ((size = read(0, &ch, 1)) < 0) {
			write(2, READ_ERROR_MESSAGE, strlen(READ_ERROR_MESSAGE));
			exit(READ_ERROR);
		}
		if (ch >= '0' && ch <= '9') {
			info = info * 10 + (ch - '0');
		} else if (ch == ',' || ch == '\n') {
			board[index++] = info; // add info to board
			info = 0; // erase old info
		}
	} while (ch != '\n');
}

/***********************************************
* Function name: PrintBoard
* The input: array of integers
* The output: none
* The Function operation: prints the array in
*		game board format
***********************************************/
void PrintBoard(int *board) {
	int i;
	char buffer[NUMBER_OF_DIGITS + 1];
	for (i = 0 ; i < BOARD_SIZE; i++) {
		if (board[i] == 0) {
			if (write(1, "|    ", 5) < 0) {
				write(2, WRITE_ERROR_MESSAGE, strlen(WRITE_ERROR_MESSAGE));
				exit(WRITE_ERROR);
			}
		} else {
			ConvertIntToString(board[i], buffer);
			if (write(1, "|" ,1) < 0) {
				write(2, WRITE_ERROR_MESSAGE, strlen(WRITE_ERROR_MESSAGE));
				exit(WRITE_ERROR);
			}
			if (write(1, buffer, strlen(buffer)) < 0) {
				write(2, WRITE_ERROR_MESSAGE, strlen(WRITE_ERROR_MESSAGE));
				exit(WRITE_ERROR);
			}
		}
		if (i % 4 == 3) {
			if (write(1, "|\n", 2) < 0) {
				write(2, WRITE_ERROR_MESSAGE, strlen(WRITE_ERROR_MESSAGE));
				exit(WRITE_ERROR);
			}
		}
	}
	if (write(1, "\n", 1) < 0) {
		write(2, WRITE_ERROR_MESSAGE, strlen(WRITE_ERROR_MESSAGE));
		exit(WRITE_ERROR);
	}
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
	int i;
	for (i = 0; i < NUMBER_OF_DIGITS; i++, number = number / 10) {
		numberString[NUMBER_OF_DIGITS - 1 - i] = (number % 10) + '0';
	}
	numberString[i] = '\0';
}
