#include <picture.hpp>
#include <utility>

ID3::Picture::Picture(std::shared_ptr<std::vector<char>> t_data, std::string t_mime_type, ID3::PictureType t_pic_type) noexcept :
                      m_data(std::move(t_data)), m_mime_type(std::move(t_mime_type)), m_pic_type(t_pic_type) {}
