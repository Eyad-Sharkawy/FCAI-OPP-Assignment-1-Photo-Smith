/**
 * @file photo_smith.cpp
 * @brief Main Qt application class for Photo Smith with comprehensive GUI and image processing capabilities.
 * 
 * This file contains the complete implementation of the PhotoSmith class, which serves as the main
 * application window for the Photo Smith image processing application. It provides a modern Qt-based
 * GUI with drag-and-drop support, real-time progress tracking, comprehensive undo/redo functionality,
 * and seamless integration with the core image processing filters.
 * 
 * @details The PhotoSmith class implements:
 * - Complete Qt MainWindow with modern UI design
 * - Drag-and-drop image loading with format validation
 * - Real-time image processing with progress tracking and cancellation
 * - Comprehensive undo/redo system with configurable history limits
 * - Interactive cropping with rubber-band selection
 * - Smart image display with aspect ratio preservation
 * - Menu system with keyboard shortcuts
 * - Status bar with real-time operation feedback
 * - Exception safety and comprehensive error handling
 * 
 * @features
 * - Modern Qt 6 GUI with responsive design
 * - Drag-and-drop image loading (PNG, JPEG, BMP, TGA)
 * - 15+ image processing filters with progress tracking
 * - Interactive cropping with visual selection
 * - Unlimited undo/redo with memory management
 * - Real-time progress bars and status updates
 * - Keyboard shortcuts and menu integration
 * - Smart window resizing with aspect ratio preservation
 * - Comprehensive error handling and user feedback
 * - Thread-safe cancellation for long-running operations
 * 
 * @author Team Members:
 * - Ahmed Mohamed ElSayed Tolba (ID: 20242023) - Small ID: 1, 4, 7, 10, (17 "bonus"), menu
 * - Eyad Mohamed Saad Ali (ID: 20242062) - Middle ID: 2, 5, 8, 11, (15 "bonus"), menu  
 * - Tarek Sami Mohamed (ID: 20242190) - Large ID: 3, 6, 9, 12, (16 "bonus"), menu
 * 
 * @institution Faculty of Computers and Artificial Intelligence, Cairo University
 * @section S33, Group B, Lab 7
 * @ta Laila Mostafa
 * @version 2.0.0
 * @date October 13, 2025
 * @copyright FCAI Cairo University
 */

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include <QSlider>
#include <QPixmap>
#include <QImage>
#include <QIcon>
#include <QMenuBar>
#include <QStatusBar>
#include <QFileInfo>
#include <QDir>
#include <QResizeEvent>
#include <QTimer>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QRubberBand>
#include <QMouseEvent>
#include <QProgressBar>
#include <QCloseEvent>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QImageCapture>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QVideoWidget>
#include <cmath>
#include <algorithm>
#include <stack>
#include <random>
#include <chrono>
#include <atomic>
#include <functional>
#include "../core/image/Image_Class.h"
#include "../core/filters/ImageFilters.h"
#include "ui_mainwindow.h"
#include "../core/history/HistoryManager.h"
#include "../core/io/ImageIO.h"

/**
 * @class PhotoSmith
 * @brief Main Qt application window for Photo Smith with comprehensive image processing capabilities.
 * 
 * The PhotoSmith class serves as the primary application window and controller for the Photo Smith
 * image processing application. It provides a complete Qt-based GUI with modern design patterns,
 * comprehensive image processing capabilities, and seamless integration with the core processing engine.
 * 
 * @details This class implements a complete image processing application with:
 * - Modern Qt 6 MainWindow with responsive UI design
 * - Drag-and-drop image loading with format validation and error handling
 * - Real-time image processing with progress tracking and cancellation support
 * - Comprehensive undo/redo system with configurable memory limits
 * - Interactive cropping with rubber-band selection and coordinate mapping
 * - Smart image display with aspect ratio preservation and dynamic resizing
 * - Complete menu system with keyboard shortcuts and accessibility
 * - Status bar with real-time operation feedback and image information
 * - Exception safety with comprehensive error handling and user feedback
 * - Thread-safe operations with atomic cancellation support
 * 
 * @features
 * - **GUI Components**: Complete Qt Designer integration with modern UI
 * - **Image Loading**: Drag-and-drop support for PNG, JPEG, BMP, TGA formats
 * - **Image Processing**: 15+ professional filters with real-time progress tracking
 * - **History Management**: Unlimited undo/redo with configurable memory limits
 * - **Interactive Cropping**: Visual selection with coordinate mapping
 * - **Smart Display**: Aspect ratio preservation with dynamic window resizing
 * - **Menu Integration**: Complete menu system with keyboard shortcuts
 * - **Progress Tracking**: Real-time progress bars and status updates
 * - **Error Handling**: Comprehensive exception safety and user feedback
 * - **Cancellation Support**: Thread-safe cancellation for long-running operations
 * 
 * @note This class is designed to work with Qt 6.8.1+ and requires C++20 support.
 * @see ImageFilters class for image processing operations
 * @see HistoryManager class for undo/redo functionality
 * @see ImageIO class for file operations
 * @see Image class for image data structure
 * 
 * @example
 * @code
 * QApplication app(argc, argv);
 * PhotoSmith window;
 * window.show();
 * return app.exec();
 * @endcode
 */
