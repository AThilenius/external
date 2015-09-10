// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <iostream>
#include <stdexcept>
#include <string>

// A ever so slightly less shit implementation of a list using dynamic arrays
template <typename T>
class Vector {
 public:
  // Set defaults for data, array_size and count
  Vector() : data_(new T[4]), array_size_(4), count_(0) {}

  // Clean up after ourself in the destructor
  ~Vector() {
    if (data_) {
      delete[] data_;
      data_ = nullptr;
    }
  }

  // Adds an item to the end of the list, expanding the dynamic array if needed
  void PushBack(const T& val) {
    if (count_ == array_size_) {
      Expand();
    }
    data_[count_++] = val;
  }

  // Removes one item form the back of the list
  T RemoveBack() {
    if (count_ == 0) {
      throw std::out_of_range("Vector is already empty");
    }
    return data_[count_--];
  }

  int Size() {
    return count_;
  }

  // Gets an item from the list (just giving back the value, does not remove it
  // from the list)
  T& operator[](const int index) {
    if (index < 0 || index >= count_) {
      throw std::out_of_range("Index is not in the range [0, " +
                              std::to_string(count_ - 1));
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
    T* old_array = data_;
    data_ = new T[array_size_ * 2];
    for (int i = 0; i < count_; i++) {
      data_[i] = old_array[i];
    }
    array_size_ = array_size_ * 2;
    delete[] old_array;
  }

  // The only 3 things an instance of this class will ever have, a pointer to
  // our data on the heap, and 2 ints to track array size and number of items
  // respectively
  T* data_;
  int array_size_;
  int count_;
};

int main() {
  // Instantiate a Vector (Create an object)
  Vector<std::string> my_vector;
  my_vector.PushBack("Hello");
  my_vector.PushBack("World!");
  my_vector.Print();

  my_vector.RemoveBack();
  std::cout << "Vector is not of size: " << my_vector.Size() << std::endl;
  std::cout << "The 0th item is: " << my_vector[0] << std::endl;
}

// On a side note, code like this should never, ever, ever be used is real life.
// It's terrible. But it gets the point across without being too complex.
