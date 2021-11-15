//
// Created by xflajs00 on 15.11.2021.
//

#include "FolderAboutDataLoader.h"
#include <fstream>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

namespace pf {
FolderAboutDataLoader::FolderAboutDataLoader(std::filesystem::path aboutPath) : folder(std::move(aboutPath)) {}

std::vector<AboutData> FolderAboutDataLoader::getAboutData() {
  auto dirIter = std::filesystem::directory_iterator(folder);
  return dirIter | ranges::views::transform([](const auto &entry) {
           const auto label = entry.path().filename().string();
           std::basic_ifstream<char8_t> is{entry.path()};
           std::basic_stringstream<char8_t> ss;
           ss << is.rdbuf();
           return AboutData{label, ss.str()};
         })
      | ranges::to_vector;
}

}// namespace pf