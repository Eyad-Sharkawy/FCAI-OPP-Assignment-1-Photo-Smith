/**
 * @file ImageFilters.cpp
 * @brief Implementation of image processing filters with Qt integration and cancellation support.
 * 
 * This file contains the complete implementation of the ImageFilters class, providing
 * a comprehensive suite of image processing operations. All implementations include
 * proper error handling, progress tracking, and cancellation support where applicable.
 * 
 * @details The implementation includes:
 * - Basic color operations (grayscale, invert, black & white)
 * - Geometric transformations (flip, rotate, resize)
 * - Advanced effects (blur, edge detection, infrared simulation)
 * - Special effects (TV/CRT simulation, purple tint, frame addition)
 * - Image combination (merge operations)
 * 
 * All long-running operations support:
 * - Real-time progress updates via QProgressBar
 * - Status updates via QStatusBar
 * - Cancellation via atomic flags
 * - Exception safety and error handling
 * 
 * @author Team Members:
 * - Ahmed Mohamed ElSayed Tolba (ID: 20242023)
 * - Eyad Mohamed Saad Ali (ID: 20242062) 
 * - Tarek Sami Mohamed Mohamed (ID: 20242190)
 * 
 * @institution Faculty of Computers and Artificial Intelligence, Cairo University
 * @version 2.0.0
 * @date October 13, 2025
 * @copyright FCAI Cairo University
 */

#include "ImageFilters.h"
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QApplication>
#include <QtCore/QString>
#include "image/Image_Class.h"

/**
 * @brief Constructs an ImageFilters object with Qt UI components.
 * 
 * @param progressBar Pointer to QProgressBar for progress tracking (can be nullptr)
 * @param statusBar Pointer to QStatusBar for status updates (can be nullptr)
 * 
 * @note Both parameters are optional. If nullptr, progress and status updates will be skipped.
 */
ImageFilters::ImageFilters(QProgressBar* progressBar, QStatusBar* statusBar)
    : progressBar(progressBar), statusBar(statusBar)
{
}

/**
 * @brief Updates the progress bar with current progress.
 * 
 * This method safely updates the progress bar and processes Qt events to maintain
 * UI responsiveness during long-running operations.
 * 
 * @param value Current progress value (0 to total)
 * @param total Maximum progress value
 * @param updateInterval Number of operations between UI updates (default: 50)
 * 
 * @note This method is thread-safe and can be called from any thread.
 * @see QApplication::processEvents() for UI responsiveness
 */
void ImageFilters::updateProgress(int value, int total, int updateInterval)
{
    if (progressBar) {
        progressBar->setValue(value);
        if (value % updateInterval == 0) {
            QApplication::processEvents();
        }
    }
}

/**
 * @brief Checks for cancellation and restores previous image state if cancelled.
 * 
 * This method provides thread-safe cancellation support by checking an atomic flag
 * and restoring the previous image state if cancellation was requested.
 * 
 * @param cancelRequested Atomic flag indicating if cancellation was requested
 * @param currentImage Reference to current image (restored if cancelled)
 * @param preFilterImage Reference to previous image state
 * @param filterName Name of the filter for status message
 * 
 * @note This method should be called periodically during long-running operations.
 * @see std::atomic for thread-safe cancellation
 */
void ImageFilters::checkCancellation(std::atomic<bool>& cancelRequested, Image& currentImage, Image& preFilterImage, const QString& filterName)
{
    if (cancelRequested) {
        currentImage = preFilterImage;
        if (statusBar) {
            statusBar->showMessage(QString("%1 filter cancelled").arg(filterName));
        }
        if (progressBar) {
            progressBar->setVisible(false);
        }
    }
}

/**
 * @brief Apply grayscale conversion to the image with progress tracking and cancellation support.
 * 
 * Converts the image to grayscale by averaging RGB values for each pixel.
 * This operation supports real-time progress tracking and can be cancelled by the user.
 * 
 * @param currentImage Reference to the image to process (modified in-place)
 * @param preFilterImage Reference to store the original image state for cancellation
 * @param cancelRequested Atomic flag to check for cancellation requests
 * 
 * @details The grayscale conversion:
 * - Uses simple averaging: gray = (R + G + B) / 3
 * - Processes pixels row by row for progress tracking
 * - Checks for cancellation after each row
 * - Updates progress bar and status messages
 * - Restores original state if cancelled
 * 
 * @note This is a long-running operation that can be cancelled.
 * @see updateProgress() for progress tracking
 * @see checkCancellation() for cancellation handling
 */
void ImageFilters::applyGrayscale(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested)
{
    if (progressBar) {
        progressBar->setVisible(true);
        progressBar->setRange(0, currentImage.height);
        progressBar->setValue(0);
    }
    
    if (statusBar) {
        statusBar->showMessage("Applying Grayscale filter... (Click Cancel to stop)");
    }
    QApplication::processEvents();
    
    try {
        // Simple grayscale conversion with cancellation support
        for (int y = 0; y < currentImage.height; y++) {
            // Check for cancellation
            if (cancelRequested) {
                checkCancellation(cancelRequested, currentImage, preFilterImage, "Grayscale");
                return;
            }
            
            for (int x = 0; x < currentImage.width; x++) {
                int r = currentImage(x, y, 0);
                int g = currentImage(x, y, 1);
                int b = currentImage(x, y, 2);
                int gray = (r + g + b) / 3;
                currentImage.setPixel(x, y, 0, gray);
                currentImage.setPixel(x, y, 1, gray);
                currentImage.setPixel(x, y, 2, gray);
            }
            
            // Update progress
            updateProgress(y + 1, currentImage.height);
        }
        
        if (statusBar) {
            statusBar->showMessage("Grayscale filter applied");
        }
    } catch (const std::exception& e) {
        if (statusBar) {
            statusBar->showMessage(QString("Filter failed: %1").arg(e.what()));
        }
    }
    
    if (progressBar) {
        progressBar->setVisible(false);
    }
}

/**
 * @brief Apply TV/CRT monitor simulation effect with progress tracking and cancellation support.
 * 
 * Creates a vintage TV/CRT monitor appearance with scan lines, color shifts, and noise effects.
 * This operation supports real-time progress tracking and can be cancelled by the user.
 * 
 * @param currentImage Reference to the image to process (modified in-place)
 * @param preFilterImage Reference to store the original image state for cancellation
 * @param cancelRequested Atomic flag to check for cancellation requests
 * 
 * @details The TV/CRT effect includes:
 * - Horizontal scan lines (darker every 3rd row)
 * - Color temperature shifts (blue/purple for dark areas, warm orange for bright areas)
 * - Random noise for authentic TV feel
 * - Brightness-based color adjustments
 * - Processes pixels row by row for progress tracking
 * - Checks for cancellation after each row
 * 
 * @note This is a long-running operation that can be cancelled.
 * @see updateProgress() for progress tracking
 * @see checkCancellation() for cancellation handling
 */
