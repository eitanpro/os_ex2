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
#include <unistd.h>
#include <termios.h>
#include <stdio.h>

#define BOARD_SIZE 4
#define ELEMENTS_NUMBER (BOARD_SIZE * BOARD_SIZE)
#define MAX_BOARD_DESCRIPTION_SIZE (BOARD_SIZE * BOARD_SIZE * 4 + 15)
#define PARAMETER_NUMBER_ERROR_MESSAGE \
"expect to get one parameter to main function\n"
#define PARAMETER_NUMBER_ERROR 1
#define SIGACTION_ERROR_MESSAGE "sigaction error\n"
#define SIGACTION_ERROR 2
#define ALERM_ERROR_MESSAGE "interapt alarm waiting\n"
#define ALERM_ERROR 3
#define WRITE_ERROR_MESSAGE "write error\n"
#define WRITE_ERROR 4
#define SIGNAL_SEND_ERROR_MESSAGE "send signal error\n"
#define SIGNAL_SEND_ERROR 5
#define READ_ERROR_MESSAGE "read error\n"
#define READ_ERROR 6

typedef enum {UP, RIGHT, DOWN, LEFT} Direction;

typedef struct
{
	int board[BOARD_SIZE][BOARD_SIZE];
	int waitingTime;
} Board;

void InitBoard(Board *board);
void PrintBoard(Board *board);
void MoveToDirection(Board *board, Direction direction);
void ShiftRowLeft(Board *board, int index);
void ShiftRowRight(Board *board, int index);
void CompresRow(Board *board, int index);
void Transpose(Board *board);
void AddRandomCell(Board *board);
void AlarmAction(int signalNumber);
void InitAlarmAction();
void ConvertIntToString(int number, char *numberString);
int ConvertStringToInt(char *numberString);
int GetRandomBetween(int min, int max);
char GetChar();

Board board;
int pid;

int main(int argc, char *argv[]) {
	if (argc != 2) { // if get only one parameter extra
		write(2, PARAMETER_NUMBER_ERROR_MESSAGE, 
				strlen(PARAMETER_NUMBER_ERROR_MESSAGE));
		exit(PARAMETER_NUMBER_ERROR);
	}

	pid = ConvertStringToInt(argv[1]);

	InitAlarmAction(); // initialize alarm
	InitBoard(&board); // initialize board

	while (1) {
		char ch = GetChar();
		switch(ch) {
		case 'w':
		case 'W':
			MoveToDirection(&board, UP);
			break;
		case 'd':
		case 'D':
			MoveToDirection(&board, RIGHT);
			break;
		case 'x':
		case 'X':
			MoveToDirection(&board, DOWN);
			break;
		case 'a':
		case 'A':
			MoveToDirection(&board, LEFT);
			break;
		case 's':
		case 'S':
			if (kill(pid, SIGUSR2) < 0) { // send SIGUSER2 signal
				write(2, SIGNAL_SEND_ERROR_MESSAGE,
						strlen(SIGNAL_SEND_ERROR_MESSAGE));
				exit(SIGNAL_SEND_ERROR);
			}
			InitBoard(&board); // initialize board
			break;
		default:
			// do nothing
			break;
		}
	}

	return 0;
}

/***********************************************
* Function name: InitBoard
* The input: board
* The output: none
* The Function operation: initialize the board
*		with two cells of 2
***********************************************/
void InitBoard(Board *board) {
	int leftTime;
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			board->board[i][j] = 0;
		}
	}
	// add two elem in random positions
	AddRandomCell(board);
	AddRandomCell(board);
	
	board->waitingTime = GetRandomBetween(1, 5);
	leftTime = alarm(board->waitingTime);
	if (leftTime != 0) {
		// means we already had been played the game
		// and we started a new game
	}

	PrintBoard(board);

	if (kill(pid, SIGUSR1) < 0) {
		write(2, SIGNAL_SEND_ERROR_MESSAGE, 
				strlen(SIGNAL_SEND_ERROR_MESSAGE));
		exit(SIGNAL_SEND_ERROR);
	}
}

/***********************************************
* Function name: PrintBoard
* The input: board
* The output: none
* The Function operation: prints the board
***********************************************/
void PrintBoard(Board *board) {
	char description[MAX_BOARD_DESCRIPTION_SIZE + 1];
	char numberString[5];
	int i, j;

	ConvertIntToString(board->board[0][0], numberString);
	strcpy(description, numberString);
	for (i = 1; i < BOARD_SIZE * BOARD_SIZE; i++) {
		strcat(description, ",");
		ConvertIntToString(board->board[0][i], numberString);
		strcat(description, numberString);
	}

	if (write(1, description, strlen(description)) < 0) {
		write(2, WRITE_ERROR_MESSAGE, strlen(WRITE_ERROR_MESSAGE));
		exit(WRITE_ERROR);
	}
	if (write(1, "\n", 1) < 0) {
		write(2, WRITE_ERROR_MESSAGE, strlen(WRITE_ERROR_MESSAGE));
		exit(WRITE_ERROR);
	}
}

