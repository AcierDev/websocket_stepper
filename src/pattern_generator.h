#pragma once

#include <vector>

/**
 * @brief Represents a 2D point in space
 */
struct Point {
  double x;
  double y;

  Point(double x = 0, double y = 0) : x(x), y(y) {}
};

/**
 * @brief Generates optimized movement patterns for pick and place operations
 */
class PatternGenerator {
 public:
  PatternGenerator(double pieceSize = 3.0, double borderSize = 0)
      : pieceSize_(pieceSize), borderSize_(borderSize) {}

  std::vector<Point> generatePattern(int rows, int cols, double startX = 3,
                                     double startY = 8, double endX = 35.0,
                                     double endY = 26.0);

 private:
  const double pieceSize_;   // Size of each piece in inches
  const double borderSize_;  // Required border size in inches
};