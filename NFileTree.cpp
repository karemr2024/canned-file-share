#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <cctype>
using namespace std;

const int chunkSize = 4096;
class TreeNode;
long unsigned int divide_chunks(string, TreeNode);


// Define a class to represent a node in the tree
class TreeNode {
public:
  std::string fileName;
  std::time_t timestampCreated;
  size_t fileSize;
  std::string fileType;             // "file" or "folder"
  std::vector<TreeNode *> children; // Pointers to children nodes
  TreeNode *parent;                 // Pointer to parent node
  long unsigned int chunk_inds;
  int numChunks;


  // File Constructor
  TreeNode(std::string name, size_t size, std::string type,
           TreeNode *parentNode)
    : fileName(name), fileSize(size), fileType(type), parent(parentNode) {
    timestampCreated = std::time(nullptr); // Set the current time as timestamp
  }

  //Folder Constructor
  TreeNode(std::string name, std::string type,
	   TreeNode* parentNode)
    : fileName(name), fileType(type), parent(parentNode){
    timestampCreated = std::time(nullptr);
  }

	void printTreeNode(TreeNode* node){
    std::cout << "Node Details: " << std::endl;
    std::cout << "filename : " << node->fileName << std::endl;
    std::cout << "filetype : " << node->fileType << std::endl;
    if(node->fileType != "folder"){ std::cout << "filesize : " << node->fileSize << std::endl;}
    std::cout << "timeStamp : " << node->timestampCreated << std::endl;
    std::cout << "timeStamp : " << node->numChunks << std::endl;
  }

  // Function to add a child to this node
  void addChild(TreeNode *child) { children.push_back(child); }

  //Function to remove a child from this node, returns true if done successfully
  bool removeChild(TreeNode* child){
    for(int i=0;i<this->children.size();i++){
      if(this->children[i]->fileName == child->fileName){
        this->children.erase(this->children.begin()+i);
        return true;
      }
    }
    return false;
  }

};

// A class for the tree structure itself
class FileTree {
private:
  TreeNode *root;

  TreeNode *searchHelper(TreeNode *currentNode, const std::string &name) {
    if (currentNode == nullptr) {
      return nullptr;
    }

    if (currentNode->fileName == name) {
      return currentNode;
    }
      
    for (TreeNode *child : currentNode->children) {
      TreeNode *result = searchHelper(child, name);
      if (result != nullptr) {
        return result;
      }
    }

    return nullptr;
  }

  void deleteNode(TreeNode *node){
    if (node == nullptr) {
      return;
    }

    for (TreeNode* child : node->children) {
        deleteNode(child); // Recursively delete children

    }
    delete node;

  }


  void printTreeHelper(TreeNode *node, int level) {
    if (node == nullptr) {
      return;
    }


    // Print the current node with indentation based on its level
    std::string indentation(level * 2, ' '); // 2 spaces per level of depth
    std::cout << indentation << "- " << node->fileName << " (" << node->fileType
              << ")" << std::endl;

    // Recursively print each child
    for (TreeNode *child : node->children) {
      printTreeHelper(child, level + 1);
    }
  }

public:
  FileTree() : root(nullptr) {}

  // Function to set the root of the tree
  void setRoot(TreeNode *rootNode) { root = rootNode; }

  TreeNode *searchByName(const std::string &name) {
    return searchHelper(root, name);
  }
  void printTree() { printTreeHelper(root, 0); }

  void moveFile(std::string fileName,std::string destinationName){
    TreeNode* file = this->searchByName(fileName);
    TreeNode* destination = this->searchByName(destinationName);

    //Check if file and destination folder exist
    if(file == nullptr){
      std::cout<<"File does not exist!" << std::endl;
      return;
    }
    else if(destination == nullptr || destination->fileType != "folder"){
      std::cout<<"Folder does not exist!" << std::endl;
      return;
    }

    //Update parent/children relationships
    file->parent->removeChild(file);
    destination->addChild(file);
    file->parent = destination;
  }


  void copyFile(std::string fileName, std::string newFile, std::string destinationName){
    TreeNode* file = this->searchByName(fileName);
    TreeNode* newfile = this->searchByName(newFile);
    TreeNode* destination = this->searchByName(destinationName);

    if(newfile != nullptr){
      std::cout<<"File name exists already! Use a different name" << std::endl;
      return;
    }

    //Check if file and destination folder exist
    if(file == nullptr){
      std::cout<<"File does not exist!" << std::endl;
      return;
    }
    else if(destination == nullptr || destination->fileType != "folder"){
      std::cout<<"Folder does not exist!" << std::endl;
      return;
    }

    TreeNode* newNode = new TreeNode(newFile, file->fileType, destination);
    destination->addChild(newNode);
  }

