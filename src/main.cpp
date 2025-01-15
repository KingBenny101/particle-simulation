#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <atomic>
#include <mutex>
#include <queue>
#include <thread>

#include "renderer.hpp"

std::queue<sf::Image> image_queue;
std::mutex queue_mutex;
std::atomic<bool> running(true);

void save_images()
{
    size_t frame_count = 0;
    while (running) {
        sf::Image image;
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            if (!image_queue.empty()) {
                image = image_queue.front();
                image_queue.pop();
            }
        }
        if (!image.getSize().x)
            continue;
        std::ostringstream filename;
        filename << "./frames/frame_" << std::setw(5) << std::setfill('0') << frame_count++ << ".png";

        if (image.saveToFile(filename.str())) {
            std::cout << "Saved: " << filename.str() << std::endl;
        }
    }
}

int main()
{
    constexpr uint16_t window_width = 840;
    constexpr uint16_t window_height = 840;
    const sf::String window_title = "Particle Simulation";
    constexpr uint16_t frame_rate = 100;

    auto settings = sf::ContextSettings();
    settings.antiAliasingLevel = 16;

    auto window = sf::RenderWindow(
        sf::VideoMode({ window_width, window_height }), window_title,
        sf::Style::Titlebar | sf::Style::Close, sf::State::Windowed, settings);
    window.setFramerateLimit(frame_rate);

    auto renderer = Renderer(window);


    // sf::Texture texture;
    // texture.resize({ window_width, window_height });
    // std::thread save_thread(save_images);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (event->is<sf::Event::MouseButtonPressed>()) {
                auto mouse_pos = sf::Mouse::getPosition(window);
                Eigen::Vector2f mouse_pos_eigen = { static_cast<float>(mouse_pos.x),
                    static_cast<float>(mouse_pos.y) };

                Eigen::Vector2f vel = { window_width / 2, window_height / 2 };
                vel = (vel - mouse_pos_eigen).normalized() * 100.f;
                // renderer.spawn_particle(mouse_pos_eigen,vel);
                renderer.add_exploding_force(mouse_pos_eigen);
            }
        }

        window.clear(sf::Color::Black);

        renderer.tick();

        window.display();

      
        // {
        //     texture.update(window);
        //     std::lock_guard<std::mutex> lock(queue_mutex);
        //     image_queue.push(texture.copyToImage());
        // }
    }

    // running = false;
    // save_thread.join();
}
