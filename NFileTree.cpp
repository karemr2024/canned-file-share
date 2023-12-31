#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <cctype>
#include <unordered_map>
using namespace std;

const int chunkSize = 4096;
class TreeNode;
long unsigned int divide_chunks(string, TreeNode);
size_t gchunkcount=0;
std::string storePrefix = "chunk_";
std::unordered_map<int, int> chunkHashMap;


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

  //Constructor for reading tree from .data (use for fileType other than folders)
  TreeNode(std::string name, time_t time, size_t size, std::string type,
	   TreeNode* parentNode, long unsigned int inds, int numberChunks){
    this->fileName = name;
    this->timestampCreated = time;
    this->fileSize = size;
    this->fileType = type;
    this->parent = parentNode;
    this->chunk_inds = inds;
    this->numChunks = numberChunks;
  }

  //Constructor for reading ree from .data (used for fileType:"folder")
  TreeNode(std::string name, time_t time, size_t size, std::string type,
	   TreeNode* parentNode, int numberChunks){
    this->fileName = name;
    this->timestampCreated = time;
    this->fileSize = size;
    this->fileType = type;
    this->parent = parentNode;
    this->numChunks = numberChunks;
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

  void serializeNode(std::ofstream& file){
    file << this->fileName << '\n';
    file << this->timestampCreated << '\n';
    file << this->fileSize << '\n';
    file << this->fileType << '\n';

    if(this->parent == nullptr){
      file << '\n';
    }
    else{
      file << this->parent->fileName << '\n';
    }

    file << this->chunk_inds << '\n';
    file << this->numChunks << '\n';
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
    //std::cout<<"Inside1" << std::endl;
    if (currentNode->fileName == name) {
      return currentNode;
    }
    //std::cout<<"Inside2" << std::endl;  
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
    node->parent->removeChild(node);
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
    newNode->chunk_inds=file->chunk_inds;
    newNode->numChunks=file->numChunks;
    destination->addChild(newNode);
    for(int i=file->chunk_inds; i<file->numChunks; i++){
      chunkHashMap[i]++;
    }
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

    if(efile != nullptr){
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
        fileExtension = substrings.back();
        //std::cout << "File extension: " << fileExtension << std::endl;
    } else {
        fileExtension = "txt";
        std::cout << "No file extension found." << std::endl;
    }


    
    TreeNode* file = new TreeNode(filename, fileExtension, folder);
    folder->addChild(file);
    splitFile(filename, file);
    const char* cstr_filename = filename.c_str();
    //std::remove(cstr_filename);


    // std::cout << "file->TYPE : " << file->fileType << std::endl;
    // std::cout << "file->fileExtension : " << fileExtension << std::endl;
    // std::cout << "file num chunks : " << file->numChunks << std::endl;
    // std::cout << "file chunk start : " << file->chunk_inds << std::endl;


    return true;
  }

  bool deleteFile(std::string filename){
    TreeNode* file = this->searchByName(filename);
    if(file == nullptr){
      std::cout<<"File does not exist!" <<endl;
      return false;
    }

    size_t chunk_start=file->chunk_inds;
    // std::cout << "chunk_start: " << chunk_start <<endl;
    size_t chunk_end=file->numChunks;
    // std::cout << "chunk_end: " << chunk_end <<endl;

    for(int i=chunk_start; i<chunk_end; i++){
      std::string deleteFileName = "./store/" + storePrefix + std::to_string(i);
      const char* cstr_filename = deleteFileName.c_str();
      if(chunkHashMap[i]==1){
        if (std::remove(cstr_filename) != 0) {
            std::perror("Error deleting chunks\n");
        } else {
	  chunkHashMap[i]--;
            std::cout << "chunks deleted successfully" <<endl;
        }
      } else {
        chunkHashMap[i]--;
      }
    }

    deleteNode(file);
    return true;


  }

  bool updateFile(std::string filename){
    TreeNode* file = this->searchByName(filename);
    if(file == nullptr){
      std::cout<<"File does not exist!" <<endl;
      return false;
    }

    TreeNode* parent= file->parent;

    removeFile(filename);
    std::string parent_name= parent->fileName;
    std::cout<< "parent_name: " << parent_name <<endl;
    storeFile(filename, parent_name);

  }


  bool getFile(std::string filename){
    //std::cout<<"Inside" <<endl;
    TreeNode* file = this->searchByName(filename);
    //std::cout<<"Inside" <<endl;
    if(file == nullptr){
      std::cout<<"File does not exist!" <<endl;
      return false;
    }
    std::string outputFileName = filename + "_fetched";
    mergeChunks(outputFileName, file);



  }

  bool removeFile(std::string filename){
      TreeNode* file = this->searchByName(filename);
      if(file == nullptr){
        std::cout<<"File does not exist!" <<endl;
        return false;
      }

      removeFileHelper(file);

    }


  //Run DFS to delete all subfolders/subfiles
  bool removeFileHelper(TreeNode* currentFile){

    if(currentFile->children.size()!=0){
      for(int i=0;i<currentFile->children.size();i++){
	      removeFileHelper(currentFile->children[i]);
      }
    }

    //free
    delete currentFile;
    return true;
  }

  //Writes each file node's data to a file (whose name is provided in argument .data)
  void writeTreeFile(std::string treeName){
    std::string fileName = treeName + ".data";
    std::ofstream outFile(fileName);
    if(!outFile.is_open()){
      std::cout<<"Unable to open file" << std::endl;
      return;
    }

    //Store gchunkcount
    outFile << gchunkcount << '\n';

    //Store chunkHashMap
    for(const auto& pair : chunkHashMap){
      outFile << pair.first << '\n';
      outFile << pair.second << '\n';
    }

    //Don't need to store the root node
    for(int i=0;i<this->root->children.size();i++){
      this->writeTreeHelper(outFile,this->root->children[i]);
    }

    outFile.close();

  }

  void writeTreeHelper(std::ofstream& file,TreeNode* currentNode){
    if(currentNode == nullptr){
      return;
    }

    currentNode->serializeNode(file);

    for(int i=0;i<currentNode->children.size();i++){
      this->writeTreeHelper(file,currentNode->children[i]);
    }
  }

  void splitFile(const std::string& inputFileName, TreeNode* currNode) {
    std::ifstream inputFile(inputFileName, std::ios::binary);

    

    if (!inputFile) {
        std::cerr << "Error opening input file: " << inputFileName << std::endl;
        return;
    }

    size_t chunkCounter = gchunkcount;
    currNode->chunk_inds=gchunkcount;
    char buffer[chunkSize];

    while (!inputFile.eof()) {
        inputFile.read(buffer, chunkSize);

        std::streamsize bytesRead = inputFile.gcount();

        if (bytesRead > 0) {
            chunkHashMap[chunkCounter]=1;
            std::string outputFileName = "./store/" + storePrefix + std::to_string(chunkCounter++);
            std::ofstream outputFile(outputFileName, std::ios::binary);
            outputFile.write(buffer, bytesRead);
        }
    }

    currNode->numChunks=chunkCounter;
    gchunkcount=chunkCounter;
    //std::cout<<"chunkCounter: " << chunkCounter << std::endl;
    //std::cout<<"gchunkcount: " << gchunkcount << std::endl;

    inputFile.close();
}


void mergeChunks(const std::string& outputFileName, TreeNode* currNode) {
    std::ofstream outputFile(outputFileName, std::ios::binary);

    if (!outputFile) {
        std::cerr << "Error opening output file: " << outputFileName << std::endl;
        return;
    }

    size_t chunkCounter = currNode->chunk_inds;
    size_t chunk_end = currNode->numChunks;
    std::string inputFileName;

    for(int i=chunkCounter; i<=chunk_end; i++){
      inputFileName = "./store/" + storePrefix + std::to_string(i);
        std::ifstream inputFile(inputFileName, std::ios::binary);

        if (!inputFile) {
            break;  // No more chunks to read
        }

        outputFile << inputFile.rdbuf();

        inputFile.close();
    }

    // do {
    //     inputFileName = "./store/" + storePrefix + std::to_string(chunkCounter++);
    //     std::ifstream inputFile(inputFileName, std::ios::binary);

    //     if (!inputFile) {
    //         break;  // No more chunks to read
    //     }

    //     outputFile << inputFile.rdbuf();

    //     inputFile.close();
    // } while (true);

    outputFile.close();
}
  
};

