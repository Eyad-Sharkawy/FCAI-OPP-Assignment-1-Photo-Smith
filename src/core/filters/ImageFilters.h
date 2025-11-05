/**
 * @file ImageFilters.h
 * @brief Image processing filters with Qt integration, progress tracking, and cancellation support.
 * 
 * This file contains the declaration of the ImageFilters class, which provides a comprehensive
 * set of image processing operations including basic filters (grayscale, invert), geometric
 * transformations (flip, rotate, resize), advanced effects (blur, edge detection, infrared),
 * and special filters (TV/CRT, purple tint). All filters support progress tracking and
 * cancellation for long-running operations.
 * 
 * @details The ImageFilters class is designed to work seamlessly with Qt's GUI framework,
 * providing real-time progress updates and the ability to cancel operations mid-execution.
 * It uses atomic operations for thread-safe cancellation and integrates with Qt's progress
 * bar and status bar components.
 * 
 * @features
 * - Progress tracking for all long-running operations
 * - Cancellation support using atomic flags
 * - Qt integration with progress bars and status updates
 * - Comprehensive error handling and exception safety
 * - Support for various image formats through the Image class
 * - Memory-efficient processing with in-place operations where possible
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

#ifndef IMAGEFILTERS_H
#define IMAGEFILTERS_H

// Forward declaration to avoid including the full Image_Class.h implementation
class Image;
#undef pixel  // Undefine the pixel macro to avoid conflicts with Qt
class QProgressBar; // forward declaration to avoid heavy Qt includes in header
class QStatusBar;   // forward declaration
class QString;      // forward declaration
#include <atomic>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>

/**
 * @class ImageFilters
 * @brief Comprehensive image processing class with Qt integration and cancellation support.
 * 
 * The ImageFilters class provides a complete suite of image processing operations designed
 * to work seamlessly with Qt applications. It supports progress tracking, cancellation,
 * and real-time status updates for all operations.
 * 
 * @details This class implements various image processing algorithms including:
 * - Basic color operations (grayscale, invert, black & white)
 * - Geometric transformations (flip, rotate, resize, crop)
 * - Advanced effects (blur, edge detection, infrared simulation)
 * - Special effects (TV/CRT simulation, purple tint, frame addition)
 * - Image combination (merge operations)
 * 
 * All long-running operations support cancellation and progress tracking through Qt's
 * progress bar and status bar components.
 * 
 * @note This class is designed to work with the Image class and requires Qt 6.8.1 or later.
 * @see Image class for image data structure and basic operations
 * @see QProgressBar for progress tracking
 * @see QStatusBar for status updates
 */
class ImageFilters
{
public:
    /**
     * @brief Constructs an ImageFilters object with Qt UI components.
     * 
     * @param progressBar Pointer to QProgressBar for progress tracking (can be nullptr)
     * @param statusBar Pointer to QStatusBar for status updates (can be nullptr)
     * 
     * @note Both parameters are optional. If nullptr, progress and status updates will be skipped.
     */
    ImageFilters(QProgressBar* progressBar, QStatusBar* statusBar);
    
    // ============================================================================
    // BASIC COLOR FILTERS (with progress tracking and cancellation)
    // ============================================================================
    
    /**
     * @brief Converts the image to grayscale.
     * 
     * Applies a simple grayscale conversion by averaging RGB values for each pixel.
     * This operation supports progress tracking and cancellation.
     * 
     * @param currentImage Reference to the image to process (modified in-place)
     * @param preFilterImage Reference to store the original image state for cancellation
     * @param cancelRequested Atomic flag to check for cancellation requests
     * 
     * @note This is a long-running operation that can be cancelled.
     * @see updateProgress() for progress tracking implementation
     * @see checkCancellation() for cancellation handling
     */
    void applyGrayscale(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested);
    
