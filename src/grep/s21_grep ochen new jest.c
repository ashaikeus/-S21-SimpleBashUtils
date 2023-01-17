#define _GNU_SOURCE
#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#define MAXLINES 4096
#define MAXLEN 4096

int arrFind(char *needle, char *haystack, int c);
void getTemplates(char **templates, int *t_c, char *filename);
int grep(char **lines, char **templates, char *options);
// int match(char *line, char *template, char *options);
// void findParts(char *line, char *template, int iFlag, int i, int tempi,
              //  int flag);
void bubsort(int *printed, int *printc);
void parse(int argc, char *argv[], char *options, char **templates, char **files, int *o_c, int *t_c, int *f_c);
void freeArr(char **array, int c);
int unpackFile(char **array, FILE *fregptr);

// -e: multiple search patterns
// -i: case-insensitive
// -v: all lines without template
// -c: output count of matching lines (int only)
// -l: output names of matching files only
// -n: precede each matching line with a number

// -h: output matching lines without preceding them by file names
// -s: suppress error messages about nonexistent files
// -f file: take regexes from file
// -o: output matched parts of a matching line



// KNOWN ERRORS:
// (fixed) parse: sega if there is no -e optarg +
// parse: no -f flag support; sega +


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
        int printed[MAXLINES] = {0};
        char *lines[MAXLINES] = {0};
        unpackFile(lines, files[f]);
        for (int t = 0; t < t_c; t++) {
          int c = grep(lines, templates, options);
        }
      }
      freeArr(templates, t_c);
      freeArr(files, f_c);
    } else {
      printf("Invalid params.");
    }
  }
  return 0;
}

void parse(int argc, char *argv[], char *options, char **templates, char **files, int *o_c, int *t_c, int *f_c) {
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
  char *tmp_line = NULL;
  int i = 0;
  size_t size = MAXLINES;
  int linec = 0;
  while (getline(&tmp_line, &size, fregptr) != -1) {
    array[i++] = strdup(tmp_line);
    linec = i;
  }
  free(tmp_line);
  return linec;
}

int grep(char **lines, char **templates, int *t_c, char *options) {
  int print = 1;
  int count = 0;
  if (arrFind("-c", options, o_c) || arrFind("-l", options, o_c)) print = 0;
  for (int i = 0; i < linec; i++) {
    if (match(lines[i], templates[t], options)) {
      printed[count++] = i;
    }
  }
  return count;
}


// void findParts(char *line, char *template, int iFlag, int i, int tempi,
//                int flag) {
//     // use regex & pmatch
// }

int match(char *line, char *template, char *options) {
  regex_t regex;
  int reti;
  int result = 0;
  char msgbuf[100];

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

void bubsort(int *printed, int *printc) {
  for (int i = 0; i < *printc - 1; i++) {
    if (printed[i] > printed[i + 1]) {
      printed[i] += printed[i + 1];
      printed[i + 1] = printed[i] - printed[i + 1];
      printed[i] -= printed[i + 1];
    }
  }
}