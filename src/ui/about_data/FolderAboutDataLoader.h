//
// Created by xflajs00 on 15.11.2021.
//

#ifndef PHYSARUMSIM_FOLDERABOUTDATALOADER_H
#define PHYSARUMSIM_FOLDERABOUTDATALOADER_H

#include "AboutDataLoader.h"
#include <filesystem>
#include <vector>

namespace pf {
class FolderAboutDataLoader : public AboutDataLoader {
 public:
  explicit FolderAboutDataLoader(std::filesystem::path aboutPath);

  [[nodiscard]] std::vector<AboutData> getAboutData() override;

 private:
  std::filesystem::path folder;
};
}// namespace pf

#endif//PHYSARUMSIM_FOLDERABOUTDATALOADER_H
