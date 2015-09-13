// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "linked_list.h"

#include <iostream>

namespace thilenius {
namespace external {
namespace csci2270 {
namespace assignments {
namespace linked_list {

//  __         __     __   __     __  __     ______     _____
// /\ \       /\ \   /\ "-.\ \   /\ \/ /    /\  ___\   /\  __-.
// \ \ \____  \ \ \  \ \ \-.  \  \ \  _"-.  \ \  __\   \ \ \/\ \
//  \ \_____\  \ \_\  \ \_\\"\_\  \ \_\ \_\  \ \_____\  \ \____-
//   \/_____/   \/_/   \/_/ \/_/   \/_/\/_/   \/_____/   \/____/
//                              __         __     ______     ______   ______
//                             /\ \       /\ \   /\  ___\   /\__  _\ /\  ___\
//                             \ \ \____  \ \ \  \ \___  \  \/_/\ \/ \ \___  \
//                              \ \_____\  \ \_\  \/\_____\    \ \_\  \/\_____\
//                               \/_____/   \/_/   \/_____/     \/_/   \/_____/

// Welcome to assignment two, linked lists! This is a tough one, I'll be honest!
// It will be your first 'real' use of pointers. Try your best to keep your code
// 'clean'. That probably means you'll need to rewrite things a few times as you
// find better ways of doing things. That's how coding actually works, including
// at Google. None of this 'puke on a page and turn it in' stuff like we do in
// our humanities classes :p Done a little elegantly, this assignment can be
// completed in less than 50 lines of code.

// A few tips that will make this assignment way, way easier:
//
//  - Implement each function in the order that they appear in this file, then
//    use the functions you already implemented to write the later ones. For
//    example, (hint hint) maybe Size would be helpful in InsertAtIndex, or
//    (hint hint) maybe InsertAtIndex would be helpful for PushFront
//
//  - When you're stuck, whiteboard out exactly what needs to happen. Chances
//    are your struggle is conceptual, not programmatic. Once you see clearly
//    what to do on a whiteboard, the code will be pretty easy.
//
//  - If you find yourself writing code to test every possible edge case you
//    might want to step back and think of a 'cleaner' way of doing things. I
//    promise, there is one. The less code, the more you thought the problem
//    out, always.
//
//  - Work with your peers! Tossing ideas back and forth is an excellent way to
//    wrap your head around something. No, that doesn't mean ask your peers for
//    the answer because it's the night it's due and you're pretty screwed
//    because the assignment is so hard :)

// The constructor. It needs to set 'head' to 'nullptr'. There are 2 ways to do
// this in a constructor, you can use either.
LinkedList::LinkedList() {}

// Should return the size of the linked list. You'll need to walk the linked
// list and count nodes to do this.
int LinkedList::Size() { return -1; }

// Inserts an element at index (so that once inserted it is at that index) into
// the linked list. This is one of the more tricky ones. You'll need to talk the
// list until you find the index you want to insert at (maybe one before that,
// hint hint) and 'wire' in the new element. Whiteboard this one out!!
bool LinkedList::InsertAtIndex(int value, int index) { return true; }

// Adds an element to the front of the linked list. If this one takes you more
// than one or two lines of code then you clearly didn't read the tips above :p
void LinkedList::PushFront(int value) {}

// Adds an element to the end of the linked list. If this one takes you more
// than one or two lines of code then you clearly didn't read the tips above :p
void LinkedList::PushBack(int value) {}

// This is the subscript operator. It allows the user of your code to access the
// nth element by just using the array operator, like this:
// std::cout << "The third element in the list is: " << my_linked_list[2];
// It takes in the index of the item the user wants to get at, and returns the
// item at that index (as a reference).
int& LinkedList::operator[](int index) {}

// Removes an item at the given index. This means you'll need to walk the list
// until you get to that index (or one before it, hint hint), and remove that
// item by 'wiring' the next_node to skip that item. You'll also need to call
// delete on that item to free the memory back to the OS.
bool LinkedList::RemoveAtIndex(int index) { return true; }

// Should remove all items from the list. If this one takes you more than a few
// lines of code then you clearly didn't read the tips above. Hint hint.
void LinkedList::Clear() {}

// Extra Credit (worth 5 points)
// Removes all occurrences of 'value' from the list. For example, if I have the
// linked list [0, 1, 1, 1, 2] and I call RemoveAll(1), it should remove the
// middle 3 items, leaving only [0, 2]
void LinkedList::RemoveAll(int value) {}

}  // namespace linked_list
}  // namespace assignments
}  // namespace csci2270
}  // namespace external
}  // namespace thilenius
