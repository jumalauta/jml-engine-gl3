#include "Font.h"

#include "Settings.h"

Font::Font(std::string filePath) : File(filePath) {
    //Font files may be large, 1 second grace period should be OK
    setModifyGracePeriod(Settings::gui.largeFileModifyGracePeriod);
}
