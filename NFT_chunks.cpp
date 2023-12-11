#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
using namespace std;

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

  // Constructor
  TreeNode(std::string name, size_t size, std::string type,
           TreeNode *parentNode)
    : fileName(name), fileSize(size), fileType(type), parent(parentNode) {
    timestampCreated = std::time(nullptr); // Set the current time as timestamp
  }

  TreeNode(std::string name, std::string type,
	   TreeNode* parentNode)
    : fileName(name), fileType(type), parent(parentNode){
    timestampCreated = std::time(nullptr);
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

    for (TreeNode *child : currentNode->children) {
      TreeNode *result = searchHelper(child, name);
      if (result != nullptr) {
        return result;
      }
    }

    return nullptr;
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

  //Store file in tree by creating new node and calling divide_chunks
  bool storeFile(std::string filename,std::string type, std::string location){

    TreeNode* folder = this->searchByName(location);

    if(folder == nullptr || folder->filetype != "folder"){
      std::cout<<"Invalid folder!" << std::endl;
      return false;
    }
    
    TreeNode* file = new TreeNode(filename,type,folder);
    divide_chunks(filename, *file);

    return true;
  }
  // TODO: Implement the following functions
  /*
  
   */
};

long unsigned int divide_chunks(string filename, TreeNode myNode) {
  int chunkSize = 4096;
  int fileSize = myNode.fileSize;
  //char *file = filename;

  char *dest = (char *) malloc(chunkSize);
  char ch;

  ifstream src_file;
  const char *file_n = filename.c_str();
  src_file.open(file_n);
  FILE *pFile1;
  FILE *pFile2;
  pFile1 = fopen(file_n, "r");

  ifstream file;
  file.open(filename, ifstream::binary);
  ofstream ofile;
  ofile.open(dest, ofstream::binary);

  int numChunks = (fileSize + chunkSize - 1) /chunkSize;

  long unsigned int chunk_inds[numChunks];

  vector<char> buff (chunkSize,0);
  streamsize s;
  int count = 0;
  while (!file.eof()) {
    char *buffer = (char *) malloc(chunkSize);
    file.read(buff.data(), buff.size());
    s = file.gcount();
    string str(buff.begin(), buff.end());
    str.copy(buffer,4096,0);
    chunk_inds[count] = (long unsigned int) buffer;
    count++;
    //ofiles.write(buff.data(), buff.size());

  }
  myNode.chunk_inds = (long unsigned int) chunk_inds;
  myNode.numChunks = numChunks;
  return (long unsigned int) chunk_inds;
}

int main() {
  // Example usage
  FileTree myTree;

  // Creating the root node (typically a folder)
  TreeNode *root = new TreeNode("RootFolder", 0, "folder", nullptr);
  myTree.setRoot(root);

  // Adding files/folders to the root
  TreeNode *file1 = new TreeNode("File1.txt", 1024, "txt", root);
  root->addChild(file1);

  TreeNode *folder1 = new TreeNode("Folder1", 0, "folder", root);
  root->addChild(folder1);

  // Adding a file to folder1
  TreeNode *file2 = new TreeNode("File2.md", 2048, "md", folder1);
  folder1->addChild(file2);

  // Print the tree
  myTree.printTree();

  //long unsigned int test;

  //  test = divide_chunks("test.txt",*file1);

  myTree.moveFile("File1.txt","Flder2.md");
  myTree.printTree();
  return 0;
}