class PhotoSmith : public QMainWindow
{
    Q_OBJECT

private:
    // File filter constants for Qt file dialogs
    static constexpr const char* IMAGE_FILTER = "Image Files (*.png *.jpg *.jpeg *.bmp *.tga);;All Files (*)";
    static constexpr const char* SAVE_FILTER = "PNG Files (*.png);;JPEG Files (*.jpg);;BMP Files (*.bmp);;All Files (*)";

public:
    /**
     * @brief Constructs the main PhotoSmith application window.
     * 
     * Initializes the Qt MainWindow with comprehensive UI setup, signal/slot connections,
     * drag-and-drop support, and all necessary components for image processing operations.
     * 
     * @param parent Parent widget (default: nullptr for main window)
     * 
     * @details The constructor performs:
     * - Qt Designer UI setup and styling
     * - Signal/slot connections for all buttons and menu actions
     * - Drag-and-drop event handling setup
     * - Progress bar and status bar initialization
     * - Image filters system initialization
     * - Timer setup for smooth window resizing
     * - Event filter installation for cropping functionality
     * 
     * @note This constructor sets up the complete application state and should be
     *       called only once per application instance.
     * @see ~PhotoSmith() for cleanup operations
     */
    PhotoSmith(QWidget *parent = nullptr) : QMainWindow(parent), hasImage(false)
    {
        ui.setupUi(this);
        
        // Ensure momentary buttons (restore color after click)
        ui.grayscaleButton->setCheckable(false);
        ui.blackWhiteButton->setCheckable(false);
        ui.invertButton->setCheckable(false);
        ui.blurButton->setCheckable(false);
        ui.darkLightButton->setCheckable(false);
        ui.purpleButton->setCheckable(false);
        ui.infraredButton->setCheckable(false);
        ui.embossButton->setCheckable(false);
        ui.doubleVisionButton->setCheckable(false);
        ui.oilPaintingButton->setCheckable(false);
        ui.sunlightButton->setCheckable(false);
        ui.fishEyeButton->setCheckable(false);
        ui.edgesButton->setCheckable(false);
        ui.tvFilterButton->setCheckable(false);
        ui.frameButton->setCheckable(false);

        setWindowTitle("Photo Smith");
        setWindowIcon(QIcon("assets/icons/logo.png"));
        setMinimumSize(600, 400);
        
        // Set up the image label styling
        ui.imageLabel->setAlignment(Qt::AlignCenter);
        ui.imageLabel->setMinimumSize(400, 300);
        ui.imageLabel->setScaledContents(false); // Disable automatic scaling to prevent stretching
        ui.imageLabel->setStyleSheet(
            "QLabel {"
            "  border: 2px solid #9e9e9e;"
            "  background-color: #f4f4f4;"
            "  color: #2b2b2b;"
            "  font-size: 14px;"
            "}"
        );
        ui.imageLabel->setText("No image loaded\nClick 'Load Image' or drag & drop an image here");
        
        // Connect signals
        connect(ui.loadButton, &QPushButton::clicked, this, &PhotoSmith::loadImage);
        connect(ui.cameraButton, &QPushButton::clicked, this, &PhotoSmith::loadFromCamera);
        connect(ui.saveButton, &QPushButton::clicked, this, &PhotoSmith::saveImage);
        connect(ui.unloadButton, &QPushButton::clicked, this, &PhotoSmith::unloadImage);
        connect(ui.resetButton, &QPushButton::clicked, this, &PhotoSmith::resetImage);
        connect(ui.undoButton, &QPushButton::clicked, this, &PhotoSmith::undo);
        connect(ui.redoButton, &QPushButton::clicked, this, &PhotoSmith::redo);
        connect(ui.grayscaleButton, &QPushButton::clicked, this, &PhotoSmith::applyGrayscale);
        connect(ui.blackWhiteButton, &QPushButton::clicked, this, &PhotoSmith::applyBlackAndWhite);
        connect(ui.invertButton, &QPushButton::clicked, this, &PhotoSmith::applyInvert);
        connect(ui.mergeButton, &QPushButton::clicked, this, &PhotoSmith::applyMerge);
        connect(ui.flipButton, &QPushButton::clicked, this, &PhotoSmith::applyFlip);
        connect(ui.rotateButton, &QPushButton::clicked, this, &PhotoSmith::applyRotate);
        connect(ui.darkLightButton, &QPushButton::clicked, this, &PhotoSmith::applyDarkAndLight);
        connect(ui.frameButton, &QPushButton::clicked, this, &PhotoSmith::applyFrame);
        connect(ui.cropButton, &QPushButton::clicked, this, &PhotoSmith::startCropMode);
        connect(ui.edgesButton, &QPushButton::clicked, this, &PhotoSmith::applyEdges);
        connect(ui.resizeButton, &QPushButton::clicked, this, &PhotoSmith::applyResize);
        connect(ui.blurButton, &QPushButton::clicked, this, &PhotoSmith::applyBlur);
        connect(ui.infraredButton, &QPushButton::clicked, this, &PhotoSmith::applyInfrared);
        connect(ui.purpleButton, &QPushButton::clicked, this, &PhotoSmith::applyPurpleFilter);
        connect(ui.tvFilterButton, &QPushButton::clicked, this, &PhotoSmith::applyTVFilter);
        connect(ui.embossButton, &QPushButton::clicked, this, &PhotoSmith::applyEmboss);
        connect(ui.doubleVisionButton, &QPushButton::clicked, this, &PhotoSmith::applyDoubleVision);
        connect(ui.oilPaintingButton, &QPushButton::clicked, this, &PhotoSmith::applyOilPainting);
        connect(ui.sunlightButton, &QPushButton::clicked, this, &PhotoSmith::applyEnhanceSunlight);
        connect(ui.fishEyeButton, &QPushButton::clicked, this, &PhotoSmith::applyFishEye);
        connect(ui.cancelButton, &QPushButton::clicked, this, &PhotoSmith::cancelFilter);
        // Wire Skew button in controls section
        connect(ui.skewButton, &QPushButton::clicked, this, &PhotoSmith::applySkew);
        
        // Connect menu actions
        connect(ui.actionLoadImage, &QAction::triggered, this, &PhotoSmith::loadImage);
        connect(ui.actionSaveImage, &QAction::triggered, this, &PhotoSmith::saveImage);
        connect(ui.actionUnloadImage, &QAction::triggered, this, &PhotoSmith::unloadImage);
        connect(ui.actionResetImage, &QAction::triggered, this, &PhotoSmith::resetImage);
        connect(ui.actionUndo, &QAction::triggered, this, &PhotoSmith::undo);
        connect(ui.actionRedo, &QAction::triggered, this, &PhotoSmith::redo);
        connect(ui.actionExit, &QAction::triggered, this, &QWidget::close);
        connect(ui.actionGrayscale, &QAction::triggered, this, &PhotoSmith::applyGrayscale);
        connect(ui.actionBlackWhite, &QAction::triggered, this, &PhotoSmith::applyBlackAndWhite);
        connect(ui.actionInvert, &QAction::triggered, this, &PhotoSmith::applyInvert);
        connect(ui.actionMerge, &QAction::triggered, this, &PhotoSmith::applyMerge);
        connect(ui.actionFlip, &QAction::triggered, this, &PhotoSmith::applyFlip);
        connect(ui.actionRotate, &QAction::triggered, this, &PhotoSmith::applyRotate);
        connect(ui.actionCrop, &QAction::triggered, this, &PhotoSmith::startCropMode);
        connect(ui.actionDarkLight, &QAction::triggered, this, &PhotoSmith::applyDarkAndLight);
        connect(ui.actionFrame, &QAction::triggered, this, &PhotoSmith::applyFrame);
        connect(ui.actionEdgeDetection, &QAction::triggered, this, &PhotoSmith::applyEdges);
        connect(ui.actionResize, &QAction::triggered, this, &PhotoSmith::applyResize);
        connect(ui.actionBlur, &QAction::triggered, this, &PhotoSmith::applyBlur);
        connect(ui.actionInfrared, &QAction::triggered, this, &PhotoSmith::applyInfrared);
        connect(ui.actionPurpleFilter, &QAction::triggered, this, &PhotoSmith::applyPurpleFilter);
        connect(ui.actionTVFilter, &QAction::triggered, this, &PhotoSmith::applyTVFilter);
        
        // Create status bar
        statusBar()->showMessage("Ready - Drag an image here or click 'Load Image'");
        
        // Initialize resize timer for smooth resizing
        resizeTimer = new QTimer(this);
        resizeTimer->setSingleShot(true);
        resizeTimer->setInterval(100); // 100ms delay
        connect(resizeTimer, &QTimer::timeout, this, &PhotoSmith::updateImageDisplay);
        
        // Enable drag and drop
        setAcceptDrops(true);
        ui.imageLabel->setAcceptDrops(true);
        ui.scrollArea->setAcceptDrops(true);
        // Install event filter on imageLabel to capture mouse events for crop
        ui.imageLabel->installEventFilter(this);
        
        // Initialize image filters
        imageFilters = new ImageFilters(ui.progressBar, statusBar());
        
        // Initially disable filter buttons
        refreshButtons(false);
    }
    
    /**
     * @brief Destructor for the PhotoSmith application window.
     * 
     * Performs cleanup operations to ensure proper resource management and
     * prevent memory leaks. This includes cleaning up dynamically allocated
     * resources and Qt objects.
     * 
     * @details The destructor:
     * - Deletes the ImageFilters instance
     * - Qt automatically handles cleanup of child widgets
     * - Ensures proper resource deallocation
     * 
     * @note This destructor is automatically called when the window is closed
     *       or the application exits.
     */
    ~PhotoSmith() {
        delete imageFilters;
    }

private slots:
    /**
     * @brief Open file dialog and load an image into the application.
     * @details Enables image actions, initializes history and UI state.
     */
    void loadImage()
    {
        QString fileName = QFileDialog::getOpenFileName(this,
            "Load Image", QDir::homePath(), IMAGE_FILTER);
        if (!fileName.isEmpty()) {
            loadImageFromPath(fileName, false);
        }
    }
    
    /**
     * @brief Save the current image to a file.
     * 
     * Opens a file dialog to allow the user to choose a location and format
     * for saving the current image. Validates that an image is loaded before
     * attempting to save.
     * 
     * @details This method:
     * - Checks if an image is currently loaded
     * - Shows a warning if no image is available
     * - Opens a Qt file dialog for save location selection
     * - Handles file format selection (PNG, JPEG, BMP)
     * - Updates the unsaved changes flag on successful save
     * - Provides user feedback through status bar messages
     * ;pl
     * @note The save operation uses the ImageIO class for Qt-integrated file operations.
     * @see saveImageWithDialog() for the actual save implementation
     * @see ImageIO::saveToFile() for file writing operations
     */
    void saveImage()
    {
        if (!hasImage) {
            QMessageBox::warning(this, "Warning", "No image to save!");
            return;
        }
        (void)saveImageWithDialog();
    }

    /**
     * @brief Capture a single frame from the default camera and load it.
     */
    void loadFromCamera()
    {
        // Choose default camera
        QCameraDevice device = QMediaDevices::defaultVideoInput();
        if (!device.isNull()) {
            // Build capture session and preview dialog
            QDialog *dlg = new QDialog(this);
            dlg->setWindowTitle("Camera Preview");
            QVBoxLayout *layout = new QVBoxLayout(dlg);
            QCamera *camera = new QCamera(device);
            // Parent the session to the dialog to ensure it stays alive
            QMediaCaptureSession *session = new QMediaCaptureSession(dlg);
            session->setCamera(camera);
            QVideoWidget *view = new QVideoWidget(dlg);
            view->setAspectRatioMode(Qt::KeepAspectRatio);
            view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            view->setMinimumSize(640, 480);
            session->setVideoOutput(view);
            QImageCapture *imageCapture = new QImageCapture(dlg);
            session->setImageCapture(imageCapture);

            QPushButton *captureBtn = new QPushButton("Capture", dlg);
            QPushButton *closeBtn = new QPushButton("Close", dlg);
            QHBoxLayout *btns = new QHBoxLayout();
            btns->addStretch(); btns->addWidget(captureBtn); btns->addWidget(closeBtn);
            layout->addWidget(view);
            layout->addLayout(btns);
            dlg->resize(900, 700);

            // Temp file
            QString tempPath = QDir::temp().filePath("photosmith_capture.png");
            QFile::remove(tempPath);

            QObject::connect(captureBtn, &QPushButton::clicked, dlg, [this, imageCapture, tempPath]() {
                imageCapture->captureToFile(tempPath);
            });
            QObject::connect(closeBtn, &QPushButton::clicked, dlg, [camera, dlg]() {
                camera->stop();
                dlg->reject();
            });
            QObject::connect(imageCapture, &QImageCapture::imageSaved, dlg, [this, camera, dlg](int, const QString &filePath){
                camera->stop();
                dlg->accept();
                loadImageFromPath(filePath, false);
            });
            QObject::connect(imageCapture, &QImageCapture::errorOccurred, dlg, [camera, dlg](int, QImageCapture::Error, const QString &errorString){
                camera->stop();
                QMessageBox::critical(dlg, "Camera Error", errorString);
            });
            QObject::connect(camera, &QCamera::errorOccurred, dlg, [dlg](QCamera::Error, const QString &errorString){
                QMessageBox::critical(dlg, "Camera Error", errorString);
            });

            camera->start();
            dlg->exec();

            // Cleanup
            dlg->deleteLater();
            session->deleteLater();
            camera->deleteLater();
        } else {
            QMessageBox::warning(this, "Camera", "No camera device found.");
        }
    }
    
