// asio_demo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"


#include <iostream>
using namespace  std;

class Array {
public:
  Array(int size) : size_(size) {
    cout << "cccc" << endl;
    data_ = new int[size_];
  }
  // �������
  Array(const Array& temp_array) {
    size_ = temp_array.size_;
    data_ = new int[size_];
    for (int i = 0; i < size_; i ++) {
      data_[i] = temp_array.data_[i];
    }
  }

  // �����ֵ
  Array& operator=(const Array& temp_array) {
    delete[] data_;

    size_ = temp_array.size_;
    data_ = new int[size_];
    for (int i = 0; i < size_; i ++) {
      data_[i] = temp_array.data_[i];
    }
  }

  ~Array() {
    delete[] data_;
    cout << "~dddd" << endl;
  }

public:
  int *data_;
  int size_;
};

Array GetArr()
{
  Array my(5);
  //return my;
  Array res = std::move(my);
  return std::move(res);
}

auto MyAdd(int a, int b)
{
  return a + b;
}

int main(void)
{
  auto res = MyAdd(3, 5);

  
  return 0;
}
