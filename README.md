# canned-file-share

## Getting started

Compile the NfileTree.cpp file using the provided makefile by simply navigating to the directory canned-file-share and typing in your terminal:
```
make
```
 We have provided an example text file input.txt with a set of commands, and three files with sample text to be uploaded. Run the resulting executable file by running the following in the terminal in the directory that you have compiled NFileTree.cpp. 
 ```
./NFileTree < input.txt
```
You can also enter your own commands in the file management system by writing commands in the virtual command line interface (CLI) followed by the necessary arguments. Running the executable on its own will print the following prompt:
```
./NFileTree
Enter your command:
```
After which you can use the provided functionalities following the format shown below. 

## Using canned-file-share via Command Line Interface 

### moveFile

```
 moveFile [name of the file to be moved] [name of the folder to move the file to]
```
This function moves a file from one folder to another. The inputs are the name of the file and the folder that you wish to move the file to. It searches the nodes for the file and folder and then updates the parent/children relationships within the tree.

### copyFile

```
 copyFile [name of the file to be copied] [name of the new file] [name of the folder to copy new file into]
```
This function will create a copy of an existing file under a new file name. It uses a Copy on Write mechanism, meaning that when copy is called the first time only the metadata of the file is copied (eg. the pointers to the chunks) but the actual data is not copied over until one of the files is modified. This is accomplished by each chunk having a reference counter that tells you if more than one file uses that chunk. When a file is being modified, the function will first need to check if the chunk belongs to only one file. If it belongs to more than one file, then at that point the data will be copied to another space in memory. The value here is that we save memory by not having unnecessary copies of the identical data.

### deleteFile

```
 deleteFile [name of the file to be deleted]
```
This function takes an inputted file name, finds that file in name in the tree, and removes it. It will free up the space designated to the chunks, and will remove the node for that file from the tree.

```
 createFolder [new folder name] [location to create folder in (name of folder)]
```

### storeFile

```
 storeFile [file to be stored] [location to store file in (name of folder)]
```
This function loads a new file into the storage system from the directory that the program is running in. It takes as an input the name of the file being uploaded. It will create a new tree node to represent the file

### printTree

```
 printTree
```
This function will print out all the files in the tree. It uses the search function (which is a depth first search), and will print out the names of the files and folders as it traverses through each node. 

### quit usage
```
 quit
```
## Results from example file input.txt

### Part 1
<table>
<tr>
<th> Commands </th>
<th> Result </th>
</tr>
<tr>
<td>

```
createFolder Folder1 RootFolder
createFolder Folder2 Folder1
storeFile sample.txt Folder1
storeFile sample2.txt Folder2
printTree
```

</td>
<td>

```
- RootFolder (folder)
  - Folder1 (folder)
    - Folder2 (folder)
      - sample2.txt (txt)
    - sample.txt (txt)
```

</td>
</tr>
</table>

### Part 2

<table>
<tr>
<th> Commands </th>
<th> Result </th>
</tr>
<tr>
<td>

```
getFile sample2.txt
createFolder Folder3 Folder2
moveFile sample2.txt Folder3
deleteFile sample2.txt
printTree
```

</td>
<td>

```
- RootFolder (folder)
  - Folder1 (folder)
    - Folder2 (folder)
      - Folder3 (folder)
    - sample.txt (txt)
```

</td>
</tr>
</table>

### Part 3


<table>
<tr>
<th> Commands </th>
<th> Result </th>
</tr>
<tr>
<td>

```
createFolder Folder4 Folder2
storeFile sample3.txt Folder3
getFile sample3.txt
printTree
quit
```

</td>
<td>

```
- RootFolder (folder)
  - Folder1 (folder)
    - Folder2 (folder)
      - Folder3 (folder)
        - sample3.txt (txt)
      - Folder4 (folder)
    - sample.txt (txt)
```

</td>
</tr>
</table>
