#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>

inline bool ends_with(std::string const& value, std::string const& ending)
{
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::string rename(const std::string& filename)
{
    auto pos = filename.find_last_of('.');
    if (pos == std::string::npos)
    {
        std::cout << filename << " does not have a file extension!" << std::endl;
        exit(-2);
    }
    std::string first = filename.substr(0, pos);
    std::string second = filename.substr(pos);
    return first + "_border" + second;
}

const unsigned int TARGET_WIDTH = 1080 * 4;
const unsigned int TARGET_HEIGHT = 1080 * 4;

sf::Color avg(const sf::Image& image, unsigned int samples = 2000)
{
    double r = 0;
    double g = 0;
    double b = 0;

    for (unsigned int i = 0; i < samples; i++)
    {
        unsigned int x = rand() % image.getSize().x;
        unsigned int y = rand() % image.getSize().y;
        sf::Color color = image.getPixel({x, y});
        r += color.r;
        g += color.g;
        b += color.b;
    }
    r /= static_cast<double>(samples);
    g /= static_cast<double>(samples);
    b /= static_cast<double>(samples);

    const double darken = 2.5;
    r /= darken;
    g /= darken;
    b /= darken;

    return sf::Color(static_cast<std::uint8_t>(r), static_cast<std::uint8_t>(g), static_cast<std::uint8_t>(b));
}

bool add_border(const std::string& file, sf::RenderTexture& render_texture)
{

    sf::Image image;
    if (!image.loadFromFile(file))
    {
        std::cout << "Failed to load " << file << std::endl;
        return false;
    }
    unsigned int x = image.getSize().x;
    unsigned int y = image.getSize().y;

    const unsigned int TARGET_MAX = x > y ? TARGET_WIDTH : TARGET_HEIGHT;
    const unsigned int TARGET_MIN = x < y ? TARGET_WIDTH : TARGET_HEIGHT;

    const double BORDER = 100 * 4;
    const double HALF_BORDER = BORDER / 2.0;

    std::cout << x << " " << y << std::endl;

    unsigned int max_size = std::max(image.getSize().x, image.getSize().y);
    double factor = (TARGET_MAX - BORDER) / static_cast<double>(max_size);
    unsigned int min_size = std::min(image.getSize().x, image.getSize().y);
    double new_min = min_size * factor;
    double offset = std::abs(TARGET_MIN - new_min) / 2.0;
    std::cout << offset << std::endl;
    std::cout << factor << std::endl;
    sf::Color clear_color = avg(image);
    render_texture.clear(clear_color);
    sf::Texture texture;
    if (!texture.loadFromImage(image))
    {
        std::cout << "Failed to create texture for " << file << std::endl;
        return false;
    }
    texture.setSmooth(true);
    sf::Sprite sprite(texture);
    sprite.setScale({static_cast<float>(factor), static_cast<float>(factor)});
    if (x > y)
    {
        sprite.setPosition({static_cast<float>(HALF_BORDER), static_cast<float>(offset)});
    }
    else
    {
        sprite.setPosition({static_cast<float>(offset), static_cast<float>(HALF_BORDER)});
    }
    render_texture.draw(sprite);
    render_texture.display();
    sf::Image rendered = render_texture.getTexture().copyToImage();
    if (!rendered.saveToFile(rename(file)))
    {
        std::cout << "Failed to save " << rename(file) << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char** argv)
{
    srand(time(0));
    sf::RenderTexture render_texture({TARGET_WIDTH, TARGET_HEIGHT}, sf::ContextSettings{});
    for (int i = 1; i < argc; i++)
    {
        std::string file(argv[i]);
        std::cout << file << std::endl;
        if (!std::filesystem::is_regular_file(file))
        {
            std::cout << file << " does not exist or is not a regular file!" << std::endl;
        }
        else
        {
            auto pos = file.find_last_of('.');
            if (pos != std::string::npos)
            {
                std::string first = file.substr(0, pos);
                if (ends_with(first, "_border"))
                {
                    continue;
                }
            }

            if (add_border(file, render_texture))
            {
                std::filesystem::remove(file);
            }
        }
    }
}