//
// Created by xflajs00 on 22.10.2021.
//

#ifndef OPENGL_TEMPLATE_SRC_RENDERERS_RENDERER_H
#define OPENGL_TEMPLATE_SRC_RENDERERS_RENDERER_H

#include <optional>
#include <string>

namespace pf::ogl {

class Renderer {
 public:
  virtual std::optional<std::string> init() = 0;

  virtual void render() = 0;

  virtual ~Renderer() = default;
};

}// namespace pf::ogl

#endif//OPENGL_TEMPLATE_SRC_RENDERERS_RENDERER_H
