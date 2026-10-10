
#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void do_ls(const char *dir, int long_format, int horizontal);
void print_long(const char *path, const char *name);

int main(int argc, char *argv[])
{
    int opt;
    int long_format = 0;
    int horizontal = 0;

    while ((opt = getopt(argc, argv, "lx")) != -1)
    {
        if (opt == 'l')
            long_format = 1;
        else if (opt == 'x')
            horizontal = 1;
        else
        {
            fprintf(stderr, "Usage: %s [-l] [-x] [directory ...]\n",
                    argv[0]);
            return 1;
        }
    }

    if (optind == argc)
        do_ls(".", long_format, horizontal);
    else
    {
        for (int i = optind; i < argc; i++)
        {
            printf("Directory listing of %s:\n", argv[i]);
            do_ls(argv[i], long_format, horizontal);
            puts("");
        }
    }

    return 0;
}

void do_ls(const char *dir, int long_format, int horizontal)
{
    DIR *dp = opendir(dir);

    if (dp == NULL)
    {
        perror(dir);
        return;
    }

    struct dirent *entry;
    char **names = NULL;
    size_t count = 0;
    size_t capacity = 0;

    errno = 0;

    while ((entry = readdir(dp)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;

        if (count == capacity)
        {
            size_t new_capacity = capacity == 0 ? 16 : capacity * 2;
            char **temp = realloc(names,
                                  new_capacity * sizeof(*names));

            if (temp == NULL)
            {
                perror("realloc");
                break;
            }

            names = temp;
            capacity = new_capacity;
        }

        names[count] = strdup(entry->d_name);

        if (names[count] == NULL)
        {
            perror("strdup");
            break;
        }

        count++;
    }

    if (errno != 0)
        perror("readdir");

    closedir(dp);

    if (long_format)
    {
        for (size_t i = 0; i < count; i++)
            print_long(dir, names[i]);
    }
    else
    {
        struct winsize ws;
        int terminal_width = 80;

        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 &&
            ws.ws_col > 0)
            terminal_width = ws.ws_col;

        size_t max_length = 0;

        for (size_t i = 0; i < count; i++)
        {
            size_t length = strlen(names[i]);
            if (length > max_length)
                max_length = length;
        }

        size_t cell_width = max_length + 2;
        size_t columns = (size_t)terminal_width / cell_width;

        if (columns == 0)
            columns = 1;

        size_t rows = (count + columns - 1) / columns;

        if (horizontal)
        {
            /* Horizontal: fill each row from left to right. */
            for (size_t i = 0; i < count; i++)
            {
                printf("%-*s", (int)cell_width, names[i]);

                if ((i + 1) % columns == 0 || i + 1 == count)
                    putchar('\n');
            }
        }
        else
        {
            /* Default: fill down each column, then across. */
            for (size_t r = 0; r < rows; r++)
            {
                for (size_t c = 0; c < columns; c++)
                {
                    size_t i = c * rows + r;

                    if (i < count)
                        printf("%-*s", (int)cell_width, names[i]);
                }

                putchar('\n');
            }
        }
    }

    for (size_t i = 0; i < count; i++)
        free(names[i]);

    free(names);
}

void print_long(const char *path, const char *name)
{
    char fullpath[4096];
    struct stat info;

    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);

    if (lstat(fullpath, &info) == -1)
    {
        perror(fullpath);
        return;
    }

    char permissions[11] = "----------";

    if (S_ISDIR(info.st_mode)) permissions[0] = 'd';
    else if (S_ISLNK(info.st_mode)) permissions[0] = 'l';
    else if (S_ISCHR(info.st_mode)) permissions[0] = 'c';
    else if (S_ISBLK(info.st_mode)) permissions[0] = 'b';
    else if (S_ISFIFO(info.st_mode)) permissions[0] = 'p';
    else if (S_ISSOCK(info.st_mode)) permissions[0] = 's';

    if (info.st_mode & S_IRUSR) permissions[1] = 'r';
    if (info.st_mode & S_IWUSR) permissions[2] = 'w';
    if (info.st_mode & S_IXUSR) permissions[3] = 'x';
    if (info.st_mode & S_IRGRP) permissions[4] = 'r';
    if (info.st_mode & S_IWGRP) permissions[5] = 'w';
    if (info.st_mode & S_IXGRP) permissions[6] = 'x';
    if (info.st_mode & S_IROTH) permissions[7] = 'r';
    if (info.st_mode & S_IWOTH) permissions[8] = 'w';
    if (info.st_mode & S_IXOTH) permissions[9] = 'x';

    struct passwd *owner = getpwuid(info.st_uid);
    struct group *group = getgrgid(info.st_gid);

    char timebuf[32];
    struct tm *tm_info = localtime(&info.st_mtime);

    if (tm_info != NULL)
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm_info);
    else
        snprintf(timebuf, sizeof(timebuf), "unknown time");

    printf("%s %lu %s %s %8ld %s %s\n",
           permissions,
           (unsigned long)info.st_nlink,
           owner ? owner->pw_name : "?",
           group ? group->gr_name : "?",
           (long)info.st_size,
           timebuf,
           name);
}