    /**
     * @brief Unload the current image with optional save confirmation.
     * 
     * Provides a safe way to unload the current image with proper confirmation
     * dialogs and save options. Handles unsaved changes gracefully by offering
     * the user options to save, discard, or cancel the operation.
     * 
     * @details This method:
     * - Validates that an image is currently loaded
     * - Shows appropriate confirmation dialogs based on save state
     * - Offers save option if there are unsaved changes
     * - Handles user cancellation gracefully
     * - Clears all image data and resets UI state
     * - Resets undo/redo history and button states
     * 
     * @note This method provides comprehensive state management to ensure
     *       the application remains in a consistent state after unloading.
     * @see resetUiToNoImageState() for UI state reset
     * @see saveImageWithDialog() for save operations
     */
    void unloadImage()
    {
        if (!hasImage) {
            QMessageBox::warning(this, "Warning", "No image to unload!");
            return;
        }
        
        // Ask for confirmation with save option (only if there are unsaved changes)
        QMessageBox::StandardButton reply;
        if (hasUnsavedChanges) {
            reply = QMessageBox::question(this, "Unload Image",
                "The image has unsaved changes. Do you want to save before unloading?",
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        } else {
            reply = QMessageBox::question(this, "Unload Image",
                "Are you sure you want to unload the current image?",
                QMessageBox::Yes | QMessageBox::No);
            // Convert Yes/No to Save/Discard for consistent handling
            if (reply == QMessageBox::Yes) {
                reply = QMessageBox::Discard;
            } else {
                reply = QMessageBox::Cancel;
            }
        }
        
        if (reply == QMessageBox::Save) {
            if (!saveImageWithDialog()) {
                return; // Don't unload if save failed or cancelled
            }
        } else if (reply == QMessageBox::Cancel) {
            return; // User cancelled the unload operation
        }
        
        // Proceed with unload (either after successful save or user chose Discard)
        if (reply == QMessageBox::Save || reply == QMessageBox::Discard) {
            // Clear image data
            hasImage = false;
            currentFilePath.clear();
            hasUnsavedChanges = false;
            
            // Reset UI and history
            resetUiToNoImageState();
        }
    }
    
    /**
     * @brief Apply grayscale conversion to the current image.
     * 
     * Converts the current image to grayscale using average RGB values.
     * This operation supports progress tracking and cancellation.
     * 
     * @note This is a long-running operation that can be cancelled.
     * @see ImageFilters::applyGrayscale() for implementation details
     */
    void applyGrayscale()
    {
        if (!hasImage) return;
        runCancelableFilter([&]() {
            imageFilters->applyGrayscale(currentImage, preFilterImage, cancelRequested);
        });
        setActiveFilterValue("Grayscale");
        ui.colorModeValue->setText("Grayscale");
        updatePropertiesPanel();
    }
    
    /**
     * @brief Apply TV/CRT monitor simulation effect to the current image.
     * 
     * Creates a vintage TV/CRT monitor appearance with scan lines, color shifts,
     * and noise effects. This operation supports progress tracking and cancellation.
     * 
     * @note This is a long-running operation that can be cancelled.
     * @see ImageFilters::applyTVFilter() for implementation details
     */
    void applyTVFilter()
    {
        if (!hasImage) return;
        runCancelableFilter([&]() {
            imageFilters->applyTVFilter(currentImage, preFilterImage, cancelRequested);
        });
        setActiveFilterValue("TV/CRT Filter");
        updatePropertiesPanel();
    }
    
    /**
     * @brief Reset the current image to its original state.
     * 
     * Restores the current image to the state it was in when first loaded,
     * discarding all modifications made since loading.
     * 
     * @note This operation does not affect the undo/redo history.
     * @see originalImage for the stored original image state
     */
    void resetImage()
    {
        if (!hasImage) return;
        
        currentImage = originalImage;
        updateImageDisplay();
        statusBar()->showMessage("Image reset to original");
        setActiveFilterValue("None");
        updatePropertiesPanel();
    }
    
    /**
     * @brief Apply black and white conversion to the current image.
     * 
     * Converts the current image to pure black and white using threshold-based
     * processing. This operation supports progress tracking and cancellation.
     * 
     * @note This is a long-running operation that can be cancelled.
     * @see ImageFilters::applyBlackAndWhite() for implementation details
     */
    void applyBlackAndWhite()
    {
        if (!hasImage) return;
        runCancelableFilter([&]() {
            imageFilters->applyBlackAndWhite(currentImage, preFilterImage, cancelRequested);
        });
        setActiveFilterValue("Black & White");
        ui.colorModeValue->setText("Grayscale");
        updatePropertiesPanel();
    }
    
    /**
     * @brief Apply color inversion to the current image.
     * 
     * Inverts all color values in the current image to create a negative effect.
     * This operation supports progress tracking and cancellation.
     * 
     * @note This is a long-running operation that can be cancelled.
     * @see ImageFilters::applyInvert() for implementation details
     */
    void applyInvert()
    {
        if (!hasImage) return;
        runCancelableFilter([&]() {
            imageFilters->applyInvert(currentImage, preFilterImage, cancelRequested);
        });
        setActiveFilterValue("Invert");
        ui.colorModeValue->setText("RGB");
        updatePropertiesPanel();
    }
    
    /**
     * @brief Merge the current image with another image.
     * 
     * Opens a file dialog to select a second image and merges it with the current
     * image by averaging their pixel values. The resulting image will have the
     * dimensions of the smaller input image.
     * 
     * @details This method:
     * - Opens a file dialog for image selection
     * - Loads the selected image using the Image class
     * - Merges the images by averaging pixel values
     * - Handles errors gracefully with user feedback
     * - Updates the display after successful merge
     * 
     * @note This is an immediate operation without progress tracking.
     * @see ImageFilters::applyMerge() for implementation details
     * @see ImageIO::loadFromFile() for image loading
     */
    void applyMerge()
    {
        if (!hasImage) return;
        
        QString fileName = QFileDialog::getOpenFileName(this,
            "Select Image to Merge", QDir::homePath(), IMAGE_FILTER);
        
        if (!fileName.isEmpty()) {
            mergeWithPath(fileName);
        }
    }

    /**
     * @brief Merge the current image with another image from the specified path.
     * 
     * Loads an image from the given file path and merges it with the current image.
     * Handles different image dimensions by offering resize options or merging
     * the common overlapping area.
     * 
     * @param fileName Qt string containing the path to the image file to merge
     * 
     * @details This method:
     * - Saves the current state for undo functionality
     * - Loads the merge image from the specified path
     * - Handles dimension mismatches with user dialog options
     * - Applies the merge operation using ImageFilters
     * - Updates the display and properties panel
     * - Provides comprehensive error handling
     * 
     * @note This is an immediate operation without progress tracking.
     * @see ImageFilters::applyMerge() for the actual merge implementation
     * @see ImageIO::loadFromFile() for image loading operations
     * 
     * @throws std::exception if image loading or merge operation fails
     */
    void mergeWithPath(const QString &fileName)
    {
        try {
            saveStateForUndo();
            Image mergeImage;
            mergeImage.loadNewImage(fileName.toStdString());
            // If dimensions differ, ask user how to merge
            if (mergeImage.width != currentImage.width || mergeImage.height != currentImage.height) {
                QStringList options;
                options << "Resize smaller image to match larger" << "Merge common overlapping area";
                bool ok = false;
                QString choice = QInputDialog::getItem(this, "Merge Images",
                    "Images have different sizes. Choose merge option:", options, 0, false, &ok);
                if (!ok || choice.isEmpty()) {
                    return; // user cancelled
                }

                if (choice == options[0]) {
                    // Resize the smaller image to match the larger image dimensions
                    const int targetW = std::max((int)currentImage.width, (int)mergeImage.width);
                    const int targetH = std::max((int)currentImage.height, (int)mergeImage.height);
                    if (currentImage.width != targetW || currentImage.height != targetH) {
                        imageFilters->applyResize(currentImage, targetW, targetH);
                    }
                    if (mergeImage.width != targetW || mergeImage.height != targetH) {
                        imageFilters->applyResize(mergeImage, targetW, targetH);
                    }
                }
                // else: Merge common overlapping area by default behavior below
            }

            imageFilters->applyMerge(currentImage, mergeImage);
            updateImageDisplay();
            setActiveFilterValue("Merge");
            updatePropertiesPanel();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Merge failed: %1").arg(e.what()));
        }
    }
    
    /**
     * @brief Apply horizontal or vertical flip transformation to the current image.
     * 
     * Presents a dialog to the user to choose between horizontal or vertical flip
     * direction, then applies the selected transformation to the current image.
     * 
     * @details This method:
     * - Validates that an image is currently loaded
     * - Shows a selection dialog for flip direction
     * - Saves the current state for undo functionality
     * - Applies the flip transformation using ImageFilters
     * - Updates the display and properties panel
     * - Handles errors gracefully with user feedback
     * 
     * @note This is an immediate operation without progress tracking.
     * @see ImageFilters::applyFlip() for the actual flip implementation
     * @see getInputFromList() for user input dialog
     */
    void applyFlip()
    {
        if (!hasImage) return;
        
        QStringList options;
        options << "Horizontal" << "Vertical";
        
        QString choice = getInputFromList("Flip Image", "Choose flip direction:", options);
        
        if (!choice.isEmpty()) {
            saveStateForUndo();
            
            try {
                imageFilters->applyFlip(currentImage, choice);
                updateImageDisplay();
                setActiveFilterValue("Flip");
                updatePropertiesPanel();
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Error", QString("Filter failed: %1").arg(e.what()));
            }
        }
    }
    
    /**
     * @brief Apply rotation transformation to the current image.
     * 
     * Presents a dialog to the user to choose between 90°, 180°, or 270° rotation
     * angles, then applies the selected rotation to the current image.
     * 
     * @details This method:
     * - Validates that an image is currently loaded
     * - Shows a selection dialog for rotation angle
     * - Saves the current state for undo functionality
     * - Applies the rotation transformation using ImageFilters
     * - Updates the display and properties panel
     * - Handles errors gracefully with user feedback
     * 
     * @note This is an immediate operation without progress tracking.
     * @see ImageFilters::applyRotate() for the actual rotation implementation
     * @see getInputFromList() for user input dialog
     */
    void applyRotate()
    {
        if (!hasImage) return;
        
        QStringList options;
        options << "90°" << "180°" << "270°";
        
        QString choice = getInputFromList("Rotate Image", "Choose rotation angle:", options);
        
        if (!choice.isEmpty()) {
            saveStateForUndo();
            
            try {
                imageFilters->applyRotate(currentImage, choice);
                updateImageDisplay();
                setActiveFilterValue("Rotate");
                updatePropertiesPanel();
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Error", QString("Filter failed: %1").arg(e.what()));
            }
        }
    }
    
    /**
     * @brief Apply brightness adjustment (darken or lighten) to the current image.
     * 
     * Presents dialogs to the user to choose between darkening or lightening
     * and to specify the intensity percentage, then applies the brightness
     * adjustment to the current image.
     * 
     * @details This method:
     * - Validates that an image is currently loaded
     * - Shows a selection dialog for dark/light choice
     * - Shows a slider dialog for intensity percentage (0-100%)
     * - Applies the brightness adjustment using ImageFilters
     * - Updates the display and properties panel
     * - Handles user cancellation gracefully
     * 
     * @note This is an immediate operation without progress tracking.
     * @see ImageFilters::applyDarkAndLight() for the actual brightness implementation
     * @see getInputFromList() for choice selection dialog
     * @see getPercentWithSlider() for percentage input dialog
     */
    void applyDarkAndLight()
    {
        if (!hasImage) return;
        
        // Ask user for dark or light
        QStringList options; options << "dark" << "light";
        QString choice = getInputFromList("Darken or Lighten", "Choose:", options);
        if (choice.isEmpty()) return;

        // Ask for percentage 0-100 via slider dialog
        bool ok = false;
        int percent = getPercentWithSlider("Adjust Brightness", choice == "dark" ? "Darken percentage" : "Lighten percentage", 50, &ok);
        if (!ok) return;

        runSimpleFilter([&]() {
            imageFilters->applyDarkAndLight(currentImage, choice, percent);
        });
        setActiveFilterValue("Dark & Light");
        ui.colorModeValue->setText("RGB");
        updatePropertiesPanel();
    }
    
    /**
     * @brief Add a decorative frame around the current image.
     * 
     * Presents a dialog to the user to choose from various frame styles,
     * then applies the selected frame decoration to the current image.
     * 
     * @details This method:
     * - Validates that an image is currently loaded
     * - Shows a selection dialog with multiple frame options
     * - Applies the selected frame using ImageFilters
     * - Updates the display and properties panel
     * - Handles user cancellation gracefully
     * 
     * Available frame types include:
     * - Simple Frame: Basic blue and white border
     * - Double Border: White double border
     * - Solid Frames: Various colored solid borders
     * - Shadow Frame: Frame with shadow effect
     * - Gold Decorated Frame: Ornate gold-style frame
     * 
     * @note This is an immediate operation without progress tracking.
     * @see ImageFilters::applyFrame() for the actual frame implementation
     * @see getInputFromList() for frame selection dialog
     */
    void applyFrame()
    {
        if (!hasImage) return;
        
        QStringList options;
        options << "Simple Frame"
                << "Double Border - White"
                << "Solid Frame - Blue"
                << "Solid Frame - Red"
                << "Solid Frame - Green"
                << "Solid Frame - Black"
                << "Solid Frame - White"
                << "Shadow Frame"
                << "Gold Decorated Frame";
        
        QString choice = getInputFromList("Add Frame", "Choose frame type:", options);
        
        if (!choice.isEmpty()) {
            runSimpleFilter([&]() {
                imageFilters->applyFrame(currentImage, choice);
            });
            setActiveFilterValue("Frame");
            updatePropertiesPanel();
        }
    }
    
    /**
     * @brief Apply edge detection filter to the current image.
     * 
     * Applies Sobel edge detection algorithm with Gaussian blur preprocessing
     * to highlight edges and outlines in the current image.
     * 
     * @details This method:
     * - Validates that an image is currently loaded
     * - Applies edge detection using ImageFilters
     * - Updates the display and properties panel
     * - Uses Sobel algorithm with Gaussian blur for noise reduction
     * 
     * The edge detection algorithm:
     * 1. Converts to grayscale using weighted average
     * 2. Applies 5x5 Gaussian blur to reduce noise
     * 3. Uses 3x3 Sobel kernels for edge detection
     * 4. Calculates gradient magnitude and applies threshold
     * 
     * @note This is an immediate operation without progress tracking.
     * @see ImageFilters::applyEdges() for the actual edge detection implementation
     */
    void applyEdges()
    {
        if (!hasImage) return;
        runSimpleFilter([&]() {
            imageFilters->applyEdges(currentImage);
        });
        setActiveFilterValue("Edge Detection");
        updatePropertiesPanel();
    }
    
    /**
     * @brief Resize the current image to specified dimensions.
     * 
     * Presents dialogs to the user to input new width and height values,
     * then resizes the current image using nearest-neighbor interpolation.
     * 
     * @details This method:
     * - Validates that an image is currently loaded
     * - Shows input dialogs for width and height (1-10000 pixels)
     * - Uses current image dimensions as default values
     * - Applies resize transformation using ImageFilters
     * - Updates the display and properties panel
     * - Handles user cancellation gracefully
     * 
     * @note This is an immediate operation without progress tracking.
     * @note The aspect ratio is not preserved during resize.
     * @see ImageFilters::applyResize() for the actual resize implementation
     */
    void applyResize()
    {
        if (!hasImage) return;
        
        bool ok1, ok2;
        int width = QInputDialog::getInt(this, "Resize Image", "Enter new width:", 
            currentImage.width, 1, 10000, 1, &ok1);
        int height = QInputDialog::getInt(this, "Resize Image", "Enter new height:", 
            currentImage.height, 1, 10000, 1, &ok2);
        
        if (ok1 && ok2) {
            runSimpleFilter([&]() {
                imageFilters->applyResize(currentImage, width, height);
            });
            setActiveFilterValue("Resize");
            updatePropertiesPanel();
        }
    }
    
    /**
     * @brief Apply blur effect to the current image with adjustable strength.
     * 
     * Presents a slider dialog to the user to specify blur strength (0-100%),
     * then applies the blur effect to the current image with progress tracking
     * and cancellation support.
     * 
     * @details This method:
     * - Validates that an image is currently loaded
     * - Shows a slider dialog for blur strength (0-100%)
     * - Uses 60% as the default blur strength
     * - Applies blur with progress tracking and cancellation support
     * - Updates the display and properties panel
     * - Handles user cancellation gracefully
     * 
     * @note This is a long-running operation that can be cancelled.
     * @see ImageFilters::applyBlur() for the actual blur implementation
     * @see getPercentWithSlider() for percentage input dialog
     */
    void applyBlur()
    {
        if (!hasImage) return;
        // Ask user for blur strength 0..100
        bool ok = false;
        int percent = getPercentWithSlider("Blur Strength", "Choose blur level (0-100%)", 60, &ok);
        if (!ok) return;
        runCancelableFilter([&]() {
            imageFilters->applyBlur(currentImage, preFilterImage, cancelRequested, percent);
        });
        setActiveFilterValue("Blur");
        updatePropertiesPanel();
    }
    
    /**
     * @brief Apply the Infrared effect with progress and cancellation support.
     */
    void applyInfrared()
    {
        if (!hasImage) return;
        runCancelableFilter([&]() {
            imageFilters->applyInfrared(currentImage, preFilterImage, cancelRequested);
        });
        setActiveFilterValue("Infrared");
        updatePropertiesPanel();
    }
    
    /**
     * @brief Apply the Purple color filter with progress and cancellation.
     */
    void applyPurpleFilter()
    {
        if (!hasImage) return;
        runCancelableFilter([&]() {
            imageFilters->applyPurpleFilter(currentImage, preFilterImage, cancelRequested);
        });
        setActiveFilterValue("Purple Filter");
        updatePropertiesPanel();
    }
    void applyEmboss()
    {
        if (!hasImage) return;
        runCancelableFilter([&]() { imageFilters->applyEmboss(currentImage, preFilterImage, cancelRequested); });
        setActiveFilterValue("Emboss");
        updatePropertiesPanel();
    }
    void applyDoubleVision()
    {
        if (!hasImage) return;
        runCancelableFilter([&]() { imageFilters->applyDoubleVision(currentImage, preFilterImage, cancelRequested, 15); });
        setActiveFilterValue("Double Vision");
        updatePropertiesPanel();
    }
    void applyOilPainting()
    {
        if (!hasImage) return;
        runCancelableFilter([&]() { imageFilters->applyOilPainting(currentImage, preFilterImage, cancelRequested, 3, 30); });
        setActiveFilterValue("Oil Painting");
        updatePropertiesPanel();
    }
    void applyEnhanceSunlight()
    {
        if (!hasImage) return;
        runCancelableFilter([&]() { imageFilters->applyEnhanceSunlight(currentImage, preFilterImage, cancelRequested); });
        setActiveFilterValue("Enhance Sunlight");
        updatePropertiesPanel();
    }
    void applyFishEye()
    {
        if (!hasImage) return;
        runCancelableFilter([&]() { imageFilters->applyFishEye(currentImage, preFilterImage, cancelRequested); });
        setActiveFilterValue("Fish-Eye");
        updatePropertiesPanel();
    }
    
    /**
     * @brief Apply horizontal skew with an angle chosen via slider.
     */
    void applySkew()
    {
        if (!hasImage) return;
        bool ok = false;
        // Reuse the slider dialog helper for percentage-like values, map -60..+60
        // Build a custom slider dialog for angle specifically (since helper is 0..100)
        QDialog dialog(this);
        dialog.setWindowTitle("Skew Angle");
        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        QLabel *lbl = new QLabel("Choose angle (degrees)", &dialog);
        QSlider *slider = new QSlider(Qt::Horizontal, &dialog);
        slider->setRange(-60, 60);
        slider->setValue(40);
        QLabel *valueLabel = new QLabel(QString::number(slider->value()) + "°", &dialog);
        QObject::connect(slider, &QSlider::valueChanged, &dialog, [valueLabel](int v){ valueLabel->setText(QString::number(v) + "°"); });
        QHBoxLayout *buttons = new QHBoxLayout();
        QPushButton *okBtn = new QPushButton("OK", &dialog);
        QPushButton *cancelBtn = new QPushButton("Cancel", &dialog);
        buttons->addStretch();
        buttons->addWidget(okBtn);
        buttons->addWidget(cancelBtn);
        layout->addWidget(lbl);
        layout->addWidget(slider);
        layout->addWidget(valueLabel);
        layout->addLayout(buttons);

        double chosenAngle = slider->value();
        QObject::connect(okBtn, &QPushButton::clicked, &dialog, [&](){ chosenAngle = slider->value(); dialog.accept(); });
        QObject::connect(cancelBtn, &QPushButton::clicked, &dialog, [&](){ dialog.reject(); });

        if (dialog.exec() != QDialog::Accepted) return;

        runSimpleFilter([&]() {
            imageFilters->applySkew(currentImage, chosenAngle);
        });
        setActiveFilterValue("Skew");
        updatePropertiesPanel();
    }
    
    /**
     * @brief Enter interactive crop mode using a rubber-band selection.
     */
    void startCropMode()
    {
        if (!hasImage) return;
        if (!rubberBand) {
            rubberBand = new QRubberBand(QRubberBand::Rectangle, ui.imageLabel);
        }
        cropping = true;
        statusBar()->showMessage("Crop mode: drag to select area, release to crop");
    }
    
    /**
     * @brief Undo the last image modification.
     */
    void undo()
    {
        if (!hasImage) return;
        if (!history.undo(currentImage)) return;
        // Manage parallel filter name history
        redoFilterNames.push(ui.activeFilterValue->text());
        if (!undoFilterNames.empty()) {
            setActiveFilterValue(undoFilterNames.top());
            undoFilterNames.pop();
        }
        
        updateImageDisplay();
        updateUndoRedoButtons();
        statusBar()->showMessage("Undo applied");
    }
    
    /**
     * @brief Redo the last undone image modification.
     */
    void redo()
    {
        if (!hasImage) return;
        if (!history.redo(currentImage)) return;
        // Manage parallel filter name history
        undoFilterNames.push(ui.activeFilterValue->text());
        if (!redoFilterNames.empty()) {
            setActiveFilterValue(redoFilterNames.top());
            redoFilterNames.pop();
        }
        
        updateImageDisplay();
        updateUndoRedoButtons();
        statusBar()->showMessage("Redo applied");
    }
    
    /**
     * @brief Request cancellation for a long-running filter.
     */
    void cancelFilter()
    {
        cancelRequested = true;
        statusBar()->showMessage("Cancelling filter...");
    }

private:
    Ui::MainWindow ui;
    
    /**
     * @brief Push current image state onto the undo stack and mark unsaved changes.
     */
    void saveStateForUndo()
    {
        if (!hasImage) return;
        // Save current state to history
        history.pushUndo(currentImage);
        // Mark as having unsaved changes
        hasUnsavedChanges = true;
        // Track active filter name in parallel with history
        undoFilterNames.push(ui.activeFilterValue->text());
        // Any new branch invalidates redo filter names
        while (!redoFilterNames.empty()) redoFilterNames.pop();
        
        updateUndoRedoButtons();
    }
    
    /**
     * @brief Refresh enabled state of Undo/Redo buttons based on history.
     */
    void updateUndoRedoButtons()
    {
        ui.undoButton->setEnabled(history.canUndo());
        ui.redoButton->setEnabled(history.canRedo());
    }

    /**
     * @brief Refresh the enabled state of all UI buttons based on image availability.
     * 
     * Enables or disables all image processing buttons and controls based on whether
     * an image is currently loaded. This ensures the UI remains in a consistent state
     * and prevents operations on non-existent images.
     * 
     * @param isActive True to enable all buttons, false to disable them
     * 
     * @details This method controls the state of:
     * - File operations (save, unload, reset)
     * - History operations (undo, redo)
     * - All image processing filters
     * - Interactive tools (crop)
     * 
     * @note This method should be called whenever the image state changes
     *       to maintain UI consistency.
     */
    void refreshButtons(const bool isActive) {
        ui.saveButton->setEnabled(isActive);
        ui.unloadButton->setEnabled(isActive);
        ui.resetButton->setEnabled(isActive);
        ui.undoButton->setEnabled(isActive);
        ui.redoButton->setEnabled(isActive);
        ui.grayscaleButton->setEnabled(isActive);
        ui.blackWhiteButton->setEnabled(isActive);
        ui.invertButton->setEnabled(isActive);
        ui.mergeButton->setEnabled(isActive);
        ui.flipButton->setEnabled(isActive);
        ui.rotateButton->setEnabled(isActive);
        ui.cropButton->setEnabled(isActive);
        ui.darkLightButton->setEnabled(isActive);
        ui.frameButton->setEnabled(isActive);
        ui.edgesButton->setEnabled(isActive);
        ui.resizeButton->setEnabled(isActive);
        ui.blurButton->setEnabled(isActive);
        ui.infraredButton->setEnabled(isActive);
        ui.purpleButton->setEnabled(isActive);
        ui.tvFilterButton->setEnabled(isActive);
        ui.skewButton->setEnabled(isActive);
        ui.embossButton->setEnabled(isActive);
        ui.doubleVisionButton->setEnabled(isActive);
        ui.oilPaintingButton->setEnabled(isActive);
        ui.sunlightButton->setEnabled(isActive);
        ui.fishEyeButton->setEnabled(isActive);
    }

    /**
     * @brief Compute and enforce a minimum window size to avoid scrollbars for the current image.
     */
    void updateMinimumWindowSize()
    {
        // Keep a stable, small base minimum to allow shrinking after expansion.
        setMinimumSize(600, 400);
    }
    
protected:
    /**
     * @brief Intercept window close to prompt for saving unsaved changes.
     */
    void closeEvent(QCloseEvent *event) override
    {
        if (hasImage && hasUnsavedChanges) {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Save Changes",
                "The image has unsaved changes. Do you want to save before exiting?",
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            
            if (reply == QMessageBox::Save) {
                if (saveImageWithDialog()) {
                    event->accept();
                } else {
                    event->ignore();
                    return;
                }
            } else if (reply == QMessageBox::Discard) {
                event->accept(); // Allow the application to close without saving
            } else {
                event->ignore(); // User cancelled the exit
                return;
            }
        }
        
        QMainWindow::closeEvent(event);
    }
    
    /**
     * @brief Throttle image re-layout on window resize using a timer.
     */
    void resizeEvent(QResizeEvent *event) override
    {
        QMainWindow::resizeEvent(event);
        // Use timer to avoid too many updates during rapid resizing
        if (hasImage) {
            resizeTimer->start();
        }
    }
    
    /**
     * @brief Accept drag if it contains a supported image file.
     */
    void dragEnterEvent(QDragEnterEvent *event) override
    {
        if (event->mimeData()->hasUrls()) {
            QList<QUrl> urls = event->mimeData()->urls();
            if (!urls.isEmpty()) {
                QString fileName = urls.first().toLocalFile();
                QFileInfo fileInfo(fileName);
                QString suffix = fileInfo.suffix().toLower();
                
                // Check if it's a supported image format
                if (suffix == "png" || suffix == "jpg" || suffix == "jpeg" || 
                    suffix == "bmp" || suffix == "tga") {
                    event->acceptProposedAction();
                    if (hasImage) {
                        statusBar()->showMessage("Drop image to merge: " + fileInfo.fileName());
                    } else {
                        statusBar()->showMessage("Drop image to load: " + fileInfo.fileName());
                    }
                    return;
                }
            }
        }
        event->ignore();
    }
    
    /**
     * @brief Allow drag move within the window for image files.
     */
    void dragMoveEvent(QDragMoveEvent *event) override
    {
        if (event->mimeData()->hasUrls()) {
            event->acceptProposedAction();
        }
    }
    
    /**
     * @brief Restore status message when a drag leaves the window.
     */
    void dragLeaveEvent(QDragLeaveEvent *event) override
    {
        Q_UNUSED(event)
        statusBar()->showMessage(hasImage ? "Ready" : "Ready - Drag an image here or click 'Load Image'");
    }
    
    /**
     * @brief Load an image when a supported file is dropped onto the window.
     */
    void dropEvent(QDropEvent *event) override
    {
        if (event->mimeData()->hasUrls()) {
            QList<QUrl> urls = event->mimeData()->urls();
            if (!urls.isEmpty()) {
                QString fileName = urls.first().toLocalFile();
                QFileInfo fileInfo(fileName);
                QString suffix = fileInfo.suffix().toLower();
                
                // Check if it's a supported image format
                if (suffix == "png" || suffix == "jpg" || suffix == "jpeg" || 
                    suffix == "bmp" || suffix == "tga") {
                    
                    event->acceptProposedAction();
                    if (hasImage) {
                        // Merge with current image
                        mergeWithPath(fileName);
                    } else {
                        // Load the dropped image
                        loadImageFromPath(fileName, true);
                    }
                    return;
                }
            }
        }
        event->ignore();
        statusBar()->showMessage("Unsupported file format. Please drop a PNG, JPG, JPEG, BMP, or TGA file.");
    }
    
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == ui.imageLabel && cropping && hasImage) {
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent *me = static_cast<QMouseEvent*>(event);
                cropOrigin = me->pos();
                if (!rubberBand) rubberBand = new QRubberBand(QRubberBand::Rectangle, ui.imageLabel);
                rubberBand->setGeometry(QRect(cropOrigin, QSize()));
                rubberBand->show();
                return true;
            } else if (event->type() == QEvent::MouseMove) {
                QMouseEvent *me = static_cast<QMouseEvent*>(event);
                if (rubberBand) rubberBand->setGeometry(QRect(cropOrigin, me->pos()).normalized());
                return true;
            } else if (event->type() == QEvent::MouseButtonRelease) {
                QMouseEvent *me = static_cast<QMouseEvent*>(event);
                if (rubberBand) {
                    QRect selection = QRect(cropOrigin, me->pos()).normalized();
                    performCropFromSelection(selection);
                    rubberBand->hide();
                }
                cropping = false;
                statusBar()->showMessage("Cropped");
                return true;
            }
        }
        return QMainWindow::eventFilter(watched, event);
    }
    /**
     * @brief Update the image display with smart resizing and aspect ratio preservation.
     * 
     * Converts the current image to a QPixmap and displays it in the image label with
     * smart resizing that maintains aspect ratio and fits within the available space.
     * Updates the status bar with detailed image information.
     * 
     * @details This method:
     * - Converts the Image object to QPixmap for display
     * - Calculates optimal display size maintaining aspect ratio
     * - Scales the image with smooth transformation
     * - Updates the image label with the scaled pixmap
     * - Resizes the label to match the scaled image
     * - Updates the minimum window size to prevent scrollbars
     * - Shows detailed image information in the status bar
     * 
     * @note This method is called whenever the image changes and should be
     *       called from the main thread for UI updates.
     * @see buildQImage() for Image to QImage conversion
     * @see calculateAspectRatioSize() for size calculation
     */
    void updateImageDisplay()
    {
        if (!hasImage) return;
        
        QPixmap pixmap = QPixmap::fromImage(buildQImage(currentImage));
        
        // Get the available space in the scroll area
        QSize scrollAreaSize = ui.scrollArea->size();
        QSize availableSize(scrollAreaSize.width() - 20, scrollAreaSize.height() - 20); // Account for scrollbars
        
        // Calculate the target size maintaining aspect ratio
        QSize targetSize = calculateAspectRatioSize(QSize(currentImage.width, currentImage.height), availableSize);
         
         // Scale the image with smooth transformation
         QPixmap scaledPixmap = pixmap.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
         
         // Set the pixmap and ensure the label size matches the scaled image
         ui.imageLabel->setPixmap(scaledPixmap);
         ui.imageLabel->setText("");
         ui.imageLabel->setScaledContents(false); // Disable automatic scaling
         ui.imageLabel->resize(scaledPixmap.size()); // Resize label to match pixmap
         
         // Calculate minimum window size to prevent scrollbars
         updateMinimumWindowSize();
        
        // Update status bar with image info
        double imageAspectRatio = static_cast<double>(currentImage.width) / currentImage.height;
        statusBar()->showMessage(QString("Image: %1x%2 | Display: %3x%4 | Aspect Ratio: %5")
            .arg(currentImage.width)
            .arg(currentImage.height)
            .arg(scaledPixmap.width())
            .arg(scaledPixmap.height())
            .arg(QString::number(imageAspectRatio, 'f', 2)));
        // Keep right-side properties in sync
        updatePropertiesPanel();
    }
    
    /**
     * @brief Crop the current image using a selection rectangle relative to the label.
     * @param selectionOnLabel The selection bounds in label coordinates.
     */
    void performCropFromSelection(const QRect &selectionOnLabel)
    {
        if (!hasImage) return;
        if (selectionOnLabel.width() <= 1 || selectionOnLabel.height() <= 1) return;
        
        // Determine the scale used to draw the pixmap on the label
        QSize scrollAreaSize = ui.scrollArea->size();
        QSize availableSize(scrollAreaSize.width() - 20, scrollAreaSize.height() - 20);
        
        // Calculate the target size maintaining aspect ratio
        QSize targetSize = calculateAspectRatioSize(QSize(currentImage.width, currentImage.height), availableSize);
        
        // Build the current displayed pixmap to get its actual size
        QPixmap pixmap = QPixmap::fromImage(buildQImage(currentImage));
        QPixmap scaledPixmap = pixmap.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        
        // The image is centered inside imageLabel.
        QSize labelSize = ui.imageLabel->size();
        int offsetX = (labelSize.width() - scaledPixmap.width()) / 2;
        int offsetY = (labelSize.height() - scaledPixmap.height()) / 2;
        
        // Intersect selection with the drawn image rect
        QRect drawnRect(offsetX, offsetY, scaledPixmap.width(), scaledPixmap.height());
        QRect selection = selectionOnLabel.intersected(drawnRect);
        if (selection.isEmpty()) return;
        
        // Map from label coords to image pixel coords
        double scaleX = static_cast<double>(currentImage.width) / scaledPixmap.width();
        double scaleY = static_cast<double>(currentImage.height) / scaledPixmap.height();
        
        int x0 = std::max(0, static_cast<int>((selection.left() - offsetX) * scaleX));
        int y0 = std::max(0, static_cast<int>((selection.top() - offsetY) * scaleY));
        int x1 = std::min((int)currentImage.width, static_cast<int>((selection.right() - offsetX + 1) * scaleX));
        int y1 = std::min((int)currentImage.height, static_cast<int>((selection.bottom() - offsetY + 1) * scaleY));
        
        int newW = std::max(1, x1 - x0);
        int newH = std::max(1, y1 - y0);
        
        if (newW <= 1 || newH <= 1) return;
        
        saveStateForUndo();
        Image result(newW, newH);
        for (int y = 0; y < newH; y++) {
            for (int x = 0; x < newW; x++) {
                for (int c = 0; c < 3; c++) {
                    result.setPixel(x, y, c, currentImage(x0 + x, y0 + y, c));
                }
            }
        }
        currentImage = result;
        updateImageDisplay();
        setActiveFilterValue("Crop");
        updatePropertiesPanel();
    }
    
    Image originalImage;
    Image currentImage;
    bool hasImage;
    QString currentFilePath;
    
    // Cancel mechanism
    std::atomic<bool> cancelRequested{false};
    Image preFilterImage; // Store image state before filter for cancellation
    
    // Undo/Redo system
    HistoryManager history{20};
    std::stack<QString> undoFilterNames; // Parallel stack for active filter names
    std::stack<QString> redoFilterNames; // Parallel stack for active filter names
    
    // Save state tracking
    bool hasUnsavedChanges = false;
    
    // Resize handling
    QTimer *resizeTimer;
    
    // Crop handling
    bool cropping = false;
    QRubberBand *rubberBand = nullptr;
    QPoint cropOrigin;
    
    // Image filters
    ImageFilters* imageFilters;

    // Helpers
    /**
     * @brief Set the active filter value in the properties panel.
     * 
     * Updates the active filter display in the properties panel to show
     * which filter was last applied to the image.
     * 
     * @param name The name of the filter to display
     * 
     * @note This method is used to maintain consistency between the actual
     *       filter applied and the displayed filter name in the UI.
     */
    void setActiveFilterValue(const QString &name)
    {
        ui.activeFilterValue->setText(name);
    }

    /**
     * @brief Format a byte count into a human-readable string.
     * 
     * Converts a byte count into a formatted string with appropriate units
     * (B, KB, MB, GB, TB) and proper decimal precision.
     * 
     * @param bytes The number of bytes to format
     * @return Formatted string with units (e.g., "1.5 MB", "256 B")
     * 
     * @details The formatting:
     * - Uses binary units (1024-based) for consistency with file systems
     * - Shows no decimal places for bytes, 2 decimal places for larger units
     * - Returns "—" for negative values
     * - Handles values up to TB (terabytes)
     * 
     * @note This is a static utility method used for displaying file sizes.
     */
    static QString formatBytes(qint64 bytes)
    {
        if (bytes < 0) return "—";
        static const char* suffixes[] = {"B", "KB", "MB", "GB", "TB"};
        double value = static_cast<double>(bytes);
        int i = 0;
        while (value >= 1024.0 && i < 4) { value /= 1024.0; ++i; }
        return QString::number(value, 'f', i == 0 ? 0 : 2) + " " + suffixes[i];
    }

    /**
     * @brief Update the properties panel with current image information.
     * 
     * Refreshes all the image properties displayed in the right-side panel,
     * including dimensions, file size, format, and color mode information.
     * 
     * @details This method updates:
     * - Image dimensions (width × height)
     * - File size (if file path is known)
     * - File format (if file path is known)
     * - Color mode (defaults to RGB if unset)
     * 
     * @note This method should be called whenever the image changes or
     *       when the properties panel needs to be refreshed.
     * @see formatBytes() for file size formatting
     */
    void updatePropertiesPanel()
    {
        if (!hasImage) return;
        // Dimensions
        ui.dimensionsValue->setText(QString("%1 × %2").arg(currentImage.width).arg(currentImage.height));
        // File size and format (if path known)
        if (!currentFilePath.isEmpty()) {
            QFileInfo fi(currentFilePath);
            ui.fileSizeValue->setText(formatBytes(fi.size()));
            ui.formatValue->setText(fi.suffix().toUpper());
        }
        // Default color mode if unset
        if (ui.colorModeValue->text().trimmed().isEmpty() || ui.colorModeValue->text() == "—") {
            ui.colorModeValue->setText("RGB");
        }
    }
    /**
     * @brief Save the current image using a file dialog.
     * 
     * Opens a save dialog to allow the user to choose a location and format
     * for saving the current image, then performs the save operation.
     * 
     * @return true if the image was saved successfully, false if cancelled or failed
     * 
     * @details This method:
     * - Opens a Qt file dialog for save location selection
     * - Handles file format selection (PNG, JPEG, BMP)
     * - Uses ImageIO for the actual save operation
     * - Updates the unsaved changes flag on successful save
     * - Updates the current file path and properties panel
     * - Provides user feedback through status bar messages
     * - Handles errors gracefully with error dialogs
     * 
     * @note This method is used internally by save operations and close events.
     * @see ImageIO::saveToFile() for the actual file writing
     * @see SAVE_FILTER for supported file formats
     */
    bool saveImageWithDialog()
    {
        QString fileName = QFileDialog::getSaveFileName(this,
            "Save Image", QDir::homePath(), SAVE_FILTER);
        if (fileName.isEmpty()) return false;
        try {
            ImageIO::saveToFile(currentImage, fileName);
            hasUnsavedChanges = false; // Mark as saved
            statusBar()->showMessage(QString("Saved: %1").arg(QFileInfo(fileName).fileName()));
            currentFilePath = fileName;
            updatePropertiesPanel();
            return true;
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Failed to save image: %1").arg(e.what()));
            return false;
        }
    }

    /**
     * @brief Reset the UI to the no-image state.
     * 
     * Resets all UI elements to their initial state when no image is loaded,
     * clearing history, disabling buttons, and resetting all displays.
     * 
     * @details This method:
     * - Clears undo/redo history and updates button states
     * - Resets the image label with placeholder text
     * - Updates minimum window size and disables all buttons
     * - Updates the status bar with appropriate message
     * - Clears all properties panel values
     * - Resets filter name tracking stacks
     * 
     * @note This method is called when unloading images or initializing
     *       the application to ensure a consistent UI state.
     * @see refreshButtons() for button state management
     * @see updatePropertiesPanel() for properties display
     */
    void resetUiToNoImageState()
    {
        // Clear undo/redo history
        history.clear();
        updateUndoRedoButtons();
        // Reset image label
        ui.imageLabel->clear();
        ui.imageLabel->setText("No image loaded\nClick 'Load Image' or drag & drop an image here");
        // Reset minimum window size and disable buttons
        updateMinimumWindowSize();
        refreshButtons(false);
        // Update status bar
        statusBar()->showMessage("Image unloaded - Ready to load a new image");
        // Clear right-side properties panel
        ui.dimensionsValue->setText("—");
        ui.fileSizeValue->setText("—");
        ui.colorModeValue->setText("—");
        ui.formatValue->setText("—");
        ui.activeFilterValue->setText("None");
        // Clear filter name stacks
        while (!undoFilterNames.empty()) undoFilterNames.pop();
        while (!redoFilterNames.empty()) redoFilterNames.pop();
    }

    /**
     * @brief Finalize the successful loading of an image.
     * 
     * Completes the image loading process by updating all UI elements,
     * clearing history, and setting up the application state for the new image.
     * 
     * @param filePath The path to the successfully loaded image file
     * @param viaDrop Whether the image was loaded via drag and drop
     * 
     * @details This method:
     * - Sets the current file path and clears unsaved changes flag
     * - Updates the image display and minimum window size
     * - Enables all buttons and clears undo/redo history
     * - Updates the status bar with appropriate message
     * - Resets filter and color mode displays
     * - Updates the properties panel
     * - Clears filter name tracking stacks
     * 
     * @note This method is called after successful image loading to ensure
     *       the application is in a consistent state.
     * @see loadImageFromPath() for the main loading implementation
     */
    void finalizeSuccessfulLoad(const QString &filePath, bool viaDrop)
    {
        currentFilePath = filePath;
        hasUnsavedChanges = false;
        updateImageDisplay();
        updateMinimumWindowSize();
        refreshButtons(true);
        history.clear();
        updateUndoRedoButtons();
        const QString baseName = QFileInfo(filePath).fileName();
        statusBar()->showMessage(viaDrop ? QString("Loaded via drag & drop: %1").arg(baseName)
                                         : QString("Loaded: %1").arg(baseName));
        ui.activeFilterValue->setText("None");
        ui.colorModeValue->setText("RGB");
        updatePropertiesPanel();
        // Reset filter name stacks
        while (!undoFilterNames.empty()) undoFilterNames.pop();
        while (!redoFilterNames.empty()) redoFilterNames.pop();
    }

    /**
     * @brief Load an image from the specified file path.
     * 
     * Loads an image from the given file path using the ImageIO class,
     * handling errors gracefully and updating the application state.
     * 
     * @param filePath The path to the image file to load
     * @param viaDrop Whether the image was loaded via drag and drop
     * 
     * @details This method:
     * - Uses ImageIO to load the image with proper error handling
     * - Sets both original and current image references
     * - Marks that an image is loaded
     * - Calls finalizeSuccessfulLoad() to complete the setup
     * - Shows error dialogs and status messages on failure
     * 
     * @note This method is the main entry point for image loading operations.
     * @see ImageIO::loadFromFile() for the actual file loading
     * @see finalizeSuccessfulLoad() for post-load setup
     * 
     * @throws std::exception if image loading fails (handled internally)
     */
    void loadImageFromPath(const QString &filePath, bool viaDrop)
    {
        try {
            originalImage = ImageIO::loadFromFile(filePath);
            currentImage = originalImage;
            hasImage = true;
            finalizeSuccessfulLoad(filePath, viaDrop);
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Failed to load image: %1").arg(e.what()));
            statusBar()->showMessage("Failed to load image");
        }
    }

    /**
     * @brief Show a dialog to get user input from a list of options.
     * 
     * Displays a Qt input dialog with a dropdown list of options for the user
     * to choose from. Returns the selected option or an empty string if cancelled.
     * 
     * @param title The dialog window title
     * @param label The label text for the dialog
     * @param options List of options to choose from
     * @return The selected option string, or empty string if cancelled
     * 
     * @note This is a utility method used throughout the application for
     *       getting user choices for various operations.
     * @see QInputDialog::getItem() for the underlying Qt dialog
     */
    QString getInputFromList(const QString &title, const QString &label, const QStringList &options)
    {
        bool ok;
        return QInputDialog::getItem(this, title, label, options, 0, false, &ok);
    }

    /**
     * @brief Show a dialog with a slider to get a percentage value from the user.
     * 
     * Creates a custom dialog with a horizontal slider (0-100%) and displays
     * the current value in real-time. Returns the selected percentage value.
     * 
     * @param title The dialog window title
     * @param label The label text for the dialog
     * @param defaultValue The initial slider value (clamped to 0-100)
     * @param ok Pointer to boolean to receive success status (can be nullptr)
     * @return The selected percentage value (0-100)
     * 
     * @details The dialog includes:
     * - A label with the prompt text
     * - A horizontal slider with range 0-100
     * - A value label showing current percentage
     * - OK and Cancel buttons
     * - Real-time value updates as slider moves
     * 
     * @note This is a utility method used for getting percentage values
     *       for operations like blur strength and brightness adjustment.
     * @see QSlider for the underlying slider widget
     */
    int getPercentWithSlider(const QString &title, const QString &label, int defaultValue, bool *ok)
    {
        // Build a tiny dialog with a slider 0-100 and OK/Cancel
        QDialog dialog(this);
        dialog.setWindowTitle(title);
        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        QLabel *lbl = new QLabel(label, &dialog);
        QSlider *slider = new QSlider(Qt::Horizontal, &dialog);
        slider->setRange(0, 100);
        slider->setValue(std::clamp(defaultValue, 0, 100));
        QLabel *valueLabel = new QLabel(QString::number(slider->value()) + "%", &dialog);
        QObject::connect(slider, &QSlider::valueChanged, &dialog, [valueLabel](int v){ valueLabel->setText(QString::number(v) + "%"); });
        QHBoxLayout *buttons = new QHBoxLayout();
        QPushButton *okBtn = new QPushButton("OK", &dialog);
        QPushButton *cancelBtn = new QPushButton("Cancel", &dialog);
        buttons->addStretch();
        buttons->addWidget(okBtn);
        buttons->addWidget(cancelBtn);
        layout->addWidget(lbl);
        layout->addWidget(slider);
        layout->addWidget(valueLabel);
        layout->addLayout(buttons);

        int resultPercent = slider->value();
        QObject::connect(okBtn, &QPushButton::clicked, &dialog, [&](){ resultPercent = slider->value(); dialog.accept(); });
        QObject::connect(cancelBtn, &QPushButton::clicked, &dialog, [&](){ dialog.reject(); });

        int res = dialog.exec();
        if (ok) *ok = (res == QDialog::Accepted);
        return resultPercent;
    }

    /**
     * @brief Calculate the optimal display size maintaining aspect ratio.
     * 
     * Determines the best size to display an image within the available space
     * while preserving the original aspect ratio and preventing upscaling.
     * 
     * @param imageSize The original image dimensions
     * @param availableSize The available display space
     * @return QSize representing the optimal display dimensions
     * 
     * @details The calculation:
     * - Compares image and available space aspect ratios
     * - Fits to width if image is wider than available space
     * - Fits to height if image is taller than available space
     * - Prevents upscaling by returning original size if larger
     * - Maintains exact aspect ratio throughout
     * 
     * @note This method is used for smart image display that prevents
     *       scrollbars while maintaining image quality.
     * @see updateImageDisplay() for usage in image display
     */
    QSize calculateAspectRatioSize(const QSize &imageSize, const QSize &availableSize)
    {
        double imageAspectRatio = static_cast<double>(imageSize.width()) / imageSize.height();
        double availableAspectRatio = static_cast<double>(availableSize.width()) / availableSize.height();
        
        QSize targetSize;
        if (imageAspectRatio > availableAspectRatio) {
            // Image is wider than available space - fit to width
            targetSize.setWidth(availableSize.width());
            targetSize.setHeight(static_cast<int>(availableSize.width() / imageAspectRatio));
        } else {
            // Image is taller than available space - fit to height
            targetSize.setHeight(availableSize.height());
            targetSize.setWidth(static_cast<int>(availableSize.height() * imageAspectRatio));
        }
        
        // Ensure we don't exceed the original image size (don't upscale)
        if (targetSize.width() > imageSize.width() || targetSize.height() > imageSize.height()) {
            targetSize = imageSize;
        }
        
        return targetSize;
    }
    /**
     * @brief Execute a filter operation with cancellation support.
     * 
     * Runs a filter operation that can be cancelled by the user, providing
     * progress tracking and the ability to restore the previous state.
     * 
     * @param filterCall Function to execute for the filter operation
     * 
     * @details This method:
     * - Resets the cancellation flag
     * - Saves the current image state for potential restoration
     * - Saves state for undo functionality
     * - Shows the cancel button for user interaction
     * - Executes the filter operation
     * - Updates the image display on success
     * - Handles errors gracefully with error dialogs
     * - Hides the cancel button when complete
     * 
     * @note This method is used for long-running operations that support
     *       cancellation, such as grayscale, blur, and infrared filters.
     * @see runSimpleFilter() for operations without cancellation support
     */
    void runCancelableFilter(const std::function<void()> &filterCall)
    {
        cancelRequested = false;
        preFilterImage = currentImage;
        saveStateForUndo();
        ui.cancelButton->setVisible(true);
        try {
            filterCall();
            updateImageDisplay();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Filter failed: %1").arg(e.what()));
        }
        ui.cancelButton->setVisible(false);
    }

    /**
     * @brief Execute a simple filter operation without cancellation support.
     * 
     * Runs a filter operation that completes quickly and doesn't require
     * cancellation support or progress tracking.
     * 
     * @param filterCall Function to execute for the filter operation
     * 
     * @details This method:
     * - Saves state for undo functionality
     * - Executes the filter operation
     * - Updates the image display on success
     * - Handles errors gracefully with error dialogs
     * 
     * @note This method is used for immediate operations that don't need
     *       cancellation support, such as flip, rotate, and edge detection.
     * @see runCancelableFilter() for operations with cancellation support
     */
    void runSimpleFilter(const std::function<void()> &filterCall)
    {
        saveStateForUndo();
        try {
            filterCall();
            updateImageDisplay();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Filter failed: %1").arg(e.what()));
        }
    }

    /**
     * @brief Convert an Image object to a QImage for Qt display.
     * 
     * Converts the internal Image data structure to a QImage that can be
     * displayed in Qt widgets. This method handles the conversion from
     * the custom Image format to Qt's standard image format.
     * 
     * @param img Reference to the Image object to convert
     * @return QImage object ready for Qt display
     * 
     * @details This method:
     * - Creates a QImage with RGB888 format
     * - Iterates through all pixels in the image
     * - Converts RGB values from the Image format to Qt format
     * - Returns a QImage ready for display or further processing
     * 
     * @note This method performs a complete copy of the image data and
     *       should be used efficiently for large images.
     * @see QImage for Qt image format details
     * @see Image class for the source image format
     */
    QImage buildQImage(const Image &img)
    {
        QImage qimg(img.width, img.height, QImage::Format_RGB888);
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                int r = img(x, y, 0);
                int g = img(x, y, 1);
                int b = img(x, y, 2);
                qimg.setPixel(x, y, qRgb(r, g, b));
            }
        }
        return qimg;
    }
};

/**
 * @brief Main entry point for the Image Studio application.
 * 
 * Initializes the Qt application, creates the main window, and starts the event loop.
 * This function sets up the complete application environment and handles the
 * application lifecycle from startup to shut down.
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return Application exit code (0 for success, non-zero for error)
 * 
 * @details The main function:
 * - Creates and configures the Qt application instance
 * - Sets the application window icon
 * - Creates the main PhotoSmith window
 * - Shows the window and starts the event loop
 * - Handles application shutdown gracefully
 * 
 * @note This function follows Qt's standard application structure and should
 *       be called only once per application instance.
 * @see QApplication for Qt application management
 * @see PhotoSmith for the main application window
 * 
 * @example
 * @code
 * int main(int argc, char *argv[]) {
 *     QApplication app(argc, argv);
 *     PhotoSmith window;
 *     window.show();
 *     return app.exec();
 * }
 * @endcode
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon("assets/icons/logo.png"));
    
    PhotoSmith window;
    window.show();
    
    return app.exec();
}

#include "photo_smith.moc"