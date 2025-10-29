# Assignment 1 – Advanced Operating Systems
**File Reversal & Verification (Q1 & Q2)** 

## Overview
This assignment implements two related C++ programs using **only system calls** (no C library file I/O). 

- **Q1.cpp** – Reads an input file and creates a reversed version based on the chosen *mode* (flag). The reversed file is stored in the `Assignment1` directory.
- **Q2.cpp** – Verifies that the reversed file was created correctly, checks its permissions, and ensures directory and file properties match the requirements.

Both programs are designed to work efficiently with **very large files** (even >1GB) that may be bigger than the available RAM.



## Q1 – File Reversal Modes

### **Flag 0 – Block-wise Reversal**
- The file is split into fixed-size blocks (given by the user). 
- Each block is reversed **individually**, but block order remains the same.

**Example (Block size = 4)** 

Original:  [A B C D] [E F G H]
Reversed:  [D C B A] [H G F E]

Command:
./a.out <input_file> 0 <block_size>


### **Flag 1 – Full File Reversal**
- The **entire** file is reversed as a whole.
- The first byte becomes the last, second becomes second-last, etc.

**Example** 

Original:  A  B  C  D  E  F
Reversed:  F  E  D  C  B  A

Command:
./a.out <input_file> 1


### **Flag 2 – Partial Range Reversal**
- Takes two indices: **start** and **end** (0-based). 
- Three regions are handled:
  1. **Prefix** (0 to `start-1`) → reversed.
  2. **Middle** (`start` to `end`) → unchanged.
  3. **Suffix** (`end+1` to EOF) → reversed.

**Example** (`start=2`, `end=7`) 

Original:  A  B  [C  D  E  F  G  H]  I  J  K
              ^       unchanged       ^
Reversed:  B  A  [C  D  E  F  G  H]  K  J  I

Command:
./a.out <input_file> 2 <start_index> <end_index>


## **Q1 Features**
- Creates `Assignment1/` automatically (permissions: `700`).
- Output file permissions: `600`.
- Shows a **progress bar** during processing.
- Works on huge files efficiently via chunked I/O.
- Validates inputs and handles errors cleanly.



## Q2 – File & Permission Verifier

### **Checks Performed**
1. Directory existence.
2. Content verification based on the flag (0/1/2).
3. File size comparison.
4. Permission check for:
   - New file
   - Original file
   - Directory (9 lines each).


### **Expected Permissions**
| Path              | Mode | User | Group | Others |
|-------------------|------|------|-------|--------|
| Directory         | 700  | rwx  | ---   | ---    |
| New file          | 600  | rw-  | ---   | ---    |
| Original file     | 644  | rw-  | r--   | r--    |


### **Q2 Usage**

# Flag 0
./a.out <newfile> <oldfile> <directory> 0 <block_size>

# Flag 1
./a.out <newfile> <oldfile> <directory> 1

# Flag 2
./a.out <newfile> <oldfile> <directory> 2 <start_index> <end_index>


## **Compilation**

g++ Q1.cpp -o q1
g++ Q2.cpp -o q2



## **Example Workflow**

# 1. Create reversed file (full reversal)
./q1 input.txt 1

# 2. Verify it
./q2 Assignment1/1_input.txt input.txt Assignment1 1


## **Implementation Notes**
- Only **low-level system calls** are used: `read`, `write`, `lseek`, `stat`, `mkdir`, `open`, `close`, etc.
- Buffer sizes are tuned for performance.
- Handles invalid ranges, missing files, and wrong arguments.
- Works reliably even if the file is larger than system memory.