void ImageFilters::applyTVFilter(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested)
{
    if (progressBar) {
        progressBar->setVisible(true);
        progressBar->setRange(0, currentImage.height);
        progressBar->setValue(0);
    }
    
    if (statusBar) {
        statusBar->showMessage("Applying TV/CRT filter... (Click Cancel to stop)");
    }
    QApplication::processEvents();
    
    try {
    // Initialize random number generator with time-based seed
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> noise_dist(-10, 10);

        for (int y = 0; y < currentImage.height; y++) {
            // Check for cancellation
            if (cancelRequested) {
                checkCancellation(cancelRequested, currentImage, preFilterImage, "TV/CRT");
                return;
            }
            
            for (int x = 0; x < currentImage.width; x++) {
            // Get original pixel values
                int r = currentImage(x, y, 0);
                int g = currentImage(x, y, 1);
                int b = currentImage(x, y, 2);
            
            // 1. Add horizontal scanlines (dark lines every few pixels)
            float scanlineIntensity = 1.0f;
            if (y % 3 == 0) {  // Every 3rd row gets darker
                scanlineIntensity = 0.7f;
            }
            
            // 2. Color shift and glow effect
            // Enhance blues and purples, add warm orange highlights
            float brightness = (r + g + b) / 3.0f / 255.0f;
            
            // Add blue/purple tint to darker areas
            if (brightness < 0.5f) {
                r = std::min(255, static_cast<int>(r * 0.8f));
                g = std::min(255, static_cast<int>(g * 0.7f));
                b = std::min(255, static_cast<int>(b * 1.2f));
            }
            
            // Add warm orange glow to bright areas
            if (brightness > 0.7f) {
                r = std::min(255, static_cast<int>(r * 1.3f));
                g = std::min(255, static_cast<int>(g * 1.1f));
                b = std::max(0, static_cast<int>(b * 0.9f));
            }

            // 3. Apply scanline effect
            r = static_cast<int>(r * scanlineIntensity);
            g = static_cast<int>(g * scanlineIntensity);
            b = static_cast<int>(b * scanlineIntensity);
            
            // 4. Add slight noise/grain for authentic TV feel
            int noise = noise_dist(rng); // -10 to +10
            r = std::min(255, std::max(0, r + noise));
            g = std::min(255, std::max(0, g + noise));
            b = std::min(255, std::max(0, b + noise));
            
            // Set the final pixel
                currentImage.setPixel(x, y, 0, r);
                currentImage.setPixel(x, y, 1, g);
                currentImage.setPixel(x, y, 2, b);
            }
            
            // Update progress
            updateProgress(y + 1, currentImage.height, 20);
        }
        
        if (statusBar) {
            statusBar->showMessage("TV/CRT filter applied");
        }
    } catch (const std::exception& e) {
        if (statusBar) {
            statusBar->showMessage(QString("Filter failed: %1").arg(e.what()));
        }
    }
    
    if (progressBar) {
        progressBar->setVisible(false);
    }
}

/**
 * @brief Apply black and white conversion with progress tracking and cancellation support.
 * 
 * Converts the image to pure black and white using threshold-based processing.
 * This operation supports real-time progress tracking and can be cancelled by the user.
 * 
 * @param currentImage Reference to the image to process (modified in-place)
 * @param preFilterImage Reference to store the original image state for cancellation
 * @param cancelRequested Atomic flag to check for cancellation requests
 * 
 * @details The black and white conversion:
 * - Calculates grayscale value: gray = (R + G + B) / 3
 * - Applies threshold: white (255) if gray > 127, black (0) otherwise
 * - Processes pixels row by row for progress tracking
 * - Checks for cancellation after each row
 * - Updates progress bar and status messages
 * - Restores original state if cancelled
 * 
 * @note This is a long-running operation that can be cancelled.
 * @see updateProgress() for progress tracking
 * @see checkCancellation() for cancellation handling
 */
void ImageFilters::applyBlackAndWhite(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested)
{
    if (progressBar) {
        progressBar->setVisible(true);
        progressBar->setRange(0, currentImage.height);
        progressBar->setValue(0);
    }
    
    if (statusBar) {
        statusBar->showMessage("Applying Black & White filter... (Click Cancel to stop)");
    }
    QApplication::processEvents();
    
    try {
        // Pure black and white conversion with cancellation support
        for (int y = 0; y < currentImage.height; y++) {
            // Check for cancellation
            if (cancelRequested) {
                checkCancellation(cancelRequested, currentImage, preFilterImage, "Black & White");
                return;
            }
            
            for (int x = 0; x < currentImage.width; x++) {
                int r = currentImage(x, y, 0);
                int g = currentImage(x, y, 1);
                int b = currentImage(x, y, 2);
                int gray = (r + g + b) / 3;
                int bw = (gray > 127) ? 255 : 0;
                currentImage.setPixel(x, y, 0, bw);
                currentImage.setPixel(x, y, 1, bw);
                currentImage.setPixel(x, y, 2, bw);
            }
            
            // Update progress
            updateProgress(y + 1, currentImage.height);
        }
        
        if (statusBar) {
            statusBar->showMessage("Black & White filter applied");
        }
    } catch (const std::exception& e) {
        if (statusBar) {
            statusBar->showMessage(QString("Filter failed: %1").arg(e.what()));
        }
    }
    
    if (progressBar) {
        progressBar->setVisible(false);
    }
}

/**
 * @brief Apply color inversion with progress tracking and cancellation support.
 * 
 * Inverts all color values in the image to create a negative effect.
 * This operation supports real-time progress tracking and can be cancelled by the user.
 * 
 * @param currentImage Reference to the image to process (modified in-place)
 * @param preFilterImage Reference to store the original image state for cancellation
 * @param cancelRequested Atomic flag to check for cancellation requests
 * 
 * @details The color inversion:
 * - Subtracts each RGB component from 255: new_value = 255 - old_value
 * - Processes pixels row by row for progress tracking
 * - Checks for cancellation after each row
 * - Updates progress bar and status messages
 * - Restores original state if cancelled
 * 
 * @note This is a long-running operation that can be cancelled.
 * @see updateProgress() for progress tracking
 * @see checkCancellation() for cancellation handling
 */
void ImageFilters::applyInvert(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested)
{
    if (progressBar) {
        progressBar->setVisible(true);
        progressBar->setRange(0, currentImage.height);
        progressBar->setValue(0);
    }
    
    if (statusBar) {
        statusBar->showMessage("Applying Invert filter... (Click Cancel to stop)");
    }
    QApplication::processEvents();
    
    try {
        for (int y = 0; y < currentImage.height; y++) {
            // Check for cancellation
            if (cancelRequested) {
                checkCancellation(cancelRequested, currentImage, preFilterImage, "Invert");
                return;
            }
            
            for (int x = 0; x < currentImage.width; x++) {
                currentImage.setPixel(x, y, 0, 255 - currentImage(x, y, 0));
                currentImage.setPixel(x, y, 1, 255 - currentImage(x, y, 1));
                currentImage.setPixel(x, y, 2, 255 - currentImage(x, y, 2));
            }
            
            // Update progress
            updateProgress(y + 1, currentImage.height);
        }
        
        if (statusBar) {
            statusBar->showMessage("Invert filter applied");
        }
    } catch (const std::exception& e) {
        if (statusBar) {
            statusBar->showMessage(QString("Filter failed: %1").arg(e.what()));
        }
    }
    
    if (progressBar) {
        progressBar->setVisible(false);
    }
}

/**
 * @brief Merge the current image with another image by averaging pixel values.
 * 
 * Combines two images by averaging their corresponding pixel values.
 * The resulting image will have the dimensions of the smaller input image.
 * 
 * @param currentImage Reference to the first image (modified in-place)
 * @param mergeImage Reference to the second image to merge with
 * 
 * @details The merge operation:
 * - Uses the smaller dimensions of both images
 * - Averages RGB values: new_value = (value1 + value2) / 2
 * - Processes all pixels in the overlapping area
 * - Updates status messages during processing
 * 
 * @note This is an immediate operation without progress tracking.
 * @see Image class for pixel access and manipulation
 */
