#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* notes from https://gist.github.com/roachhd/dce54bec8ba55fb17d3a -- really good resource
> = increases memory pointer, or moves the pointer to the right 1 block.
< = decreases memory pointer, or moves the pointer to the left 1 block.
+ = increases value stored at the block pointed to by the memory pointer
- = decreases value stored at the block pointed to by the memory pointer
[ = like c while(cur_block_value != 0) loop.
] = if block currently pointed to's value is not zero, jump back to [
, = like c getchar(). input 1 character.
. = like c putchar(). print 1 character to the console
*/

char memory[10], *program;
int memidx, progidx, loopbaseidx;

int IdxMod(int a, int b) { return ((a % b) + b) % b; }

int main(int argc, char *argv[])
{
    memset(memory, 0, sizeof(memory));
    program = 0;
    memidx = 0;
    progidx = 0;
    loopbaseidx = -1;

    if (2 > argc)
    {
        printf("Usage: <%s> <filename.bf>\n", argv[0]);
        return (1);
    }

    FILE *f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    rewind(f);
    program = malloc(len);
    memset(program, 0, len);

    for (int i = 0; i < len; i++)
    {
        int c = fgetc(f);

        if (c == EOF)
        {
            printf("Unexpected EOF! %d\n", i);
            goto DIE;
        }
        program[i] = (char)c;
    }

    int ticks = 0;

    for (;;ticks++)
    {
        char op = program[progidx];

        if (op == '>')
        {
            memidx = IdxMod(memidx + 1, sizeof(memory));
            progidx += 1;
        }
        else if (op == '<')
        {
            memidx = IdxMod(memidx - 1, sizeof(memory));
            progidx += 1;
        }
        else if (op == '+')
        {
            memory[memidx] += 1;
            progidx += 1;
        }
        else if (op == '-')
        {
            memory[memidx] -= 1;
            progidx += 1;
        }
        else if (op == ',')
        {
            memory[memidx] = getc(stdin);
            progidx += 1;
        }
        else if (op == '.')
        {
            putc(memory[memidx], stdout);
            progidx += 1;
        }
        else if (op == '[')
        {
            if (loopbaseidx != -1)
            {
                printf("INVALID CODE, UNEXPECTED [ at %d\n", progidx);
                goto DIE;
            }

            loopbaseidx = progidx + 1;
            progidx += 1;
        }
        else if (op == ']')
        {
            if (loopbaseidx == -1)
            {
                printf("INVALID CODE, UNEXPECTED ] at %d\n", progidx);
                goto DIE;
            }

            if (memory[memidx] != 0)
            {
                progidx = loopbaseidx;
            }
            else
            {
                progidx += 1;
                loopbaseidx = -1;
            }
        }
        else
        {
            progidx += 1;
        }

        if (progidx > len)
            goto DIE;
    }

DIE:

    for (int i = 0; i < sizeof(memory); i++)
    {
        printf(" %x ", i);
    }
    printf("\n");

    for (int i = 0; i < sizeof(memory); i++)
    {
        printf("[%x]", memory[i]);
    }
    printf("\n");

    for (int i = 0; i < memidx; i++)
    {
        printf("   ");
    }
    printf(" ^\n");

    printf("------ DEBUG ------\n");
    printf("%d  \n", ticks);

    free(program);
    return (0);
}