#include <iostream>
#include <vector>
#include <set>
#include <random>
#include <algorithm>

using KeyType = int64_t;
using ValueType = int;
using SizeType = unsigned int;

struct BTreeNode {
  BTreeNode* parent;
  KeyType* keys;
  BTreeNode** children;
  SizeType size;
};

BTreeNode* NewBTreeNode(SizeType size, SizeType max_size) {
  auto new_node = new BTreeNode();
  new_node->parent = nullptr;
  // std::cout << "new node\n";
  new_node->size = size;
  new_node->keys = new KeyType[max_size];
  new_node->children = new BTreeNode* [max_size + 1] { nullptr };
  return new_node;
}

void SetParent(BTreeNode* node, BTreeNode* parent) {
  if (node == nullptr) {
    return;
  }
  node->parent = parent;
}

SizeType LowerBound(BTreeNode* node, KeyType pivot) {
  SizeType i;
  for (i = 0; i < node->size && node->keys[i] < pivot; ++i) {
  }
  return i;
}

SizeType FindChild(BTreeNode* parent, BTreeNode* child) {

  SizeType i;
  for (i = 0; i <= parent->size && parent->children[i] != child; ++i) {
  }
  return i;
}

struct Splitted {
  BTreeNode* left;
  BTreeNode* right;
  KeyType middle;
};

class BTree {
 private:
  void SplitIthChild(BTreeNode* node, SizeType index) {
    if (node->children[index]->size != upper_factor_) {
      return;
    }
    auto splitted = SplitInHalf(node->children[index]);
    InsertInNode(node, splitted);
  }

  void InsertInNode(BTreeNode* node, Splitted splitted) {
    SizeType lb = LowerBound(node, splitted.middle);
    ShiftRight(node, lb);
    node->keys[lb] = splitted.middle;
    node->children[lb] = splitted.left;
    node->children[lb]->parent = node;
    node->children[lb + 1] = splitted.right;
    node->children[lb + 1]->parent = node;
  }

  void ShiftRight(BTreeNode* node, SizeType index) {
    SizeType prev_key = node->keys[index];
    BTreeNode* prev_child = node->children[index];
    for (SizeType i = index + 1; i <= node->size; ++i) {
      auto temp_key = node->keys[i];
      auto temp_child = node->children[i];
      node->keys[i] = prev_key;
      node->children[i] = prev_child;
      prev_key = temp_key;
      prev_child = temp_child;
    }
    node->children[node->size + 1] = prev_child;
    node->children[index] = nullptr;
    ++node->size;
  }

  void ShiftLeft(BTreeNode* node, SizeType index) {
    for (SizeType i = index; i < node->size - 1; ++i) {
      node->keys[i] = node->keys[i + 1];
      node->children[i] = node->children[i + 1];
    }
    node->children[node->size - 1] = node->children[node->size];
    node->children[node->size] = nullptr;
    --node->size;
  }

  Splitted SplitInHalf(BTreeNode* node) {
    auto splitted = Splitted();
    splitted.middle = node->keys[node->size / 2];
    splitted.right = BTreeNodeFrom(node, node->size / 2 + 1, node->size);
    splitted.left = BTreeNodeFrom(node, 0, node->size / 2);
    ClearOnlyNode(node);
    return splitted;
  }

  BTreeNode* BTreeNodeFrom(BTreeNode* node, SizeType left, SizeType right) {
    auto new_node = NewBTreeNode(right - left, upper_factor_);
    for (SizeType i = left; i < right; ++i) {
      new_node->keys[i - left] = node->keys[i];
      new_node->children[i - left] = node->children[i];
      SetParent(new_node->children[i - left], new_node);
    }
    new_node->children[right - left] = node->children[right];
    SetParent(new_node->children[right - left], new_node);
    return new_node;
  }

  void MergeSplitted(BTreeNode* node, Splitted splitted) {
    node->keys[0] = splitted.middle;
    node->children[0] = splitted.left;
    node->children[1] = splitted.right;
    node->children[0]->parent = node;
    node->children[1]->parent = node;
  }

  void ClearOnlyNode(BTreeNode* node) {
    if (node == nullptr) {
      return;
    }

    node->size = 0;
    delete[] node->keys;
    delete[] node->children;
    delete node;
  }

