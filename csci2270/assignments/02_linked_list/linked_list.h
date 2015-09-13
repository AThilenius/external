// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef EXTERNAL_CSCI2270_ASSIGNMENTS_02_LINKED_LIST_H_
#define EXTERNAL_CSCI2270_ASSIGNMENTS_02_LINKED_LIST_H_

#include "suite.h"

namespace thilenius {
namespace external {
namespace csci2270 {
namespace assignments {
namespace linked_list {

struct LinkedListNode {
  int data;
  LinkedListNode* next_node;
};

class LinkedList {
 public:
  LinkedList();
  int Size();
  bool InsertAtIndex(int value, int index);
  void PushFront(int value);
  void PushBack(int value);
  int& operator[] (int index);
  bool RemoveAtIndex(int index);
  void Clear();

  void RemoveAll(int value);

  LinkedListNode* head;
};


}  // namespace linked_list
}  // namespace assignments
}  // namespace csci2270
}  // namespace external
}  // namespace thilenius

#endif  // EXTERNAL_CSCI2270_ASSIGNMENTS_02_LINKED_LIST_H_
