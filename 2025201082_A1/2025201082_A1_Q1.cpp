// Q1 Code
// Reversing a file using system calls only

#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>    // for open
#include <unistd.h>   // for read, write, close, lseek
#include <sys/stat.h> // for mkdir
#include <cstring>    // for strerror
#include <cerrno>     // for errno
#include <cstdlib>    // for atoi

using namespace std;

// function for reversing the buffer
void reverse_it(char *buffer, int size)
{
    int start = 0;
    int end = size - 1;
    
    // reversing the buffer contents by using 2 pointer approach
    while (start < end)
    {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
}

int main(int argc, char *argv[])
{
    // checking for correct number of arguments
    if (argc < 3)
    {
        cout << "Invalid input." << endl;
        cout << "Usage is like this: ./a.out <filename> <flag> [other stuff]" << endl;
        return 1;
    }

    // reading the command line arguments
    char *inFile = argv[1];
    int flag = atoi(argv[2]);

    // checking for valid values of flag
    if (flag < 0 || flag > 2)
    {
        cout << "Error: Invalid flag. Please use 0, 1, or 2." << endl;
        return 1;
    }

    // making the output directory, 0700 for user permissions
    mkdir("Assignment1", 0700);

    // making the output filename from the flag
    string outFile = "Assignment1/" + to_string(flag) + "_" + inFile;

    // opening input file in read only mode
    int fd_in = open(inFile, O_RDONLY);
    if (fd_in == -1)
    {
        cout << "Cannot open input file" << endl;
        return 1;
    }

    // opening output file for writing, will create if not there
    int fd_out = open(outFile.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0600);
    if (fd_out == -1)
    {
        cout << "Could not make output file" << endl;
        close(fd_in);
        return 1;
    }

    // getting total file size for progress bar
    long totalSize = lseek(fd_in, 0, SEEK_END);
    // going back to the start for reading
    lseek(fd_in, 0, SEEK_SET);

    long bytesDone = 0;
    cout << "Starting to process the file..." << endl;

    // --- Main Logic based on Flag ---
    if (flag == 0)
    {
        // flag 0: for block wise reversal
        if (argc != 4)
        {
            cout << "Usage for flag 0: ./a.out <filename> 0 <block_size>" << endl;
            return 1;
        }
        long blockSize = atoi(argv[3]);
        if (blockSize <= 0)
        {
            cout << "Error: Block size must be a positive number." << endl;
            return 1;
        }

        char *buf = new char[blockSize]; // buffer for reversing the bytes

        // looping through the file block by block
        while (true)
        {
            int readCount = read(fd_in, buf, blockSize);
            if (readCount <= 0)
                break; // means end of file or some error

            reverse_it(buf, readCount);    // reversing the block
            write(fd_out, buf, readCount); // writing it out

            // updating and printing progress
            bytesDone += readCount;
            int pct = (bytesDone * 100) / totalSize;
            cout << "Progress: " << pct << "%\r";
            fflush(stdout);
        }
        delete[] buf; 
    }
    else if (flag == 1) // flag 1: for reversing the whole file
    {
        if (argc != 3)
        {
            cout << "Usage for flag 1: ./a.out <filename> 1" << endl;
            return 1;
        }
        const long CHUNK = 4 * 1024 * 1024; // 4MB chunk
        char *buf = new char[CHUNK];        // buffer for reversing the bytes
        // starting from file end and going backwards
        long pos = totalSize;
        while (pos > 0)
        {
            long readSize = CHUNK;
            
            // this if is for the last remaining bytes, which might be smaller than the chunk size
            // so we don't try to read beyond index=0
            if (pos < CHUNK)
                readSize = pos;
            pos -= readSize; // move 1 block backwards for the next read

            lseek(fd_in, pos, SEEK_SET); // bringing the reading head to the correct position for the next read
            int readCount = read(fd_in, buf, readSize);
            if (readCount <= 0)
                break;

            reverse_it(buf, readCount);    // reversing the chunk we just read
            write(fd_out, buf, readCount); // writing it

            // updating and printing progress
            bytesDone += readCount;
            int pct = (bytesDone * 100) / totalSize;
            cout << "Progress: " << pct << "%\r";
            fflush(stdout);
        }
        delete[] buf; // must free the memory
    }
    else if (flag == 2)
    {
        
        if (argc != 5)
        {
            cout << "Usage for flag 2: ./a.out <filename> 2 <start> <end>" << endl;
            return 1;
        }
        long start = atoi(argv[3]);
        long end = atoi(argv[4]);

        // all the validation checks
        if (start < 0 || end < 0)
        {
            cout << "Error: Index numbers cannot be negative." << endl;
            return 1;
        }
        if (start > end)
        {
            cout << "Error: Start index cannot be greater than end index." << endl;
            return 1;
        }
        if (end >= totalSize)
        {
            cout << "Error: End index is out of bounds." << endl;
            return 1;
        }

        const long CHUNK = 4 * 1024 * 1024;
        char *buf = new char[CHUNK]; // making buffer for reversing the bytes

        // Part 1: reversing the first part of file
        long p1_pos = start; // start reversing from here backwards
        while (p1_pos > 0)
        {
            long readSize = CHUNK;
            if (p1_pos < CHUNK)
                readSize = p1_pos;
            p1_pos -= readSize; // decrement position for next loop

            lseek(fd_in, p1_pos, SEEK_SET); // seek and read
            int readCount = read(fd_in, buf, readSize);
            if (readCount <= 0)
                break;

            reverse_it(buf, readCount);    // reverse
            write(fd_out, buf, readCount); // write

            bytesDone += readCount;
            int pct = (bytesDone * 100) / totalSize;
            cout << "Progress: " << pct << "%\r";
            fflush(stdout);
        }

        // Part 2: copying middle part as it is
        // read from the input file and write to the ouput file
        lseek(fd_in, start, SEEK_SET);    // go to start of middle section
        long midSize = (end - start) + 1; // calculate size of this part
        while (midSize > 0)
        {
            long readSize = CHUNK;
            if (midSize < CHUNK)
                readSize = midSize; // for the last bit

            int readCount = read(fd_in, buf, readSize);
            if (readCount <= 0)
                break;

            write(fd_out, buf, readCount); // no reversal needed here

            midSize -= readCount; // decrease remaining size
            bytesDone += readCount;
            int pct = (bytesDone * 100) / totalSize;
            cout << "Progress: " << pct << "%\r";
            fflush(stdout);
        }

        // Part 3: reversing the last part of file
        // this is also like flag 1, but with different start and end points
        long p3_start = end + 1; // starts after the middle part
        long p3_pos = totalSize; // start from the very end of the file
        while (p3_pos > p3_start)
        {
            long readSize = CHUNK;
            
            if (p3_pos - p3_start < CHUNK)
                readSize = p3_pos - p3_start;
            p3_pos -= readSize;

            lseek(fd_in, p3_pos, SEEK_SET); // bring reading head to the correct position for next read
            int readCount = read(fd_in, buf, readSize);
            if (readCount <= 0)
                break;

            reverse_it(buf, readCount);    // reverse
            write(fd_out, buf, readCount); // write

            bytesDone += readCount;
            int pct = (bytesDone * 100) / totalSize;
            cout << "Progress: " << pct << "%\r";
            fflush(stdout);
        }

        delete[] buf; 
    }


    close(fd_in);
    close(fd_out);

    return 0;
}
