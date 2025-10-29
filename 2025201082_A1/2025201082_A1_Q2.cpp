// Q2 Code
// This program is for checking if Q1 ran correctly or not.
// It will also check permissions and also if the file content is correct.

#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include <cstdlib>
#include <algorithm>

using namespace std;

// function to reverse a buffer by using two pointer technique
void reverse_it(char *buffer, int size)
{
    int start = 0;
    int end = size - 1;
    while (start < end)
    {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
}

void print_permissions(const char *name, const char *path)
{
    struct stat fileStat;
    if (stat(path, &fileStat) < 0)
    {
        cout << "Could not get permissions for " << name << endl;
        return;
    }

    cout << "User has read permission: " << ((fileStat.st_mode & S_IRUSR) ? "Yes" : "No") << endl;
    cout << "User has write permission: " << ((fileStat.st_mode & S_IWUSR) ? "Yes" : "No") << endl;
    cout << "User has execute permission: " << ((fileStat.st_mode & S_IXUSR) ? "Yes" : "No") << endl;
    cout << "Group has read permission: " << ((fileStat.st_mode & S_IRGRP) ? "Yes" : "No") << endl;
    cout << "Group has write permission: " << ((fileStat.st_mode & S_IWGRP) ? "Yes" : "No") << endl;
    cout << "Group has execute permission: " << ((fileStat.st_mode & S_IXGRP) ? "Yes" : "No") << endl;
    cout << "Others has read permission: " << ((fileStat.st_mode & S_IROTH) ? "Yes" : "No") << endl;
    cout << "Others has write permission: " << ((fileStat.st_mode & S_IWOTH) ? "Yes" : "No") << endl;
    cout << "Others has execute permission: " << ((fileStat.st_mode & S_IXOTH) ? "Yes" : "No") << endl;
}

int main(int argc, char *argv[])
{
    // checking the no. of arguments given
    if (argc < 5)
    {
        cout << "Invalid input" << endl;
        cout << "Correct Usage for Flag=0: /a.out <newfilepath> <oldfilepath> <directory> <flag> <blockSize>" << endl;
        return 1;
    }

    // getting all the paths and flag from arguments
    char *newFile = argv[1];
    char *oldFile = argv[2];
    char *dir = argv[3];
    int flag = atoi(argv[4]);

    // checking if the flag value itself is valid
    if (flag < 0 || flag > 2)
    {
        cout << "Error: Invalid flag. Must be 0, 1, or 2." << endl;
        return 1;
    }


    // Checking if the directory exists
    struct stat dirStat;
    
    // checking if the path is a directory
    if (stat(dir, &dirStat) == 0 && S_ISDIR(dirStat.st_mode))
    {
        cout << "Directory is created: Yes" << endl;
    }
    else
    {
        cout << "Directory is created: No" << endl;
    }

    // opening both files to compare them
    int fd_new = open(newFile, O_RDONLY);
    int fd_old = open(oldFile, O_RDONLY);

    if (fd_new == -1 || fd_old == -1)
    {
        cout << "Error opening one of the files for checking" << endl;
        return 1;
    }

    // Checking if file sizes are same or not
    struct stat stat_new, stat_old;
    fstat(fd_new, &stat_new); // fstat is like stat but for file descriptors
    fstat(fd_old, &stat_old);

    if (stat_new.st_size == stat_old.st_size)
    {
        cout << "Both Files Sizes are Same: Yes" << endl;
    }
    else
    {
        cout << "Both Files Sizes are Same: No" << endl;
    }

    // Checking if content is processed correctly based on flag
    bool content_ok = true; // assuming it to be correct initially

    if (flag == 0)
    {
        if (argc != 6)
        {
            cout << "Missing block size for flag 0 check" << endl;
            return 1;
        }
        long blockSize = atoi(argv[5]);
        if (blockSize <= 0)
        {
            cout << "Error: Block size must be positive." << endl;
            return 1;
        }

        char *buf_new = new char[blockSize];
        char *buf_old = new char[blockSize];

        while (true)
        {
            int bytes_old = read(fd_old, buf_old, blockSize);
            int bytes_new = read(fd_new, buf_new, blockSize);
            if (bytes_old <= 0)
                break; // end of file

            reverse_it(buf_old, bytes_old); // reverse the original block
            if (memcmp(buf_new, buf_old, bytes_old) != 0) // memcmp compares memory blocks (return 0 if equal, non-zero otherwise)
            {                       
                content_ok = false; // if they don't match, something is wrong
                break;
            }
        }
        delete[] buf_new;
        delete[] buf_old;
    }
    else if (flag == 1)
    {
        if (argc != 5)
        {
            cout << "Invalid input" << endl;
            cout << "Correct Usage for Flag=1: /a.out <newfilepath> <oldfilepath> <directory> <flag> " << endl;
            return 1;
        }
        const int CHUNK = 4096;
        char *buf_new = new char[CHUNK];
        char *buf_old = new char[CHUNK];
        long totalSize = stat_old.st_size;
        long current_pos = 0;

        while (current_pos < totalSize)
        {
            int bytes_read = read(fd_old, buf_old, CHUNK);
            if (bytes_read <= 0)
                break;

            // read from index=0 of the input file, but from end of new file
            lseek(fd_new, totalSize - current_pos - bytes_read, SEEK_SET);
            read(fd_new, buf_new, bytes_read);
            reverse_it(buf_new, bytes_read); // reverse the new file's chunk to match old

            if (memcmp(buf_old, buf_new, bytes_read) != 0)
            {
                content_ok = false;
                break;
            }
            current_pos += bytes_read;
        }
        delete[] buf_new;
        delete[] buf_old;
    }
    else if (flag == 2)
    {

        if (argc != 7)
        {
            cout << "Invalid input" << endl;
            cout << "Correct Usage for Flag=0: /a.out <newfilepath> <oldfilepath> <directory> <flag> <start> <end>" << endl;
            return 1;
        }
        long start = atoi(argv[5]);
        long end = atoi(argv[6]);

        if (start < 0 || end < 0)
        {
            cout << "Error: Indices cannot be negative." << endl;
            return 1;
        }
        if (start > end)
        {
            cout << "Error: Start index cannot be greater than end index." << endl;
            return 1;
        }
        if (end >= stat_old.st_size)
        {
            cout << "Error: End index is out of bounds." << endl;
            return 1;
        }

        // checking part 1 (reversed)
        if (start > 0)
        {
            char *p1_old = new char[start];
            char *p1_new = new char[start];
            lseek(fd_old, 0, SEEK_SET);
            read(fd_old, p1_old, start);
            lseek(fd_new, 0, SEEK_SET);
            read(fd_new, p1_new, start);
            reverse_it(p1_old, start);
            if (memcmp(p1_old, p1_new, start) != 0)
                content_ok = false;
            delete[] p1_old;
            delete[] p1_new;
        }

        // checking part 2 (same)
        long midSize = end - start + 1;
        if (midSize > 0 && content_ok)
        {
            char *p2_old = new char[midSize];
            char *p2_new = new char[midSize];
            lseek(fd_old, start, SEEK_SET);
            read(fd_old, p2_old, midSize);
            lseek(fd_new, start, SEEK_SET);
            read(fd_new, p2_new, midSize);
            if (memcmp(p2_old, p2_new, midSize) != 0)
                content_ok = false;
            delete[] p2_old;
            delete[] p2_new;
        }

        // checking part 3 (reversed)
        long p3_start = end + 1;
        long p3_size = stat_old.st_size - p3_start;
        if (p3_size > 0 && content_ok)
        {
            char *p3_old = new char[p3_size];
            char *p3_new = new char[p3_size];
            lseek(fd_old, p3_start, SEEK_SET);
            read(fd_old, p3_old, p3_size);
            lseek(fd_new, p3_start, SEEK_SET);
            read(fd_new, p3_new, p3_size);
            reverse_it(p3_old, p3_size);
            if (memcmp(p3_old, p3_new, p3_size) != 0)
                content_ok = false;
            delete[] p3_old;
            delete[] p3_new;
        }
    }

    cout << "Whether file contents are correctly processed: " << (content_ok ? "Yes" : "No") << endl;

    // Printing all the permissions for all files and directory
    print_permissions("New File", newFile);
    print_permissions("Old File", oldFile);
    print_permissions("Directory", dir);

    close(fd_new);
    close(fd_old);

    return 0;
}
