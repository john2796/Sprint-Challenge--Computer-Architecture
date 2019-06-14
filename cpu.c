#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_LEN 6
#define SP 7

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, int argc, char *argv[]) // make ./ls8 examples/print8.ls8
{
  // TODO: Replace this with something less hard-coded
  if (argc < 2)
  {
    printf("File does not exist.\n");
    exit(1);
  }

  char *file = argv[1];
  FILE *fp = fopen(file, "r");

  if (fp == NULL)
  {
    printf("File does not exist");
    exit(1);
  }
  else
  {
    char file_line[1024];
    int address = 0;

    while (fgets(file_line, sizeof(file_line), fp) != NULL)
    {
      char *ptr;
      unsigned char return_value;
      return_value = strtol(file_line, &ptr, 2);
      if (file_line == NULL)
      {
        continue;
      }

      cpu->ram[address] = return_value;
      address++;
    }
  }
  fclose(fp);
}

unsigned char cpu_ram_read(struct cpu *cpu, unsigned char memadr)
{
  return cpu->ram[memadr];
}

// cpu write ram
// would have to take a cpu struct, an index for ram, and char value
void cpu_ram_write(struct cpu *cpu, unsigned char memadr, unsigned char value)
{
  cpu->ram[memadr] = value;
}

// cpu push
void cpu_push(struct cpu *cpu, unsigned char value)
{
  // decrement stack pointer
  cpu->reg[SP]--;
  cpu_ram_write(cpu, cpu->reg[SP], value);
}

// cpu pop
unsigned char cpu_pop(struct cpu *cpu)
{
  unsigned char ret = cpu->ram[cpu->reg[SP]];
  cpu->reg[SP]++;
  return ret;
}

//

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op)
  {
  case ALU_MUL:
    // TODO
    cpu->reg[regA] *= cpu->reg[regB];
    break;

  // TODO: implement more ALU ops
  case ALU_ADD:
    cpu->reg[regA] += cpu->reg[regB];
    break;

  case ALU_CMP:
    if (cpu->reg[regA] == cpu->reg[regB])
    {
      cpu->FL = 1;
    }
    else if (cpu->reg[regA] > cpu->reg[regB])
    {
      cpu->FL = 0;
    }
    break;
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; 
  unsigned char operandA;
  unsigned char operandB;

  while (running)
  {
    // TODO
    unsigned char instruction = cpu->ram[cpu->PC];
    unsigned int num_operands = instruction >> 6;

    if (num_operands == 2)
    {
      operandA = cpu_ram_read(cpu, (cpu->PC + 1) & 0xff);
      operandB = cpu_ram_read(cpu, (cpu->PC + 2) & 0xff);
    }
    else if (num_operands == 1)
    {
      operandA = cpu_ram_read(cpu, (cpu->PC + 1) & 0xff);
    }

    switch (instruction)
    {
    case HLT:
      running = 0;
      break;

    case PRN:
      printf("%d\n", cpu->reg[operandA]);
      break;

    case LDI:
      cpu->reg[operandA] = operandB;
      break;

    case MUL:
      alu(cpu, ALU_MUL, operandA, operandB);
      break;

    case PUSH:
      cpu_push(cpu, cpu->reg[operandA]);
      break;

    case POP:
      cpu->reg[operandA] = cpu_pop(cpu);
      break;

    case ADD:
      alu(cpu, ALU_ADD, operandA, operandB);
      break;

    case CALL:
      cpu_push(cpu, cpu->PC + 1);
      cpu->PC = cpu->reg[operandA] - 1;
      break;

    case RET:
      cpu->PC = cpu_pop(cpu);
      break;

    case CMP:
      alu(cpu, ALU_CMP, operandA, operandB);
      break;

    case JMP:
      cpu->PC = cpu->reg[operandA];
      cpu->PC += 1;
      break;

    case JEQ:
      if (cpu->FL == 1)
      {
        cpu->PC = cpu->reg[operandA];
        cpu->PC -= 1;
      }
      break;

    case JNE:
      if (cpu->FL != 1)
      {
        cpu->PC = cpu->reg[operandA];
        cpu->PC -= 1;
      }
      break;
      
    default:
      break;
    }
    cpu->PC += num_operands + 1;
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  cpu->PC = 0;
  cpu->FL = 0;

  cpu->reg[SP] = ADDR_EMPTY_STACK;

  memset(cpu->reg, 0, sizeof(cpu->reg));
  memset(cpu->ram, 0, sizeof(cpu->ram));
}