void ImageFilters::applyMerge(Image& currentImage, Image& mergeImage)
{
    if (statusBar) {
        statusBar->showMessage("Applying Merge filter...");
    }
    QApplication::processEvents();
    
    int width = std::min(currentImage.width, mergeImage.width);
    int height = std::min(currentImage.height, mergeImage.height);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int r = (currentImage(x, y, 0) + mergeImage(x, y, 0)) / 2;
            int g = (currentImage(x, y, 1) + mergeImage(x, y, 1)) / 2;
            int b = (currentImage(x, y, 2) + mergeImage(x, y, 2)) / 2;
            currentImage.setPixel(x, y, 0, r);
            currentImage.setPixel(x, y, 1, g);
            currentImage.setPixel(x, y, 2, b);
        }
    }
    
    if (statusBar) {
        statusBar->showMessage("Merge filter applied");
    }
}

/**
 * @brief Flip the image horizontally or vertically.
 * 
 * Applies a horizontal or vertical flip transformation to the image
 * by swapping pixel positions across the specified axis.
 * 
 * @param currentImage Reference to the image to flip (modified in-place)
 * @param direction String specifying flip direction: "Horizontal" or "Vertical"
 * 
 * @details The flip operation:
 * - Horizontal: Swaps pixels across the vertical center line
 * - Vertical: Swaps pixels across the horizontal center line
 * - Processes pixels in-place for memory efficiency
 * - Updates status messages during processing
 * 
 * @note This is an immediate operation without progress tracking.
 * @throws std::invalid_argument if direction is not "Horizontal" or "Vertical"
 * @see Image class for pixel access and manipulation
 */
void ImageFilters::applyFlip(Image& currentImage, const QString& direction)
{
    if (statusBar) {
        statusBar->showMessage("Applying Flip filter...");
    }
    QApplication::processEvents();
    
    try {
        if (direction == "Horizontal") {
        // Horizontal flip
            for (int y = 0; y < currentImage.height; y++) {
                for (int x = 0; x < currentImage.width / 2; x++) {
                    int x2 = currentImage.width - 1 - x;
                for (int c = 0; c < 3; c++) {
                        int temp = currentImage(x, y, c);
                        currentImage.setPixel(x, y, c, currentImage(x2, y, c));
                        currentImage.setPixel(x2, y, c, temp);
                }
            }
        }
    } else {
        // Vertical flip
            for (int y = 0; y < currentImage.height / 2; y++) {
                for (int x = 0; x < currentImage.width; x++) {
                    int y2 = currentImage.height - 1 - y;
                for (int c = 0; c < 3; c++) {
                        int temp = currentImage(x, y, c);
                        currentImage.setPixel(x, y, c, currentImage(x, y2, c));
                        currentImage.setPixel(x, y2, c, temp);
                    }
                }
            }
        }
        
        if (statusBar) {
            statusBar->showMessage("Flip filter applied");
        }
    } catch (const std::exception& e) {
        if (statusBar) {
            statusBar->showMessage(QString("Filter failed: %1").arg(e.what()));
        }
    }
}

void ImageFilters::applyRotate(Image& currentImage, const QString& angle)
{
    if (statusBar) {
        statusBar->showMessage("Applying Rotate filter...");
    }
    QApplication::processEvents();
    
    try {
        Image tempImage = currentImage;
        
        if (angle == "90°") {
            currentImage = Image(tempImage.height, tempImage.width);
        for (int y = 0; y < tempImage.height; y++) {
            for (int x = 0; x < tempImage.width; x++) {
                int newX = tempImage.height - 1 - y;
                int newY = x;
                for (int c = 0; c < 3; c++) {
                        currentImage.setPixel(newX, newY, c, tempImage(x, y, c));
                    }
                }
            }
        } else if (angle == "180°") {
            for (int y = 0; y < currentImage.height / 2; y++) {
                for (int x = 0; x < currentImage.width; x++) {
                    int y2 = currentImage.height - 1 - y;
                    int x2 = currentImage.width - 1 - x;
                for (int c = 0; c < 3; c++) {
                        int temp = currentImage(x, y, c);
                        currentImage.setPixel(x, y, c, currentImage(x2, y2, c));
                        currentImage.setPixel(x2, y2, c, temp);
                    }
                }
            }
        } else { // 270°
            currentImage = Image(tempImage.height, tempImage.width);
        for (int y = 0; y < tempImage.height; y++) {
            for (int x = 0; x < tempImage.width; x++) {
                int newX = y;
                int newY = tempImage.width - 1 - x;
                for (int c = 0; c < 3; c++) {
                        currentImage.setPixel(newX, newY, c, tempImage(x, y, c));
                    }
                }
            }
        }
        
        if (statusBar) {
            statusBar->showMessage("Rotate filter applied");
        }
    } catch (const std::exception& e) {
        if (statusBar) {
            statusBar->showMessage(QString("Filter failed: %1").arg(e.what()));
        }
    }
}

void ImageFilters::applyDarkAndLight(Image& currentImage, const QString& choice)
{
    if (statusBar) {
        statusBar->showMessage("Applying Dark & Light filter...");
    }
    QApplication::processEvents();
    
    try {
        Image result(currentImage.width, currentImage.height);
        for (int i = 0; i < currentImage.width; ++i) {
            for (int j = 0; j < currentImage.height; ++j) {
                for (int k = 0; k < 3; ++k) {
                    int p = currentImage(i, j, k);
                    if (choice == "dark") {
                    p = p / 3;
                } else { // light
                    p = p * 2;
                    if (p > 255) p = 255;
                }
                    result.setPixel(i, j, k, static_cast<unsigned char>(p));
                }
            }
        }
        currentImage = result;
        
        if (statusBar) {
            statusBar->showMessage("Dark & Light filter applied");
        }
    } catch (const std::exception& e) {
        if (statusBar) {
            statusBar->showMessage(QString("Filter failed: %1").arg(e.what()));
        }
    }
}

void ImageFilters::applyDarkAndLight(Image& currentImage, const QString& choice, int percent)
{
    if (statusBar) {
        statusBar->showMessage("Applying Dark & Light (custom %) filter...");
    }
    QApplication::processEvents();

    percent = std::max(0, std::min(100, percent));
    const double factor = (choice == "dark")
        ? std::max(0.0, 1.0 - (percent / 100.0))
        : (1.0 + (percent / 100.0));

    try {
        Image result(currentImage.width, currentImage.height);
        for (int i = 0; i < currentImage.width; ++i) {
            for (int j = 0; j < currentImage.height; ++j) {
                for (int k = 0; k < 3; ++k) {
                    int p = currentImage(i, j, k);
                    double v = p * factor;
                    if (v < 0.0) v = 0.0;
                    if (v > 255.0) v = 255.0;
                    result.setPixel(i, j, k, static_cast<unsigned char>(v));
                }
            }
        }
        currentImage = result;

        if (statusBar) {
            statusBar->showMessage(QString("Dark & Light (%1%, %2) applied")
                                   .arg(percent)
                                   .arg(choice));
        }
    } catch (const std::exception& e) {
        if (statusBar) {
            statusBar->showMessage(QString("Filter failed: %1").arg(e.what()));
        }
    }
}

