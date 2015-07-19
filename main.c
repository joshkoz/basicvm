//
// Name: 'basicvm'
// author: Joshua Kozminska
// Last Modified: 8 May 2015
//
// Description: 'basicvm' is a really simple virtual machine written
// in C and is only one small file.
//
//   register    purpose
//   -------------------------------
//   A           general purpose
//   B           general purpose
//   C           general purpose
//   D           general purpose
//   E           general purpose
//   F           general purpose
//   IP          instruction pointer
//   SP          stack pointer
//
//   op   val    function
//   ---------------------------------------------------------------------
//   HLT  0      halts the program
//   PSH  1      pushes the next instruction
//   POP  2      pops the stack
//   ADD  3      pops stack twice, adds result pushes to stack
//   MUL  4      pops stack twice, multiplies the result pushes to stack
//   DIV  5      pops stack twice, divides result pushes to stack
//   SUB  6      pops stack twice, substracts result pushes to stack
//   NOP  7      nothing

//TEST SET OF INSTRUCTIONS TO PROVIDE TO VM;
#define instructions test_b


#include <stdbool.h>
#include <stdio.h>


#define STACK_SIZE 256 
static int stack[STACK_SIZE];

//Registers
typedef enum 
{
	A,
	B, 
	C, 
	D, 
	E, 
	F,
	I,
	J,
	EX,
	EXA,
	IP,
	SP,
	REGISTER_SIZE
} Registers;

int registers[REGISTER_SIZE];

//Instruction Enumerator
typedef enum 
{
	HLT, // 0 -- hlt :: halts program
	PSH, // 1 -- psh val :: pushes <val> to stack
	POP, // 2 -- pop :: pops value from stack
	ADD, // 3 -- add :: adds top two vals on stack
	MUL, // 4 -- mul :: multiplies top two vals on stack
	DIV, // 5 -- div :: divides top two vals on stack
	SUB, // 6 -- sub :: subtracts top two vals on stack
	MOV, // 7 -- mov reg_a, reg_b :: movs the value in reg_a to reg_b 
	SET, // 8 -- set reg, val :: sets the reg to value
	LOG, // 9 -- log a :: prints out a
	IF,  // 10 -- if reg val ip :: if the register == val branch to the ip
	IFN, // 11 -- ifn reg val ip :: if the register != val branch to the ip
	GLD, // 10 -- gld reg :: loads a register to the stack
	GPT, // 11 -- gpt reg :: pushes top of stack to the given register
	NOP  // 12 -- nop :: nothing
} InstructionSet;

bool running = true;


//Test Set One;
int test_a[] = {
	PSH, 5,
	PSH, 2,
	ADD,
	PSH, 10,
	PSH, 12,
	ADD,
	ADD,
	POP,
	HLT
};

// testing multiplication
int test_b[] = {
	PSH, 5,
	PSH, 2,
	MUL,
	PSH, 10,
	PSH, 20,
	ADD,
	ADD,
	HLT
};

/** testing set and move */
int test_c[] = {
	SET, A, 21,		// 3
	GLD, A,			// 5
	PSH, 1,			// 7
	SUB,			// 8
	GPT, A,			// 10
	IFN, A, 0, 2,	// 14
	LOG, A,			// 16
	LOG, B,
	HLT				// 17
};


#define SP (registers[SP])
#define IP (registers[IP])

#define FETCH (instructions[IP])


void print_stack()
{
	for (int i = 0; i < SP; i++)
	{
		printf("0x%04d ", stack[i]);
		if ((i + 1) % 4 == 0) printf("\n");
	}
	if (SP != 0)
	{
		printf("\n");
	}
}


void print_registers()
{
	printf("register_dump\n");
	for (int i = 0; i < REGISTER_SIZE; i++)
	{
		printf("%04d ", registers[i]);
		if ((i + 1) % 4 == 0) printf("\n");
	}
}

int find_empty_register() {
	for (int i = 0; i < REGISTER_SIZE; i++) {
		if (i != registers[EX] && i != registers[EXA]) {
			return i;
		}
	}
	return EX;
}


void eval(int instr)
{
	switch(instr)
	{
		case HLT:
		{
			running = false;
			printf("Finished Execution\n");
			//print_stack(0 , 16);
			//print_registers();
			break;
		}
		case PSH: 
		{
			SP = SP + 1;
			IP = IP + 1;
			stack[SP] = instructions[IP];
			break;
		}
		case POP:
		{
			SP = SP - 1;
			break;
		}
		case IF: 
		{
			if(registers[instructions[IP + 1]] == instructions[IP + 2])
			{
				registers[IP + 1] = instructions[IP + 3];
			}
			IP = IP + 3;
			break;
		}
		case IFN:
		{
			if (registers[instructions[IP + 1]] != instructions[IP + 2])
			{
				registers[EX] = instructions[IP + 3];
				registers[IP] = registers[EX];
			}
			else
			{
				IP = IP + 3;
			}
			break;

		}
		case MOV:
		{
			registers[instructions[IP + 2]] = registers[instructions[IP + 1]];
			IP = IP + 2;
			break;
		}
		case SET: 
		{
			registers[instructions[IP + 1]] = instructions[IP + 1];
			IP = IP + 2;
			break;
		}
		case GLD:
		{
			SP = SP + 1;
			IP = IP + 1;
			stack[SP] = registers[instructions[IP]];
			break;
		}
		case GPT:
		{
			registers[instructions[IP + 1]] = stack[SP];
			IP = IP + 1;
			break;
		}
		case LOG:
		{
			printf("%d\n", registers[instructions[IP + 1]]);
			IP = IP + 1;
			break;
		}
		case ADD:
		{
			registers[A] = stack[SP];
			SP = SP - 1;
			registers[B] = stack[SP];
			SP = SP - 1;

			registers[C] = registers[A] + registers[B];
			SP = SP + 1;

			stack[SP] = registers[C];
			printf("%d + %d = %d\n",registers[B], registers[A], registers[C] );
			break;
		}
		case SUB:
		{
			registers[A] = stack[SP];
			SP = SP - 1;
			registers[B] = stack[SP];
			SP = SP - 1;

			registers[C] = registers[A] - registers[B];
			SP = SP + 1;

			stack[SP] = registers[C];
			printf("%d - %d = %d \n",registers[B], registers[A], registers[C] );
			break;
		}
		case DIV:
		{
			registers[A] = stack[SP];
			SP = SP - 1;
			registers[B] = stack[SP];
			SP = SP - 1;

			registers[C] = registers[A] / registers[B];
			SP = SP + 1;

			stack[SP] = registers[C];
			printf("%d / %d = %d\n",registers[B], registers[A], registers[C] );
			break;
		}
		case MUL:
		{
			registers[A] = stack[SP];
			SP = SP - 1;
			registers[B] = stack[SP];
			SP = SP - 1;

			registers[C] = registers[A] * registers[B];
			SP = SP + 1;

			stack[SP] = registers[C];
			printf("%d * %d = %d\n",registers[B], registers[A], registers[C] );
			break;
		}
		case NOP:
		{
			printf("do nothing\n");
			break;
		}
	}
}


int main ()
{
	SP = -1;

	while (running)
	{
		eval(FETCH);
		IP = IP + 1;
	}

	return 0;

}