    /**
     * @brief Applies a TV/CRT monitor simulation effect.
     * 
     * Creates a vintage TV/CRT monitor appearance with scan lines, color shifts,
     * and noise effects. This operation supports progress tracking and cancellation.
     * 
     * @param currentImage Reference to the image to process (modified in-place)
     * @param preFilterImage Reference to store the original image state for cancellation
     * @param cancelRequested Atomic flag to check for cancellation requests
     * 
     * @details The effect includes:
     * - Horizontal scan lines (darker every 3rd row)
     * - Color temperature shifts (blue/purple for dark areas, warm orange for bright areas)
     * - Random noise for authentic TV feel
     * - Brightness-based color adjustments
     * 
     * @note This is a long-running operation that can be cancelled.
     */
    void applyTVFilter(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested);
    
    /**
     * @brief Converts the image to pure black and white (binary).
     * 
     * Applies a threshold-based conversion where pixels above 127 become white (255)
     * and pixels below become black (0). This operation supports progress tracking and cancellation.
     * 
     * @param currentImage Reference to the image to process (modified in-place)
     * @param preFilterImage Reference to store the original image state for cancellation
     * @param cancelRequested Atomic flag to check for cancellation requests
     * 
     * @note This is a long-running operation that can be cancelled.
     */
    void applyBlackAndWhite(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested);
    
    /**
     * @brief Inverts all color values in the image.
     * 
     * Subtracts each RGB component from 255 to create a negative effect.
     * This operation supports progress tracking and cancellation.
     * 
     * @param currentImage Reference to the image to process (modified in-place)
     * @param preFilterImage Reference to store the original image state for cancellation
     * @param cancelRequested Atomic flag to check for cancellation requests
     * 
     * @note This is a long-running operation that can be cancelled.
     */
    void applyInvert(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested);
    
    // ============================================================================
    // GEOMETRIC TRANSFORMATIONS (immediate operations)
    // ============================================================================
    
    /**
     * @brief Merges the current image with another image.
     * 
     * Combines two images by averaging their pixel values. The resulting image
     * will have the dimensions of the smaller input image.
     * 
     * @param currentImage Reference to the first image (modified in-place)
     * @param mergeImage Reference to the second image to merge with
     * 
     * @note This is an immediate operation without progress tracking.
     */
    void applyMerge(Image& currentImage, Image& mergeImage);
    
    /**
     * @brief Flips the image horizontally or vertically.
     * 
     * @param currentImage Reference to the image to flip (modified in-place)
     * @param direction String specifying flip direction: "Horizontal" or "Vertical"
     * 
     * @note This is an immediate operation without progress tracking.
     * @throws std::invalid_argument if direction is not "Horizontal" or "Vertical"
     */
    void applyFlip(Image& currentImage, const QString& direction);
    
    /**
     * @brief Rotates the image by the specified angle.
     * 
     * @param currentImage Reference to the image to rotate (modified in-place)
     * @param angleDegrees Rotation angle in degrees (0-360)
     * 
     * @note This is an immediate operation without progress tracking.
     * @note For best performance with 90°, 180°, and 270°, special optimized paths are used.
     */
    void applyRotate(Image& currentImage, int angleDegrees);
    
    /**
     * @brief Adjusts image brightness (darken or lighten).
     * 
     * @param currentImage Reference to the image to adjust (modified in-place)
     * @param choice String specifying adjustment: "dark" or "light"
     * 
     * @details:
     * - "dark": Divides each pixel value by 3 (makes image darker)
     * - "light": Multiplies each pixel value by 2, clamped to 255 (makes image brighter)
     * 
     * @note This is an immediate operation without progress tracking.
     * @throws std::invalid_argument if choice is not "dark" or "light"
     */
    void applyDarkAndLight(Image& currentImage, const QString& choice);
    /**
     * @brief Adjusts image brightness by a given percentage.
     *
     * @param currentImage Reference to the image to adjust (modified in-place)
     * @param choice "dark" to darken, "light" to lighten
     * @param percent Percentage in [0, 100]; 0 = no change, 100 = full effect
     */
    void applyDarkAndLight(Image& currentImage, const QString& choice, int percent);
    
    /**
     * @brief Adds a decorative frame around the image.
     * 
     * @param currentImage Reference to the image to frame (modified in-place)
     * @param frameType String specifying frame style: "Simple Frame" or "Decorated Frame"
     * 
     * @details:
     * - "Simple Frame": Blue outer border with white inner border
     * - "Decorated Frame": Brown/beige decorative frame with accent patterns
     * 
     * @note This is an immediate operation without progress tracking.
     * @throws std::invalid_argument if frameType is not "Simple Frame" or "Decorated Frame"
     */
    void applyFrame(Image& currentImage, const QString& frameType);
    
