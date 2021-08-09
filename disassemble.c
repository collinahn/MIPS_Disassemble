#include <stdio.h>
#define M_SIZE 1024

unsigned char MEM[M_SIZE];
unsigned char buf[4];
unsigned int nInst, nData;
union InstructionRegister
{
    unsigned int I;
    struct Rformat
    {
        unsigned int funct : 6;
        unsigned int sh : 5;
        unsigned int rd : 5;
        unsigned int rt : 5;
        unsigned int rs : 5;
        unsigned int opcode : 6;
    } RI;
    struct Iformat
    {
        int offset : 16;
        unsigned int rt : 5;
        unsigned int rs : 5;
        unsigned int opcode : 6;
    } II;
    struct Jformat
    {
        unsigned int address : 26;
        unsigned int opcode : 6;
    } JI;
} IR;

unsigned int SaveBigEndian(unsigned char *arr);
void InstEncoding(const unsigned int x);
void DisassembleReg(const unsigned int x);

int main()
{
    FILE *pFile = NULL;
    errno_t err;
    char fname[20] = {
        0,
    };

    printf("File Name: ");
    gets_s(fname, sizeof(fname));
    printf("------------------\n");

    err = fopen_s(&pFile, fname, "rb");
    if (err)
    {
        printf("Cannot open file\n");
        return 1;
    }

    fread(buf, sizeof(buf[0]), 4, pFile);
    nInst = SaveBigEndian(buf);

    fread(buf, sizeof(buf[0]), 4, pFile);
    nData = SaveBigEndian(buf);

    fread(MEM, sizeof(MEM[0]), (nInst + nData) * 4, pFile);
    for (unsigned int i = 0; i < nInst * 4; i += 4)
    {
        IR.I = SaveBigEndian(&MEM[i]);
        InstEncoding(IR.I);
        DisassembleReg(IR.I);
        printf("\n");
    }

    fclose(pFile);

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
        switch (IR.RI.funct)
        {
        case 0:
            printf("sll");
            break;
        case 2:
            printf("srl");
            break;
        case 3:
            printf("sra");
            break;
        case 8:
            printf("jr");
            break;
        case 12:
            printf("syscall");
            break;
        case 16:
            printf("mfhi");
            break;
        case 18:
            printf("mflo");
            break;
        case 24:
            printf("mul");
            break;
        case 32:
            printf("add");
            break;
        case 34:
            printf("sub");
            break;
        case 36:
            printf("and");
            break;
        case 37:
            printf("or");
            break;
        case 38:
            printf("xor");
            break;
        case 39:
            printf("nor");
            break;
        case 42:
            printf("slt");
            break;
        default:
            printf("RI_err");
            break;
        }
    }
    else
    {
        switch (IR.RI.opcode)
        {
        case 1:
            printf("bltz");
            break;
        case 2:
            printf("j"); //j-format
            break;
        case 3:
            printf("jal"); //j-format
            break;
        case 4:
            printf("beq");
            break;
        case 5:
            printf("bne");
            break;
        case 8:
            printf("addi");
            break;
        case 10:
            printf("slti");
            break;
        case 12:
            printf("andi");
            break;
        case 13:
            printf("ori");
            break;
        case 14:
            printf("xori");
            break;
        case 15:
            printf("lui");
            break;
        case 32:
            printf("lb");
            break;
        case 35:
            printf("lw");
            break;
        case 36:
            printf("lbu");
            break;
        case 40:
            printf("sb");
            break;
        case 43:
            printf("sw");
            break;
        default:
            printf("nRI_err");
            break;
        }
    }
    printf(" ");

    return;
};

void DisassembleReg(const unsigned int x)
{
    unsigned int PC = 0x00400020;
    if (IR.RI.opcode == 0)
    {
        switch (IR.RI.funct)
        {
        case 0:
        case 2:
        case 3:
            printf("$%d, $%d, %d", IR.RI.rd, IR.RI.rt, IR.RI.sh); //case of sll, srl, sra
            break;
        case 8:
            printf("$%d", IR.RI.rs); //case of jr
            break;
        case 12:
            break; //case of syscall
        default:
            printf("$%d, $%d, $%d", IR.RI.rd, IR.RI.rs, IR.RI.rt); //Rformat disassemble
            break;
        }
    }
    else if (IR.RI.opcode == 2 || IR.RI.opcode == 3)
        printf("0x%08x", ((PC >> 28) << 28) + ((unsigned int)IR.JI.address << 2)); //Jformat disassemble
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
            printf("$%d, %d($%d)", IR.II.rt, IR.II.offset, IR.II.rs); //case of lb, lw, lbu, sb, sw
            break;
        default:
            printf("$%d, $%d, %d", IR.II.rt, IR.II.rs, IR.II.offset); //Iformat disassemble
            break;
        }
    }

    return;
};
