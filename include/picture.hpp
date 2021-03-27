#ifndef PICTURE_HPP
#define PICTURE_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include <string>

namespace ID3 {

    // list of picture types
    typedef enum {

        OTHER = 0x00,
        FILE_ICON_32 = 0x01,
        OTHER_FILE_ICON = 0x02,
        COVER_FRONT = 0x03,
        COVER_BACK = 0x04,
        LEAFLET_PAGE = 0x05,
        MEDIA = 0x06,
        LEAD = 0x07,
        ARTIST = 0x08,
        CONDUCTOR = 0x09,
        BAND = 0x0a,
        COMPOSER = 0x0b,
        LYRICIST = 0x0c,
        RECORDING_LOCATION = 0x0d,
        DURING_RECORDING = 0x0e,
        DURING_PERFORMANCE = 0x0f,
        SCREEN_CAPTURE = 0x10,
        COLOURED_FISH = 0x11,
        ILLUSTRATION = 0x12,
        BAND_LOGOTYPE = 0x13,
        PUBLISHER_LOGOTYPE = 0x14

    } PictureType;

    class Picture
    {
    public:
        Picture(std::shared_ptr<std::vector<char>> data, std::string mime_type, ID3::PictureType pic_type);

        // picture data
        std::shared_ptr<std::vector<char>> m_data;

        // MIME type
        std::string m_mime_type;

        // type of picture (see PictureType)
        ID3::PictureType m_pic_type;

    };
}




#endif /* ifndef PICTURE_HPP */
