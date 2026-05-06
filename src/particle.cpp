#include <particle.hpp>




Particle::Particle( float x, float y, float radius )
    : PhysicsObject(x,y,"particle")
{
    M_radius = radius;
    Inspector::instance()->registerObject("Particle", {
        {"Radius", Inspector::Property::Float, &M_radius, 0,1000. },
        {"Gravity", Inspector::Property::Float, &M_gravity, -20.,20. },
        {"Damping", Inspector::Property::Float, &M_dampingFactor, 0.,1. }
    } );

    M_collider = std::make_unique<SphereCollider>(&M_position, M_radius);

};




void
Particle::render( ImDrawList* drawList ) const override
{
    ImVec2 center(M_position.first, M_position.second);
    drawList->AddCircleFilled(center, M_radius, IM_COL32(186, 222, 238, 255));
}




void
Particle::collisionDetected( CollisionData const& collisionData ) override
{
    if( !collisionData.other ) return;

    if ( collisionData.other->classname() == "boundary" )
    {
        M_position.first += collisionData.normalX*collisionData.penetration;
        M_position.second += collisionData.normalY*collisionData.penetration;

        float dotProduct = (M_velocity.first * collisionData.normalX) + (M_velocity.second * collisionData.normalY);

        if (dotProduct < 0.0f)
        {
            M_velocity.first -= 2.0f * dotProduct * collisionData.normalX;
            M_velocity.second -= 2.0f * dotProduct * collisionData.normalY;

            M_velocity.first *= M_dampingFactor;
            M_velocity.second *= M_dampingFactor;
        }
    }
    if ( collisionData.other->classname() == "particle" )
    {
        M_position.first += collisionData.normalX * collisionData.penetration * 0.5f;
        M_position.second += collisionData.normalY * collisionData.penetration * 0.5f;

        Particle* otherParticle = static_cast<Particle*>(collisionData.other);

        // 2. Momentum Exchange
        // Find the relative velocity between the two balls
        float relVelX = M_velocity.first - otherParticle->M_velocity.first;
        float relVelY = M_velocity.second - otherParticle->M_velocity.second;

        // Find how much of that velocity is along the collision normal
        float velocityAlongNormal = (relVelX * collisionData.normalX) + (relVelY * collisionData.normalY);

        // Safety check: If they are already moving apart, do nothing
        if (velocityAlongNormal > 0.0f) 
            return;

        // Calculate impulse (Assuming both particles have equal mass)
        float restitution = std::min(M_dampingFactor, otherParticle->M_dampingFactor);
        float j = -(1.0f + restitution) * velocityAlongNormal / 2.0f;

        // Apply impulse to this particle
        float impulseX = j * collisionData.normalX;
        float impulseY = j * collisionData.normalY;

        M_velocity.first += impulseX;
        M_velocity.second += impulseY;

        // Instantly apply the opposite impulse to the other particle
        otherParticle->M_velocity.first -= impulseX;
        otherParticle->M_velocity.second -= impulseY;

    }
}




void
Particle::fall( float time )
{
    M_velocity.second += M_gravity*time*100;

    M_position.first += M_velocity.first*time; 
    M_position.second += M_velocity.second*time;
}



