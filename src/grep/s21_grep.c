#include "s21_grep.h"

int main(int argc, char* argv[]) {
  int exit_code = 0;

  if (argc < 2) {
    print_usage_info(argv[0]);
    exit_code = 1;
  } else {
    GrepOptions options = {0};
    char* files[argc];
    int file_count = 0;

    exit_code = parse_command_line(argc, argv, &options, files, &file_count);

    if (exit_code == 0 && options.pattern_count == 0) {
      fprintf(stderr, "No search pattern specified\n");
      print_usage_info(argv[0]);
      exit_code = 1;
    }

    if (exit_code == 0) {
      process_input_files(&options, file_count, files);
    }
  }

  return exit_code;
}
int parse_command_line(int argc, char* argv[], GrepOptions* options,
                       char* files[], int* file_count) {
  int exit_code = 0;
  int pattern_found = 0;

  exit_code = parse_arguments(argc, argv, options, &pattern_found);

  if (exit_code == 0) {
    exit_code = process_remaining_arguments(argc, argv, options, &pattern_found,
                                            files, file_count);
  }

  return exit_code;
}

int parse_arguments(int argc, char* argv[], GrepOptions* options,
                    int* pattern_found) {
  int exit_code = 0;
  int opt;

  while ((opt = getopt(argc, argv, "e:ivclnshf:o")) != -1 && exit_code == 0) {
    exit_code = handle_command_line_option(opt, options, pattern_found);
  }

  return exit_code;
}

int handle_command_line_option(int opt, GrepOptions* options,
                               int* pattern_found) {
  int exit_code = 0;

  switch (opt) {
    case 'e':
      options->enable_pattern = 1;
      add_search_pattern(options, optarg);
      *pattern_found = 1;
      break;
    case 'i':
      options->ignore_case = 1;
      break;
    case 'v':
      options->invert_match = 1;
      break;
    case 'c':
      options->count_matches = 1;
      break;
    case 'l':
      options->list_files = 1;
      break;
    case 'n':
      options->show_line_numbers = 1;
      break;
    case 's':
      options->suppress_errors = 1;
      break;
    case 'h':
      options->suppress_filenames = 1;
      break;
    case 'f':
      options->read_patterns_file = 1;
      load_patterns_from_file(options, optarg);
      *pattern_found = 1;
      break;
    case 'o':
      options->show_only_matching = 1;
      break;
    case '?':
      print_usage_info("s21_grep");
      exit_code = 1;
      break;
  }

  return exit_code;
}

int process_remaining_arguments(int argc, char* argv[], GrepOptions* options,
                                int* pattern_found, char* files[],
                                int* file_count) {
  int exit_code = 0;

  for (int i = optind; i < argc && exit_code == 0; i++) {
    exit_code = handle_single_argument(argv, i, pattern_found, options, files,
                                       file_count, argc);
  }

  return exit_code;
}

int handle_single_argument(char* argv[], int index, int* pattern_found,
                           GrepOptions* options, char* files[], int* file_count,
                           int argc) {
  int exit_code = 0;

  if (argv[index][0] == '-' && strlen(argv[index]) > 1) {
    exit_code = handle_combined_options(argv[index] + 1, argv, &index, options,
                                        pattern_found, argc);
  } else if (!(*pattern_found)) {
    add_search_pattern(options, argv[index]);
    *pattern_found = 1;
  } else {
    files[*file_count] = argv[index];
    (*file_count)++;
  }

  return exit_code;
}

int handle_combined_options(char* option_string, char* argv[], int* index,
                            GrepOptions* options, int* pattern_found,
                            int argc) {
  int exit_code = 0;
  size_t option_length = strlen(option_string);

  for (size_t j = 0; j < option_length && exit_code == 0; j++) {
    switch (option_string[j]) {
      case 'i':
        options->ignore_case = 1;
        break;
      case 'v':
        options->invert_match = 1;
        break;
      case 'c':
        options->count_matches = 1;
        break;
      case 'l':
        options->list_files = 1;
        break;
      case 'n':
        options->show_line_numbers = 1;
        break;
      case 's':
        options->suppress_errors = 1;
        break;
      case 'h':
        options->suppress_filenames = 1;
        break;
      case 'o':
        options->show_only_matching = 1;
        break;
      case 'e':
        exit_code = handle_pattern_option(option_string, j, option_length, argv,
                                          index, options, pattern_found, argc);
        j = option_length;
        break;
      case 'f':
        exit_code =
            handle_file_pattern_option(option_string, j, option_length, argv,
                                       index, options, pattern_found, argc);
        j = option_length;
        break;
      default:
        fprintf(stderr, "grep: invalid option -- '%c'\n", option_string[j]);
        exit_code = 1;
        break;
    }
  }

  return exit_code;
}

int handle_pattern_option(char* opt_str, size_t pos, size_t opt_len,
                          char* argv[], int* index, GrepOptions* options,
                          int* pattern_found, int argc) {
  int exit_code = 0;

  if (pos + 1 < opt_len) {
    add_search_pattern(options, opt_str + pos + 1);
    *pattern_found = 1;
  } else if (*index + 1 < argc) {
    add_search_pattern(options, argv[*index + 1]);
    *pattern_found = 1;
    (*index)++;
  } else {
    fprintf(stderr, "grep: option requires an argument -- 'e'\n");
    exit_code = 1;
  }

  return exit_code;
}

