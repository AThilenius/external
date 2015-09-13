// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "linked_list.h"

#include <sstream>
#include <vector>

#include "printer.h"
#include "suite.h"

namespace thilenius {
namespace external {
namespace csci2270 {
namespace assignments {
namespace linked_list {
namespace {

bool CheckLinkedListValues(const std::vector<int>& values,
                           const LinkedList& linked_list) {
  LinkedListNode* node = linked_list.head;
  for (int i = 0; i < values.size(); i++) {
    if (node == nullptr || node->data != values[i]) {
      return false;
    }
    // Needs to be nullptr terminated
    if (i == values.size() - 1 && node->next_node != nullptr) {
      return false;
    }
    node = node->next_node;
  }
  return true;
}

LinkedList CreateLinkedList(const std::vector<int>& values) {
  LinkedList linked_list;
  LinkedListNode** next_pointer = &linked_list.head;
  for (const auto& value : values) {
    *next_pointer = new LinkedListNode{value, nullptr};
    next_pointer = &(*next_pointer)->next_node;
  }
  return std::move(linked_list);
}

void FreeLinkedListNotes(LinkedListNode* linked_list_node) {
  if (linked_list_node == nullptr) {
    return;
  }
  if (linked_list_node->next_node != nullptr) {
    FreeLinkedListNotes(linked_list_node->next_node);
  }
  // Delete on stack unwind (reverse order)
  delete linked_list_node;
}

std::string LinksToString(const LinkedList& linked_list) {
  std::stringstream string_stream;
  string_stream << "[";
  LinkedListNode* cursor = linked_list.head;
  while (cursor != nullptr) {
    string_stream << cursor->data << ", ";
    cursor = cursor->next_node;
  }
  string_stream << "]";
  std::string str = string_stream.str();
  if (str.find(", ]") != std::string::npos) {
    str.erase(str.find(", ]"), 2);
  }
  return std::move(str);
}

SUITE(LinkedListTestSuite) {
  TEST("Head set to nullptr", 5, 5) {
    // non-null head memory location so the test isn't flaky
    LinkedList linked_list;
    linked_list.head = nullptr;
    linked_list = LinkedList();
    EXPECT_TRUE(
        linked_list.head == nullptr, "Head should be nullptr",
        "Head should be set to nullptr when a new LinkedList is created");
  }

  TEST("Insert At Index", 25, 25) {
    // Single Node
    LinkedList linked_list;
    linked_list.head = nullptr;
    linked_list.InsertAtIndex(42, 0);
    EXPECT_TRUE(linked_list.head != nullptr, "Head is not null",
                "The head pointer should not be null once something is added");
    EXPECT_TRUE(
        CheckLinkedListValues({42}, linked_list),
        "Linked list should have nodes: [42]",
        "You didn't add 42 at index 0, list is: " + LinksToString(linked_list));
    EXPECT_TRUE(
        linked_list.head != nullptr && linked_list.head->next_node == nullptr,
        "List should end in nullptr",
        "The next_node of your last node should be nullptr");
    // Add a second node
    linked_list.InsertAtIndex(69, 1);
    EXPECT_TRUE(
        CheckLinkedListValues({42, 69}, linked_list),
        "Linked list should have nodes: [42, 69]",
        "You didn't add 69 at index 1, list is: " + LinksToString(linked_list));
    // Add to beginning of list
    linked_list.InsertAtIndex(-1, 0);
    EXPECT_TRUE(CheckLinkedListValues({-1, 42, 69}, linked_list),
                "Linked list should have nodes: [-1, 42, 69]",
                "You didn't add -1 at index 0 correctly, list is: " +
                    LinksToString(linked_list));
    // Add to middle of list
    linked_list.InsertAtIndex(420, 1);
    EXPECT_TRUE(CheckLinkedListValues({-1, 420, 42, 69}, linked_list),
                "Linked list should have nodes: [-1, 420, 42, 69]",
                "You didn't add 420 at index 1 correctly, list is: " +
                    LinksToString(linked_list));
    EXPECT_TRUE(
        !linked_list.InsertAtIndex(0, -1), "Return false for index -1",
        "InsertAtIndex should have returned false for a negative index");
    EXPECT_TRUE(
        !linked_list.InsertAtIndex(0, 10), "Return false for index 10",
        "InsertAtIndex should have returned false for an out of bound index");
  }

  TEST("Push Front", 10, 10) {
    LinkedList linked_list;
    linked_list.head = nullptr;
    linked_list.PushFront(3);
    EXPECT_TRUE(CheckLinkedListValues({3}, linked_list),
                "Linked list should have nodes: [3]",
                "PushFront didn't add 3 correctly, list is: " +
                    LinksToString(linked_list));
    linked_list.PushFront(2);
    EXPECT_TRUE(CheckLinkedListValues({2, 3}, linked_list),
                "Linked list should have nodes: [2, 3]",
                "PushFront didn't add 2 correctly, list is: " +
                    LinksToString(linked_list));
    linked_list.PushFront(1);
    EXPECT_TRUE(CheckLinkedListValues({1, 2, 3}, linked_list),
                "Linked list should have nodes: [1, 2, 3]",
                "PushFront didn't add 1 correctly, list is: " +
                    LinksToString(linked_list));
  }

  TEST("Push Back", 10, 10) {
    LinkedList linked_list;
    linked_list.head = nullptr;
    linked_list.PushBack(3);
    EXPECT_TRUE(CheckLinkedListValues({3}, linked_list),
                "Linked list should have nodes: [3]",
                "PushBack didn't add 3 correctly, list is: " +
                    LinksToString(linked_list));
    linked_list.PushBack(2);
    EXPECT_TRUE(CheckLinkedListValues({3, 2}, linked_list),
                "Linked list should have nodes: [3, 2]",
                "PushBack didn't add 2 correctly, list is: " +
                    LinksToString(linked_list));
    linked_list.PushBack(1);
    EXPECT_TRUE(CheckLinkedListValues({3, 2, 1}, linked_list),
                "Linked list should have nodes: [3, 2, 1]",
                "PushBack didn't add 1 correctly, list is: " +
                    LinksToString(linked_list));
  }

  TEST("Subscript Operator", 15, 15) {
    LinkedList linked_list = CreateLinkedList({-42, -1, 0, 1, 42});
    EXPECT_TRUE(linked_list[0] == -42, "Get index 0",
                "For the list [-42, -1, 0, 1, 42], getting item at index 0 "
                "didn't return -42");
    EXPECT_TRUE(linked_list[2] == 0, "Get index 2",
                "For the list [-42, -1, 0, 1, 42], getting item at index 2 "
                "didn't return 0");
    EXPECT_TRUE(linked_list[4] == 42, "Get index 4",
                "For the list [-42, -1, 0, 1, 42], getting item at index 4 "
                "didn't return 42");
  }

  TEST("Remote At Index", 20, 20) {
    ACTIVE_TEST_CASE.leak_check = true;
    LinkedList linked_list = CreateLinkedList({-42, -1, 0, 1, 42});
    bool could_remove = linked_list.RemoveAtIndex(0);
    EXPECT_TRUE(
        could_remove && CheckLinkedListValues({-1, 0, 1, 42}, linked_list),
        "RemoveAtIndex 0 for [-42, -1, 0, 1, 42]",
        "RemoveAtIndex(0) didn't remove -42, list is: " +
            LinksToString(linked_list));
    FreeLinkedListNotes(linked_list.head);
    linked_list = CreateLinkedList({-42, -1, 0, 1, 42});
    could_remove = linked_list.RemoveAtIndex(4);
    EXPECT_TRUE(
        could_remove && CheckLinkedListValues({-42, -1, 0, 1}, linked_list),
        "RemoveAtIndex 4 for [-42, -1, 0, 1, 42]",
        "RemoveAtIndex(4) didn't remove 42, list is: " +
            LinksToString(linked_list));
    FreeLinkedListNotes(linked_list.head);
    linked_list = CreateLinkedList({-42, -1, 0, 1, 42});
    could_remove = linked_list.RemoveAtIndex(2);
    EXPECT_TRUE(
        could_remove && CheckLinkedListValues({-42, -1, 1, 42}, linked_list),
        "RemoveAtIndex 2 for [-42, -1, 0, 1, 42]",
        "RemoveAtIndex(2) didn't remove 0, list is: " +
            LinksToString(linked_list));
    FreeLinkedListNotes(linked_list.head);
    linked_list = CreateLinkedList({42});
    could_remove = linked_list.RemoveAtIndex(0);
    EXPECT_TRUE(could_remove && linked_list.head == nullptr,
                "RemoveAtIndex 0 for [42]",
                "RemoveAtIndex(0) didn't clear the list, list is: " +
                    LinksToString(linked_list));
    FreeLinkedListNotes(linked_list.head);
    linked_list = CreateLinkedList({-42, -1, 0, 1, 42});
    could_remove = linked_list.RemoveAtIndex(-1);
    EXPECT_TRUE(!could_remove, "RemoveAtIndex -1",
                "RemoveAtIndex(-1) returned true");
    FreeLinkedListNotes(linked_list.head);
    linked_list = CreateLinkedList({-42, -1, 0, 1, 42});
    could_remove = linked_list.RemoveAtIndex(5);
    EXPECT_TRUE(
        !could_remove, "RemoveAtIndex 5",
        "RemoveAtIndex(5) returned true for a list with only 5 elements in it");
    FreeLinkedListNotes(linked_list.head);
  }

  TEST("Clear", 10, 10) {
    ACTIVE_TEST_CASE.leak_check = true;
    LinkedList linked_list = CreateLinkedList({0, 1, 2, 3});
    linked_list.Clear();
    EXPECT_TRUE(linked_list.head == nullptr, "No items after clear",
                "Head should be nullptr after calling Clear");
    FreeLinkedListNotes(linked_list.head);
  }

  TEST("Size", 5, 5) {
    LinkedList linked_list = CreateLinkedList({0, 1, 2, 3});
    EXPECT_TRUE(linked_list.Size() == 4, "Size 4 for [0, 1, 2, 3]",
                "Size should have returned 4");
    linked_list = CreateLinkedList({1});
    EXPECT_TRUE(linked_list.Size() == 1, "Size 1 for [1]",
                "Size should have returned 1");
    linked_list = CreateLinkedList({});
    EXPECT_TRUE(linked_list.Size() == 0, "Size 0 for []",
                "Size should have returned 0");
  }

  TEST("RemoveAll - Extra Credit [5]", 5, 0) {
    ACTIVE_TEST_CASE.leak_check = true;
    LinkedList linked_list = CreateLinkedList({0, 1, 2, 3});
    linked_list.RemoveAll(1);
    EXPECT_TRUE(CheckLinkedListValues({0, 2, 3}, linked_list),
                "RemoveAll of 1 from [0, 1, 2, 3]",
                "RemoveAll should have removed the 1, list is: " +
                    LinksToString(linked_list));
    FreeLinkedListNotes(linked_list.head);
    linked_list = CreateLinkedList({0, 1, 1, 1, 2});
    linked_list.RemoveAll(1);
    EXPECT_TRUE(CheckLinkedListValues({0, 2}, linked_list),
                "RemoveAll of 1 from [0, 1, 1, 1, 2]",
                "RemoveAll should have removed all the 1s, list is: " +
                    LinksToString(linked_list));
    FreeLinkedListNotes(linked_list.head);
  }
}

}  // namespace
}  // namespace linked_list
}  // namespace assignments
}  // namespace csci2270
}  // namespace external
}  // namespace thilenius

int main(int argc, char* argv[]) {
  ::thilenius::external::vanguard::Suite suite(
      "Linked List", ::thilenius::external::csci2270::assignments::linked_list::
                         LinkedListTestSuite);
  suite.RunAllTestCasesProtected();
  ::thilenius::external::vanguard::Printer printer;
  printer.WriteStdCout(suite, &std::cout);
  printer.WriteSuiteResults(suite, &std::cout);
  printer.WriteSuiteReportCard(suite, &std::cout);
}
