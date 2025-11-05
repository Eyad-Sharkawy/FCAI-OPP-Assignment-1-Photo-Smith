/**
 * @file ColorWheelDialog.cpp
 * @brief Implementation of RGB color wheel dialog for image filters.
 * 
 * This file contains the complete implementation of the ColorWheelDialog class,
 * providing a user-friendly interface for selecting colors with RGB, HSV, and
 * hex code inputs.
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

#include "ColorWheelDialog.h"
#include <QGroupBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QComboBox>
#include <QPixmap>
#include <QPainter>
#include <cmath>

ColorWheelDialog::ColorWheelDialog(QWidget *parent, const QColor &initialColor, bool frameMode, int imageWidth, int imageHeight)
    : QDialog(parent), currentColor(initialColor), updatingFromRGB(false), updatingFromHSV(false), isFrameMode(frameMode),
      frameTypeCombo(nullptr), frameWidthSpinBox(nullptr), imageWidth(imageWidth), imageHeight(imageHeight)
{
    setWindowTitle(frameMode ? "Select Frame Style & Color" : "Select Color");
    setModal(true);
    resize(450, frameMode ? 650 : 550);
    
    setupUI();
    setInitialColor(initialColor);
}

void ColorWheelDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Color Preview
    QGroupBox *previewGroup = new QGroupBox("Color Preview", this);
    QHBoxLayout *previewLayout = new QHBoxLayout(previewGroup);
    
    colorPreviewLabel = new QLabel(this);
    colorPreviewLabel->setMinimumSize(200, 80);
    colorPreviewLabel->setStyleSheet("border: 2px solid #333; border-radius: 4px;");
    colorPreviewLabel->setAlignment(Qt::AlignCenter);
    
    rgbLabel = new QLabel(this);
    rgbLabel->setText("RGB: (255, 255, 255)");
    
    hsvLabel = new QLabel(this);
    hsvLabel->setText("HSV: (0, 0%, 100%)");
    
    hexLabel = new QLabel(this);
    hexLabel->setText("Hex: #FFFFFF");
    
    QVBoxLayout *previewInfoLayout = new QVBoxLayout();
    previewInfoLayout->addWidget(rgbLabel);
    previewInfoLayout->addWidget(hsvLabel);
    previewInfoLayout->addWidget(hexLabel);
    previewInfoLayout->addStretch();
    
    previewLayout->addWidget(colorPreviewLabel);
    previewLayout->addLayout(previewInfoLayout);
    mainLayout->addWidget(previewGroup);
    
    // RGB Controls
    QGroupBox *rgbGroup = new QGroupBox("RGB", this);
    QGridLayout *rgbLayout = new QGridLayout(rgbGroup);
    
    redSlider = new QSlider(Qt::Horizontal, this);
    redSlider->setRange(0, 255);
    redSlider->setValue(255);
    greenSlider = new QSlider(Qt::Horizontal, this);
    greenSlider->setRange(0, 255);
    greenSlider->setValue(255);
    blueSlider = new QSlider(Qt::Horizontal, this);
    blueSlider->setRange(0, 255);
    blueSlider->setValue(255);
    
    redSpinBox = new QSpinBox(this);
    redSpinBox->setRange(0, 255);
    redSpinBox->setValue(255);
    greenSpinBox = new QSpinBox(this);
    greenSpinBox->setRange(0, 255);
    greenSpinBox->setValue(255);
    blueSpinBox = new QSpinBox(this);
    blueSpinBox->setRange(0, 255);
    blueSpinBox->setValue(255);
    
    rgbLayout->addWidget(new QLabel("Red:", this), 0, 0);
    rgbLayout->addWidget(redSlider, 0, 1);
    rgbLayout->addWidget(redSpinBox, 0, 2);
    rgbLayout->addWidget(new QLabel("Green:", this), 1, 0);
    rgbLayout->addWidget(greenSlider, 1, 1);
    rgbLayout->addWidget(greenSpinBox, 1, 2);
    rgbLayout->addWidget(new QLabel("Blue:", this), 2, 0);
    rgbLayout->addWidget(blueSlider, 2, 1);
    rgbLayout->addWidget(blueSpinBox, 2, 2);
    
    mainLayout->addWidget(rgbGroup);
    
    // HSV Controls
    QGroupBox *hsvGroup = new QGroupBox("HSV", this);
    QGridLayout *hsvLayout = new QGridLayout(hsvGroup);
    
    hueSlider = new QSlider(Qt::Horizontal, this);
    hueSlider->setRange(0, 360);
    saturationSlider = new QSlider(Qt::Horizontal, this);
    saturationSlider->setRange(0, 100);
    valueSlider = new QSlider(Qt::Horizontal, this);
    valueSlider->setRange(0, 100);
    
    hueSpinBox = new QSpinBox(this);
    hueSpinBox->setRange(0, 360);
    saturationSpinBox = new QSpinBox(this);
    saturationSpinBox->setRange(0, 100);
    valueSpinBox = new QSpinBox(this);
    valueSpinBox->setRange(0, 100);
    
    hsvLayout->addWidget(new QLabel("Hue:", this), 0, 0);
    hsvLayout->addWidget(hueSlider, 0, 1);
    hsvLayout->addWidget(hueSpinBox, 0, 2);
    hsvLayout->addWidget(new QLabel("Saturation:", this), 1, 0);
    hsvLayout->addWidget(saturationSlider, 1, 1);
    hsvLayout->addWidget(saturationSpinBox, 1, 2);
    hsvLayout->addWidget(new QLabel("Value:", this), 2, 0);
    hsvLayout->addWidget(valueSlider, 2, 1);
    hsvLayout->addWidget(valueSpinBox, 2, 2);
    
    mainLayout->addWidget(hsvGroup);
    
    // Hex Input
    QGroupBox *hexGroup = new QGroupBox("Hex Code", this);
    QHBoxLayout *hexLayout = new QHBoxLayout(hexGroup);
    hexLayout->addWidget(new QLabel("Hex:", this));
    hexLineEdit = new QLineEdit(this);
    hexLineEdit->setPlaceholderText("#RRGGBB");
    hexLineEdit->setMaxLength(7);
    hexLayout->addWidget(hexLineEdit);
    mainLayout->addWidget(hexGroup);
    
    // Quick Colors - Common colors for easy selection
    QGroupBox *presetGroup = new QGroupBox("Quick Colors", this);
    QGridLayout *presetLayout = new QGridLayout(presetGroup);
    
    // More relevant and commonly used colors
    QColor presetColors[] = {
        QColor(255, 255, 255),  // White
        QColor(0, 0, 0),        // Black
        QColor(255, 0, 0),     // Red
        QColor(0, 255, 0),      // Green
        QColor(0, 0, 255),      // Blue
        QColor(255, 255, 0),    // Yellow
        QColor(255, 0, 255),    // Magenta
        QColor(0, 255, 255),    // Cyan
        QColor(255, 128, 0),    // Orange
        QColor(128, 0, 128),    // Purple
        QColor(128, 128, 128),  // Gray
        QColor(255, 192, 203),  // Pink
        QColor(139, 69, 19),    // Brown
        QColor(255, 20, 147),   // Deep Pink
        QColor(0, 128, 128),    // Teal
        QColor(255, 165, 0)     // Orange (Golden)
    };
    
    for (int i = 0; i < 16; ++i) {
        QPushButton *colorBtn = new QPushButton(this);
        colorBtn->setFixedSize(35, 35);
        colorBtn->setStyleSheet(QString("background-color: %1; border: 2px solid #555; border-radius: 4px;")
                               .arg(presetColors[i].name()));
        colorBtn->setProperty("color", presetColors[i]);
        connect(colorBtn, &QPushButton::clicked, this, &ColorWheelDialog::onColorButtonClicked);
        presetLayout->addWidget(colorBtn, i / 4, i % 4);
    }
    
    mainLayout->addWidget(presetGroup);
    
    // Frame Mode Controls
    if (isFrameMode) {
        QGroupBox *frameGroup = new QGroupBox("Frame Settings", this);
        QVBoxLayout *frameMainLayout = new QVBoxLayout(frameGroup);
        
        QFormLayout *frameLayout = new QFormLayout();
        
        frameTypeCombo = new QComboBox(this);
        frameTypeCombo->addItem("Solid Frame");
        frameTypeCombo->addItem("Simple Frame");
        frameTypeCombo->addItem("Double Border");
        frameTypeCombo->addItem("Shadow Frame");
        frameTypeCombo->addItem("Gold Decorated Frame");
        frameTypeCombo->addItem("Decorated Frame");
        frameLayout->addRow("Frame Type:", frameTypeCombo);
        
        frameWidthSpinBox = new QSpinBox(this);
        frameWidthSpinBox->setRange(1, 100);
        
        // Calculate default frame width based on image dimensions
        // Use 2-3% of the smaller dimension, with reasonable min/max bounds
        int defaultWidth = 20; // Default fallback
        if (imageWidth > 0 && imageHeight > 0) {
            int smallerDim = std::min(imageWidth, imageHeight);
            // Use 2.5% of smaller dimension, clamped between 5 and 50 pixels
            defaultWidth = std::max(5, std::min(50, static_cast<int>(smallerDim * 0.025)));
        }
        
        frameWidthSpinBox->setValue(defaultWidth);
        frameLayout->addRow("Frame Width (px):", frameWidthSpinBox);
        
        frameMainLayout->addLayout(frameLayout);
        
        mainLayout->addWidget(frameGroup);
    }
    
    // Buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, this);
    okButton = buttonBox->button(QDialogButtonBox::StandardButton::Ok);
    cancelButton = buttonBox->button(QDialogButtonBox::StandardButton::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
    
    // Connect signals
    connect(redSlider, &QSlider::valueChanged, redSpinBox, &QSpinBox::setValue);
    connect(redSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), redSlider, &QSlider::setValue);
    connect(redSlider, &QSlider::valueChanged, this, &ColorWheelDialog::updateFromRGB);
    connect(redSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorWheelDialog::updateFromRGB);
    
    connect(greenSlider, &QSlider::valueChanged, greenSpinBox, &QSpinBox::setValue);
    connect(greenSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), greenSlider, &QSlider::setValue);
    connect(greenSlider, &QSlider::valueChanged, this, &ColorWheelDialog::updateFromRGB);
    connect(greenSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorWheelDialog::updateFromRGB);
    
    connect(blueSlider, &QSlider::valueChanged, blueSpinBox, &QSpinBox::setValue);
    connect(blueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), blueSlider, &QSlider::setValue);
    connect(blueSlider, &QSlider::valueChanged, this, &ColorWheelDialog::updateFromRGB);
    connect(blueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorWheelDialog::updateFromRGB);
    
    connect(hueSlider, &QSlider::valueChanged, hueSpinBox, &QSpinBox::setValue);
    connect(hueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), hueSlider, &QSlider::setValue);
    connect(hueSlider, &QSlider::valueChanged, this, &ColorWheelDialog::updateFromHSV);
    connect(hueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorWheelDialog::updateFromHSV);
    
    connect(saturationSlider, &QSlider::valueChanged, saturationSpinBox, &QSpinBox::setValue);
    connect(saturationSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), saturationSlider, &QSlider::setValue);
    connect(saturationSlider, &QSlider::valueChanged, this, &ColorWheelDialog::updateFromHSV);
    connect(saturationSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorWheelDialog::updateFromHSV);
    
    connect(valueSlider, &QSlider::valueChanged, valueSpinBox, &QSpinBox::setValue);
    connect(valueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), valueSlider, &QSlider::setValue);
    connect(valueSlider, &QSlider::valueChanged, this, &ColorWheelDialog::updateFromHSV);
    connect(valueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorWheelDialog::updateFromHSV);
    
    connect(hexLineEdit, &QLineEdit::textChanged, this, &ColorWheelDialog::updateFromHex);
    
    updateColorPreview();
}

void ColorWheelDialog::setInitialColor(const QColor &color)
{
    currentColor = color;
    
    // Update RGB controls
    updatingFromRGB = true;
    redSlider->setValue(color.red());
    greenSlider->setValue(color.green());
    blueSlider->setValue(color.blue());
    
    // Update spinboxes
    redSpinBox->blockSignals(true);
    greenSpinBox->blockSignals(true);
    blueSpinBox->blockSignals(true);
    redSpinBox->setValue(color.red());
    greenSpinBox->setValue(color.green());
    blueSpinBox->setValue(color.blue());
    redSpinBox->blockSignals(false);
    greenSpinBox->blockSignals(false);
    blueSpinBox->blockSignals(false);
    updatingFromRGB = false;
    
    // Update HSV controls
    int h, s, v;
    rgbToHsv(color.red(), color.green(), color.blue(), h, s, v);
    updatingFromHSV = true;
    hueSlider->setValue(h);
    saturationSlider->setValue(s);
    valueSlider->setValue(v);
    
    // Update HSV spinboxes
    hueSpinBox->blockSignals(true);
    saturationSpinBox->blockSignals(true);
    valueSpinBox->blockSignals(true);
    hueSpinBox->setValue(h);
    saturationSpinBox->setValue(s);
    valueSpinBox->setValue(v);
    hueSpinBox->blockSignals(false);
    saturationSpinBox->blockSignals(false);
    valueSpinBox->blockSignals(false);
    updatingFromHSV = false;
    
    // Update hex and previews
    updateHex();
    updateColorPreview();
}

QColor ColorWheelDialog::getSelectedColor() const
{
    return currentColor;
}

void ColorWheelDialog::getRGB(int &r, int &g, int &b) const
{
    r = currentColor.red();
    g = currentColor.green();
    b = currentColor.blue();
}

void ColorWheelDialog::updateFromRGB()
{
    if (updatingFromRGB) return;
    
    // Get values from both slider and spinbox (they should be in sync, but use slider as source)
    int r = redSlider->value();
    int g = greenSlider->value();
    int b = blueSlider->value();
    
    // Ensure spinboxes are in sync
    if (redSpinBox->value() != r) {
        redSpinBox->blockSignals(true);
        redSpinBox->setValue(r);
        redSpinBox->blockSignals(false);
    }
    if (greenSpinBox->value() != g) {
        greenSpinBox->blockSignals(true);
        greenSpinBox->setValue(g);
        greenSpinBox->blockSignals(false);
    }
    if (blueSpinBox->value() != b) {
        blueSpinBox->blockSignals(true);
        blueSpinBox->setValue(b);
        blueSpinBox->blockSignals(false);
    }
    
    currentColor.setRgb(r, g, b);
    
    updatingFromHSV = true;
    int h, s, v;
    rgbToHsv(r, g, b, h, s, v);
    hueSlider->setValue(h);
    saturationSlider->setValue(s);
    valueSlider->setValue(v);
    if (hueSpinBox->value() != h) {
        hueSpinBox->blockSignals(true);
        hueSpinBox->setValue(h);
        hueSpinBox->blockSignals(false);
    }
    if (saturationSpinBox->value() != s) {
        saturationSpinBox->blockSignals(true);
        saturationSpinBox->setValue(s);
        saturationSpinBox->blockSignals(false);
    }
    if (valueSpinBox->value() != v) {
        valueSpinBox->blockSignals(true);
        valueSpinBox->setValue(v);
        valueSpinBox->blockSignals(false);
    }
    updatingFromHSV = false;
    
    updateHex();
    updateColorPreview();
}

void ColorWheelDialog::updateFromHSV()
{
    if (updatingFromHSV) return;
    
    // Get values from both slider and spinbox (they should be in sync, but use slider as source)
    int h = hueSlider->value();
    int s = saturationSlider->value();
    int v = valueSlider->value();
    
    // Ensure spinboxes are in sync (block signals to avoid recursion)
    if (hueSpinBox->value() != h) {
        hueSpinBox->blockSignals(true);
        hueSpinBox->setValue(h);
        hueSpinBox->blockSignals(false);
    }
    if (saturationSpinBox->value() != s) {
        saturationSpinBox->blockSignals(true);
        saturationSpinBox->setValue(s);
        saturationSpinBox->blockSignals(false);
    }
    if (valueSpinBox->value() != v) {
        valueSpinBox->blockSignals(true);
        valueSpinBox->setValue(v);
        valueSpinBox->blockSignals(false);
    }
    
    int r, g, b;
    hsvToRgb(h, s, v, r, g, b);
    
    currentColor.setRgb(r, g, b);
    
    updatingFromRGB = true;
    redSlider->setValue(r);
    greenSlider->setValue(g);
    blueSlider->setValue(b);
    
    // Update RGB spinboxes
    if (redSpinBox->value() != r) {
        redSpinBox->blockSignals(true);
        redSpinBox->setValue(r);
        redSpinBox->blockSignals(false);
    }
    if (greenSpinBox->value() != g) {
        greenSpinBox->blockSignals(true);
        greenSpinBox->setValue(g);
        greenSpinBox->blockSignals(false);
    }
    if (blueSpinBox->value() != b) {
        blueSpinBox->blockSignals(true);
        blueSpinBox->setValue(b);
        blueSpinBox->blockSignals(false);
    }
    updatingFromRGB = false;
    
    updateHex();
    updateColorPreview();
}

void ColorWheelDialog::updateFromHex()
{
    QString hex = hexLineEdit->text().trimmed();
    if (hex.startsWith('#')) {
        hex = hex.mid(1);
    }
    
    if (hex.length() == 6) {
        bool ok;
        int r = hex.mid(0, 2).toInt(&ok, 16);
        if (!ok) return;
        int g = hex.mid(2, 2).toInt(&ok, 16);
        if (!ok) return;
        int b = hex.mid(4, 2).toInt(&ok, 16);
        if (!ok) return;
        
        setInitialColor(QColor(r, g, b));
    }
}

void ColorWheelDialog::updateHex()
{
    if (!hexLineEdit) return;
    
    QString hex = QString("#%1%2%3")
        .arg(currentColor.red(), 2, 16, QChar('0'))
        .arg(currentColor.green(), 2, 16, QChar('0'))
        .arg(currentColor.blue(), 2, 16, QChar('0'))
        .toUpper();
    
    // Always update hex value - block signals to prevent recursion
    hexLineEdit->blockSignals(true);
    hexLineEdit->setText(hex);
    hexLineEdit->blockSignals(false);
}

void ColorWheelDialog::updateColorPreview()
{
    QString style = QString("background-color: %1; border: 2px solid #333; border-radius: 4px;")
        .arg(currentColor.name());
    colorPreviewLabel->setStyleSheet(style);
    
    // Update RGB label
    rgbLabel->setText(QString("RGB: (%1, %2, %3)")
        .arg(currentColor.red())
        .arg(currentColor.green())
        .arg(currentColor.blue()));
    
    // Update HSV label
    int h, s, v;
    rgbToHsv(currentColor.red(), currentColor.green(), currentColor.blue(), h, s, v);
    hsvLabel->setText(QString("HSV: (%1°, %2%, %3%)")
        .arg(h)
        .arg(s)
        .arg(v));
    
    // Update Hex label
    QString hex = QString("#%1%2%3")
        .arg(currentColor.red(), 2, 16, QChar('0'))
        .arg(currentColor.green(), 2, 16, QChar('0'))
        .arg(currentColor.blue(), 2, 16, QChar('0'))
        .toUpper();
    hexLabel->setText(QString("Hex: %1").arg(hex));
    
}


void ColorWheelDialog::onColorButtonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        QColor color = btn->property("color").value<QColor>();
        setInitialColor(color);
    }
}

void ColorWheelDialog::hsvToRgb(int h, int s, int v, int &r, int &g, int &b) const
{
    // Convert HSV to RGB
    // h: 0-360, s: 0-100, v: 0-100
    // Output: r, g, b: 0-255
    
    double hh = h / 60.0;
    double ss = s / 100.0;
    double vv = v / 100.0;
    
    int i = static_cast<int>(std::floor(hh));
    double f = hh - i;
    double p = vv * (1.0 - ss);
    double q = vv * (1.0 - ss * f);
    double t = vv * (1.0 - ss * (1.0 - f));
    
    double rr, gg, bb;
    switch (i % 6) {
        case 0: rr = vv; gg = t; bb = p; break;
        case 1: rr = q; gg = vv; bb = p; break;
        case 2: rr = p; gg = vv; bb = t; break;
        case 3: rr = p; gg = q; bb = vv; break;
        case 4: rr = t; gg = p; bb = vv; break;
        default: rr = vv; gg = p; bb = q; break;
    }
    
    r = static_cast<int>(std::round(rr * 255));
    g = static_cast<int>(std::round(gg * 255));
    b = static_cast<int>(std::round(bb * 255));
    
    r = std::max(0, std::min(255, r));
    g = std::max(0, std::min(255, g));
    b = std::max(0, std::min(255, b));
}

void ColorWheelDialog::rgbToHsv(int r, int g, int b, int &h, int &s, int &v) const
{
    // Convert RGB to HSV
    // Input: r, g, b: 0-255
    // Output: h: 0-360, s: 0-100, v: 0-100
    
    double rr = r / 255.0;
    double gg = g / 255.0;
    double bb = b / 255.0;
    
    double max = std::max({rr, gg, bb});
    double min = std::min({rr, gg, bb});
    double delta = max - min;
    
    v = static_cast<int>(std::round(max * 100));
    
    if (max < 0.0001) {
        s = 0;
        h = 0;
        return;
    }
    
    s = static_cast<int>(std::round((delta / max) * 100));
    
    if (delta < 0.0001) {
        h = 0;
        return;
    }
    
    if (max == rr) {
        h = static_cast<int>(std::round(60.0 * (((gg - bb) / delta) + (gg < bb ? 6.0 : 0.0))));
    } else if (max == gg) {
        h = static_cast<int>(std::round(60.0 * (((bb - rr) / delta) + 2.0)));
    } else {
        h = static_cast<int>(std::round(60.0 * (((rr - gg) / delta) + 4.0)));
    }
    
    h = h % 360;
    if (h < 0) h += 360;
}

QString ColorWheelDialog::getFrameType() const
{
    if (!isFrameMode || !frameTypeCombo) {
        return QString();
    }
    return frameTypeCombo->currentText();
}

int ColorWheelDialog::getFrameWidth() const
{
    if (!isFrameMode || !frameWidthSpinBox) {
        return 20; // Default
    }
    return frameWidthSpinBox->value();
}

