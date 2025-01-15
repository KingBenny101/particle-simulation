#ifndef UTILS_HPP
#define UTILS_HPP

#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <iostream>

namespace utils {
sf::Vector2f convect_eigen_vector(const Eigen::Vector2f vector)
{
    return sf::Vector2f(vector(0, 0), vector(1, 0));
}

float calc_eigen_vector_norm(const Eigen::Vector2f vector)
{
    return std::sqrt(vector(0, 0) * vector(0, 0) + vector(1, 0) * vector(1, 0));
}

void print_eigen_vector(const Eigen::Vector2f vector)
{
    std::cout << "\rx: " << vector(0, 0) << " y: " << vector(1, 0);
}

constexpr std::array<sf::Color, 7> base_palette = {
    sf::Color(75, 0, 130),    // Violet
    sf::Color(138, 43, 226),  // Indigo
    sf::Color(0, 0, 255),     // Blue
    sf::Color(0, 255, 0),     // Green
    sf::Color(255, 255, 0),   // Yellow
    sf::Color(255, 165, 0),   // Orange
    sf::Color(255, 0, 0)      // Red
};

constexpr std::size_t steps_per_segment = 32;
constexpr std::size_t gradient_palette_size = (base_palette.size() - 1) * steps_per_segment + 1;

constexpr std::array<sf::Color, gradient_palette_size> generate_gradient()
{
    std::array<sf::Color, gradient_palette_size> gradient{};
    std::size_t index = 0;

    for (std::size_t i = 0; i < base_palette.size() - 1; ++i) {
        sf::Color start = base_palette[i];
        sf::Color end = base_palette[i + 1];

        for (std::size_t step = 0; step < steps_per_segment; ++step) {
            float t = static_cast<float>(step) / steps_per_segment;
            uint8_t r = static_cast<uint8_t>(start.r + t * (end.r - start.r));
            uint8_t g = static_cast<uint8_t>(start.g + t * (end.g - start.g));
            uint8_t b = static_cast<uint8_t>(start.b + t * (end.b - start.b));

            gradient[index++] = sf::Color(r, g, b);
        }
    }

    gradient[index] = base_palette.back();

    return gradient;
}

constexpr std::array<sf::Color, gradient_palette_size> palette = generate_gradient();

inline int random_radius(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

}

#endif