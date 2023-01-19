#include <stdio.h>
#include <string.h>
#define MAXLINES 1024
#define MAXLEN 1024

void cat(char **param, char *filename, int o_c);
int vflag(char ch);
int eflag(int index, int pos, int npos);
int arrFind(char *needle, char **haystack, int c);
int isSpaceStr(char *string);

// -b / --number-nonblank: number non-empty lines
// -e (+ -v) / -E: end-of-line as $
// -n / --number: number all output lines
// -s / --squeeze-blank: squeeze adjacent blank lines
// -t (+ -v) / -T: tabs as ^I

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Invalid param count");
  } else {
    char *options[argc - 2];
    int o_c = 0;
    char *filenames[argc - 2];
    int f_c = 0;
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        options[o_c++] = argv[i];
      } else {
        filenames[f_c++] = argv[i];
      }
    }

    for (int i = 0; i < f_c; i++) {
      cat(options, filenames[i], o_c);
    }
  }
  return 0;
}

void cat(char **param, char *filename, int o_c) {
  FILE *fptr;
  if (!(fptr = fopen(filename, "r"))) {
    printf("s21_cat: %s: No such file or directory", filename);
    return;
  }

  int linec = 0;
  char lines[MAXLINES][MAXLEN] = {0};
  int positions[MAXLINES] = {-2};  // "natural" \0 positions
  int npos[MAXLINES] = {-2};       // "natural" \n positions
  int e = (arrFind("-e", param, o_c) || (arrFind("-E", param, o_c)));

  int ch = 33, i = 0;
  while (ch != EOF) {
    while ((ch = fgetc(fptr)) != '\0' && ch != EOF && ch != '\n') {
      lines[linec][i++] = ch;
    }
    if (ch == 0) {
      lines[linec][i] = '\0';
      positions[linec] = i;
    } else if (ch == '\n') {
      lines[linec][i] = '\n';
      npos[linec++] = i;
      i = 0;
    }
  }

  int nonEmptyC = 0;
  for (int i = 0; i < linec + 1; i++) {
    if (lines[i][0] == '\0' && i == linec) break;
    if ((arrFind("-b", param, o_c)) ||
        (arrFind("--number-nonblank", param, o_c))) {
      if (isSpaceStr(lines[i])) {
        printf("     %d\t", ++nonEmptyC);
      }
    } else if ((arrFind("-n", param, o_c)) ||
               (arrFind("--number", param, o_c))) {
      printf("     %d\t", i + 1);
    }
    if ((arrFind("-s", param, o_c)) ||
        (arrFind("--squeeze-blank", param, o_c))) {
      if (i != 0) {
        if (!isSpaceStr(lines[i]) && !isSpaceStr(lines[i - 1])) continue;
      }
    }

    for (int ch = 0; ch < (int)strlen(lines[i]); ch++) {
      if ((arrFind("-t", param, o_c)) || (arrFind("-T", param, o_c))) {
        if (lines[i][ch] == '\t') {
          printf("^I");
          continue;
        } else if (arrFind("-t", param, o_c)) {
          if (vflag(lines[i][ch])) {
            if (eflag(ch + 1, positions[i], npos[i])) printf("$");
            continue;
          }
        }
      } else if ((arrFind("-e", param, o_c)) || (arrFind("-v", param, o_c))) {
        if (vflag(lines[i][ch])) continue;
      }
      if (e && lines[i][ch] == '\n') printf("$");
      printf("%c", lines[i][ch]);
      if ((arrFind("-e", param, o_c)) || (arrFind("-v", param, o_c)) ||
          (arrFind("-t", param, o_c))) {
        if (ch + 1 == positions[i] && linec - 1 != i) {
          printf("^@");
        }
      }
    }
  }
  fclose(fptr);
}

int vflag(char ch) {
  int res = 0;
  if (ch == 127) {
    printf("^?");
    res++;
  } else if ((ch > 0 && ch < 9) || (ch >= 11 && ch <= 31)) {
    printf("^%c", ch + 64);
    res++;
  } else if (ch < 0) {
    if ((ch + 128 + 64) < (96))
      printf("M-^%c", ch + 128 + 64);
    else
      printf("?");
    res++;
  }
  return res;
}

int eflag(int index, int pos, int npos) {
  int e = 0;
  if (index == npos || index == pos) e = 1;
  return e;
}

int arrFind(char *needle, char **haystack, int c) {
  int result = 0;
  for (int i = 0; i < c; i++) {
    if (strcmp(haystack[i], needle) == 0) {
      result = 1;
      break;
    }
  }
  return result;
}

int isSpaceStr(char *string) {
  int result = 0;
  for (int i = 0; i < (int)strlen(string); i++) {
    if (string[i] != '\n') {
      result = 1;
      break;
    }
  }
  return result;
}
