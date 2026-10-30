#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM 30000

typedef struct {
    int* data;
    size_t size;
    size_t cap;
} Stack;

void stack_init(Stack *s) {
    s->cap = 16;
    s->size = 0;
    s->data = malloc(s->cap * sizeof(int));
    if (!s->data) { perror("malloc"); exit(1); }
}

void stack_push(Stack *s, int val) {
    if (s->size >= s->cap) {
        s->cap *= 2;
        int *nd = realloc(s->data, s->cap * sizeof(int));
        if (!nd) { perror("realloc"); exit(1); }
        s->data = nd;
    }
    s->data[s->size++] = val;
}

int stack_pop(Stack *s) {
    if (s->size == 0) {
        fprintf(stderr, "Stack underflow\n");
        exit(1);
    }
    return s->data[--s->size];
}

void stack_free(Stack *s) {
    free(s->data);
}

int main(int argc, char **argv) {

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    exit(1);
  }

  FILE *f = fopen(argv[1], "r");
  if (f == NULL) {
    perror("Error when opening file");
    exit(1);
  }

  unsigned char mem[MEM] = {0};

  size_t cap = 1024;
  char *prog = malloc(cap);
  if (!prog) {
    perror("Error when allocating memory for program");
    fclose(f);
    exit(1);
  }

  size_t n = 0;
  int syntax = 0;
  int c;

  while ((c = fgetc(f)) != EOF) {
    if (strchr("+-<>[],.", c)) {
      if (n + 1 >= cap) {
        cap *= 2;
        char *np = realloc(prog, cap);
        if (!np) {
          perror("Error when reallocating memory for program");
          free(prog);
          fclose(f);
          exit(1);
        }
        prog = np;
      }
      prog[n++] = (char)c;
    }
    if (c == '[') syntax++;
    if (c == ']')
    {
        syntax--;
        if(syntax < 0) { fprintf(stderr, "Check [] quantity\n"); exit(1); }
    }
  }

  prog[n] = '\0';

  if (n == 0) {
    free(prog);
    fclose(f);
    exit(0);
  }

  if (syntax != 0) {
    fprintf(stderr, "Check [] quantity\n");
    exit(1);
  }

  int i = 0;
  int ptr = 0;
  int flag;
  Stack stack;
  stack_init(&stack);

  while (prog[i] != '\0') {
    switch (prog[i]) {
    case '+':
      mem[ptr]++;
      i++;
      break;
    case '.':
      putchar(mem[ptr]);
      i++;
      break;
    case '-':
      mem[ptr]--;
      i++;
      break;
    case '<':
      ptr--;
      if (ptr < 0)
        ptr = 29999;
      i++;
      break;
    case '>':
      ptr++;
      if (ptr > MEM - 1)
        ptr = 0;
      i++;
      break;
    case '[':
      if (mem[ptr] == 0) {
          int depth = 1;
          while (depth > 0) {
              i++;
              if (prog[i] == '[') depth++;
              else if (prog[i] == ']') depth--;
          }
      }

      else {
          stack_push(&stack, i);
      }
      i++;
      break;
    case ']':
      if(mem[ptr] != 0)
          i = stack_pop(&stack);
      else {
          stack_pop(&stack);
          i++;
      }
      break;
    case ',':
      mem[ptr] = getc(stdin);
      i++;
      break;
    }
  }
  stack_free(&stack);
  fclose(f);
  free(prog);
  return 0;
}
