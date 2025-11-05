/**
 * @file ColorWheelDialog.h
 * @brief RGB color wheel dialog for selecting colors in image filters.
 * 
 * This file contains the declaration of the ColorWheelDialog class, which provides
 * a user-friendly RGB color picker interface. The dialog includes:
 * - Visual color wheel representation
 * - RGB sliders (Red, Green, Blue)
 * - HSV sliders (Hue, Saturation, Value) for alternative color selection
 * - Real-time color preview
 * - Hex color code display and input
 * - Predefined color palette
 * 
 * @details The ColorWheelDialog is designed to be used by filters that require
 * color selection, such as:
 * - Color tint filters (replacing the purple filter)
 * - Frame color selection
 * - Color overlay effects
 * - Custom color adjustments
 * 
 * @author Team Members:
 * - Ahmed Mohamed ElSayed Tolba (ID: 20242023)
 * - Eyad Mohamed Saad Ali (ID: 20242062)
 * - Tarek Sami Mohamed Mohamed (ID: 20242190)
 * 
 * @institution Faculty of Computers and Artificial Intelligence, Cairo University
 * @version 1.0.0
 * @date October 2025
 * @copyright FCAI Cairo University
 */

#ifndef COLORWHEELDIALOG_H
#define COLORWHEELDIALOG_H

#include <QDialog>
#include <QColor>
#include <QSlider>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFrame>
#include <QString>
#include <QGroupBox>
#include <QPixmap>
#include <QPainter>

/**
 * @class ColorWheelDialog
 * @brief Dialog for selecting RGB colors using a color wheel interface.
 * 
 * The ColorWheelDialog provides a comprehensive color selection interface
 * with multiple ways to choose colors:
 * - RGB sliders (0-255 for each channel)
 * - HSV sliders for intuitive color selection
 * - Hex color code input
 * - Real-time color preview
 * - Predefined color swatches
 * 
 * @details The dialog returns the selected color via getSelectedColor()
 * and can be initialized with a default color.
 * 
 * @example
 * @code
 * ColorWheelDialog dialog(this);
 * dialog.setInitialColor(QColor(128, 0, 255)); // Purple
 * if (dialog.exec() == QDialog::Accepted) {
 *     QColor selectedColor = dialog.getSelectedColor();
 *     // Use selectedColor for filter
 * }
 * @endcode
 */
class ColorWheelDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a ColorWheelDialog.
     * 
     * @param parent Parent widget (typically the main window)
     * @param initialColor Initial color to display (default: white)
     * @param frameMode If true, shows frame type selector (default: false)
     * @param imageWidth Image width for calculating default frame width (default: 0)
     * @param imageHeight Image height for calculating default frame width (default: 0)
     */
    explicit ColorWheelDialog(QWidget *parent = nullptr, const QColor &initialColor = QColor(255, 255, 255), bool frameMode = false, int imageWidth = 0, int imageHeight = 0);

    /**
     * @brief Gets the currently selected color.
     * 
     * @return The selected QColor with RGB values
     */
    QColor getSelectedColor() const;

    /**
     * @brief Sets the initial color displayed in the dialog.
     * 
     * @param color The initial color to set
     */
    void setInitialColor(const QColor &color);

    /**
     * @brief Gets RGB components as separate integers.
     * 
     * @param r Reference to store red component (0-255)
     * @param g Reference to store green component (0-255)
     * @param b Reference to store blue component (0-255)
     */
    void getRGB(int &r, int &g, int &b) const;
    
    /**
     * @brief Gets the selected frame type (only available in frame mode).
     * 
     * @return The selected frame type string, or empty if not in frame mode
     */
    QString getFrameType() const;
    
    /**
     * @brief Gets the frame width (only available in frame mode).
     * 
     * @return The frame width in pixels
     */
    int getFrameWidth() const;

private slots:
    /**
     * @brief Updates color preview when RGB sliders change.
     */
    void updateFromRGB();

    /**
     * @brief Updates RGB sliders when HSV sliders change.
     */
    void updateFromHSV();

    /**
     * @brief Updates color from hex code input.
     */
    void updateFromHex();

    /**
     * @brief Updates hex code from current color.
     */
    void updateHex();
    
    /**
     * @brief Handles predefined color button clicks.
     */
    void onColorButtonClicked();

private:
    /**
     * @brief Initializes the dialog UI components.
     */
    void setupUI();

    /**
     * @brief Updates the color preview display.
     */
    void updateColorPreview();

    /**
     * @brief Converts HSV to RGB.
     */
    void hsvToRgb(int h, int s, int v, int &r, int &g, int &b) const;

    /**
     * @brief Converts RGB to HSV.
     */
    void rgbToHsv(int r, int g, int b, int &h, int &s, int &v) const;

    // UI Components
    QLabel *colorPreviewLabel;      ///< Preview of selected color
    QLabel *rgbLabel;                 ///< RGB values label
    QLabel *hsvLabel;                  ///< HSV values label
    QLabel *hexLabel;                 ///< Hex code label
    
    // RGB Controls
    QSlider *redSlider;               ///< Red channel slider (0-255)
    QSlider *greenSlider;             ///< Green channel slider (0-255)
    QSlider *blueSlider;              ///< Blue channel slider (0-255)
    QSpinBox *redSpinBox;            ///< Red channel spin box
    QSpinBox *greenSpinBox;          ///< Green channel spin box
    QSpinBox *blueSpinBox;           ///< Blue channel spin box
    
    // HSV Controls
    QSlider *hueSlider;              ///< Hue slider (0-360)
    QSlider *saturationSlider;        ///< Saturation slider (0-100)
    QSlider *valueSlider;             ///< Value slider (0-100)
    QSpinBox *hueSpinBox;             ///< Hue spin box
    QSpinBox *saturationSpinBox;      ///< Saturation spin box
    QSpinBox *valueSpinBox;           ///< Value spin box
    
    // Hex Input
    QLineEdit *hexLineEdit;           ///< Hex color code input field
    
    // Buttons
    QPushButton *okButton;            ///< OK button to accept color
    QPushButton *cancelButton;        ///< Cancel button
    
    // Current color
    QColor currentColor;              ///< Currently selected color
    
    // Flag to prevent circular updates
    bool updatingFromRGB;            ///< Flag to prevent RGB->HSV->RGB loops
    bool updatingFromHSV;             ///< Flag to prevent HSV->RGB->HSV loops
    
    // Frame mode components
    bool isFrameMode;                 ///< Whether dialog is in frame selection mode
    QComboBox *frameTypeCombo;        ///< Frame type selector (only in frame mode)
    QSpinBox *frameWidthSpinBox;      ///< Frame width selector (only in frame mode)
    int imageWidth;                    ///< Image width for calculating default frame width
    int imageHeight;                   ///< Image height for calculating default frame width
};

#endif // COLORWHEELDIALOG_H

