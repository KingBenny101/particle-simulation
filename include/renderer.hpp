#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <Eigen/Dense>
#include <iostream>
#include <random>
#include <sfml/graphics.hpp>
#include <utils.hpp>
#include <vector>

#include "particle.hpp"

using Eigen::Vector2f;

class Renderer {
private:
    sf::RenderTarget& target;
    uint16_t total_particles = 400;
    uint16_t spawned_particles = 0;
    uint16_t spawn_every = 1;
    uint16_t spawn_counter = 0;
    std::vector<Particle> particles;
    sf::CircleShape boundary_circle;
    sf::Vector2u boundary_size;
    float radius;
    float width;
    float physics_time;
    float render_time;
    sf::Clock physics_clock;
    sf::Clock render_clock;
    sf::Clock fps_clock;
    sf::Font font;
    sf::Text fps_text;
    sf::Text physics_time_text;
    sf::Text render_time_text;
    sf::Text particle_count_text;
    float update_interval = 0.01;
    float substeps = 16;

public:
    Renderer(sf::RenderTarget& target_)
        : target { target_ }
        , fps_text(font)
        , physics_time_text(font)
        , render_time_text(font)
        , particle_count_text(font)
    {
        boundary_size = target.getSize();
        width = boundary_size.x;
        radius = boundary_size.x / 2;

        if (!font.openFromFile("./resources/fonts/Roboto-Regular.ttf")) {
            std::cerr << "Error loading font file" << std::endl;
            std::exit(1);
        }

        fps_text.setFont(font);
        fps_text.setCharacterSize(12);
        fps_text.setPosition({ width - 70, 10.f });
        fps_text.setFillColor(sf::Color::Green);

        physics_time_text.setFont(font);
        physics_time_text.setCharacterSize(12);
        physics_time_text.setPosition({ width - 215, 10.f });
        physics_time_text.setFillColor(sf::Color::Green);

        render_time_text.setFont(font);
        render_time_text.setCharacterSize(12);
        render_time_text.setPosition({ width - 350, 10.f });
        render_time_text.setFillColor(sf::Color::Green);

        particle_count_text.setFont(font);
        particle_count_text.setCharacterSize(12);
        particle_count_text.setPosition({ 10.f, 10.f });
        particle_count_text.setFillColor(sf::Color::Green);
    }

    void spawn_particle()
    {
        Vector2f pos = { radius, 200.f };
        Vector2f vel = { 1500.f, 1500.f };
        Vector2f angle = { 0.f, 0.f };
        const float angle_increment = 3.14159 / 32;
        if (spawned_particles < total_particles) {
            angle = { cos(spawned_particles * angle_increment), sin(spawned_particles * angle_increment) };
            vel = { 1000.f * angle(0, 0), 1000.f * angle(1, 0) };
            sf::Color color = utils::palette[spawned_particles % utils::gradient_palette_size];
            float radius = utils::random_radius(3, 15);
            particles.push_back(Particle(target, pos, vel, color, radius));
            spawned_particles++;
            set_particle_count();
        }
    }

    void set_particle_count()
    {
        std::ostringstream particle_count_stream;
        particle_count_stream << "Particles: " << spawned_particles;
        particle_count_text.setString(particle_count_stream.str());
    }

    void tick()
    {
        physics();
        render();
    }

    void physics()
    {
        physics_clock.restart();
        if (spawn_counter == spawn_every) {
            spawn_counter = 0;
            spawn_particle();
        }
        spawn_counter++;
        float sub_interval = update_interval / substeps;
        for (uint16_t i = 0; i < substeps; i++) {
            for (size_t i = 0; i < particles.size(); i++) {
                particles[i].update(sub_interval);
            }
            compute_collisions();
        }

        set_physics_time();
    }

    void set_physics_time()
    {
        sf::Time elapsed = physics_clock.restart();
        physics_time = elapsed.asSeconds();
        std::ostringstream physics_time_stream;
        physics_time_stream << "PhysicsTime: " << std::fixed << std::setprecision(3)
                            << physics_time * 1000.f << " ms";
        physics_time_text.setString(physics_time_stream.str());
    }

    void render()
    {
        render_clock.restart();
        draw_boundary_circle();

        for (size_t i = 0; i < particles.size(); i++) {
            particles[i].render();
        }

        target.draw(fps_text);
        target.draw(physics_time_text);
        target.draw(render_time_text);
        target.draw(particle_count_text);

        set_render_time();
        set_fps();
    }

    void set_render_time()
    {
        sf::Time elapsed = render_clock.restart();
        render_time = elapsed.asSeconds();
        std::ostringstream render_time_stream;
        render_time_stream << "RenderTime: " << std::fixed << std::setprecision(3)
                           << render_time * 1000.f << " ms";
        render_time_text.setString(render_time_stream.str());
    }

    void set_fps()
    {
        sf::Time elapsed = fps_clock.restart();
        float fps = 1.f / elapsed.asSeconds();
        std::ostringstream fps_stream;
        fps_stream << "FPS: " << static_cast<int>(fps);
        fps_text.setString(fps_stream.str());
    }

    void draw_boundary_circle()
    {
        target.clear(sf::Color(200, 200, 200));
        boundary_circle.setRadius(radius);
        boundary_circle.setPointCount(128);
        boundary_circle.setOrigin({ radius, radius });
        boundary_circle.setPosition({ radius, radius });
        boundary_circle.setFillColor(sf::Color::Black);
        target.draw(boundary_circle);
    }

    void compute_collisions()
    {
        for (size_t i = 0; i < particles.size(); i++) {
            for (size_t j = i + 1; j < particles.size(); j++) {
                if (particles[i].check_collision(particles[j])) {
                    particles[i].resolve_collision(particles[j]);
                }
            }
        }
    }

    void add_exploding_force(Vector2f pos)
    {
        constexpr float acc_factor = 5000;
        constexpr float interval = 5;
        for (size_t i = 0; i < particles.size(); i++) {
            Vector2f distance = (particles[i].get_pos() - pos);
            float distance_mag = utils::calc_eigen_vector_norm(distance);
            float magnitude = acc_factor / distance_mag;
            Vector2f acc = distance * magnitude / distance_mag;
            particles[i].apply_acceleration(acc, interval);
        }
    }
};

#endif