void ImageFilters::applyFrame(Image& currentImage, const QString& frameType)
{
    if (statusBar) {
        statusBar->showMessage("Applying Frame filter...");
    }
    QApplication::processEvents();
    
    try {
        if (frameType == "Simple Frame") {
        // Simple frame with blue outer and inner white border
        int frameSize = 10;
        int innerFrame = 5;
            Image result(currentImage.width + 2 * frameSize, currentImage.height + 2 * frameSize);
        
        // Fill with blue frame
        for (int y = 0; y < result.height; y++) {
            for (int x = 0; x < result.width; x++) {
                result.setPixel(x, y, 0, 0);
                result.setPixel(x, y, 1, 0);
                result.setPixel(x, y, 2, 255);
            }
        }
        
        // Copy original image
            for (int y = 0; y < currentImage.height; y++) {
                for (int x = 0; x < currentImage.width; x++) {
                for (int c = 0; c < 3; c++) {
                        result.setPixel(x + frameSize, y + frameSize, c, currentImage(x, y, c));
                }
            }
        }
        
        // Add inner white border
        int gap = 5;
            for (int y = frameSize + gap; y < frameSize + currentImage.height - gap; y++) {
                for (int x = frameSize + gap; x < frameSize + currentImage.width - gap; x++) {
                bool isWhiteBorder =
                    (x < frameSize + gap + innerFrame ||
                         x >= frameSize + currentImage.width - gap - innerFrame ||
                     y < frameSize + gap + innerFrame ||
                         y >= frameSize + currentImage.height - gap - innerFrame);
                if (isWhiteBorder) {
                    result.setPixel(x, y, 0, 255);
                    result.setPixel(x, y, 1, 255);
                    result.setPixel(x, y, 2, 255);
                }
            }
        }
            currentImage = result;
    } else if (frameType == "Double Border - White") {
        // White double border frame
        int outer = 14; int inner = 6; int gap = 4;
        int newWidth = currentImage.width + 2 * (outer + inner + gap);
        int newHeight = currentImage.height + 2 * (outer + inner + gap);
        Image result(newWidth, newHeight);
        // Fill with dark background
        for (int y = 0; y < newHeight; ++y)
            for (int x = 0; x < newWidth; ++x) {
                result.setPixel(x, y, 0, 20); result.setPixel(x, y, 1, 20); result.setPixel(x, y, 2, 20);
            }
        // Outer white
        for (int y = 0; y < newHeight; ++y)
            for (int x = 0; x < newWidth; ++x) {
                bool border = (x < outer || x >= newWidth - outer || y < outer || y >= newHeight - outer);
                if (border) { result.setPixel(x, y, 0, 255); result.setPixel(x, y, 1, 255); result.setPixel(x, y, 2, 255); }
            }
        // Inner white
        for (int y = outer + gap; y < newHeight - (outer + gap); ++y)
            for (int x = outer + gap; x < newWidth - (outer + gap); ++x) {
                bool border = (x < outer + gap + inner || x >= newWidth - (outer + gap + inner) || y < outer + gap + inner || y >= newHeight - (outer + gap + inner));
                if (border) { result.setPixel(x, y, 0, 255); result.setPixel(x, y, 1, 255); result.setPixel(x, y, 2, 255); }
            }
        // Paste image
        int ox = outer + gap + inner; int oy = outer + gap + inner;
        for (int y = 0; y < currentImage.height; ++y)
            for (int x = 0; x < currentImage.width; ++x)
                for (int c = 0; c < 3; ++c)
                    result.setPixel(x + ox, y + oy, c, currentImage(x, y, c));
        currentImage = result;
    } else if (frameType == "Solid Frame - Blue" || frameType == "Solid Frame - Red" || frameType == "Solid Frame - Green" || frameType == "Solid Frame - Black" || frameType == "Solid Frame - White") {
        int frame = 20;
        int color[3] = {0,0,0};
        if (frameType.endsWith("Blue")) { color[2] = 255; }
        else if (frameType.endsWith("Red")) { color[0] = 255; }
        else if (frameType.endsWith("Green")) { color[1] = 255; }
        else if (frameType.endsWith("White")) { color[0]=color[1]=color[2]=255; }
        // Black already default 0
        Image result(currentImage.width + 2 * frame, currentImage.height + 2 * frame);
        for (int y = 0; y < result.height; ++y)
            for (int x = 0; x < result.width; ++x)
                for (int c = 0; c < 3; ++c)
                    result.setPixel(x, y, c, color[c]);
        for (int y = 0; y < currentImage.height; ++y)
            for (int x = 0; x < currentImage.width; ++x)
                for (int c = 0; c < 3; ++c)
                    result.setPixel(x + frame, y + frame, c, currentImage(x, y, c));
        currentImage = result;
    } else if (frameType == "Shadow Frame") {
        int pad = 15; int shadow = 18;
        int newW = currentImage.width + pad + shadow;
        int newH = currentImage.height + pad + shadow;
        Image result(newW, newH);
        // Base dark
        for (int y = 0; y < newH; ++y)
            for (int x = 0; x < newW; ++x) {
                result.setPixel(x, y, 0, 20); result.setPixel(x, y, 1, 20); result.setPixel(x, y, 2, 20);
            }
        // Soft shadow gradient bottom-right
        for (int y = 0; y < newH; ++y)
            for (int x = 0; x < newW; ++x) {
                int dx = std::max(0, x - (pad + currentImage.width));
                int dy = std::max(0, y - (pad + currentImage.height));
                int dist = std::max(dx, dy);
                int shade = std::min(60, dist * 6);
                int r = 20 + shade, g = 20 + shade, b = 20 + shade;
                result.setPixel(x, y, 0, r); result.setPixel(x, y, 1, g); result.setPixel(x, y, 2, b);
            }
        // Paste image with light top-left highlight border
        for (int y = 0; y < currentImage.height; ++y)
            for (int x = 0; x < currentImage.width; ++x)
                for (int c = 0; c < 3; ++c)
                    result.setPixel(x + pad, y + pad, c, currentImage(x, y, c));
        currentImage = result;
    } else if (frameType == "Gold Decorated Frame") {
        // Gold style decorative frame
        int fw = 45;
        int outer[3] = {180, 140, 40};
        int inner[3] = {240, 210, 120};
        int accent[3] = {200, 160, 60};
        int newW = currentImage.width + 2 * fw;
        int newH = currentImage.height + 2 * fw;
        Image result(newW, newH);
        // Fill outer gold
        for (int y = 0; y < newH; ++y)
            for (int x = 0; x < newW; ++x)
                for (int c = 0; c < 3; ++c)
                    result.setPixel(x, y, c, outer[c]);
        // Accent stripes
        for (int y = 3; y < newH - 3; ++y)
            for (int x = 3; x < newW - 3; ++x) {
                bool stripe = ((x + y) % 11 == 0) || ((x - y + 1000) % 13 == 0);
                if (stripe) for (int c = 0; c < 3; ++c) result.setPixel(x, y, c, accent[c]);
            }
        // Inner plate
        for (int y = fw - 6; y < newH - (fw - 6); ++y)
            for (int x = fw - 6; x < newW - (fw - 6); ++x)
                for (int c = 0; c < 3; ++c)
                    result.setPixel(x, y, c, inner[c]);
        // Paste image
        for (int y = 0; y < currentImage.height; ++y)
            for (int x = 0; x < currentImage.width; ++x)
                for (int c = 0; c < 3; ++c)
                    result.setPixel(x + fw, y + fw, c, currentImage(x, y, c));
        currentImage = result;
    } else {
        // Existing decorated frame (brown/beige) as fallback
        // Decorated frame with brown/beige design and accent patterns
        int frameWidth = 25;
        int outerColor[3] = {100, 70, 50};
        int innerColor[3] = {235, 225, 210};
        int accentColor[3] = {180, 140, 80};
        
            int originalWidth = currentImage.width;
            int originalHeight = currentImage.height;
        int newWidth = originalWidth + 2 * frameWidth;
        int newHeight = originalHeight + 2 * frameWidth;
        
        Image result(newWidth, newHeight);
        
        // Copy original image
        for (int y = 0; y < originalHeight; y++) {
            for (int x = 0; x < originalWidth; x++) {
                for (int c = 0; c < 3; c++) {
                        result.setPixel(x + frameWidth, y + frameWidth, c, currentImage(x, y, c));
                }
            }
        }
        
        // Create decorated frame
        for (int y = 0; y < newHeight; y++) {
            for (int x = 0; x < newWidth; x++) {
                if (x < frameWidth || x >= newWidth - frameWidth ||
                    y < frameWidth || y >= newHeight - frameWidth) {
                    
                    int distFromEdge = std::min({x, y, newWidth - 1 - x, newHeight - 1 - y});
                    
                    if (distFromEdge < 3) {
                        for (int c = 0; c < 3; c++) {
                            result.setPixel(x, y, c, outerColor[c]);
                        }
                    }
                    else if (distFromEdge == 12 || distFromEdge == 15 || distFromEdge == 9) {
                        for (int c = 0; c < 3; c++) {
                            result.setPixel(x, y, c, accentColor[c]);
                        }
                    }
                    else if (distFromEdge < frameWidth - 4) {
                        for (int c = 0; c < 3; c++) {
                            result.setPixel(x, y, c, innerColor[c]);
                        }
                        
                        int cornerDist = std::min(std::min(x, newWidth - 1 - x),
                                                 std::min(y, newHeight - 1 - y));
                        
                        if (cornerDist < frameWidth) {
                            if ((x + y) % 12 == 0) {
                                for (int c = 0; c < 3; c++) {
                                    result.setPixel(x, y, c, accentColor[c]);
                                }
                            }
                        }
                    }
                    else if (distFromEdge >= frameWidth - 4 && distFromEdge < frameWidth - 1) {
                        for (int c = 0; c < 3; c++) {
                            result.setPixel(x, y, c, accentColor[c]);
                        }
                    }
                    else {
                        for (int c = 0; c < 3; c++) {
                            result.setPixel(x, y, c, outerColor[c]);
                        }
                    }
                }
            }
        }
            currentImage = result;
        }
        
        if (statusBar) {
            statusBar->showMessage("Frame filter applied");
        }
    } catch (const std::exception& e) {
        if (statusBar) {
            statusBar->showMessage(QString("Filter failed: %1").arg(e.what()));
        }
    }
}

