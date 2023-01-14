#define _GNU_SOURCE
#include <ctype.h>
#include <pcre.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAXLINES 4096
#define MAXLEN 4096

int grep(FILE *fptr, int t_c, char *templates[MAXLINES], int o_c,
         char *options[MAXLINES], char *filename, int f_c, int *printc,
         int printed[MAXLINES]);
int arrFind(char *needle, char *haystack[MAXLINES], int c);
int match(char *line, char *template, char *options[MAXLINES], int o_c);
void findParts(char *line, char *template, int iFlag, int i, int tempi,
               int flag);
void bubsort(int *printed, int *printc);

int main(int argc, char *argv[argc]) {
  if (argc < 3) {
    printf("Invalid param count.");
  } else {
    char *options[MAXLINES] = {0};
    int o_c = 0;
    char *files[MAXLINES] = {0};
    int f_c = 0;
    char *templates[MAXLINES] = {0};
    int t_c = 0;
    int isTemplate = 1;
    // int isRegexFile = 0;
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        strcpy(options[o_c++], argv[i]);
      } else {
        strcpy(files[f_c++], argv[i]);
        if (isTemplate) {
          strcpy(templates[t_c++], argv[i]);
          isTemplate = 0;
        }
      }
    }
    printf("sega 1\n");

    for (int f = 0; f < f_c; f++) {
    printf("sega 2, opening %s\n", files[f_c]);
      FILE *fptr = fopen(files[f], "r");
    printf("sega 3\n");
      if (fptr) {
        fclose(fptr);
        int printed[MAXLINES] = {-1};
        int printc = 0;
        FILE *fptr = fopen(files[f], "r");
        int c = grep(fptr, t_c, templates, o_c, options, files[f], f_c, &printc,
                    printed);
        if (arrFind("-c", options, o_c)) {
          if ((c) && arrFind("-l", options, o_c)) c = 1;
          if ((f_c > 1) && !arrFind("-h", options, o_c)) printf("%s:", files[f]);
          printf("%d\n", c);
        }
        if ((c) && (arrFind("-l", options, o_c))) printf("%s\n", files[f]);
        fclose(fptr);
      } else {
        if (!arrFind("-s", options, o_c)) {
          printf("s21_grep: %s: No such file or directory\n", files[f]);
        }
      }
    }
  }
  return 0;
}