  void Clear(BTreeNode* node) {
    if (node == nullptr) {
      return;
    }

    for (SizeType i = 0; i <= node->size; ++i) {
      Clear(node->children[i]);
    }

    node->size = 0;
    delete[] node->keys;
    delete[] node->children;
    delete node;
  }

  KeyType FindMax(BTreeNode* root) {
    if (root == nullptr) {
      return -1;
    }
    if (root->children[root->size] == nullptr) {
      return root->keys[root->size - 1];
    }
    return FindMax(root->children[root->size]);
  }

  KeyType FindMin(BTreeNode* root) {
    if (root == nullptr) {
      return -1;
    }
    if (root->children[0] == nullptr) {
      return root->keys[0];
    }
    return FindMin(root->children[0]);
  }

  BTreeNode* MergeTwoNodes(BTreeNode* left, BTreeNode* right, KeyType middle) {
    if (left == nullptr) {
      ShiftRight(right, 0);
      right->keys[0] = middle;
      right->children[0] = nullptr;
      return right;
    }

    if (right == nullptr) {
      left->keys[left->size] = middle;
      left->children[left->size + 1] = nullptr;
      return left;
    }

    left->keys[left->size] = middle;
    for (SizeType i = 0; i < right->size; ++i) {
      left->keys[left->size + i + 1] = right->keys[i];
      left->children[left->size + i + 1] = right->children[i];
      SetParent(left->children[left->size + i + 1], left);
    }

    left->children[left->size + right->size + 1] = right->children[right->size];
    SetParent(left->children[left->size + right->size + 1], left);
    left->size += right->size + 1;
    ClearOnlyNode(right);
    return left;
  }

  BTreeNode* MergeTwoNeighbours(BTreeNode* root, SizeType index) {
    BTreeNode* merged = MergeTwoNodes(root->children[index - 1], root->children[index], root->keys[index - 1]);
    ShiftLeft(root, index - 1);
    if (root->size == 0) {
      root_ = merged;
    } else {
      root->children[index - 1] = merged;
      SetParent(root->children[index - 1], root);
    }
    return merged;
  }

  void Merge(BTreeNode* root, SizeType index) {
    BTreeNode* middle_node = root->children[index];
    if (index > 0 && root->children[index - 1] != nullptr && root->children[index - 1]->size > lower_factor_) {
      BTreeNode* left_child = root->children[index - 1];
      ShiftRight(middle_node, 0);
      middle_node->keys[0] = root->keys[index - 1];
      root->keys[index - 1] = left_child->keys[left_child->size - 1];
      middle_node->children[0] = left_child->children[left_child->size];
      SetParent(middle_node->children[0], middle_node);
      --left_child->size;
    } else if (index < root->size && root->children[index + 1] != nullptr &&
               root->children[index + 1]->size > lower_factor_) {
      BTreeNode* right_node = root->children[index + 1];
      middle_node->keys[middle_node->size] = root->keys[index];
      root->keys[index] = right_node->keys[0];
      middle_node->children[middle_node->size + 1] = right_node->children[0];
      SetParent(middle_node->children[middle_node->size + 1], middle_node);
      ++middle_node->size;
      ShiftLeft(right_node, 0);
    } else if (index > 0) {
      MergeTwoNeighbours(root, index);
    } else {
      MergeTwoNeighbours(root, index + 1);
    }
  }

 public:
  explicit BTree(SizeType size) : root_(nullptr), lower_factor_(size - 1), upper_factor_(2 * size - 1) {
  }

  void Insert(BTreeNode* root, KeyType new_key) {
    if (root == nullptr) {
      root = NewBTreeNode(1, upper_factor_);
      root->keys[0] = new_key;
      root_ = root;
      return;
    }

    SizeType lb = LowerBound(root, new_key);
    if (lb < root->size && root->keys[lb] == new_key) {
      return;
    }

    if (root->size == upper_factor_ && root == root_) {
      auto new_root = NewBTreeNode(1, upper_factor_);
      MergeSplitted(new_root, SplitInHalf(root));
      root_ = new_root;
      Insert(root_, new_key);
    } else if (root->size == upper_factor_) {
      Insert(root->parent, new_key);
    } else if (root->children[lb] != nullptr) {
      if (root->children[lb]->size == upper_factor_) {
        SplitIthChild(root, lb);
        Insert(root, new_key);
      } else {
        Insert(root->children[lb], new_key);
      }
    } else {
      ShiftRight(root, lb);
      root->keys[lb] = new_key;
    }
  }