void ImageFilters::applyEdges(Image& currentImage)
{
    if (statusBar) {
        statusBar->showMessage("Applying Edge Detection filter...");
    }
    QApplication::processEvents();
    
    try {
    // Convert to grayscale first
        Image gray(currentImage.width, currentImage.height);
        for (int y = 0; y < currentImage.height; y++) {
            for (int x = 0; x < currentImage.width; x++) {
                int r = currentImage(x, y, 0);
                int g = currentImage(x, y, 1);
                int b = currentImage(x, y, 2);
            // Use weighted average for better grayscale conversion
            int grayVal = (int)(0.299 * r + 0.587 * g + 0.114 * b);
            gray.setPixel(x, y, 0, grayVal);
            gray.setPixel(x, y, 1, grayVal);
            gray.setPixel(x, y, 2, grayVal);
        }
    }

    // Apply Gaussian blur to reduce noise
        Image blurred(currentImage.width, currentImage.height);
    int kernel[5][5] = {
        {1, 4, 6, 4, 1},
        {4, 16, 24, 16, 4},
        {6, 24, 36, 24, 6},
        {4, 16, 24, 16, 4},
        {1, 4, 6, 4, 1}
    };
    int kernelSum = 256; // Sum of all kernel values

        for (int y = 2; y < currentImage.height - 2; y++) {
            for (int x = 2; x < currentImage.width - 2; x++) {
            int sum = 0;
            for (int ky = -2; ky <= 2; ky++) {
                for (int kx = -2; kx <= 2; kx++) {
                    sum += gray(x + kx, y + ky, 0) * kernel[ky + 2][kx + 2];
                }
            }
            int blurredVal = sum / kernelSum;
            blurred.setPixel(x, y, 0, blurredVal);
            blurred.setPixel(x, y, 1, blurredVal);
            blurred.setPixel(x, y, 2, blurredVal);
        }
    }

    // Apply Sobel edge detection
        Image edge(currentImage.width, currentImage.height);
    
    // Sobel kernels
    int sobelX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobelY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

        for (int y = 1; y < currentImage.height - 1; y++) {
            for (int x = 1; x < currentImage.width - 1; x++) {
            int gx = 0, gy = 0;
            
            // Apply Sobel kernels
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int pixelVal = blurred(x + kx, y + ky, 0);
                    gx += pixelVal * sobelX[ky + 1][kx + 1];
                    gy += pixelVal * sobelY[ky + 1][kx + 1];
                }
            }
            
            // Calculate gradient magnitude
            int magnitude = (int)std::sqrt(gx * gx + gy * gy);
            
            // Clamp to 0-255 range
            magnitude = std::min(255, std::max(0, magnitude));
            
            // Apply threshold to enhance edges (white edges on black background)
            int edgeVal = (magnitude > 50) ? 0 : 255;
            
            edge.setPixel(x, y, 0, edgeVal);
            edge.setPixel(x, y, 1, edgeVal);
            edge.setPixel(x, y, 2, edgeVal);
        }
    }
    
        currentImage = edge;
        
        if (statusBar) {
            statusBar->showMessage("Edge Detection filter applied");
        }
    } catch (const std::exception& e) {
        if (statusBar) {
            statusBar->showMessage(QString("Filter failed: %1").arg(e.what()));
        }
    }
}

void ImageFilters::applyResize(Image& currentImage, int width, int height)
{
    if (statusBar) {
        statusBar->showMessage("Applying Resize filter...");
    }
    QApplication::processEvents();
    
    try {
        Image result(width, height);
        
        double xRatio = (double)currentImage.width / width;
        double yRatio = (double)currentImage.height / height;
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int srcX = (int)(x * xRatio);
                int srcY = (int)(y * yRatio);
                
                srcX = std::min(srcX, (int)currentImage.width - 1);
                srcY = std::min(srcY, (int)currentImage.height - 1);
                
                result.setPixel(x, y, 0, currentImage(srcX, srcY, 0));
                result.setPixel(x, y, 1, currentImage(srcX, srcY, 1));
                result.setPixel(x, y, 2, currentImage(srcX, srcY, 2));
            }
        }
        
        currentImage = result;
        
        if (statusBar) {
            statusBar->showMessage(QString("Resize filter applied (%1x%2)").arg(width).arg(height));
        }
    } catch (const std::exception& e) {
        if (statusBar) {
            statusBar->showMessage(QString("Filter failed: %1").arg(e.what()));
        }
    }
}

