# canned-file-share
### TreeNode Class:

* The tree nodes contain information such as file name, file type, file creation timestamp, pointers to children, pointer to parent.
* Tree nodes that are folders can have children while files can not.
  
```
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
```
### FileTree Class:

* File trees can be searched for any folder or file within them.
* File trees can be printed with indentations according to the levels of the nested folder structures.

```
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
```
