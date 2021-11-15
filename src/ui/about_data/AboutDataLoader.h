//
// Created by xflajs00 on 15.11.2021.
//

#ifndef PHYSARUMSIM_ABOUTDATALOADER_H
#define PHYSARUMSIM_ABOUTDATALOADER_H

#include "AboutData.h"
#include <vector>

namespace pf {
class AboutDataLoader {
 public:
  [[nodiscard]] virtual std::vector<AboutData> getAboutData() = 0;
  virtual ~AboutDataLoader() = default;
};
}

#endif//PHYSARUMSIM_ABOUTDATALOADER_H
