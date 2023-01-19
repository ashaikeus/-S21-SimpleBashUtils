#define _GNU_SOURCE
#include <ctype.h>
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXLINES 4096
#define MAXLEN 4096

int arrFind(char *needle, char *haystack, int c);
void getTemplates(char **templates, int *t_c, char *filename);
int grep(int *printed, char **lines, char **templates, int t_c, char *options,
         int o_c, int linec, char *filename, int f_c);
void bubsort(int *printed, int printc);
void parse(int argc, char *argv[], char *options, char **templates,
           char **files, int *o_c, int *t_c, int *f_c);
void freeArr(char **array, int c);
int unpackFile(char **array, FILE *fregptr);
int match(char *line, char *template, char *options, int o_c);
int inPrinted(int *printed, int count, int n);
void findParts(char *line, char **template, int t_c);

// -e: multiple search patterns +
// -i: case-insensitive +
// -v: all lines without template +
// -c: output count of matching lines (int only) +
// -l: output names of matching files only +
// -n: precede each matching line with a number +

// -h: output matching lines without preceding them by file names +
// -s: suppress error messages about nonexistent files +
// -f file: take regexes from file
// -o: output matched parts of a matching line +

// TO FIX:
// multiple files don't show

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Invalid param count.");
  } else {
    char options[10];
    char *templates[MAXLINES];
    char *files[MAXLINES];
    int o_c = 0, t_c = 0, f_c = 0;

    parse(argc, argv, options, templates, files, &o_c, &t_c, &f_c);

    if (t_c > 0 && f_c > 0) {
      for (int f = 0; f < f_c; f++) {
        int printed[MAXLINES] = {-1};
        char *lines[MAXLINES] = {0};
        FILE *fptr = fopen(files[f], "r");
        if (fptr) {
          int linec = unpackFile(lines, fptr);
          fclose(fptr);
          int c = grep(printed, lines, templates, t_c, options, o_c, linec,
                       files[f], f_c);
          if (arrFind("-c", options, o_c)) {
            if ((c) && arrFind("-l", options, o_c)) c = 1;
            if ((f_c > 1) && !arrFind("-h", options, o_c))
              printf("%s:", files[f]);
            printf("%d\n", c);
          }
          freeArr(lines, linec);
          if ((c) && (arrFind("-l", options, o_c))) printf("%s\n", files[f]);
        } else if (!arrFind("-s", options, o_c)) {
          printf("s21_grep: %s: No such file or directory\n", files[f]);
        }
      }
    } else {
      printf("Invalid params.");
    }

    freeArr(templates, t_c);
    freeArr(files, f_c);
  }
  return 0;
}

void parse(int argc, char *argv[], char *options, char **templates,
           char **files, int *o_c, int *t_c, int *f_c) {
  int c = 0;
  opterr = 0;
  while ((c = getopt_long(argc, argv, "e:ivclnshf:o", NULL, NULL)) != -1) {
    switch (c) {
      case 'e': {
        if (optind < argc) {
          templates[(*t_c)++] = strdup(optarg);
        }
        break;
      }
      case 'f': {
        char *fregname;
        fregname = strdup(optarg);
        getTemplates(templates, t_c, fregname);
        free(fregname);
        break;
      }
      case 'i':
      case 'v':
      case 'c':
      case 'l':
      case 'n':
      case 's':
      case 'h':
      case 'o': {
        options[(*o_c)++] = c;
        break;
      }
    }
  }

  for (int i = optind; i < argc; i++) {
    if (*t_c == 0) {
      if (argv[optind] != NULL) templates[(*t_c)++] = strdup(argv[optind++]);
    } else {
      if (argv[optind] != NULL) files[(*f_c)++] = strdup(argv[optind++]);
    }
  }

  // for (int t = 0; t < *t_c; t++) printf("\ntemp: %s", templates[t]);
  // for (int f = 0; f < *f_c; f++) printf("\nfile: %s", files[f]);
  // for (int o = 0; o < *o_c; o++) printf("\noption: %c", options[o]);
}

void getTemplates(char **templates, int *t_c, char *filename) {
  FILE *fregptr = fopen(filename, "r");
  char *lines[MAXLINES] = {0};
  if (fregptr) {
    int linec = unpackFile(lines, fregptr);
    for (int i = 0; i < linec; i++) {
      templates[(*t_c)++] = lines[i];
    }
    fclose(fregptr);
  }
}

