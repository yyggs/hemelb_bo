// This file is part of HemeLB and is Copyright (C)
// the HemeLB team and/or their institutions, as detailed in the
// file AUTHORS. This software is provided under the terms of the
// license in the file LICENSE.

#include "BufferPool.h"
#include <cstddef>

BufferPool::BufferPool(unsigned int bSize) : size(bSize), unused(0) {}

BufferPool::~BufferPool() {
  // Clear the stack
  char* buf;
  while (unused.pop(buf)) {
    delete[] buf;
  }
}

char* BufferPool::New() {
  // If the stack is empty, create a new array, otherwise pop an array
  char* buf;
  if (unused.pop(buf)) {
    return buf;
  } else {
    return new char[this->size];
  }
}

void BufferPool::Free(char* buf) {
  // If the buffer is NULL, skip
  if (buf == NULL)
    return;
  // If we have fewer than 10, add this one to the unused, otherwise delete it
  unused.push(buf);
}

// Return the size of buffers handled.
unsigned int BufferPool::GetSize() const {
  return this->size;
}