    /**
     * @brief Adds a custom colored frame around the image.
     * 
     * @param currentImage Reference to the image to frame (modified in-place)
     * @param frameWidth Width of the frame in pixels
     * @param r Red component of the frame color (0-255)
     * @param g Green component of the frame color (0-255)
     * @param b Blue component of the frame color (0-255)
     * 
     * @note This is an immediate operation without progress tracking.
     */
    void applyFrame(Image& currentImage, int frameWidth, int r, int g, int b);
    
    /**
     * @brief Applies a frame with custom type and color.
     * 
     * @param currentImage Reference to the image to frame (modified in-place)
     * @param frameType Frame style type (Solid Frame, Simple Frame, Double Border, Shadow Frame, Gold Decorated Frame, Decorated Frame)
     * @param frameWidth Width of the frame in pixels
     * @param r Red component of the frame color (0-255)
     * @param g Green component of the frame color (0-255)
     * @param b Blue component of the frame color (0-255)
     * 
     * @note This is an immediate operation without progress tracking.
     */
    void applyFrame(Image& currentImage, const QString& frameType, int frameWidth, int r, int g, int b);
    
    /**
     * @brief Detects and highlights edges in the image.
     * 
     * Applies Sobel edge detection algorithm with Gaussian blur preprocessing.
     * The result shows white edges on a black background.
     * 
     * @param currentImage Reference to the image to process (modified in-place)
     * 
     * @details The algorithm:
     * 1. Converts to grayscale using weighted average (0.299R + 0.587G + 0.114B)
     * 2. Applies 5x5 Gaussian blur to reduce noise
     * 3. Uses 3x3 Sobel kernels for edge detection
     * 4. Calculates gradient magnitude and applies threshold
     * 
     * @note This is an immediate operation without progress tracking.
     */
    void applyEdges(Image& currentImage);
    
    /**
     * @brief Resizes the image to specified dimensions.
     * 
     * Uses nearest-neighbor interpolation for resizing. The aspect ratio is not preserved.
     * 
     * @param currentImage Reference to the image to resize (modified in-place)
     * @param width New width in pixels
     * @param height New height in pixels
     * 
     * @note This is an immediate operation without progress tracking.
     * @throws std::invalid_argument if width or height is less than 1
     */
    void applyResize(Image& currentImage, int width, int height);
    
    /**
     * @brief Skews the image horizontally by a given angle in degrees.
     * 
     * Creates a shearing effect along the X axis. The resulting canvas width
     * increases to accommodate the skewed content and is filled with white.
     * 
     * @param currentImage Reference to the image to skew (modified in-place)
     * @param angleDegrees Skew angle in degrees (positive skews right, negative left)
     * 
     * @note This is an immediate operation without progress tracking.
     */
    void applySkew(Image& currentImage, double angleDegrees = 40.0);

    // ===================== ADDITIONAL EFFECT FILTERS (immediate) =====================
    /** Emboss effect producing a relief-like grayscale. */
    void applyEmboss(Image& currentImage);
    /** Double vision horizontal offset blend. */
    void applyDoubleVision(Image& currentImage, int offset = 15);
    /** Oil painting effect (radius/intensity). */
    void applyOilPainting(Image& currentImage, int radius = 3, int intensity = 30);
    /** Enhance sunlight (boost warm channels). */
    void applyEnhanceSunlight(Image& currentImage);
    /** Fish-eye lens distortion effect. */
    void applyFishEye(Image& currentImage);
    
    // ============================================================================
    // ADVANCED EFFECTS (with progress tracking and cancellation)
    // ============================================================================
    
    /**
     * @brief Applies a blur effect to the image.
     * 
     * Uses a 15x15 average blur kernel to create a soft, blurred effect.
     * This operation supports progress tracking and cancellation.
     * 
     * @param currentImage Reference to the image to process (modified in-place)
     * @param preFilterImage Reference to store the original image state for cancellation
     * @param cancelRequested Atomic flag to check for cancellation requests
     * 
     * @note This is a long-running operation that can be cancelled.
     */
    void applyBlur(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested);
    /**
     * @brief Applies a blur effect with adjustable strength.
     * @param strength Percent in [0,100], mapped to kernel radius.
     */
    void applyBlur(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested, int strength);
    