void freeArr(char **array, int c) {
  for (int i = 0; i < c; i++) free(array[i]);
}

int unpackFile(char **array, FILE *fregptr) {
  char *tmp_line;
  tmp_line = (char *)malloc(MAXLINES * sizeof(char));
  int i = 0;
  size_t size = MAXLINES;
  int linec = 0;
  while (getline(&tmp_line, &size, fregptr) != -1) {
    array[i++] = strdup(tmp_line);
    if (strchr(array[linec], '\n')) *strchr(array[linec], '\n') = 0;
    linec = i;
  }
  free(tmp_line);
  return linec;
}

int grep(int *printed, char **lines, char **templates, int t_c, char *options,
         int o_c, int linec, char *filename, int f_c) {
  int print = 1;
  int count = 0;
  for (int i = 0; i < linec; i++) {
    for (int t = 0; t < t_c; t++) {
      if (match(lines[i], templates[t], options, o_c)) {
        if (!inPrinted(printed, count, i)) printed[count++] = i;
      }
    }
  }

  bubsort(printed, count);

  if (arrFind("-c", options, o_c) || arrFind("-l", options, o_c)) print = 0;
  if (print) {
    if (!arrFind("-v", options, o_c)) {
      for (int p = 0; p < count; p++) {
        if ((f_c > 1) && !arrFind("-h", options, o_c)) printf("%s:", filename);
        if (arrFind("-n", options, o_c)) printf("%d:", printed[p] + 1);
        if (!arrFind("-o", options, o_c))
          printf("%s\n", lines[printed[p]]);
        else
          findParts(lines[printed[p]], templates, t_c);
      }
    } else {
      for (int i = 0; i < linec; i++)
        if (!inPrinted(printed, count, i)) {
          if ((f_c > 1) && !arrFind("-h", options, o_c))
            printf("%s:", filename);
          if (arrFind("-n", options, o_c)) printf("%d:", i + 1);
          printf("%s\n", lines[i]);
        }
    }
  }
  if (arrFind("-v", options, o_c)) count = linec - count;
  return count;
}

int inPrinted(int *printed, int count, int n) {
  int flag = 0;
  for (int i = 0; i < count; i++)
    if (printed[i] == n) {
      flag = 1;
      break;
    }
  return flag;
}

void findParts(char *line, char **templates, int t_c) {
  regmatch_t pmatch[1];
  int cflags = REG_EXTENDED;
  const size_t nmatch = 1;
  regex_t reg;

  for (int t = 0; t < t_c; t++) {
    char *start = line;
    while (regexec(&reg, start, nmatch, pmatch, 0) != REG_NOMATCH) {
      regcomp(&reg, templates[t], cflags);
      int status = regexec(&reg, line, nmatch, pmatch, 0);
      if (status == REG_NOMATCH)
        printf("No match\n");
      else if (!status) {
        for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++)
          printf("%c", line[i]);
        printf("\n");
      }
      start += pmatch[0].rm_eo;
    }
    regfree(&reg);
  }
}

int match(char *line, char *template, char *options, int o_c) {
  regex_t regex;
  int reti;
  int result = 0;
  // char msgbuf[100];

  if (arrFind("-i", options, o_c))
    reti = regcomp(&regex, template, REG_ICASE);
  else
    reti = regcomp(&regex, template, 0);
  if (reti) printf("Not compiled.");
  reti = regexec(&regex, line, 0, NULL, 0);
  if (!reti) result = 1;

  regfree(&regex);
  return result;
}

int arrFind(char *needle, char *haystack, int c) {
  int result = 0;
  for (int i = 0; i < c; i++) {
    if (haystack[i] == needle[1]) {
      result = 1;
      break;
    }
  }
  return result;
}

void bubsort(int *printed, int printc) {
  for (int i = 0; i < printc - 1; i++) {
    if (printed[i] > printed[i + 1]) {
      printed[i] += printed[i + 1];
      printed[i + 1] = printed[i] - printed[i + 1];
      printed[i] -= printed[i + 1];
    }
  }
}