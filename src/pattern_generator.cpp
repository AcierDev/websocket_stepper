#include "pattern_generator.h"

#include <Arduino.h>

// std::vector<Point> PatternGenerator::generatePattern(int rows, int cols,
//                                                      double startX,
//                                                      double startY,
//                                                      double gridWidth,
//                                                      double gridLength) {
//   std::vector<Point> pattern;
//   Serial.println("\n=== Pattern Generation Started ===");
//   Serial.printf("Grid Parameters:\n");
//   Serial.printf("- Dimensions: %d rows x %d columns\n", rows, cols);
//   Serial.printf("- Origin: (%.2f, %.2f)\n", startX, startY);
//   Serial.printf("- Size: %.2f x %.2f\n", gridWidth, gridLength);
//   Serial.printf("- Piece size: %.2f\n", pieceSize_);
//   Serial.printf("- Border: %.2f\n", borderSize_);

//   if (rows <= 0 || cols <= 0) {
//     Serial.println("❌ Error: Invalid rows or columns");
//     return pattern;
//   }

//   // Calculate usable area dimensions
//   double usableWidth = gridWidth - 2 * borderSize_;
//   double usableLength = gridLength - 2 * borderSize_;
//   Serial.printf("\nUsable Area:\n");
//   Serial.printf("- Width: %.2f (%.2f - 2 * %.2f)\n", usableWidth, gridWidth,
//                 borderSize_);
//   Serial.printf("- Length: %.2f (%.2f - 2 * %.2f)\n", usableLength,
//   gridLength,
//                 borderSize_);

//   // Calculate spacing between pieces
//   double xSpacing = (usableWidth - (pieceSize_ * cols)) / (cols - 1);
//   double ySpacing = (usableLength - (pieceSize_ * rows)) / (rows - 1);
//   Serial.printf("\nSpacing:\n");
//   Serial.printf("- X spacing: %.2f\n", xSpacing);
//   Serial.printf("- Y spacing: %.2f\n", ySpacing);

//   // Verify spacing is sufficient
//   if (xSpacing < pieceSize_ || ySpacing < pieceSize_) {
//     Serial.println("❌ Error: Insufficient spacing between pieces");
//     Serial.printf("Minimum required: %.2f, Got: X=%.2f, Y=%.2f\n",
//     pieceSize_,
//                   xSpacing, ySpacing);
//     return pattern;
//   }

//   Serial.println("\nGenerating points in snake pattern:");
//   // Start from back corner (max Y) and work forward
//   for (int row = rows - 1; row >= 0; --row) {
//     bool leftToRight = ((rows - 1 - row) % 2) == 0;
//     Serial.printf("\nRow %d (%s):\n", row, leftToRight ? "→" : "←");

//     if (leftToRight) {
//       for (int col = 0; col < cols; ++col) {
//         double x = startX + borderSize_ + (col * xSpacing);
//         double y = startY + borderSize_ + (row * ySpacing);
//         pattern.emplace_back(x, y);
//         Serial.printf("  Point %lu: (%.2f, %.2f)\n", pattern.size(), x, y);
//       }
//     } else {
//       for (int col = cols - 1; col >= 0; --col) {
//         double x = startX + borderSize_ + (col * xSpacing);
//         double y = startY + borderSize_ + (row * ySpacing);
//         pattern.emplace_back(x, y);
//         Serial.printf("  Point %lu: (%.2f, %.2f)\n", pattern.size(), x, y);
//       }
//     }
//   }

//   Serial.printf("\n✓ Pattern generation complete: %lu points generated\n",
//                 pattern.size());
//   Serial.println("===============================\n");

//   return pattern;
// }