  bool Contains(BTreeNode* root, KeyType key) {
    if (root == nullptr) {
      return false;
    }

    SizeType lb = LowerBound(root, key);
    if (lb < root->size && root->keys[lb] == key) {
      return true;
    }

    return Contains(root->children[lb], key);
  }

  void Erase(BTreeNode* root, KeyType key) {
    if (root == nullptr) {
      return;
    }

    SizeType lb = LowerBound(root, key);
    if (lb < root->size && root->keys[lb] != key && root->children[lb] == nullptr) {
      return;
    }
    if (lb == root->size && root->children[lb] == nullptr) {
      return;
    }

    if (lb == root->size || root->keys[lb] != key) {
      if (root->children[lb]->size == lower_factor_) {
        Merge(root, lb);
        if (root->size == 0) {
          ClearOnlyNode(root);
          Erase(root_, key);
        } else {
          Erase(root, key);
        }
      } else {
        Erase(root->children[lb], key);
      }
    } else if (root->children[lb] == nullptr) {
      ShiftLeft(root, lb);
    } else if (root->children[lb + 1] == nullptr) {
      auto prev = root->children[lb];
      ShiftLeft(root, lb);
      root->children[lb] = prev;
    } else if (root->children[lb]->size > lower_factor_) {
      KeyType max_key = FindMax(root->children[lb]);
      Erase(root->children[lb], max_key);
      root->keys[lb] = max_key;
    } else if (root->children[lb + 1]->size > lower_factor_) {
      KeyType min_key = FindMin(root->children[lb + 1]);
      Erase(root->children[lb + 1], min_key);
      root->keys[lb] = min_key;
    } else {
      MergeTwoNeighbours(root, lb + 1);
      if (root->size == 0) {
        ClearOnlyNode(root);
        Erase(root_, key);
      } else {
        Erase(root, key);
      }
    }
  }

  void IterateOverAll(BTreeNode* node, std::vector<KeyType>& arr) {
    if (node == nullptr)
      return;
    for (SizeType i = 0; i < node->size; ++i) {
      IterateOverAll(node->children[i], arr);
      arr.push_back(node->keys[i]);
    }
    IterateOverAll(node->children[node->size], arr);
  }

  KeyType LB(BTreeNode* root, KeyType key) const {
    if (root == nullptr) {
      return -1;
    }

    SizeType lb = LowerBound(root, key);
    if (lb < root->size && root->keys[lb] == key) {
      return key;
    }
    if (lb == root->size) {
      return LB(root->children[lb], key);
    }
    if (root->children[lb] != nullptr) {
      // return LB(root->children[lb], key);
      auto lower = LB(root->children[lb], key);
      return (lower == -1 ? root->keys[lb] : lower);
    }
    return root->keys[lb];
  }

  void Print(BTreeNode* node) {
    if (node == nullptr)
      return;
    for (SizeType i = 0; i < node->size; ++i) {
      Print(node->children[i]);
      std::cout << node->keys[i] << ' ';
    }
    Print(node->children[node->size]);
  }

  KeyType Min() {
    return FindMin(root_);
  }

  KeyType Max() {
    return FindMax(root_);
  }

  void Clear() {
    Clear(root_);
  }

  ~BTree() {
    Clear(root_);
  }

 public:
  BTreeNode* root_;

 private:
  SizeType lower_factor_;
  SizeType upper_factor_;
};

int main() {

  BTree btree(2);

  std::random_device device;
  std::uniform_int_distribution<KeyType> range(1, 1000);

  int n = 1000;
  std::vector<KeyType> arr = {845,707,195,672,665,963,415,152,611,353};
  // for (int i = 0; i < n; ++i) {
  //   arr.push_back(range(device));
  //   std::cout << arr.back() << ' ';
  // }

  for (const auto& e : arr) {
    btree.Insert(btree.root_, e);
  }

  btree.Erase(btree.root_, arr.back());
  // for (const auto& e : arr) {
  //   btree.Erase(btree.root_, e);
  // }
}
