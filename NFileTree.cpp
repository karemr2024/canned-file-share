#include <ctime>
#include <iostream>
#include <string>
#include <vector>

// Define a class to represent a node in the tree
class TreeNode {
public:
  std::string fileName;
  std::time_t timestampCreated;
  size_t fileSize;
  std::string fileType;             // "file" or "folder"
  std::vector<TreeNode *> children; // Pointers to children nodes
  TreeNode *parent;                 // Pointer to parent node

  // Constructor
  TreeNode(std::string name, size_t size, std::string type,
           TreeNode *parentNode)
      : fileName(name), fileSize(size), fileType(type), parent(parentNode) {
    timestampCreated = std::time(nullptr); // Set the current time as timestamp
  }

  // Function to add a child to this node
  void addChild(TreeNode *child) { children.push_back(child); }
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
  
  // TODO: Implement the following functions
  /*
  
  */
};

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

  return 0;
}