std::vector<Point> PatternGenerator::generatePattern(int rows, int cols,
                                                     double startX,
                                                     double startY, double endX,
                                                     double endY) {
  std::vector<Point> pattern;
  Serial.println("\n=== Pattern Generation Started ===");
  Serial.printf("Grid Parameters:\n");
  Serial.printf("- Dimensions: %d rows x %d columns\n", rows, cols);
  Serial.printf("- Origin: (%.2f, %.2f)\n", startX, startY);
  Serial.printf("- End: (%.2f, %.2f)\n", endX, endY);
  Serial.printf("- Piece size: %.2f\n", pieceSize_);
  Serial.printf("- Border: %.2f\n", borderSize_);

  if (rows <= 0 || cols <= 0) {
    Serial.println("❌ Error: Invalid rows or columns");
    return pattern;
  }

  double gridWidth = endX - startX;
  double gridLength = endY - startY;

  Serial.printf("Grid Width: %.2f\n", gridWidth);
  Serial.printf("Grid Length: %.2f\n", gridLength);

  double xSpacing = gridWidth / (cols - 1);
  double ySpacing = gridLength / (rows - 1);

  Serial.printf("\nSpacing:\n");
  Serial.printf("- X spacing: %.2f\n", xSpacing);
  Serial.printf("- Y spacing: %.2f\n", ySpacing);

  // Verify spacing is sufficient
  if (xSpacing < pieceSize_ || ySpacing < pieceSize_) {
    Serial.println("❌ Error: Insufficient spacing between pieces");
    Serial.printf("Minimum required: %.2f, Got: X=%.2f, Y=%.2f\n", pieceSize_,
                  xSpacing, ySpacing);
    return pattern;
  }

  Serial.println("\nGenerating points in snake pattern:");

  // Calculate the center point of the first row (will be moved to end later)
  int centerCol = cols / 2;
  Point centerPoint(startX + (centerCol * xSpacing), startY);
  bool centerPointAdded = false;

  // Start from back corner (max Y) and work forward
  for (int row = rows - 1; row >= 0; --row) {
    bool leftToRight = ((rows - 1 - row) % 2) == 0;
    Serial.printf("\nRow %d (%s):\n", row, leftToRight ? "→" : "←");

    if (row == 0) {
      // Special handling for first row - do outside in
      for (int i = 0; i < cols / 2; i++) {
        // Add left side point
        double x = startX + (i * xSpacing);
        double y = startY;
        pattern.emplace_back(x, y);
        Serial.printf("  Point %lu: (%.2f, %.2f)\n", pattern.size(), x, y);

        // Add right side point if not center
        int rightCol = cols - 1 - i;
        if (rightCol != centerCol) {  // Skip center point
          x = startX + (rightCol * xSpacing);
          pattern.emplace_back(x, y);
          Serial.printf("  Point %lu: (%.2f, %.2f)\n", pattern.size(), x, y);
        } else {
          centerPointAdded = true;
        }

        // Stop if we've reached or passed the center
        if (i + 1 >= centerCol) break;
      }
    } else {
      // Normal snake pattern for other rows
      if (leftToRight) {
        for (int col = 0; col < cols; ++col) {
          double x = startX + (col * xSpacing);
          double y = startY + (row * ySpacing);
          pattern.emplace_back(x, y);
          Serial.printf("  Point %lu: (%.2f, %.2f)\n", pattern.size(), x, y);
        }
      } else {
        for (int col = cols - 1; col >= 0; --col) {
          double x = startX + (col * xSpacing);
          double y = startY + (row * ySpacing);
          pattern.emplace_back(x, y);
          Serial.printf("  Point %lu: (%.2f, %.2f)\n", pattern.size(), x, y);
        }
      }
    }
  }

  // Add the center point of the first row last
  if (centerPointAdded) {
    pattern.push_back(centerPoint);
    Serial.printf("  Final Point %lu (center): (%.2f, %.2f)\n", pattern.size(),
                  centerPoint.x, centerPoint.y);
  }

  Serial.printf("\n✓ Pattern generation complete: %lu points generated\n",
                pattern.size());
  Serial.println("===============================\n");

  return pattern;
}
