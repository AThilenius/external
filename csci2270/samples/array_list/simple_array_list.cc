// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <iostream>

// A super (SUPER) simple and pretty terrible list implementation using dynamic
// arrays
class Vector {
 public:
  // Set defaults for data, array_size and count
  Vector() : data_(new int[4]), array_size_(4), count_(0) {}

  // Clean up after ourself in the destructor
  ~Vector() {
    if (data_) {
      delete[] data_;
      data_ = nullptr;
    }
  }

  // Adds an item to the end of the list, expanding the dynamic array if needed
  void PushBack(int val) {
    if (count_ == array_size_) {
      Expand();
    }
    data_[count_] = val;
    count_++;
  }

  // Removes one item form the back of the list. Returns -1 if the list is
  // already empty
  int RemoveBack() {
    if (count_ == 0) {
      return -1;
    }
    int the_value = data_[count_ - 1];
    count_--;
    return the_value;
  }

  // Gets an item from the list (just giving back the value, does not remove it
  // from the list). Returns -1 if the index is out of bounds
  int GetItem(int index) {
    if (index < 0 || index >= count_) {
      return -1;
    }
    return data_[index];
  }

  // Prints out all the items in the list
  void Print() {
    std::cout << "Array Contents: [";
    for (int i = 0; i < count_; i++) {
      std::cout << " " << data_[i];
    }
    std::cout << " ]" << std::endl;
  }

 private:
  // "Expand" the dynamic array. It does this by making a second array that is
  // 2x the size and copies everything into that new array.
  void Expand() {
    int* old_array = data_;
    data_ = new int[array_size_ * 2];
    for (int i = 0; i < count_; i++) {
      data_[i] = old_array[i];
    }
    array_size_ = array_size_ * 2;
    delete[] old_array;
  }

  // The only 3 things an instance of this class will ever have, a pointer to
  // our data on the heap, and 2 ints to track array size and number of items
  // respectively
  int* data_;
  int array_size_;
  int count_;
};

int main() {
  // Instantiate a Vector (Create an object)
  Vector my_vector;
  // Add 1000 items to it
  for (int i = 0; i < 1000; i++) {
    my_vector.PushBack(i);
  }
  // Remove 200 items from it
  for (int i = 0; i < 200; i++) {
    my_vector.RemoveBack();
  }
  // Print'er out! (Very exciting?)
  my_vector.Print();
}

// On a side note, code like this should never, ever, ever be used is real life.
// It's terrible. But it gets the point across without being too complex.
