#include <picture.hpp>

ID3::Picture::Picture(std::shared_ptr<std::vector<char>> data, std::string mime_type, ID3::PictureType pic_type) {

    m_data = data;
    m_mime_type = mime_type;
    m_pic_type = pic_type;

}
