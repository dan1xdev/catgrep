#ifndef S21_GREP_H
#define S21_GREP_H

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATTERNS 100
#define MAX_LINE_LENGTH 1024

typedef struct {
  int enable_pattern;      // -e flag
  int ignore_case;         // -i flag
  int invert_match;        // -v flag
  int count_matches;       // -c flag
  int list_files;          // -l flag
  int show_line_numbers;   // -n flag
  int suppress_errors;     // -s flag
  int suppress_filenames;  // -h flag
  int read_patterns_file;  // -f flag
  int show_only_matching;  // -o flag

  char patterns[MAX_PATTERNS][MAX_LINE_LENGTH];
  int pattern_count;
} GrepOptions;

// Основные функции
int main(int argc, char* argv[]);
int parse_command_line(int argc, char* argv[], GrepOptions* options,
                       char* files[], int* file_count);

// Обработка аргументов командной строки
int parse_arguments(int argc, char* argv[], GrepOptions* options,
                    int* pattern_found);
int handle_command_line_option(int opt, GrepOptions* options,
                               int* pattern_found);
int process_remaining_arguments(int argc, char* argv[], GrepOptions* options,
                                int* pattern_found, char* files[],
                                int* file_count);
int handle_single_argument(char* argv[], int index, int* pattern_found,
                           GrepOptions* options, char* files[], int* file_count,
                           int argc);
int handle_combined_options(char* option_string, char* argv[], int* index,
                            GrepOptions* options, int* pattern_found, int argc);

// Обработка специфичных опций
int handle_pattern_option(char* opt_str, size_t pos, size_t opt_len,
                          char* argv[], int* index, GrepOptions* options,
                          int* pattern_found, int argc);
int handle_file_pattern_option(char* opt_str, size_t pos, size_t opt_len,
                               char* argv[], int* index, GrepOptions* options,
                               int* pattern_found, int argc);

// Управление паттернами
void add_search_pattern(GrepOptions* options, const char* pattern);
void load_patterns_from_file(GrepOptions* options, const char* filename);

// Основная логика grep
void process_input_files(GrepOptions* options, int file_count, char* files[]);
void search_in_file(GrepOptions* options, FILE* file, const char* filename,
                    int total_files);
int compile_regex_pattern(regex_t* regex, const char* pattern, int ignore_case);

// Обработка результатов поиска
void handle_match_result(GrepOptions* options, const char* filename,
                         const char* line, int line_number, int multiple_files,
                         regex_t* regex);
void handle_only_matching_output(GrepOptions* options, const char* filename,
                                 const char* line, int line_number,
                                 int multiple_files, regex_t* regex);
void generate_final_output(GrepOptions* options, const char* filename,
                           int match_count, int multiple_files);

// Вспомогательные функции
void print_usage_info(const char* program_name);

#endif