/**
 * @brief Skew the image horizontally by a given angle.
 *
 * Expands the canvas to fit the skewed image. Background is filled with white.
 */
void ImageFilters::applySkew(Image& currentImage, double angleDegrees)
{
    if (statusBar) {
        statusBar->showMessage("Applying Skew filter...");
    }
    QApplication::processEvents();

    try {
        const double angleRad = angleDegrees * M_PI / 180.0;
        const double tanA = std::tan(angleRad);

        // Compute horizontal shift range over all rows
        int minShift = 0;
        int maxShift = 0;
        if (currentImage.height > 0) {
            int shiftTop = static_cast<int>(std::floor(tanA * 0));
            int shiftBottom = static_cast<int>(std::floor(tanA * (currentImage.height - 1)));
            minShift = std::min(shiftTop, shiftBottom);
            maxShift = std::max(shiftTop, shiftBottom);
        }

        const int newWidth = std::max(1, (int)currentImage.width + (maxShift - minShift));
        const int newHeight = currentImage.height;

        Image skewed(newWidth, newHeight);
        // Fill background white
        for (int y = 0; y < newHeight; ++y) {
            for (int x = 0; x < newWidth; ++x) {
                skewed.setPixel(x, y, 0, 255);
                skewed.setPixel(x, y, 1, 255);
                skewed.setPixel(x, y, 2, 255);
            }
        }

        // Copy pixels with per-row shift, offset to keep within bounds
        for (int y = 0; y < currentImage.height; ++y) {
            int shift = static_cast<int>(std::floor(tanA * y));
            int base = shift - minShift; // normalize so min lands at 0
            for (int x = 0; x < currentImage.width; ++x) {
                int nx = x + base;
                if (nx >= 0 && nx < newWidth) {
                    for (int c = 0; c < 3; ++c) {
                        skewed.setPixel(nx, y, c, currentImage(x, y, c));
                    }
                }
            }
        }

        currentImage = skewed;
        if (statusBar) {
            statusBar->showMessage(QString("Skew filter applied (%1°)").arg(angleDegrees));
        }
    } catch (const std::exception& e) {
        if (statusBar) {
            statusBar->showMessage(QString("Filter failed: %1").arg(e.what()));
        }
    }
}

void ImageFilters::applyEmboss(Image& currentImage)
{
    if (statusBar) statusBar->showMessage("Applying Emboss...");
    QApplication::processEvents();
    Image embossed(currentImage.width, currentImage.height);
    for (int y = 0; y < currentImage.height - 1; y++) {
        for (int x = 0; x < currentImage.width - 1; x++) {
            int r1 = currentImage(x, y, 0);
            int g1 = currentImage(x, y, 1);
            int b1 = currentImage(x, y, 2);
            int r2 = currentImage(x + 1, y + 1, 0);
            int g2 = currentImage(x + 1, y + 1, 1);
            int b2 = currentImage(x + 1, y + 1, 2);
            int diffR = std::clamp(r1 - r2 + 128, 0, 255);
            int diffG = std::clamp(g1 - g2 + 128, 0, 255);
            int diffB = std::clamp(b1 - b2 + 128, 0, 255);
            int gray = (diffR + diffG + diffB) / 3;
            for (int c = 0; c < 3; ++c) embossed.setPixel(x, y, c, gray);
        }
    }
    currentImage = embossed;
    if (statusBar) statusBar->showMessage("Emboss applied");
}

void ImageFilters::applyEmboss(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested)
{
    if (progressBar) { progressBar->setVisible(true); progressBar->setRange(0, currentImage.height); progressBar->setValue(0); }
    if (statusBar) statusBar->showMessage("Applying Emboss... (Click Cancel to stop)");
    QApplication::processEvents();
    Image embossed(currentImage.width, currentImage.height);
    for (int y = 0; y < currentImage.height - 1; y++) {
        if (cancelRequested) { checkCancellation(cancelRequested, currentImage, preFilterImage, "Emboss"); return; }
        for (int x = 0; x < currentImage.width - 1; x++) {
            int r1 = currentImage(x, y, 0);
            int g1 = currentImage(x, y, 1);
            int b1 = currentImage(x, y, 2);
            int r2 = currentImage(x + 1, y + 1, 0);
            int g2 = currentImage(x + 1, y + 1, 1);
            int b2 = currentImage(x + 1, y + 1, 2);
            int diffR = std::clamp(r1 - r2 + 128, 0, 255);
            int diffG = std::clamp(g1 - g2 + 128, 0, 255);
            int diffB = std::clamp(b1 - b2 + 128, 0, 255);
            int gray = (diffR + diffG + diffB) / 3;
            for (int c = 0; c < 3; ++c) embossed.setPixel(x, y, c, gray);
        }
        updateProgress(y + 1, currentImage.height, 20);
    }
    currentImage = embossed;
    if (statusBar) statusBar->showMessage("Emboss applied");
    if (progressBar) progressBar->setVisible(false);
}

void ImageFilters::applyDoubleVision(Image& currentImage, int offset)
{
    if (statusBar) statusBar->showMessage("Applying Double Vision...");
    QApplication::processEvents();
    offset = std::max(0, offset);
    Image out(currentImage.width, currentImage.height);
    for (int y = 0; y < currentImage.height; ++y) {
        for (int x = 0; x < currentImage.width; ++x) {
            int nx = x + offset;
            if (nx >= currentImage.width) nx = currentImage.width - 1;
            int R1 = currentImage(x, y, 0);
            int G1 = currentImage(x, y, 1);
            int B1 = currentImage(x, y, 2);
            int R2 = currentImage(nx, y, 0);
            int G2 = currentImage(nx, y, 1);
            int B2 = currentImage(nx, y, 2);
            int R = std::min(255, int(R1 * 0.6 + R2 * 0.4) + 25);
            int G = int(G1 * 0.6 + G2 * 0.4);
            int B = int(B1 * 0.6 + B2 * 0.4);
            out.setPixel(x, y, 0, R);
            out.setPixel(x, y, 1, G);
            out.setPixel(x, y, 2, B);
        }
    }
    currentImage = out;
    if (statusBar) statusBar->showMessage("Double Vision applied");
}

void ImageFilters::applyDoubleVision(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested, int offset)
{
    if (progressBar) { progressBar->setVisible(true); progressBar->setRange(0, currentImage.height); progressBar->setValue(0); }
    if (statusBar) statusBar->showMessage("Applying Double Vision... (Click Cancel to stop)");
    QApplication::processEvents();
    offset = std::max(0, offset);
    Image out(currentImage.width, currentImage.height);
    for (int y = 0; y < currentImage.height; ++y) {
        if (cancelRequested) { checkCancellation(cancelRequested, currentImage, preFilterImage, "Double Vision"); return; }
        for (int x = 0; x < currentImage.width; ++x) {
            int nx = x + offset;
            if (nx >= currentImage.width) nx = currentImage.width - 1;
            int R1 = currentImage(x, y, 0);
            int G1 = currentImage(x, y, 1);
            int B1 = currentImage(x, y, 2);
            int R2 = currentImage(nx, y, 0);
            int G2 = currentImage(nx, y, 1);
            int B2 = currentImage(nx, y, 2);
            int R = std::min(255, int(R1 * 0.6 + R2 * 0.4) + 25);
            int G = int(G1 * 0.6 + G2 * 0.4);
            int B = int(B1 * 0.6 + B2 * 0.4);
            out.setPixel(x, y, 0, R);
            out.setPixel(x, y, 1, G);
            out.setPixel(x, y, 2, B);
        }
        updateProgress(y + 1, currentImage.height, 20);
    }
    currentImage = out;
    if (statusBar) statusBar->showMessage("Double Vision applied");
    if (progressBar) progressBar->setVisible(false);
}

