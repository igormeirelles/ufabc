#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void OpenGLWindow::initializeGL() {
  glClearColor(1, 1, 1, 1);

  // Enable depth buffering
  glEnable(GL_DEPTH_TEST);

  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "f.vert",
                                    getAssetsPath() + "f.frag");

  // Load model
  m_model.loadFromFile(getAssetsPath() + "F.obj");

  m_model.setupVAO(m_program);

  //////////////////////////////////////////////teste///////////////////////////////////////////////////

  m_program1 = createProgramFromFile(getAssetsPath() + "ufabc.vert",
                                    getAssetsPath() + "ufabc.frag");

  // Load model
  m_model1.loadFromFile(getAssetsPath() + "ufabc.obj");
  m_model1.setupVAO(m_program1);
  
  /////////////////////////////////////////final teste////////////////////////////////////////////////

  // Camera at (0,0,0) and looking towards the negative z
  m_viewMatrix =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));

  // Setup
  for (const auto index : iter::range(m_numStars)) {
    auto &position{m_starPositions.at(index)};
    auto &rotation{m_starRotations.at(index)};

    randomizeStar(position, rotation);
  }
}

void OpenGLWindow::randomizeStar(glm::vec3 &position, glm::vec3 &rotation) {
  // Get random position
  // x and y coordinates in the range [-20, 20]
  // z coordinates in the range [-100, 0]
  std::uniform_real_distribution<float> distPosXY(-20.0f, 20.0f);
  std::uniform_real_distribution<float> distPosZ(-100.0f, 0.0f);

  position = glm::vec3(distPosXY(m_randomEngine), distPosXY(m_randomEngine),
                       distPosZ(m_randomEngine));

  //  Get random rotation axis
  std::uniform_real_distribution<float> distRotAxis(-1.0f, 1.0f);

  rotation = glm::normalize(glm::vec3(distRotAxis(m_randomEngine),
                                      distRotAxis(m_randomEngine),
                                      distRotAxis(m_randomEngine)));
}

void OpenGLWindow::paintGL() {
  
  ///////////////////////////////////////////começo teste//////////////////////////////////////////////

  // Animate angle by 15 degrees per second
  float deltaTime1{static_cast<float>(getDeltaTime())};
  m_angle1 = glm::wrapAngle(m_angle1 + glm::radians(30.0f) * deltaTime1);

  // Clear color buffer and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  glUseProgram(m_program1);
  glBindVertexArray(m_VAO);

  // Update uniform variable
  GLint angle1Loc{glGetUniformLocation(m_program1, "angle")};
  glUniform1f(angle1Loc, m_angle1);

  // Draw triangles
  m_model1.render();

  ////////////////////////////////////////////////final teste////////////////////////////////////////

  update();

  // Clear color buffer and depth buffer
  
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  glUseProgram(m_program);

  // Get location of uniform variables (could be precomputed)
  GLint viewMatrixLoc{glGetUniformLocation(m_program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(m_program, "projMatrix")};
  GLint modelMatrixLoc{glGetUniformLocation(m_program, "modelMatrix")};
  GLint colorLoc{glGetUniformLocation(m_program, "color")};

  // Set uniform variables used by every scene object
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  glUniform4f(colorLoc, 1.0f, 0, 0, 1.0f);  // White

  // Render each star
  for (const auto index : iter::range(m_numStars)) {
    auto &position{m_starPositions.at(index)};
    auto &rotation{m_starRotations.at(index)};

    // Compute model matrix of the current star
    glm::mat4 modelMatrix{1.0f};
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
    modelMatrix = glm::rotate(modelMatrix, m_angle, rotation);

    // Set uniform variable
    glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    m_model.render();
  }

  glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  auto aspect{static_cast<float>(m_viewportWidth) /
  static_cast<float>(m_viewportHeight)};
  m_projMatrix =
    glm::perspective(glm::radians(30.0f), aspect, 0.01f, 100.0f);
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;
}

void OpenGLWindow::terminateGL() { glDeleteProgram(m_program); }

void OpenGLWindow::update() {
  // Animate angle by 90 degrees per second
  float deltaTime{static_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);

  // Update
  for (const auto index : iter::range(m_numStars)) {
    auto &position{m_starPositions.at(index)};
    auto &rotation{m_starRotations.at(index)};

    // The position in z increases 10 units per second
    position.z += deltaTime * 10.0f;

    // If it is now behind the camera, select a new random position and
    // orientation, and move it back to -100
    if (position.z > 0.1f) {
      randomizeStar(position, rotation);
      position.z = -100.0f;  // Back to -100
    }
  }
}