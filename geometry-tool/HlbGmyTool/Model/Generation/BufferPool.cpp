// This file is part of HemeLB and is Copyright (C)
// the HemeLB team and/or their institutions, as detailed in the
// file AUTHORS. This software is provided under the terms of the
// license in the file LICENSE.

#include "BufferPool.h"
#include <cstddef>
#include <thread>
#include <atomic>

constexpr unsigned int THREAD_GROUPS = 4;
constexpr unsigned int THREADS_PER_GROUP = 32;

// Global atomic counter for thread numbering
std::atomic<unsigned int> threadCounter(0);

// Thread-local variable to store the thread's group
thread_local unsigned int threadGroup = threadCounter++ / THREADS_PER_GROUP;

BufferPool::BufferPool(unsigned int bSize) : size(bSize) {
  // Initialize each stack for each thread group
  unused = new std::stack<char*>[THREAD_GROUPS];
  mutexes = new std::mutex[THREAD_GROUPS];
}

BufferPool::~BufferPool() {
  for (unsigned int i = 0; i < THREAD_GROUPS; ++i) {
    std::lock_guard<std::mutex> lock(mutexes[i]);
    // Clear the stack
    while (!unused[i].empty()) {
      delete[] unused[i].top();
      unused[i].pop();
    }
  }
  delete[] unused;
  delete[] mutexes;
}

char* BufferPool::New() {
  std::lock_guard<std::mutex> lock(mutexes[threadGroup]);
  // If the stack is empty, create a new array, otherwise pop an array
  if (unused[threadGroup].empty()) {
    return new char[size];
  } else {
    char* ans = unused[threadGroup].top();
    unused[threadGroup].pop();
    return ans;
  }
}

void BufferPool::Free(char* buf) {
  if (buf == NULL) return;
  
  std::lock_guard<std::mutex> lock(mutexes[threadGroup]);
  // If we have fewer than 10, add this one to the unused, otherwise delete it
  if (unused[threadGroup].size() < 10) {
    unused[threadGroup].push(buf);
  } else {
    delete[] buf;
  }
}

// Return the size of buffers handled.
unsigned int BufferPool::GetSize() const {
  return size;
}