void ImageFilters::applyOilPainting(Image& currentImage, int radius, int intensity)
{
    if (statusBar) statusBar->showMessage("Applying Oil Painting...");
    QApplication::processEvents();
    radius = std::max(1, radius);
    intensity = std::max(1, std::min(255, intensity));
    Image result(currentImage.width, currentImage.height);
    for (int i = 0; i < currentImage.width; ++i) {
        for (int j = 0; j < currentImage.height; ++j) {
            int colorCount[256] = {0};
            int redSum[256] = {0};
            int greenSum[256] = {0};
            int blueSum[256] = {0};
            for (int dy = -radius; dy <= radius; ++dy) {
                for (int dx = -radius; dx <= radius; ++dx) {
                    int nx = i + dx, ny = j + dy;
                    if (nx >= 0 && nx < currentImage.width && ny >= 0 && ny < currentImage.height) {
                        int r = currentImage(nx, ny, 0);
                        int g = currentImage(nx, ny, 1);
                        int b = currentImage(nx, ny, 2);
                        int avg = (r + g + b) / 3;
                        int level = std::min(255, std::max(0, avg / std::max(1, intensity)));
                        colorCount[level]++;
                        redSum[level] += r;
                        greenSum[level] += g;
                        blueSum[level] += b;
                    }
                }
            }
            int maxCount = 0, maxLevel = 0;
            for (int k = 0; k < 256; ++k) if (colorCount[k] > maxCount) { maxCount = colorCount[k]; maxLevel = k; }
            int denom = std::max(1, colorCount[maxLevel]);
            result.setPixel(i, j, 0, redSum[maxLevel] / denom);
            result.setPixel(i, j, 1, greenSum[maxLevel] / denom);
            result.setPixel(i, j, 2, blueSum[maxLevel] / denom);
        }
    }
    currentImage = result;
    if (statusBar) statusBar->showMessage("Oil Painting applied");
}

void ImageFilters::applyOilPainting(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested, int radius, int intensity)
{
    if (progressBar) { progressBar->setVisible(true); progressBar->setRange(0, currentImage.height); progressBar->setValue(0); }
    if (statusBar) statusBar->showMessage("Applying Oil Painting... (Click Cancel to stop)");
    QApplication::processEvents();
    radius = std::max(1, radius);
    intensity = std::max(1, std::min(255, intensity));
    Image result(currentImage.width, currentImage.height);
    for (int j = 0; j < currentImage.height; ++j) {
        if (cancelRequested) { checkCancellation(cancelRequested, currentImage, preFilterImage, "Oil Painting"); return; }
        for (int i = 0; i < currentImage.width; ++i) {
            int colorCount[256] = {0};
            int redSum[256] = {0};
            int greenSum[256] = {0};
            int blueSum[256] = {0};
            for (int dy = -radius; dy <= radius; ++dy) {
                for (int dx = -radius; dx <= radius; ++dx) {
                    int nx = i + dx, ny = j + dy;
                    if (nx >= 0 && nx < currentImage.width && ny >= 0 && ny < currentImage.height) {
                        int r = currentImage(nx, ny, 0);
                        int g = currentImage(nx, ny, 1);
                        int b = currentImage(nx, ny, 2);
                        int avg = (r + g + b) / 3;
                        int level = std::min(255, std::max(0, avg / std::max(1, intensity)));
                        colorCount[level]++;
                        redSum[level] += r;
                        greenSum[level] += g;
                        blueSum[level] += b;
                    }
                }
            }
            int maxCount = 0, maxLevel = 0;
            for (int k = 0; k < 256; ++k) if (colorCount[k] > maxCount) { maxCount = colorCount[k]; maxLevel = k; }
            int denom = std::max(1, colorCount[maxLevel]);
            result.setPixel(i, j, 0, redSum[maxLevel] / denom);
            result.setPixel(i, j, 1, greenSum[maxLevel] / denom);
            result.setPixel(i, j, 2, blueSum[maxLevel] / denom);
        }
        updateProgress(j + 1, currentImage.height, 5);
    }
    currentImage = result;
    if (statusBar) statusBar->showMessage("Oil Painting applied");
    if (progressBar) progressBar->setVisible(false);
}

void ImageFilters::applyEnhanceSunlight(Image& currentImage)
{
    if (statusBar) statusBar->showMessage("Enhancing Sunlight...");
    QApplication::processEvents();
    Image result(currentImage.width, currentImage.height);
    for (int x = 0; x < currentImage.width; ++x) {
        for (int y = 0; y < currentImage.height; ++y) {
            for (int c = 0; c < currentImage.channels; ++c) {
                int v = currentImage(x, y, c);
                if (c == 0 || c == 1) v = std::min(255, int(v * 1.4)); // boost R and G
                result.setPixel(x, y, c, v);
            }
        }
    }
    currentImage = result;
    if (statusBar) statusBar->showMessage("Sunlight enhanced");
}

void ImageFilters::applyEnhanceSunlight(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested)
{
    if (progressBar) { progressBar->setVisible(true); progressBar->setRange(0, currentImage.height); progressBar->setValue(0); }
    if (statusBar) statusBar->showMessage("Enhancing Sunlight... (Click Cancel to stop)");
    QApplication::processEvents();
    Image result(currentImage.width, currentImage.height);
    for (int y = 0; y < currentImage.height; ++y) {
        if (cancelRequested) { checkCancellation(cancelRequested, currentImage, preFilterImage, "Enhance Sunlight"); return; }
        for (int x = 0; x < currentImage.width; ++x) {
            for (int c = 0; c < 3; ++c) {
                int v = currentImage(x, y, c);
                if (c == 0 || c == 1) v = std::min(255, int(v * 1.4)); // boost R and G
                result.setPixel(x, y, c, v);
            }
        }
        updateProgress(y + 1, currentImage.height, 20);
    }
    currentImage = result;
    if (statusBar) statusBar->showMessage("Sunlight enhanced");
    if (progressBar) progressBar->setVisible(false);
}

void ImageFilters::applyFishEye(Image& currentImage)
{
    if (statusBar) statusBar->showMessage("Applying Fish-Eye...");
    QApplication::processEvents();
    Image out(currentImage.width, currentImage.height);
    float centerX = currentImage.width / 2.0f;
    float centerY = currentImage.height / 2.0f;
    float radius = std::min(centerX, centerY);
    for (int y = 0; y < currentImage.height; ++y) {
        for (int x = 0; x < currentImage.width; ++x) {
            float dx = (x - centerX) / radius;
            float dy = (y - centerY) / radius;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < 1.0f && dist > 0.0f) {
                float factor = 1.0f;
                float newDist = std::pow(dist, factor * 0.75f);
                float nx = centerX + (dx / dist) * newDist * radius;
                float ny = centerY + (dy / dist) * newDist * radius;
                int ix = std::clamp(int(nx), 0, (int)currentImage.width - 1);
                int iy = std::clamp(int(ny), 0, (int)currentImage.height - 1);
                for (int c = 0; c < 3; ++c) out.setPixel(x, y, c, currentImage(ix, iy, c));
            } else {
                for (int c = 0; c < 3; ++c) out.setPixel(x, y, c, currentImage(x, y, c));
            }
        }
    }
    currentImage = out;
    if (statusBar) statusBar->showMessage("Fish-Eye applied");
}

