//
// Created by xflajs00 on 14.11.2021.
//

#ifndef PHYSARUMSIM_HELPDATA_H
#define PHYSARUMSIM_HELPDATA_H

#include <string>
#include <vector>

namespace pf{

struct HelpData {
  std::vector<std::string> section;
  std::u8string markdown;
};

}

#endif//PHYSARUMSIM_HELPDATA_H
