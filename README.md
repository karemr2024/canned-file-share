# canned-file-share

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
After which you can use our functionalities following the format shown below. 

```
 moveFile [name of the file to be moved] [name of the folder to move the file to]
```

```
 copyFile [name of the file to be copied] [name of the new file] [name of the folder to copy new file into]
```

```
 deleteFile [name of the file to be deleted]
```

```
 createFolder [new folder name] [location to create folder in (name of folder)]
```

```
 storeFile [file to be stored] [location to store file in (name of folder)]
```

```
 printTree
```

```
 quit
```
