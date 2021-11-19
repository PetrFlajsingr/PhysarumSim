//
// Created by xflajs00 on 04.11.2021.
//

#include "SimConfig.h"
#include <glm/gtx/color_space.hpp>
#include <glm/vec3.hpp>
#include <utils/rand.h>

namespace pf::physarum {

PopulationColor::PopulationColor(ColorType type) : type(type) {}

std::array<glm::vec3, 256> PopulationColor::getColorLUT() const {
  switch (type) {
    case ColorType::Simple: return getSimpleLUT();
    case ColorType::TwoColorGradient: return getGradientLUT();
    case ColorType::Random: return getRandomLUT();
    case ColorType::Rainbow: return getRainbowLUT();
  }
  return {};
}

std::array<glm::vec3, 256> PopulationColor::getSimpleLUT() const {
  auto result = std::array<glm::vec3, 256>{};
  result.fill(simpleColor);
  return result;
}

std::array<glm::vec3, 256> PopulationColor::getGradientLUT() const {
  auto result = std::array<glm::vec3, 256>{};
  for (int i = 0; i < 256; ++i) { result[i] = glm::mix(gradientStart, gradientEnd, i / 256.0f); }
  return result;
}

std::array<glm::vec3, 256> PopulationColor::getRandomLUT() const {
  auto result = std::array<glm::vec3, 256>{};
  for (int i = 0; i < 256; ++i) { result[i] = fastRandomVec3(0.f, 1.f); }
  return result;
}

std::array<glm::vec3, 256> PopulationColor::getRainbowLUT() const {
  auto result = std::array<glm::vec3, 256>{};
  auto hsvCol = glm::vec3{startHue, 1, 1};
  for (int i = 0; i < 256; ++i) {
    hsvCol.r += 1 / 256.f * 360;
    hsvCol.r -= static_cast<int>(hsvCol.r / 360) * 360.f;
    result[i] = glm::rgbColor(hsvCol);
  }
  return result;
}

PopulationColor PopulationColor::FromToml(const toml::table &src) {
  PopulationColor result;
  result.type = static_cast<ColorType>(src["type"].value<int>().value());
  result.simpleColor = ui::ig::deserializeGlmVec<glm::vec3>(*src["simpleColor"].as_array());
  result.gradientStart = ui::ig::deserializeGlmVec<glm::vec3>(*src["gradientStart"].as_array());
  result.gradientEnd = ui::ig::deserializeGlmVec<glm::vec3>(*src["gradientEnd"].as_array());
  result.startHue = src["startHue"].value<float>().value();
  result.trailPow = src["trailPow"].value<float>().value();
  return result;
}

toml::table PopulationColor::toToml() const {
  return toml::table{{
      {"type", static_cast<int>(type)},
      {"simpleColor", ui::ig::serializeGlmVec(simpleColor)},
      {"gradientStart", ui::ig::serializeGlmVec(gradientStart)},
      {"gradientEnd", ui::ig::serializeGlmVec(gradientEnd)},
      {"startHue", startHue},
      {"trailPow", trailPow},
  }};
}

bool PopulationColor::operator==(const PopulationColor &rhs) const {
  const auto isSameType = type == rhs.type;
  if (!isSameType) { return false; }
  if (trailPow != rhs.trailPow) { return false; }
  switch (type) {
    case ColorType::Simple: return simpleColor == rhs.simpleColor;
    case ColorType::TwoColorGradient: return gradientStart == rhs.gradientStart && gradientEnd == rhs.gradientEnd;
    case ColorType::Random: return false;
    case ColorType::Rainbow: return startHue == rhs.startHue;
  }
  return false;
}

bool PopulationColor::operator!=(const PopulationColor &rhs) const { return !(rhs == *this); }
ColorType PopulationColor::getType() const { return type; }
const glm::vec3 &PopulationColor::getSimpleColor() const { return simpleColor; }
const glm::vec3 &PopulationColor::getGradientStart() const { return gradientStart; }
const glm::vec3 &PopulationColor::getGradientEnd() const { return gradientEnd; }
void PopulationColor::setSimpleColor(const glm::vec3 &simpleColor) { PopulationColor::simpleColor = simpleColor; }
void PopulationColor::setGradientStart(const glm::vec3 &gradientStart) {
  PopulationColor::gradientStart = gradientStart;
}
void PopulationColor::setGradientEnd(const glm::vec3 &gradientEnd) { PopulationColor::gradientEnd = gradientEnd; }
void PopulationColor::setType(ColorType type) { PopulationColor::type = type; }
float PopulationColor::getStartHue() const { return startHue; }
void PopulationColor::setStartHue(float startHue) { PopulationColor::startHue = startHue; }
float PopulationColor::getTrailPow() const { return trailPow; }
void PopulationColor::setTrailPow(float trailPow) { PopulationColor::trailPow = trailPow; }

PopulationConfig PopulationConfig::FromToml(const toml::table &src) {
  std::vector<SpeciesInteractionConfig> interactions;
  for (const auto &interactionSrc : *src["interactions"].as_array()) {
    interactions.emplace_back(SpeciesInteractionConfig::FromToml(*interactionSrc.as_table()));
  }
  return {.senseAngle = src["senseAngle"].value<float>().value(),
          .senseDistance = src["senseDistance"].value<float>().value(),
          .turnSpeed = src["turnSpeed"].value<float>().value(),
          .movementSpeed = src["movementSpeed"].value<float>().value(),
          .trailWeight = src["trailWeight"].value<float>().value(),
          .blurKernelSize = src["blurKernelSize"].value<int>().value(),
          .diffuseRate = src["diffuseRate"].value<float>().value(),
          .decayRate = src["decayRate"].value<float>().value(),
          .maxTrailValue = src["maxTrailValue"].value<float>().value(),
          .particleStart = static_cast<ParticleStart>(src["particleStart"].value<int>().value()),
          .particleCount = src["particleCount"].value<int>().value(),
          .sensorSize = src["sensorSize"].value<int>().value(),
          .color = PopulationColor::FromToml(*src["color"].as_table()),
          .filterType = static_cast<FilterType>(src["filterType"].value<int>().value()),
          .maxSteerRandomness = src["maxSteerRandomness"].value<float>().value(),
          .speciesInteractions = std::move(interactions)};
}

toml::table PopulationConfig::toToml() const {
  toml::array interactions;
  std::ranges::for_each(speciesInteractions,
                        [&](const auto &interaction) { interactions.emplace_back<toml::table>(interaction.toToml()); });
  return toml::table{{
      {"senseAngle", senseAngle},
      {"senseDistance", senseDistance},
      {"turnSpeed", turnSpeed},
      {"movementSpeed", movementSpeed},
      {"trailWeight", trailWeight},
      {"blurKernelSize", blurKernelSize},
      {"diffuseRate", diffuseRate},
      {"decayRate", decayRate},
      {"maxTrailValue", maxTrailValue},
      {"particleStart", static_cast<int>(particleStart)},
      {"particleCount", particleCount},
      {"sensorSize", sensorSize},
      {"color", color.toToml()},
      {"filterType", static_cast<int>(filterType)},
      {"maxSteerRandomness", maxSteerRandomness},
      {"interactions", interactions},
  }};
}

bool PopulationConfig::operator==(const PopulationConfig &rhs) const {
  return senseAngle == rhs.senseAngle && senseDistance == rhs.senseDistance && turnSpeed == rhs.turnSpeed
      && movementSpeed == rhs.movementSpeed && trailWeight == rhs.trailWeight && blurKernelSize == rhs.blurKernelSize
      && diffuseRate == rhs.diffuseRate && decayRate == rhs.decayRate && maxTrailValue == rhs.maxTrailValue
      && particleStart == rhs.particleStart && particleCount == rhs.particleCount && sensorSize == rhs.sensorSize
      && color == rhs.color && filterType == rhs.filterType && speciesInteractions == rhs.speciesInteractions;
}

bool PopulationConfig::operator!=(const PopulationConfig &rhs) const { return !(rhs == *this); }

bool InteractionConfig::operator==(const InteractionConfig &rhs) const {
  return interactionType == rhs.interactionType && distance == rhs.distance && power == rhs.power
      && interactedSpecies == rhs.interactedSpecies && enableDrawFalloff == rhs.enableDrawFalloff && particleCount == rhs.particleCount;
}

bool InteractionConfig::operator!=(const InteractionConfig &rhs) const { return !(rhs == *this); }

InteractionConfig InteractionConfig::FromToml(const toml::table &src) {
  return {
      .interactionType = static_cast<MouseInteraction>(src["interactionType"].value<int>().value()),
      .distance = src["distance"].value<float>().value(),
      .power = src["power"].value<float>().value(),
      .interactedSpecies = src["interactedSpecies"].value<int>().value(),
      .enableDrawFalloff = src["enableDrawFalloff"].value<bool>().value(),
      .particleCount = src["particleCount"].value<int>().value(),
  };
}

toml::table InteractionConfig::toToml() const {
  return toml::table{{
      {"interactionType", static_cast<int>(interactionType)},
      {"distance", distance},
      {"power", power},
      {"interactedSpecies", interactedSpecies},
      {"enableDrawFalloff", enableDrawFalloff},
      {"particleCount", particleCount},
  }};
}

bool SpeciesInteractionConfig::operator==(const SpeciesInteractionConfig &rhs) const {
  return interactionType == rhs.interactionType && factor == rhs.factor && speciesName == rhs.speciesName
      && speciesIndex == rhs.speciesIndex;
}

bool SpeciesInteractionConfig::operator!=(const SpeciesInteractionConfig &rhs) const { return !(rhs == *this); }

SpeciesInteractionConfig SpeciesInteractionConfig::FromToml(const toml::table &src) {
  return {
      .interactionType = static_cast<SpeciesInteraction>(src["interactionType"].value<int>().value()),
      .factor = src["factor"].value<float>().value_or(1.f),
      .speciesName = src["speciesName"].value<std::string>().value(),
      .speciesIndex = src["speciesIndex"].value<int>().value(),
  };
}
toml::table SpeciesInteractionConfig::toToml() const {
  return toml::table{{
      {"interactionType", static_cast<int>(interactionType)},
      {"factor", factor},
      {"speciesName", speciesName},
      {"speciesIndex", speciesIndex},
  }};
}
}// namespace pf::physarum