#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <Eigen/Dense>
#include <iostream>
#include <sfml/graphics.hpp>

#include "utils.hpp"

using Eigen::Vector2f;

class Particle {
private:
    sf::RenderTarget& target;
    sf::Vector2u window_size;
    sf::CircleShape circle;
    sf::Color color;
    Vector2f pos;
    Vector2f vel;
    Vector2f acc;
    float density;
    float mass;
    float radius;
    float damping_factor;
    float restitution;

public:
    Particle(sf::RenderTarget& target_, Vector2f pos_ = { 10.f, 10.f },
        Vector2f vel_ = { 50.f, 50.f },
        sf::Color color_ = sf::Color::White,
        float radius_ = 5.0)
        : target { target_ }
    {
        color = color_;
        pos = pos_;
        vel = vel_;
        radius = radius_;

        acc = { 0.f, 0.f };
        density = 1.0;
        mass = density * 3.14159 * radius * radius;
        damping_factor = 1;
        restitution = 1;

        window_size = target.getSize();
        circle.setRadius(radius);
        circle.setOrigin({ radius, radius });
        circle.setFillColor(color);
    }

    void
    update(float interval)
    {

        Vector2f acc_next = gravity();
        pos += vel * interval + acc * (interval * interval * 0.5);
        vel += (acc + acc_next) * (interval * 0.5);
        acc = acc_next;
        // boundary_window();
        boundary_circle();
    }

    Vector2f
    gravity()
    {
        return Vector2f(0.0, 1500);
    }

    void
    apply_acceleration(Vector2f acc = { 0.0, 0.0 }, float interval = 1.0)
    {
        vel += acc * interval;
    }

    void
    render()
    {
        circle.setPosition(utils::convect_eigen_vector(pos));
        target.draw(circle);
    }

    void
    boundary_window()
    {
        float max_xy = window_size.x - radius;
        float min_xy = radius;

        if (pos(0, 0) < min_xy || pos(0, 0) > max_xy) {
            vel(0, 0) *= -damping_factor;
        }
        if (pos(1, 0) < min_xy || pos(1, 0) > max_xy) {
            vel(1, 0) *= -damping_factor;
        }

        if (pos(0, 0) < min_xy) {
            pos(0, 0) = min_xy;
        }
        if (pos(0, 0) > max_xy) {
            pos(0, 0) = max_xy;
        }
        if (pos(1, 0) < min_xy) {
            pos(1, 0) = min_xy;
        }
        if (pos(1, 0) > max_xy) {
            pos(1, 0) = max_xy;
        }
    }

    void
    boundary_circle()
    {
        float boundary_circle_radius = window_size.x / 2;
        Vector2f boundary_circle_origin = { window_size.x / 2, window_size.y / 2 };
        Vector2f normal = (pos - boundary_circle_origin);
        float normal_mag = utils::calc_eigen_vector_norm(normal);
        if (normal_mag > boundary_circle_radius - radius) {

            normal /= normal_mag;

            Vector2f tangent = { -normal(1, 0), normal(0, 0) };

            float normal_vel = vel.dot(normal) * damping_factor;
            float tangent_vel = vel.dot(tangent);

            vel = tangent * tangent_vel - normal * normal_vel;

            pos = boundary_circle_origin + normal * (boundary_circle_radius - radius);
        }
    }

    bool
    check_collision(const Particle& other) const
    {
        Vector2f dis = pos - other.pos;
        float dis_mag = utils::calc_eigen_vector_norm(dis);
        float min_dis = radius + other.radius;

        return dis_mag <= min_dis;
    }

    void
    resolve_collision(Particle& other)
    { // https://en.wikipedia.org/wiki/Elastic_collision#Two-dimensional
        // this->x1
        // other->x2

        Vector2f dis = pos - other.pos;
        float dis_mag = utils::calc_eigen_vector_norm(dis);

        Vector2f rel_vel = vel - other.vel;

        Vector2f delta_v1 = (1 + restitution) * other.mass / (mass + other.mass) * rel_vel.dot(dis) * dis / (dis_mag * dis_mag);

        Vector2f delta_v2 = (1 + restitution) * mass / (mass + other.mass) * -rel_vel.dot(-dis) * -dis / (dis_mag * dis_mag);

        vel -= delta_v1;
        other.vel -= delta_v2;

        // Overlapping correction
        float overlap = (radius + other.radius - dis_mag) / 2;
        pos += overlap * dis / dis_mag;
        other.pos -= overlap * dis / dis_mag;
    }

    Vector2f
    get_pos() const
    {
        return pos;
    }
};

#endif