/***********************************************
* Function name: MoveToDirection
* The input: board and direction
* The output: none
* The Function operation: move all cells to the
*		selected direction
***********************************************/
void MoveToDirection(Board *board, Direction direction) {
	int i, leftTime;

	switch (direction) {
	case LEFT:
		for (i = 0; i < BOARD_SIZE; i++) {
			ShiftRowLeft(board, i);
			CompresRow(board, i);
			ShiftRowLeft(board, i);
		}
		break;
	case RIGHT:
		for (i = 0; i < BOARD_SIZE; i++) {
			ShiftRowRight(board, i);
			CompresRow(board, i);
			ShiftRowRight(board, i);
		}
		break;
	case UP:
		Transpose(board);
		for (i = 0; i < BOARD_SIZE; i++) {
			ShiftRowLeft(board, i);
			CompresRow(board, i);
			ShiftRowLeft(board, i);
		}
		Transpose(board);
		break;
	case DOWN:
		Transpose(board);
		for (i = 0; i < BOARD_SIZE; i++) {
			ShiftRowRight(board, i);
			CompresRow(board, i);
			ShiftRowRight(board, i);
		}
		Transpose(board);
		break;
	default:
		break;
	}

	board->waitingTime = GetRandomBetween(1, 5);
	leftTime = alarm(board->waitingTime);
	if (leftTime != 0) {
		// means we already had been played the game
		// and we started a new game
	}

	PrintBoard(board);

	if (kill(pid, SIGUSR1) < 0) {
		write(2, SIGNAL_SEND_ERROR_MESSAGE, 
				strlen(SIGNAL_SEND_ERROR_MESSAGE));
		exit(SIGNAL_SEND_ERROR);
	}
}

/***********************************************
* Function name: ShiftRowLeft
* The input: board and index
* The output: none
* The Function operation: move all cell in the
*		selected row to left
***********************************************/
void ShiftRowLeft(Board *board, int index) {
	int j, left = 0;
	for (j = 0; j < BOARD_SIZE; j++) {
		if (board->board[index][j] != 0) {
			if (j != left) {
				board->board[index][left] = board->board[index][j];
				board->board[index][j] = 0;
			}
			++left;
		}
	}
}

/***********************************************
* Function name: ShiftRowRight
* The input: board and index
* The output: none
* The Function operation: move all cell in the
*		selected row to right
***********************************************/
void ShiftRowRight(Board *board, int index) {
	int j, right = BOARD_SIZE - 1;
	for (j = BOARD_SIZE - 1; j >= 0; j--) {
		if (board->board[index][j] != 0) {
			if (j != right) {
				board->board[index][right] = board->board[index][j];
				board->board[index][j] = 0;
			}
			--right;
		}
	}
}

/***********************************************
* Function name: CompresRow
* The input: board and row index
* The output: none
* The Function operation: merge cell with the
*		the same value
***********************************************/
void CompresRow(Board *board, int index) {
	int j;
	for (j = 0; j < BOARD_SIZE - 1; j++) {
		if (board->board[index][j] == board->board[index][j + 1]) {
			board->board[index][j] += board->board[index][j + 1];
			board->board[index][j + 1] = 0;
		}
	}
}

/***********************************************
* Function name: Transpose
* The input: board
* The output: none
* The Function operation: transpose the board
***********************************************/
void Transpose(Board *board) {
	int i, j, temp;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < i; j++) {
			temp = board->board[i][j];
			board->board[i][j] = board->board[j][i];
			board->board[j][i] = temp;
		}
	}
}

/***********************************************
* Function name: AddRandomCell
* The input: board
* The output: none
* The Function operation: add random cell if
*		there is a place for it
***********************************************/
void AddRandomCell(Board *board) {
	int position = GetRandomBetween(0, ELEMENTS_NUMBER - 1);
	int i, isFullFlag = 1;
	for (i = 0; i < ELEMENTS_NUMBER && isFullFlag; i++) {
		isFullFlag = (board->board[0][i] != 0);
	}
	if (!isFullFlag) {
		while (board->board[0][position] != 0) {
			position = GetRandomBetween(0, ELEMENTS_NUMBER - 1);
		}
		board->board[0][position] = 2;
	}
}

/***********************************************
* Function name: AlarmAction
* The input: number of signal
* The output: none
* The Function operation: add random cell
*		and prints the new board
***********************************************/
void AlarmAction(int signalNumber) {
	char ch[20];

	AddRandomCell(&board);
	board.waitingTime = GetRandomBetween(1, 5);
	if (alarm(board.waitingTime) != 0) {
		// means someone send us this signal
		write(2, ALERM_ERROR_MESSAGE, strlen(ALERM_ERROR_MESSAGE));
		exit(ALERM_ERROR);
	}

	PrintBoard(&board);

	if (kill(pid, SIGUSR1) < 0) {
		write(2, SIGNAL_SEND_ERROR_MESSAGE, 
				strlen(SIGNAL_SEND_ERROR_MESSAGE));
		exit(SIGNAL_SEND_ERROR);
	}
}

/***********************************************
* Function name: InitAlarmAction
* The input: none
* The output: none
* The Function operation: initialize alarm actions
***********************************************/
void InitAlarmAction() {
	struct sigaction alarmAction;
	sigset_t blockMask;

	// block whole signals except of SIGINT signal
	sigfillset(&blockMask);
	sigdelset(&blockMask, SIGINT);

	// initialize board action
	alarmAction.sa_handler = AlarmAction;
	alarmAction.sa_mask = blockMask;
	alarmAction.sa_flags = 0;
	if (sigaction(SIGALRM, &alarmAction, NULL) != 0) {
		write(2, SIGACTION_ERROR_MESSAGE,
				strlen(SIGACTION_ERROR_MESSAGE));
		exit(SIGACTION_ERROR);
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
* Function name: GetRandomBetween
* The input: two integers
* The output: integer
* The Function operation: return random number
*		in range between min and max
***********************************************/
int GetRandomBetween(int min, int max) {
	return (rand() % (max - min + 1) + min);
}

/***********************************************
* Function name: GetChar
* The input: none
* The output: character
* The Function operation: returns character
*		inputs from the user
***********************************************/
char GetChar() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0) {
        perror("tcsetattr()");
    }
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0) {
        perror("tcsetattr ICANON");
    }
    if (read(0, &buf, 1) < 0) {
        // means read failed
    }
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) {
        perror ("tcsetattr ~ICANON");
    }
    return (buf);
}
