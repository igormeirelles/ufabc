#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <random>

#include "abcg.hpp"
#include "model.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  static const int m_numStars{1000};

  GLuint m_program{};
  GLuint m_program1{};
  GLuint m_VAO{};

  int m_viewportWidth{};
  int m_viewportHeight{};
  int m_verticesToDraw{};

  std::default_random_engine m_randomEngine;

  Model m_model;
  Model m_model1;

  std::array<glm::vec3, m_numStars> m_starPositions;
  std::array<glm::vec3, m_numStars> m_starRotations;
  float m_angle{};
  float m_angle1{};

  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
  float m_FOV{40.0f};

  void randomizeStar(glm::vec3 &position, glm::vec3 &rotation);
  void update();
};

#endif