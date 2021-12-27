
#include "config/data/AppearanceConfig.hpp"
#include "config/data/keys.hpp"

#include <QFontDatabase>

AppearanceConfig::AppearanceConfig() :
    mPatternGridFont(),
    mPatternGridHeaderFont(),
    mOrderGridFont()
{
}

QFont AppearanceConfig::patternGridFont() const {
    return mPatternGridFont;
}


QFont AppearanceConfig::orderGridFont() const {
    return mOrderGridFont;
}

QFont AppearanceConfig::patternGridHeaderFont() const {
    return mPatternGridHeaderFont;
}

void AppearanceConfig::setPatternGridFont(const QFont &font) {
    mPatternGridFont = font;
}

void AppearanceConfig::setPatternGridHeaderFont(const QFont &font) {
    mPatternGridHeaderFont = font;
}

void AppearanceConfig::setOrderGridFont(const QFont &font) {
    mOrderGridFont = font;
}

void AppearanceConfig::readSettings(QSettings &settings) {

    settings.beginGroup(Keys::Appearance);

    auto setFont = [](QFont &font, QString const& fontstr) {
        if (fontstr.isEmpty() || !font.fromString(fontstr)) {
            // use the system default fixed-width font
            font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
            font.setPointSize(12);
        }
    };

    setFont(mPatternGridFont, settings.value(Keys::patternGridFont).toString());
    setFont(mOrderGridFont, settings.value(Keys::orderGridFont).toString());
    setFont(mPatternGridHeaderFont, settings.value(Keys::patternGridHeaderFont).toString());

    settings.endGroup();

}

void AppearanceConfig::writeSettings(QSettings &settings) const {

    settings.beginGroup(Keys::Appearance);

    settings.setValue(Keys::patternGridFont, mPatternGridFont.toString());
    settings.setValue(Keys::orderGridFont, mOrderGridFont.toString());
    settings.setValue(Keys::patternGridHeaderFont, mPatternGridHeaderFont.toString());

    settings.endGroup();
}