FileTree readTreeFile(std::string fileName){
  
  FileTree ft;
  TreeNode* root = new TreeNode("RootFolder",0,"folder",nullptr);
  ft.setRoot(root);
  
  std::ifstream file(fileName);
  if(!file.is_open()){
    std::cout<<"Unable to open file" << std::endl;
    return ft;
  }

  
  int mapLineCounter = 0;
  int nodeLineCounter = 0;
  int key;
  int value;
  std::string name;
  std::time_t time;
  size_t size;
  std::string type;
  std::string parentName;
  long unsigned int inds;
  int numberChunks;
  
  std::string line;

  if(std::getline(file,line)){
    gchunkcount = stoi(line);
  }
  else{
    std::cout<<"File is empty \n";
    return ft;
  }

  //Read lines 1 through 2*gchunkcount+1 to populate chunkHashMap
  while(mapLineCounter<gchunkcount*2 && std::getline(file,line)){
    switch(mapLineCounter%2)
      {
      case 0: //map key
	key = stoi(line);
	break;
      case 1: //map value
	value = stoi(line);

	//When we read the value of the key/value pair, insert pair to map
	chunkHashMap[key] = value;
	break;
      }
    mapLineCounter++;
  }

  //Read rest of lines to populate tree with nodes
  while (std::getline(file,line)){
    
    switch(nodeLineCounter%7)
      {
      case 0: //name
	name = line;
	break;
      case 1: //time
	time = (time_t)stoll(line);
	break;
      case 2: //size
	size = std::stoul(line);
	break;
      case 3: //type
	type = line;
	break;
      case 4: //parentName
	parentName = line;
	break;
      case 5: //inds, folders have no chunk indicies
	if(type != "folder"){
	  inds = stoul(line);
	}
	break;
      case 6: //numberChunks, once we read 7 lines, we create a node
	numberChunks = stoi(line);

	//Create a node
	TreeNode* parentNode = ft.searchByName(parentName);
	if(type != "folder"){
	  TreeNode* currentNode = new TreeNode(name,time,size,type,parentNode,inds,numberChunks);
	  parentNode->addChild(currentNode);
	}
	else{
	  TreeNode* currentNode = new TreeNode(name,time,size,type,parentNode,numberChunks);
	  parentNode->addChild(currentNode);
	}
	break;
      }
    
    //std::cout << "|" <<line << "|" << '\n';
    nodeLineCounter++;
  }

  file.close();

  return ft;
}



int main() {
    bool exitProgram = false;

    FileTree ft;

    // Creating the root node (typically a folder)
    TreeNode *root = new TreeNode("RootFolder", 0, "folder", nullptr);
    ft.setRoot(root);

    while (!exitProgram) {
        std::cout << "Enter your command: \n";
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
        }

    return 0;
  }
