//
// Created by xflajs00 on 09.11.2021.
//

#ifndef PHYSARUMSIM_SRC_GLOBALTHREADPOOL_H
#define PHYSARUMSIM_SRC_GLOBALTHREADPOOL_H

#include <pf_common/parallel/ThreadPool.h>

namespace pf {
namespace  details {
inline static std::unique_ptr<ThreadPool> globalThreadPool = nullptr;
}

inline void initGlobalThreadPool(std::size_t threadCount) {
  if (details::globalThreadPool == nullptr) {
    details::globalThreadPool = std::make_unique<ThreadPool>(threadCount);
  }
}

inline ThreadPool &GlobalThreadPool() {
  assert(details::globalThreadPool != nullptr);
  return *details::globalThreadPool;
}

}

#endif//PHYSARUMSIM_SRC_GLOBALTHREADPOOL_H
