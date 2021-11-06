//
// Created by xflajs00 on 02.11.2021.
//

#ifndef PHYSARUMSIM_SRC_SIMULATION_SIMCONFIG_H
#define PHYSARUMSIM_SRC_SIMULATION_SIMCONFIG_H

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <magic_enum.hpp>
#include <pf_common/enums.h>
#include <pf_imgui/serialization.h>
#include <toml++/toml.h>

ENABLE_PF_ENUM_OUT_FOR_NAMESPACE(pf::physarum)

namespace pf::physarum {

enum class ColorType {
  Simple, TwoColorGradient, Random/*, Heatmap*/, Rainbow
};

enum class FilterType {
  Median = 0, Blur = 1
};

class PopulationColor {
 public:
  PopulationColor() = default;
  explicit PopulationColor(ColorType type);

  [[nodiscard]] std::array<glm::vec3, 256> getColorLUT() const;

  static PopulationColor FromToml(const toml::table &src);
  [[nodiscard]] toml::table toToml() const;

  [[nodiscard]] ColorType getType() const;
  [[nodiscard]] const glm::vec3 &getSimpleColor() const;
  [[nodiscard]] const glm::vec3 &getGradientStart() const;
  [[nodiscard]] const glm::vec3 &getGradientEnd() const;
  [[nodiscard]] float getStartHue() const;
  [[nodiscard]] float getTrailPow() const;

  void setTrailPow(float trailPow);
  void setStartHue(float startHue);
  void setType(ColorType type);
  void setSimpleColor(const glm::vec3 &simpleColor);
  void setGradientStart(const glm::vec3 &gradientStart);
  void setGradientEnd(const glm::vec3 &gradientEnd);

  bool operator==(const PopulationColor &rhs) const;
  bool operator!=(const PopulationColor &rhs) const;

 private:
  [[nodiscard]] std::array<glm::vec3, 256> getSimpleLUT() const;
  [[nodiscard]] std::array<glm::vec3, 256> getGradientLUT() const;
  [[nodiscard]] std::array<glm::vec3, 256> getRandomLUT() const;
  [[nodiscard]] std::array<glm::vec3, 256> getRainbowLUT() const;
  ColorType type = ColorType::Simple;

  glm::vec3 simpleColor{1.f};

  glm::vec3 gradientStart{1.f};
  glm::vec3 gradientEnd{1.f};

  float startHue = 0.0f;

  float trailPow = 1.f;
};

enum class ParticleStart {
  Random,
  Point,
  InwardCircle,
  RandomCircle
};

struct PopulationConfig {
  float senseAngle;
  float senseDistance;
  float turnSpeed;
  float movementSpeed;
  float trailWeight;
  int blurKernelSize;
  float diffuseRate;
  float decayRate;
  float maxTrailValue;
  ParticleStart particleStart;
  int particleCount;
  int sensorSize;
  PopulationColor color;
  FilterType filterType;

  static PopulationConfig FromToml(const toml::table &src);
  [[nodiscard]] toml::table toToml() const;

  bool operator==(const PopulationConfig &rhs) const;
  bool operator!=(const PopulationConfig &rhs) const;
};

}// namespace pf::physarum

#endif//PHYSARUMSIM_SRC_SIMULATION_SIMCONFIG_H