  bool createFolder(std::string filename, std::string location){
    TreeNode* folder = this->searchByName(filename);
    TreeNode* parent_folder = this->searchByName(location);
    if(folder != nullptr){
      std::cout<<"Folder or file with name already exist!" << std::endl;
      return false;
    }
    if(parent_folder == nullptr || parent_folder->fileType != "folder"){
      std::cout<<"Enter valid parent folder path" << std::endl;
      return false;
    }
    TreeNode* newNode = new TreeNode(filename, "folder", parent_folder);
    parent_folder->addChild(newNode);
    return true;
  }

  //Store file in tree by creating new node and calling divide_chunks
  bool storeFile(std::string filename,std::string location){

    std::istringstream iss(filename);
    std::string token;
    std::string fileExtension;
    std::vector<std::string> substrings;

    TreeNode* efile = this->searchByName(filename);

    if(efile == nullptr){
      std::cout<<"Folder or file with name already exist!" << std::endl;
      return false;
    }

    TreeNode* folder = this->searchByName(location);

    if(folder == nullptr || folder->fileType != "folder"){
      std::cout<<"Invalid folder!" << std::endl;
      return false;
    }

    // Split the string using dot as the delimiter
    while (std::getline(iss, token, '.')) {
        substrings.push_back(token);
    }

    // If there is at least one dot in the file name, the last element in the vector
    // will contain the file extension (e.g., "txt")
    if (substrings.size() > 1) {
        std::string fileExtension = substrings.back();
        //std::cout << "File extension: " << fileExtension << std::endl;
    } else {
        std::string fileExtension = "txt";
        //std::cout << "No file extension found." << std::endl;
    }


    
    TreeNode* file = new TreeNode(filename,fileExtension,folder);
    folder->addChild(file);
    //divide_chunks(filename, *file);

    return true;
  }

  bool deleteFile(std::string filename){
    TreeNode* file = this->searchByName(filename);
    if(file == nullptr){
      std::cout<<"File does not exist!" <<endl;
      return false;
    }

    deleteNode(file);
    return true;


  }

  bool getFile(std::string filename){
    TreeNode* file = this->searchByName(filename);
    if(file == nullptr){
      std::cout<<"File does not exist!" <<endl;
      return false;
    }

    //removeFileHelper(file);
    delete file;

  }

  bool removeFile(std::string filename){
      TreeNode* file = this->searchByName(filename);
      if(file == nullptr){
        std::cout<<"File does not exist!" <<endl;
        return false;
      }

      //Remove file from children of parent
      removeFileHelper(file);
      //Call removeFileHelper

    }


  //Run DFS to delete all subfolders/subfiles
  bool removeFileHelper(TreeNode* currentFile){
    //Check if children.size() is 0
    //If not, call helper on children
    //Free malloc'd space
    //Delete node

    if(currentFile->children.size()!=0){
      for(int i=0;i<currentFile->children.size();i++){
	      removeFileHelper(currentFile->children[i]);
      }
    }

    //free
    delete currentFile;
    
  }
  
  // TODO: Implement the following functions
  /*
  Retrieve file
  Update?
   */
};


