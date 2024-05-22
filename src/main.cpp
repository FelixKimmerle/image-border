#include <iostream>
#include <filesystem>
#include <SFML/Graphics.hpp>

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::string rename(const std::string &filename)
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

sf::Color avg(const sf::Image &image, unsigned int samples = 2000)
{
    double r = 0;
    double g = 0;
    double b = 0;

    for(unsigned int i = 0; i < samples; i++)
    {
        unsigned int x = rand() % image.getSize().x;
        unsigned int y = rand() % image.getSize().y;
        sf::Color color = image.getPixel(x,y);
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

    return sf::Color(r, g, b);
}

void add_border(std::string file, sf::RenderTexture &render_texture)
{

    sf::Image image;
    image.loadFromFile(file);
    unsigned int x = image.getSize().x;
    unsigned int y = image.getSize().y;

    const unsigned int TARGET_MAX = x > y ? TARGET_WIDTH : TARGET_HEIGHT;
    const unsigned int TARGET_MIN = x < y ? TARGET_WIDTH : TARGET_HEIGHT;

    const double BORDER = 100*4;
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
    texture.loadFromImage(image);
    texture.setSmooth(true);
    sf::Sprite sprite(texture);
    sprite.setScale(factor, factor);
    if (x > y)
    {
        sprite.setPosition(HALF_BORDER, offset);
    }
    else
    {
        sprite.setPosition(offset, HALF_BORDER);
    }
    render_texture.draw(sprite);
    render_texture.display();
    sf::Image rendered = render_texture.getTexture().copyToImage();
    rendered.saveToFile(rename(file));
}

int main(int argc, char **argv)
{
    srand(time(0));
    sf::ContextSettings cs(0, 0, 0);
    sf::RenderTexture render_texture;
    render_texture.create(TARGET_WIDTH, TARGET_HEIGHT, cs);
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
                if(ends_with(first, "_border"))
                {
                    continue;
                }
            }
            
            add_border(file, render_texture);
            std::filesystem::remove(file);
        }
    }
}