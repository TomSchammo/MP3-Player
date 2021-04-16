#include <picture.hpp>

ID3::Picture::Picture(std::shared_ptr<std::vector<char>> t_data, std::string t_mime_type, ID3::PictureType t_pic_type) {

    m_data = t_data;
    m_mime_type = t_mime_type;
    m_pic_type = t_pic_type;

}