int main() {
    bool exitProgram = false;

    FileTree ft;

    // Creating the root node (typically a folder)
    TreeNode *root = new TreeNode("RootFolder", 0, "folder", nullptr);
    ft.setRoot(root);

    // Adding files/folders to the root
    TreeNode *file1 = new TreeNode("File1.txt", 1024, "txt", root);
    root->addChild(file1);

    TreeNode *folder1 = new TreeNode("Folder1", 0, "folder", root);
    root->addChild(folder1);

    TreeNode *folder2 = new TreeNode("Folder2", 0, "folder", root);
    root->addChild(folder2);

    // Adding a file to folder1
    TreeNode *file2 = new TreeNode("File2.md", 2048, "md", folder1);
    folder1->addChild(file2);

    TreeNode *folder3 = new TreeNode("Folder3", 0, "folder", root);
    folder2->addChild(folder3);

    // Print the tree
    ft.printTree();

    while (!exitProgram) {
        std::cout << "Enter your command: ";
        std::string userInput;
        std::getline(std::cin, userInput);

        // Use std::istringstream to split the input based on spaces
        std::istringstream iss(userInput);
        std::vector<std::string> substrings;
        std::string token;

        while (iss >> token) {
            substrings.push_back(token);
        }

        int args=substrings.size();
        // Output the number of substrings
        // std::cout << "Number of substrings: " << substrings.size() << std::endl;

        if (!substrings.empty()) {
            std::string comm= substrings.at(0);
                // Compare argv[1] to a string
                if (substrings.at(0) == "moveFile") {
                  if(args<3){
                    std::cout << "Not enough arguments" << std::endl;
                  } else {
                    std::cout << substrings.at(0) << std::endl;
                    ft.moveFile(substrings.at(1), substrings.at(2));
                    //ft.printTree();
                  }
                } 
              else if (substrings.at(0) == "copyFile") {
                  if(args<4){
                    std::cout << "Not enough arguments" << std::endl;
                  } else{
                    std::cout << substrings.at(0) << std::endl;
                    ft.copyFile(substrings.at(1), substrings.at(2), substrings.at(3));
                  }
              } else if (substrings.at(0) == "createFolder") {
                  if(args<3){
                    std::cout << "Not enough arguments" << std::endl;
                  } else{
                    std::cout << substrings.at(0) << std::endl;
                    ft.createFolder(substrings.at(1), substrings.at(2));
                  }
              } else if (substrings.at(0) == "deleteFile") {
                  if(args<2){
                    std::cout << "Not enough arguments" << std::endl;
                  } else{
                    std::cout << substrings.at(0) << std::endl;
                    ft.deleteFile(substrings.at(1));
                  }
              } else if (substrings.at(0) == "storeFile") {
                  if(args<3){
                    std::cout << "Not enough arguments" << std::endl;
                  } else{
                    std::cout << substrings.at(0) << std::endl;
                    ft.storeFile(substrings.at(1), substrings.at(2));
                  }
              } else if (substrings.at(0) == "getFile") {
                  if(args<2){
                    std::cout << "Not enough arguments" << std::endl;
                  } else{
                    std::cout << substrings.at(0) << std::endl;
                    ft.getFile(substrings.at(1));
                  }
              } else if (substrings.at(0) == "printTree") {
                  if(args<0){
                    std::cout << "Not enough arguments" << std::endl;
                  } else{
                    std::cout << substrings.at(0) << std::endl;
                    ft.printTree();
                  };
              } else if (substrings.at(0) == "quit") {
                  exitProgram = false;
                  break;
              } else {
                  std::cout << "Enter a valid command." << std::endl;
              }
          } else {
              std::cout << "Enter a valid command" << std::endl;
          }

          // for (const auto& substring : substrings) {
          //     std::cout << "Substring: " << substring << std::endl;
          // }

        }


    return 0;
  }



// int main() {
//   // Example usage
//   FileTree myTree;

//   // Creating the root node (typically a folder)
//   TreeNode *root = new TreeNode("RootFolder", 0, "folder", nullptr);
//   myTree.setRoot(root);

//   // Adding files/folders to the root
//   TreeNode *file1 = new TreeNode("File1.txt", 1024, "txt", root);
//   root->addChild(file1);

//   TreeNode *folder1 = new TreeNode("Folder1", 0, "folder", root);
//   root->addChild(folder1);

//   TreeNode *folder2 = new TreeNode("Folder2", 0, "folder", root);
//   root->addChild(folder2);

//   // Adding a file to folder1
//   TreeNode *file2 = new TreeNode("File2.md", 2048, "md", folder1);
//   folder1->addChild(file2);

//   TreeNode *folder3 = new TreeNode("Folder3", 0, "folder", root);
//   folder2->addChild(folder3);

//   // Print the tree
//   myTree.printTree();

//   //long unsigned int test;

//   //  test = divide_chunks("test.txt",*file1);

//   myTree.moveFile("File1.txt","Folder3");
//   myTree.copyFile("File1.txt", "File4.txt", "Folder1");
//   myTree.createFolder("Folder4", "Folder3");
//   myTree.storeFile("file3.txt", "Folder4");
//   myTree.printTree();
//   myTree.createFolder("Folder4", "Folder3");
//   myTree.storeFile("file3.txt", "Folder4");
//   myTree.removeFile("Folder3");
//   //myTree.deleteFile("Folder3");
//   myTree.printTree();
//   return 0;
// }