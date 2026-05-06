#pragma once

#include <imgui.hpp>

#include <scene_object.hpp>
#include <inspector.hpp>



class Particle
    : public PhysicsObject
{
public:
    Particle( float x, float y, float radius = 50);

    PointType const& position() const noexcept { return M_position; };

    void
    update( float time ) override
    {
        fall( time );
    }

    void render( ImDrawList* drawList ) const override;

    void collisionDetected( CollisionData const& collisionData ) override;

    void reset() override
    {
        PhysicsObject::reset();
        M_isResting = false;
    }

    void fall( float time );



private:
    std::pair<float,float> M_velocity { 0, 0 };

    float M_radius; 
    float M_gravity = 9.81;
    float M_dampingFactor = 0.8;


    bool M_isResting = false;

};






