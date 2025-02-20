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

unsigned char memory[50], *program;
unsigned int memidx, progidx;

unsigned int loopbaseidx_stack[1000], loopbaseidx_stack_top;

void PushLoopBase(int idx)
{
    if (loopbaseidx_stack_top > sizeof(loopbaseidx_stack) / sizeof(loopbaseidx_stack[0]))
    {
        printf("LOOP STACK OVERFLOW\n");
        abort();
    }

    loopbaseidx_stack[loopbaseidx_stack_top] = idx;
    loopbaseidx_stack_top += 1;
}

int PopLoopBase()
{
    int i = loopbaseidx_stack_top - 1;
    if (0 > i)
    {
        printf("LOOP STACK UNDERFLOW (POP)\n");
        abort();
    }

    int r = loopbaseidx_stack[i];
    loopbaseidx_stack_top = i;
    return r;
}

int PeekLoopBase()
{
    int i = loopbaseidx_stack_top - 1;
    if (0 > i)
    {
        printf("LOOP STACK UNDERFLOW (PEEK)\n");
        abort();
    }

    int r = loopbaseidx_stack[i];
    return r;
}

int IdxMod(int a, int b) { return ((a % b) + b) % b; }

int main(int argc, char *argv[])
{
    memset(memory, 0, sizeof(memory));
    memset(loopbaseidx_stack, 0, sizeof(loopbaseidx_stack));
    loopbaseidx_stack_top = 0;
    program = 0;
    memidx = 0;
    progidx = 0;

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

    for (;; ticks++)
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
            PushLoopBase(progidx + 1);
            progidx += 1;
        }
        else if (op == ']')
        {

            if (memory[memidx] != 0)
            {
                progidx = PeekLoopBase();
            }
            else
            {
                progidx += 1;
                PopLoopBase();
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
        printf(" %2x ", i);
    }
    printf("\n");

    for (int i = 0; i < sizeof(memory); i++)
    {
        printf("[%2x]", memory[i]);
    }
    printf("\n");

    for (int i = 0; i < memidx; i++)
    {
        printf("    ");
    }
    printf(" ^\n");

    printf("------ DEBUG ------\n");
    printf("%d  \n", ticks);

    free(program);
    return (0);
}