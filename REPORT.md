
# Operating Systems Assignment 02
## Re-engineering the Unix ls Command

**Student:** Faria Azhar  
**Repository:** BSDSF24M032-OS-A02

## Project Overview

This project implements a simplified version of the Unix `ls` command in C. It lists directory contents and supports long-format output, column layouts, alphabetical sorting, colorized filenames, and recursive directory traversal.

## Features Implemented

### Feature 1: Basic Listing — v1.0.0
Lists non-hidden files and directories in the current directory or a specified directory.

### Feature 2: Long Listing — v1.1.0
The `-l` option displays file permissions, link count, owner, group, file size, modification time, and filename.

### Feature 3: Column Listing — v1.2.0
Displays filenames in columns based on the terminal width. The default layout fills entries down columns before moving across.

### Feature 4: Horizontal Listing — v1.3.0
The `-x` option displays filenames from left to right, filling each row before starting the next.

### Feature 5: Alphabetical Sorting — v1.4.0
Uses `qsort()` and `strcmp()` to sort filenames alphabetically.

### Feature 6: Colorized Output — v1.5.0
Uses ANSI escape sequences to distinguish directories, executable files, archive files, symbolic links, and special files.

### Feature 7: Recursive Listing — v1.6.0
The `-R` option recursively lists subdirectories and displays directory headers.

## Compilation

The project is compiled using the provided Makefile:

```bash
make clean
make
```

The compiler uses GCC with `-Wall -Wextra -std=c11`.

## Testing

The following commands were used to test the implementation:

```bash
./bin/ls
./bin/ls -l
./bin/ls -x
./bin/ls -R
./bin/ls -lR
```

A test directory containing a regular file, an executable file, an archive filename, a subdirectory, and a symbolic link was also used to test colorized output and recursive traversal.

## Implementation Techniques

- Directory traversal: `opendir()` and `readdir()`
- File metadata: `lstat()`
- User and group names: `getpwuid()` and `getgrgid()`
- Command-line options: `getopt()`
- Dynamic memory: `realloc()`, `strdup()`, and `free()`
- Alphabetical sorting: `qsort()` and `strcmp()`
- Terminal dimensions: `ioctl()` and `TIOCGWINSZ`
- Recursive traversal: calling the directory-listing function for subdirectories
- Color output: ANSI escape sequences

## Conclusion

This project demonstrates directory handling, file metadata access, dynamic memory allocation, command-line parsing, sorting, terminal formatting, color output, and recursion in C. Git branches, commits, tags, and GitHub releases were used to organize the development of each feature.
