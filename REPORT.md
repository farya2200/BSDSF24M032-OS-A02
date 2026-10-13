
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
# Assignment Report — Questions and Answers

## Feature 2: v1.1.0 — Complete Long Listing Format

### Q1. What is the difference between `stat()` and `lstat()`? When is `lstat()` more appropriate?

- `stat()` retrieves metadata about the target file. If the path is a symbolic link, it follows the link and returns metadata about the file it points to.
- `lstat()` retrieves metadata about the symbolic link itself.
- In `ls`, `lstat()` is useful when we need to identify symbolic links and display their correct file type and permissions.

### Q2. How do bitwise operators and macros extract file types and permissions from `st_mode`?

The `st_mode` field contains both file-type information and permission bits. We use bitwise operators and predefined macros to check them.

- `S_ISDIR(st.st_mode)` checks whether the file is a directory.
- `S_ISREG(st.st_mode)` checks whether it is a regular file.
- `S_ISLNK(st.st_mode)` checks whether it is a symbolic link.
- `st.st_mode & S_IRUSR` checks the owner's read permission.
- `st.st_mode & S_IWUSR` checks the owner's write permission.
- `st.st_mode & S_IXUSR` checks the owner's execute permission.

The `&` operator masks the bits we want to examine. Similar macros exist for group and other permissions.

## Feature 3: v1.2.0 — Column Display (Down Then Across)

### Q1. How does down-then-across printing work, and why is one simple loop insufficient?

First, the program reads all filenames into an array and calculates the number of rows and columns. It then prints entries column by column while traversing the array row by row. For example, the first row may contain `names[0]`, `names[rows]`, and `names[2 * rows]`.

A simple loop prints consecutive filenames in sequence and cannot produce this column order by itself. The program needs calculated indexes and spacing to align the columns.

### Q2. What is the purpose of `ioctl()`? What are the limitations of a fixed width?

`ioctl()` with `TIOCGWINSZ` retrieves the terminal's dimensions, including its width in columns. The program uses this width to determine how many filenames can fit on each line.

A fixed width such as 80 characters does not adapt to terminal resizing. Output may wrap on narrow terminals or leave unused space on wide terminals. A fallback width is useful when terminal-size detection fails.

## Feature 4: v1.3.0 — Horizontal Column Display (`-x`)

### Q1. Compare vertical and horizontal display logic. Which requires more pre-calculation?

Down-then-across display requires more pre-calculation because the program must determine the number of rows and use calculated array indexes to place filenames in the correct columns.

Horizontal display is simpler: it processes filenames from left to right, adds spacing, and starts a new line when the next filename would exceed the available width.

### Q2. How does the program manage `-l`, `-x`, and the default display mode?

The program parses command-line options using `getopt()` and stores the selected options in flag variables. It then chooses the appropriate display logic:

- `-l`: displays detailed file information, including permissions, owner, group, size, and modification time.
- `-x`: displays filenames horizontally, from left to right.
- No display option: displays filenames in down-then-across columns.

The selected mode determines which output logic is used.

## Feature 5: v1.4.0 — Alphabetical Sorting

### Q1. Why must all filenames be read into memory before sorting? What are the drawbacks for very large directories?

Sorting requires access to the collection of filenames so their order can be rearranged before printing. The program stores filenames in a dynamically allocated array and sorts the array with `qsort()`.

For directories containing millions of entries, this approach can consume substantial memory and take additional time. Dynamic allocation can also fail if insufficient memory is available.

### Q2. Explain the comparison function used by `qsort()`.

The comparison function tells `qsort()` how to order two elements. Its general signature is:

`int compare_names(const void *a, const void *b);`

The `const void *` parameters allow `qsort()` to work with different data types. Since our array contains string pointers, the function casts the arguments to the appropriate pointer type and uses `strcmp()` to compare the filenames.

`strcmp()` returns a negative value if the first string comes before the second, zero if they are equal, and a positive value if the first comes after the second.

## Feature 6: v1.5.0 — Colorized Output

### Q1. How do ANSI escape codes produce terminal colors? Show the code for green text.

ANSI escape codes are special character sequences interpreted by compatible terminals to change text appearance. The sequence `\033[0;32m` selects green text, and `\033[0m` resets the formatting.

Example in C:

`printf("\033[0;32mGreen text\033[0m\n");`

The reset code prevents the color from unintentionally affecting subsequent output.

### Q2. Which permission bits determine whether a file is executable?

The execute bits are:

- `S_IXUSR`: executable by the owner.
- `S_IXGRP`: executable by members of the group.
- `S_IXOTH`: executable by others.

These can be checked against `st_mode`. For example, `st.st_mode & S_IXUSR` checks the owner's execute bit. If any applicable execute bit is set, the program can identify the file as executable for colorized output.

## Feature 7: v1.6.0 — Recursive Listing (`-R`)

### Q1. What is a base case, and how does it prevent infinite recursion?

A base case is a condition that stops a recursive function from making further recursive calls. In recursive directory listing, the program skips the special entries `.` and `..`, which refer to the current and parent directories.

It also stops descending when there are no more subdirectories to visit or when a directory cannot be opened. Skipping `.` and `..` is essential because following them would otherwise cause the program to revisit directories indefinitely.

### Q2. Why must the program construct a full path before a recursive call?

The program must combine the current directory path and the subdirectory name, for example, `parent_dir/subdir`, so it can locate the correct directory.

If the program simply calls `do_ls("subdir")`, the operating system interprets the relative path from the process's current working directory, not necessarily from `parent_dir`. This may cause it to list the wrong directory or fail to open it. Constructing the full path makes recursive traversal reliable.