    /**
     * @brief Applies an infrared photography simulation effect.
     * 
     * Creates an infrared-like appearance by inverting brightness and applying
     * red color channel emphasis. This operation supports progress tracking and cancellation.
     * 
     * @param currentImage Reference to the image to process (modified in-place)
     * @param preFilterImage Reference to store the original image state for cancellation
     * @param cancelRequested Atomic flag to check for cancellation requests
     * 
     * @details The effect:
     * - Calculates brightness as average of RGB values
     * - Inverts brightness (255 - brightness)
     * - Sets red channel to 255, green and blue to inverted brightness
     * 
     * @note This is a long-running operation that can be cancelled.
     */
    void applyInfrared(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested);
    
    /**
     * @brief Applies a purple color tint to the image.
     * 
     * Enhances red and blue channels while reducing green to create a purple tint.
     * This operation supports progress tracking and cancellation.
     * 
     * @param currentImage Reference to the image to process (modified in-place)
     * @param preFilterImage Reference to store the original image state for cancellation
     * @param cancelRequested Atomic flag to check for cancellation requests
     * 
     * @details The effect:
     * - Red channel: multiplied by 1.3 (clamped to 255)
     * - Green channel: multiplied by 0.5 (clamped to 0)
     * - Blue channel: multiplied by 1.3 (clamped to 255)
     * 
     * @note This is a long-running operation that can be cancelled.
     */
    void applyPurpleFilter(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested);
    
    /**
     * @brief Applies a custom color tint to the image.
     * 
     * Applies a color tint by blending the selected color with the image.
     * This operation supports progress tracking and cancellation.
     * 
     * @param currentImage Reference to the image to process (modified in-place)
     * @param preFilterImage Reference to store the original image state for cancellation
     * @param cancelRequested Atomic flag to check for cancellation requests
     * @param r Red component of the tint color (0-255)
     * @param g Green component of the tint color (0-255)
     * @param b Blue component of the tint color (0-255)
     * @param intensity Tint intensity (0.0-1.0, where 0.0 = no effect, 1.0 = full tint)
     * 
     * @details The effect blends the tint color with the image based on intensity.
     * 
     * @note This is a long-running operation that can be cancelled.
     */
    void applyColorTint(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested, 
                       int r, int g, int b, double intensity = 0.5);

    // New filters with progress and cancellation
    void applyEmboss(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested);
    void applyDoubleVision(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested, int offset = 15);
    void applyOilPainting(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested, int radius = 3, int intensity = 30);
    void applyEnhanceSunlight(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested);
    void applyFishEye(Image& currentImage, Image& preFilterImage, std::atomic<bool>& cancelRequested);

private:
    QProgressBar* progressBar;  ///< Pointer to Qt progress bar for progress tracking
    QStatusBar* statusBar;      ///< Pointer to Qt status bar for status updates
    
    /**
     * @brief Updates the progress bar with current progress.
     * 
     * @param value Current progress value (0 to total)
     * @param total Maximum progress value
     * @param updateInterval Number of operations between UI updates (default: 50)
     * 
     * @note This method is thread-safe and can be called from any thread.
     * @see QApplication::processEvents() for UI responsiveness
     */
    void updateProgress(int value, int total, int updateInterval = 50);
    
    /**
     * @brief Checks for cancellation and restores previous image state if cancelled.
     * 
     * @param cancelRequested Atomic flag indicating if cancellation was requested
     * @param currentImage Reference to current image (restored if cancelled)
     * @param preFilterImage Reference to previous image state
     * @param filterName Name of the filter for status message
     * 
     * @note This method should be called periodically during long-running operations.
     * @see std::atomic for thread-safe cancellation
     */
    void checkCancellation(std::atomic<bool>& cancelRequested, Image& currentImage, Image& preFilterImage, const QString& filterName);
};

#endif // IMAGEFILTERS_H