int handle_file_pattern_option(char* opt_str, size_t pos, size_t opt_len,
                               char* argv[], int* index, GrepOptions* options,
                               int* pattern_found, int argc) {
  int exit_code = 0;

  if (pos + 1 < opt_len) {
    load_patterns_from_file(options, opt_str + pos + 1);
    *pattern_found = 1;
  } else if (*index + 1 < argc) {
    load_patterns_from_file(options, argv[*index + 1]);
    *pattern_found = 1;
    (*index)++;
  } else {
    fprintf(stderr, "grep: option requires an argument -- 'f'\n");
    exit_code = 1;
  }

  return exit_code;
}

void add_search_pattern(GrepOptions* options, const char* pattern) {
  if (options->pattern_count < MAX_PATTERNS) {
    strncpy(options->patterns[options->pattern_count], pattern,
            MAX_LINE_LENGTH - 1);
    options->patterns[options->pattern_count][MAX_LINE_LENGTH - 1] = '\0';
    options->pattern_count++;
  }
}

void load_patterns_from_file(GrepOptions* options, const char* filename) {
  FILE* file = fopen(filename, "r");
  if (file != NULL) {
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL &&
           options->pattern_count < MAX_PATTERNS) {
      line[strcspn(line, "\n")] = '\0';
      if (strlen(line) > 0) {
        add_search_pattern(options, line);
      }
    }
    fclose(file);
  } else if (!options->suppress_errors) {
    fprintf(stderr, "grep: %s: No such file or directory\n", filename);
  }
}

void process_input_files(GrepOptions* options, int file_count, char* files[]) {
  if (file_count == 0) {
    search_in_file(options, stdin, "(standard input)", file_count);
  } else {
    for (int i = 0; i < file_count; i++) {
      FILE* file = fopen(files[i], "r");
      if (file != NULL) {
        search_in_file(options, file, files[i], file_count);
        fclose(file);
      } else if (!options->suppress_errors) {
        fprintf(stderr, "grep: %s: No such file or directory\n", files[i]);
      }
    }
  }
}

int compile_regex_pattern(regex_t* regex, const char* pattern,
                          int ignore_case) {
  int flags = REG_EXTENDED;
  if (ignore_case) flags |= REG_ICASE;
  return regcomp(regex, pattern, flags);
}

void search_in_file(GrepOptions* options, FILE* file, const char* filename,
                    int total_files) {
  char line[MAX_LINE_LENGTH];
  int line_number = 0;
  int match_count = 0;
  int multiple_files = (total_files > 1 && !options->suppress_filenames);
  regex_t regex;
  int regex_compiled = 0;

  for (int i = 0; i < options->pattern_count; i++) {
    if (compile_regex_pattern(&regex, options->patterns[i],
                              options->ignore_case) == 0) {
      regex_compiled = 1;
      break;
    }
  }

  if (!regex_compiled) {
    if (!options->suppress_errors) {
      fprintf(stderr, "grep: failed to compile regex pattern\n");
    }
    return;
  }

  while (fgets(line, sizeof(line), file) != NULL) {
    line_number++;
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
    }

    int match_found = (regexec(&regex, line, 0, NULL, 0) == 0);
    if (options->invert_match) {
      match_found = !match_found;
    }

    if (match_found) {
      match_count++;
      if (!options->list_files && !options->count_matches) {
        handle_match_result(options, filename, line, line_number,
                            multiple_files, &regex);
      }
    }
  }

  generate_final_output(options, filename, match_count, multiple_files);
  regfree(&regex);
}

void handle_match_result(GrepOptions* options, const char* filename,
                         const char* line, int line_number, int multiple_files,
                         regex_t* regex) {
  if (options->show_only_matching && !options->invert_match) {
    handle_only_matching_output(options, filename, line, line_number,
                                multiple_files, regex);
  } else {
    if (multiple_files) printf("%s:", filename);
    if (options->show_line_numbers) printf("%d:", line_number);
    printf("%s\n", line);
  }
}

void handle_only_matching_output(GrepOptions* options, const char* filename,
                                 const char* line, int line_number,
                                 int multiple_files, regex_t* regex) {
  regmatch_t match;
  const char* current_position = line;
  int offset = 0;

  while (regexec(regex, current_position + offset, 1, &match, 0) == 0 &&
         match.rm_so != -1) {
    if (multiple_files) printf("%s:", filename);
    if (options->show_line_numbers) printf("%d:", line_number);

    for (int i = match.rm_so; i < match.rm_eo; i++) {
      putchar(current_position[offset + i]);
    }
    printf("\n");

    offset += match.rm_eo;
    if (match.rm_so == match.rm_eo) {
      offset++;
    }
  }
}

void generate_final_output(GrepOptions* options, const char* filename,
                           int match_count, int multiple_files) {
  if (options->list_files && match_count > 0) {
    printf("%s\n", filename);
    return;
  }

  if (options->count_matches) {
    if (multiple_files) {
      printf("%s:", filename);
    }
    printf("%d\n", match_count);
  }
}

void print_usage_info(const char* program_name) {
  fprintf(stderr, "Usage: %s [OPTIONS] PATTERN [FILE...]\n", program_name);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -e PATTERN   Specify search pattern directly\n");
  fprintf(stderr, "  -i           Ignore case distinctions\n");
  fprintf(stderr, "  -v           Select non-matching lines\n");
  fprintf(stderr, "  -c           Print only count of matching lines\n");
  fprintf(stderr, "  -l           Print only names of matching files\n");
  fprintf(stderr, "  -n           Print line numbers with output\n");
  fprintf(stderr, "  -h           Suppress file name prefix on output\n");
  fprintf(stderr, "  -s           Suppress error messages\n");
  fprintf(stderr, "  -f FILE      Read patterns from file\n");
  fprintf(stderr, "  -o           Print only the matching parts\n");
}