int grep(FILE *fptr, int t_c, char *templates[MAXLINES], int o_c,
         char *options[MAXLINES], char *filename, int f_c, int *printc,
         int printed[MAXLINES]) {
  // -e: multiple search patterns +
  // -i: case-insensitive +
  // -v: all lines without template +
  // -c: output count of matching lines (int only) +
  // -l: output names of matching files only +
  // -n: precede each matching line with a number +

  // -h: output matching lines without preceding them by file names +
  // -s: suppress error messages about nonexistent files +
  // -f file: take regexes from file +
  // -o: output matched parts of a matching line +
  char lines[MAXLINES][MAXLEN];
  int linec = 0;
  if (fptr) printf("hey!");
  while (!feof(fptr) && !ferror(fptr)) {
    if (fgets(lines[linec], MAXLEN, fptr) != NULL) {
      if (strchr(lines[linec], '\n')) *strchr(lines[linec], '\n') = 0;
      linec++;
    }
  }

  int print = 1;
  if ((arrFind("-c", options, o_c)) || (arrFind("-l", options, o_c))) {
    print = 0;
  }

  for (int t = 0; t < t_c; t++) {
    for (int i = 0; i < linec; i++) {
      int check = 1;
      for (int printi = 0; printi < *printc; printi++) {
        if (printed[printi] == i) {
          check = 0;
          break;
        }
      }

      if ((strcmp(templates[t], "\0")) &&
          (match(lines[i], templates[t], options, o_c) == 1) && check) {
        printed[*printc] = i;
        *printc = *printc + 1;
      }
    }
  }

  bubsort(printed, printc);

  if (print) {
    if (!arrFind("-v", options, o_c)) {
      for (int i = 0; i < *printc; i++) {
        if ((f_c > 1) && !arrFind("-h", options, o_c)) printf("%s:", filename);
        if (arrFind("-n", options, o_c)) printf("%d:", printed[i] + 1);
        if (arrFind("-o", options, o_c)) {
          for (int t = 0; t < t_c; t++) {
            if (match(lines[printed[i]], templates[t], options, o_c)) {
              // printf("My template is %s!", templates[t]);
              findParts(lines[printed[i]], templates[t],
                        (arrFind("-i", options, o_c)), 0, 0, 0);
            }
          }
        } else {
          printf("%s", lines[printed[i]]);
          if (lines[i][(int)strlen(lines[i]) - 1] != '\n') {
            printf("\n");
          }
        }
      }
    } else {
      for (int i = 0; i < linec; i++) {
        int flag = 1;
        for (int pr = 0; pr < *printc; pr++) {
          if (printed[pr] == i) flag = 0;
        }

        if (flag) {
          if ((f_c > 1) && !arrFind("-h", options, o_c))
            printf("%s:", filename);
          if (arrFind("-n", options, o_c)) printf("%d:", i + 1);
          if (arrFind("-o", options, o_c)) {
            for (int t = 0; t < t_c; t++) {
              if (!match(lines[i], templates[t], options, o_c))
                findParts(lines[printed[i]], templates[t],
                          (arrFind("-i", options, o_c)), 0, 0, 0);
            }
          } else {
            printf("%s", lines[i]);
            if (lines[i][(int)strlen(lines[i]) - 1] != '\n') {
              printf("\n");
            }
          }
        }
      }
    }

  } else {
    if (arrFind("-v", options, o_c)) {
      *printc = linec - *printc;
    }
  }

  return *printc;
}

void findParts(char *line, char *template, int iFlag, int i, int tempi,
               int flag) {
  char result = ' ';
  if (template[0] == '^') {
    tempi = 1;
  }
  if (i > (int)strlen(line)) {
    result = '\0';
  } else {
    if ((line[i] == template[tempi]) ||
        (iFlag && (tolower(line[i]) == tolower(template[tempi])))) {
      result = line[i];
      tempi++;
      if (result != '\n') printf("%c", result);
      flag = 1;
    } else {
      if (flag && (result == ' ')) printf("\n");
      tempi = 0;
      flag = 0;
      if (template[0] == '^') return;
    }
    if (tempi >= (int)strlen(template)) {
      tempi = 0;
    }
    i++;
    findParts(line, template, iFlag, i, tempi, flag);
  }
}

int match(char *line, char *template, char *options[MAXLINES], int o_c) {
  int result = 0;
  pcre *re;
  const char *error;
  int erroroffset;
  if (arrFind("-i", options, o_c)) {
    char low[2048];
    low[0] = '(';
    low[1] = '?';
    low[2] = 'i';
    low[3] = ')';
    for (int i = 0; i < (int)strlen(template); i++) {
      low[i + 4] = template[i];
      low[i + 5] = '\0';
    }
    re = pcre_compile((char *)low, 0, &error, &erroroffset, NULL);
  } else {
    re = pcre_compile((char *)template, 0, &error, &erroroffset, NULL);
  }
  int count = 0;
  int ovector[30];
  count =
      pcre_exec(re, NULL, (char *)line, (int)strlen(line), 0, 0, ovector, 30);
  if (count > 0) {
    result = 1;
  }
  free(re);

  return result;
}

int arrFind(char *needle, char *haystack[MAXLINES], int c) {
  int result = 0;
  for (int i = 0; i < c; i++) {
    if (strcmp(haystack[i], needle) == 0) {
      result = 1;
      break;
    }
  }
  return result;
}

void bubsort(int *printed, int *printc) {
  for (int i = 0; i < *printc - 1; i++) {
    if (printed[i] > printed[i + 1]) {
      printed[i] += printed[i + 1];
      printed[i + 1] = printed[i] - printed[i + 1];
      printed[i] -= printed[i + 1];
    }
  }
}