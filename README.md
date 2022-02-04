# Custom Written ls Command

## Description
This repository currently contains a C file which when executed will perform an ls -l on the directory passed to it.

## Usage
1. compile the code using gcc
    ```bash
    gcc main.c -o main
    ```
2. OPTION:
   1. Run the program with no input to recieve info for current directory.

        ```bash
        [billDesktop@fedora lab2]$ ./main
        rwxr-xr-x billDesktop billDesktop    90 Feb 04 2022 [07:49] .      
        rwxr-xr-x billDesktop billDesktop    74 Feb 02 2022 [16:31]  ..     
        rwxrwxr-x billDesktop billDesktop   204 Feb 04 2022 [07:48]  .git   
        rwxr-xr-x billDesktop billDesktop    86 Feb 04 2022 [07:07]  .vscode
        rw-rw-r-- billDesktop billDesktop    95 Jan 27 2022 [08:05]  .gitignore
        rwxr-xr-x billDesktop billDesktop   360 Feb 01 2022 [10:21]  build  
        rw------- billDesktop billDesktop  8601 Feb 04 2022 [07:47]  main.c 
        rwxr-xr-x billDesktop billDesktop 32160 Feb 04 2022 [07:46]  main   
        rw-r--r-- billDesktop billDesktop   840 Feb 04 2022 [07:58]  README.md

        ```
    2. Run the program with a specified directory (relative or absoloute works).
   
        ```bash
        [billDesktop@fedora lab2 first]$ ./main /home/billDesktop/Documents/TrentU/2300
        rwx------ billDesktop billDesktop   846 Feb 01 2022 [20:08]  .      
        rwx------ billDesktop billDesktop   262 Jan 30 2022 [18:20]  ..     
        rwxr-xr-x billDesktop billDesktop     0 Jan 08 2022 [10:20]  textbooknotes
        rw-r--r-- billDesktop billDesktop 4169142 Jan 11 2022 [17:03]  Mars4_5.jar
        rwxrw-r-- billDesktop billDesktop    55 Jan 11 2022 [17:06]  Mars.sh
        rwxrwxr-x billDesktop billDesktop    44 Jan 31 2022 [19:41]  asmFiles
        rw-r--r-- billDesktop billDesktop 173431 Jan 18 2022 [12:38]  Assignment1.odt
        rw-r--r-- billDesktop billDesktop 12116 Jan 18 2022 [12:38]  Assignment1.ods
        rw-r--r-- billDesktop billDesktop 119950 Jan 18 2022 [19:26]  vanleeuwen_lab1_2.png
        rw-r--r-- billDesktop billDesktop 23452 Jan 18 2022 [19:26]  VanLeeuwen_lab1_2.png
        rw-r--r-- billDesktop billDesktop     0 Jan 25 2022 [18:16]  vanleeuwen_lab2_1.ng
        rw-r--r-- billDesktop billDesktop 140721 Jan 27 2022 [08:52]  Assignment1.docx
        rw-r--r-- billDesktop billDesktop 237133 Jan 27 2022 [08:53]  Assignment1.pdf
        rwxr-xr-x billDesktop billDesktop   152 Jan 28 2022 [11:56]  lab3   
        rw-r--r-- billDesktop billDesktop 20872 Jan 31 2022 [08:59]  Textbook Notes.odt
        rw------- billDesktop billDesktop  9591 Jan 31 2022 [14:27]  lecturenotes.odt
        rw-rw-r-- billDesktop billDesktop   157 Feb 01 2022 [19:43]  main.c 
        rwxrwxr-x billDesktop billDesktop 23536 Feb 01 2022 [19:44]  main   
        rw-rw-r-- billDesktop billDesktop   997 Feb 01 2022 [19:47]  main.s 
        ```

## What I learned
- C Programming
- Systems Programming on UNIX with C