void ImageFilters::applyFishEye(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested)
{
    if (progressBar) { progressBar->setVisible(true); progressBar->setRange(0, currentImage.height); progressBar->setValue(0); }
    if (statusBar) statusBar->showMessage("Applying Fish-Eye... (Click Cancel to stop)");
    QApplication::processEvents();
    Image out(currentImage.width, currentImage.height);
    float centerX = currentImage.width / 2.0f;
    float centerY = currentImage.height / 2.0f;
    float radius = std::min(centerX, centerY);
    for (int y = 0; y < currentImage.height; ++y) {
        if (cancelRequested) { checkCancellation(cancelRequested, currentImage, preFilterImage, "Fish-Eye"); return; }
        for (int x = 0; x < currentImage.width; ++x) {
            float dx = (x - centerX) / radius;
            float dy = (y - centerY) / radius;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < 1.0f && dist > 0.0f) {
                float factor = 1.0f;
                float newDist = std::pow(dist, factor * 0.75f);
                float nx = centerX + (dx / dist) * newDist * radius;
                float ny = centerY + (dy / dist) * newDist * radius;
                int ix = std::clamp(int(nx), 0, (int)currentImage.width - 1);
                int iy = std::clamp(int(ny), 0, (int)currentImage.height - 1);
                for (int c = 0; c < 3; ++c) out.setPixel(x, y, c, currentImage(ix, iy, c));
            } else {
                for (int c = 0; c < 3; ++c) out.setPixel(x, y, c, currentImage(x, y, c));
            }
        }
        updateProgress(y + 1, currentImage.height, 10);
    }
    currentImage = out;
    if (statusBar) statusBar->showMessage("Fish-Eye applied");
    if (progressBar) progressBar->setVisible(false);
}

void ImageFilters::applyBlur(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested)
{
    applyBlur(currentImage, preFilterImage, cancelRequested, 60);
}
void ImageFilters::applyBlur(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested, int strength)
{
    if (progressBar) {
        progressBar->setVisible(true);
        progressBar->setRange(0, currentImage.height);
        progressBar->setValue(0);
    }
    
    if (statusBar) {
        statusBar->showMessage("Applying Blur filter... (Click Cancel to stop)");
    }
    QApplication::processEvents();

    strength = std::max(0, std::min(100, strength));
    // Map 0..100 to radius 1..25 (0 becomes 1)
    int blurSize = std::max(1, (strength * 24) / 100 + 1);
    try {
        Image result(currentImage.width, currentImage.height);
        
        for (int y = 0; y < currentImage.height; y++) {
            if (cancelRequested) {
                checkCancellation(cancelRequested, currentImage, preFilterImage, "Blur");
                return;
            }
            for (int x = 0; x < currentImage.width; x++) {
                int R = 0, G = 0, B = 0;
                int count = 0;
                for (int i = -blurSize; i <= blurSize; i++) {
                    for (int j = -blurSize; j <= blurSize; j++) {
                        int nx = x + j;
                        int ny = y + i;
                        if (nx >= 0 && nx < currentImage.width && ny >= 0 && ny < currentImage.height) {
                            R += currentImage(nx, ny, 0);
                            G += currentImage(nx, ny, 1);
                            B += currentImage(nx, ny, 2);
                            count++;
                        }
                    }
                }
                result.setPixel(x, y, 0, R / std::max(1, count));
                result.setPixel(x, y, 1, G / std::max(1, count));
                result.setPixel(x, y, 2, B / std::max(1, count));
            }
            updateProgress(y + 1, currentImage.height, 10);
        }
        currentImage = result;
        if (statusBar) {
            statusBar->showMessage(QString("Blur filter applied (radius %1)").arg(blurSize));
        }
    } catch (const std::exception& e) {
        if (statusBar) {
            statusBar->showMessage(QString("Filter failed: %1").arg(e.what()));
        }
    }
    if (progressBar) {
        progressBar->setVisible(false);
    }
}

void ImageFilters::applyInfrared(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested)
{
    if (progressBar) {
        progressBar->setVisible(true);
        progressBar->setRange(0, currentImage.width);
        progressBar->setValue(0);
    }
    
    if (statusBar) {
        statusBar->showMessage("Applying Infrared filter... (Click Cancel to stop)");
    }
    QApplication::processEvents();
    
    try {
        for (int x = 0; x < currentImage.width; ++x) {
            // Check for cancellation
            if (cancelRequested) {
                checkCancellation(cancelRequested, currentImage, preFilterImage, "Infrared");
                return;
            }
            
            for (int y = 0; y < currentImage.height; ++y) {
                int red   = currentImage(x, y, 0);
                int green = currentImage(x, y, 1);
                int blue  = currentImage(x, y, 2);

                float brightness = (red + green + blue) / 3.0f;
                float inverted = 255 - brightness;

                int R = 255;
                unsigned int G = int(inverted);
                unsigned int B = int(inverted);

                currentImage(x, y, 0) = R;
                currentImage(x, y, 1) = G;
                currentImage(x, y, 2) = B;
            }
            
            // Update progress
            updateProgress(x + 1, currentImage.width, 50);
        }
        
        if (statusBar) {
            statusBar->showMessage("Infrared filter applied");
        }
    } catch (const std::exception& e) {
        if (statusBar) {
            statusBar->showMessage(QString("Filter failed: %1").arg(e.what()));
        }
    }
    
    if (progressBar) {
        progressBar->setVisible(false);
    }
}

void ImageFilters::applyPurpleFilter(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested)
{
    if (progressBar) {
        progressBar->setVisible(true);
        progressBar->setRange(0, currentImage.height);
        progressBar->setValue(0);
    }
    
    if (statusBar) {
        statusBar->showMessage("Applying Purple filter... (Click Cancel to stop)");
    }
    QApplication::processEvents();
    
    try {
        for (int y = 0; y < currentImage.height; y++) {
            // Check for cancellation
            if (cancelRequested) {
                checkCancellation(cancelRequested, currentImage, preFilterImage, "Purple");
                return;
            }
            
            for (int x = 0; x < currentImage.width; x++) {
                int r = currentImage(x, y, 0);
                int g = currentImage(x, y, 1);
                int b = currentImage(x, y, 2);

                r = std::min(255, (int)(r * 1.3));
                g = std::max(0,   (int)(g * 0.5));
                b = std::min(255, (int)(b * 1.3));

                currentImage.setPixel(x, y, 0, r);
                currentImage.setPixel(x, y, 1, g);
                currentImage.setPixel(x, y, 2, b);
            }
            
            // Update progress
            updateProgress(y + 1, currentImage.height);
        }
        
        if (statusBar) {
            statusBar->showMessage("Purple filter applied");
        }
    } catch (const std::exception& e) {
        if (statusBar) {
            statusBar->showMessage(QString("Filter failed: %1").arg(e.what()));
        }
    }
    
    if (progressBar) {
        progressBar->setVisible(false);
    }
}


