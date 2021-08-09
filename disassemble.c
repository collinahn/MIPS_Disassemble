#include <stdio.h>
#define M_SIZE 1024

unsigned char MEM[M_SIZE];
unsigned char marr_Buf[4];
unsigned int mn_Instruction, mn_Data;
union InstructionRegister
{
    unsigned int I;
    struct Rformat
    {
        unsigned int funct  : 6;
        unsigned int sh     : 5;
        unsigned int rd     : 5;
        unsigned int rt     : 5;
        unsigned int rs     : 5;
        unsigned int opcode : 6;
    } RI;
    struct Iformat
    {
        int offset          : 16;
        unsigned int rt     : 5;
        unsigned int rs     : 5;
        unsigned int opcode : 6;
    } II;
    struct Jformat
    {
        unsigned int address : 26;
        unsigned int opcode  : 6;
    } JI;
} IR;

unsigned int SaveBigEndian(unsigned char *arr);
void InstEncoding(const unsigned int x);
void DisassembleReg(const unsigned int x);

int main()
{
    FILE *p_File = NULL;
    errno_t e_Errno;
    char s_FileName[20] = {
        0,
    };

    printf("File Name: ");
    gets_s(s_FileName, sizeof(s_FileName));
    printf("------------------\n");

    e_Errno = fopen_s(&p_File, s_FileName, "rb");
    if (e_Errno)
    {
        printf("Cannot open file\n");
        return 1;
    }

    fread(marr_Buf, sizeof(marr_Buf[0]), 4, p_File);
    mn_Instruction = SaveBigEndian(marr_Buf);

    fread(marr_Buf, sizeof(marr_Buf[0]), 4, p_File);
    mn_Data = SaveBigEndian(marr_Buf);

    fread(MEM, sizeof(MEM[0]), (mn_Instruction + mn_Data) * 4, p_File);
    for (unsigned int i = 0; i < mn_Instruction * 4; i += 4)
    {
        IR.I = SaveBigEndian(&MEM[i]);
        InstEncoding(IR.I);
        DisassembleReg(IR.I);
        printf("\n");
    }

    fclose(p_File);

    return 0;
}

unsigned int SaveBigEndian(unsigned char *arr)
{
    unsigned int n = 0;
    for (int j = 0; j < 4; j++)
    {
        n += (unsigned int)arr[j] << (24 - 8 * j);
    }

    return n;
};

void InstEncoding(const unsigned int x)
{
    if (IR.RI.opcode == 0)
    {
        char* s_FunctionList[50] = {"sll", "", "srl", "sra", "", "", "", "", "jr", "", 
                                    "", "", "syscall", "", "", "", "mfhi", "", "mflo", "",
                                    "", "", "", "", "mul", "", "", "", "", "",
                                    "", "", "add", "", "sub", "", "and", "or", "xor", "nor",
                                    "", "", "slt", "", "", "", "", "", "", "" };
        
        if (IR.RI.funct < 50 && IR.RI.funct >= 0) {
            char* s_Command = s_FunctionList[IR.RI.funct];
            printf("%s", s_Command);
        } else {
            printf("Unknown function");
        }
    }
    else
    {
        char* s_OpcodeList[50] =  { "", "bltz", "j", "jal", "beq", "bne", "", "", "bne", "", 
                                    "slti", "", "andi", "ori", "xori", "lui", "", "", "", "",
                                    "", "", "", "", "", "", "", "", "", "",
                                    "", "", "lb", "", "", "lw", "lbu", "", "", "",
                                    "sb", "", "", "sw", "", "", "", "", "", "" };

        if (IR.RI.opcode < 50 && IR.RI.opcode >= 0) {
            char* s_Command = s_OpcodeList[IR.RI.opcode];
            printf("%s", s_Command);
        } else {
            printf("Unknown opcode");
        }
    }
    printf(" ");

    return;
};

void DisassembleReg(const unsigned int x)
{
    unsigned int n_InitialProgramCounter = 0x00400020;
    if (IR.RI.opcode == 0)
    {
        switch (IR.RI.funct)
        {
        case 0:
        case 2:
        case 3:
            printf("$%d, $%d, %d", IR.RI.rd, IR.RI.rt, IR.RI.sh);   //case of sll, srl, sra
            break;
        case 8:
            printf("$%d", IR.RI.rs);                                //case of jr
            break;
        case 12:
            break;                                                  //case of syscall
        default:
            printf("$%d, $%d, $%d", IR.RI.rd, IR.RI.rs, IR.RI.rt);  //Rformat disassemble
            break;
        }
    }
    else if (IR.RI.opcode == 2 || IR.RI.opcode == 3)
        printf("0x%08x", ((n_InitialProgramCounter >> 28) << 28) + ((unsigned int)IR.JI.address << 2)); //Jformat disassemble
    else
    {
        switch (IR.RI.opcode)
        {
        case 1:
            printf("$%d, $%d", IR.II.rs, IR.II.offset * 4);
            break;
        case 4:
        case 5:
            printf("$%d, $%d, %d", IR.II.rt, IR.II.rs, IR.II.offset * 4); //case of bne, beq
            break;
        case 32:
        case 35:
        case 36:
        case 40:
        case 43:
            printf("$%d, %d($%d)", IR.II.rt, IR.II.offset, IR.II.rs);    //case of lb, lw, lbu, sb, sw
            break;
        default:
            printf("$%d, $%d, %d", IR.II.rt, IR.II.rs, IR.II.offset);    //Iformat disassemble
            break;
        }
    }

    return;
};
