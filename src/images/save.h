//
// Created by xflajs00 on 08.11.2021.
//

#ifndef PHYSARUMSIM_SRC_IMAGES_SAVE_H
#define PHYSARUMSIM_SRC_IMAGES_SAVE_H

#include "Formats.h"
#include <filesystem>
#include <span>

namespace pf {

void saveImage(const std::filesystem::path &path, ImageFormat format, PixelFormat pixelFormat, std::size_t width,
               std::size_t height, std::span<const std::byte> data);

}

#endif//PHYSARUMSIM_SRC_IMAGES_SAVE_H
