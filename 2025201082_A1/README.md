# **CS3.304 \- Assignment 1**

This folder contains the two C++ programs for Assignment 1\.

* 2025201082\_A1\_Q1.cpp: This program reverses a file's content based on the flag value given by the user.  
* 2025201082\_A1\_Q2.cpp: This program checks if the Q1 output is correct and verifies the permissions of the files and directory created by the 1st program.

### **How to Compile**

Both files can be compiled using g++.
\# For Q1  
g++ 2025201082\_A1\_Q1.cpp \-o q1

\# For Q2  
g++ 2025201082\_A1\_Q2.cpp \-o q2

### **How to Run Q1 (File Reversal)**

The first program takes an input file, a flag, and additional arguments depending on the flag.  
General Usage:  
./q1 \<input\_file\_name\> \<flag\> \[arguments\]  
**Examples:**

* Flag 0 (Block-wise reversal):  
  Reverses the file in blocks of 1024 bytes.  
  ./q1 my\_file.txt 0 1024

* Flag 1 (Full file reversal):  
  Reverses the entire file.  
  ./q1 my\_file.txt 1

* Flag 2 (Partial reversal):  
  Reverses from byte 0 to 99, and from byte 501 to the end. The section from 100 to 500 remains unchanged.  
  ./q1 my\_file.txt 2 100 500

### **Q1 Code Explanation**

The code for Q1 first validates the user-provided arguments. It then creates the Assignment1 directory if it does not exist.  
To handle large files, the program does not load the entire file into memory. Instead, it reads the file in 4MB blocks.

* **Flag 0:** The program reads the file block by block, reverses each block and writes it to the output file.  
* **Flag 1:** The program reads the input file from the end in blocks. It uses lseek() to jump to a position near the end, reads a block, reverses it, and writes it to the output file. This process is repeated and we keep moving backwards until the reading head reaches the start of the file.  
* **Flag 2:** This operation is done in three steps:  
  1. First, the starting part (from 0 to start\_index-1) is reversed using the same logic as Flag 1\.  
  2. Then, the middle part (from start\_index to end\_index) is copied directly without any changes.  
  3. Finally, the last part (from end\_index+1 to the last byte) is reversed, again using the same logic as Flag 1 for that section.

During the file processing, the current progress% is also displayed.

### **How to Run Q2 (Verification)**

The second program checks the output of the first program.  
General Usage:  
./q2 \<new\_file\_path\> \<old\_file\_path\> \<directory\_path\> \<flag\> \[arguments\]  
**Examples:**

* **For Flag 0 output:**  
  ./q2 Assignment1/0\_my\_file.txt my\_file.txt Assignment1 0 1024

* **For Flag 1 output:**  
  ./q2 Assignment1/1\_my\_file.txt my\_file.txt Assignment1 1

* **For** Flag **2 output:**  
  ./q2 Assignment1/2\_my\_file.txt my\_file.txt Assignment1 2 100 500

### **Q2 Code Explanation**

This program is used to check if Q1's program ran corectly or not.  
First, it performs basic checks, such as whether the Assignment1 directory was created and if the input and output files have the same size.  
Then, it checks the file contents based on the flag value given by the user:

* **Flag 0:** It reads both files block by block, reverses the block from the old file, and compares it with the block from the new file.  
* **Flag 1:** It reads the old file from the start and the new file from the end, block by block, to compare them and ensure the reversal is correct.  
* **Flag 2:** For this flag, the program reads all three sections (start, middle, and end) from both files separately and compares them to verify the logic was implemented correctly.

At last, it uses the stat() system call to get the permissions for the new file, old file, and the directory, printing them in the required Yes/No format.
