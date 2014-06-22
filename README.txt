It is a 2048 multiprocessor version for Linux users!!!


ex2_inp.c gets board string from the console. (*) the input format is:
2,4,0,0,2,2,0,16,0,0,4,0,16,0,16,0 (for example)
and prints the board 4x4 like this:
0002	|0004	|	|	|
	|0002	|0002	|	|0016	|
	|	|	|0004	|	|
	|0016	|	|0016	|	|

ex2_upd.c after random time between 1-5 seconds shows new cell with value of 2.
user can use the keyboard to make a move:
a - up
d - right
x - down
a - left
s - new random board with two random cells 2
after each change we prints to console the board state in the format above (*)

ex2.c gets as parameter to the program the game duration in seconds. after this time the game will finish.
this program also connect between the two programs and assume they were compiled before with the following names: ex2_inp.out and ex2_upd.out and found in the current path so it can executed by execl.
(I compiled me program in u2 server. If you need to compile it on Linux use gcc command with -o flag).

yours,
@eitanpro
