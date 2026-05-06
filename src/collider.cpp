#include <collider.hpp>

CollisionData
PolylineCollider::checkCollisionWith(PolylineCollider const& other) const
{
    return {};
}

CollisionData
PolylineCollider::checkCollisionWith(SphereCollider const& other) const
{
    return other.checkCollisionWith(*this);
}

CollisionData
SphereCollider::checkCollisionWith(SphereCollider const& other) const
{
    CollisionData data;

    // Vector pointing from 'other' to 'this'
    float dx = M_position->first - other.M_position->first;
    float dy = M_position->second - other.M_position->second;
    
    float dist2 = dx*dx + dy*dy;
    float r = M_radius + other.M_radius;

    // Check if they overlap (dist2 < r^2)
    if (dist2 < r * r)
    {
        data.isColliding = true;

        // Standard collision
        if (dist2 > 0.0001f) 
        {
            float dist = std::sqrt(dist2);
            data.penetration = r - dist;
            
            // Normalize the vector so it has a length of 1
            data.normalX = dx / dist;
            data.normalY = dy / dist; 
        }
        else 
        {
            // Edge case: Both spheres are exactly on top of each other.
            // We can't divide by zero, so we push them apart in an arbitrary direction.
            data.penetration = r;
            data.normalX = 0.0f;
            data.normalY = 1.0f; 
        }
    }

    return data;
}

CollisionData
SphereCollider::checkCollisionWith(PolylineCollider const& other) const
{
    CollisionData data;
    auto const& boundary = other.polyline();

    for (size_t i = 0; i < boundary.size(); ++i)
    {
        auto const& A = boundary[i];
        auto const& B = boundary[(i+1) % boundary.size()];

        float ABx = B.first - A.first;
        float ABy = B.second - A.second;
        float APx = M_position->first - A.first;
        float APy = M_position->second - A.second;

        float ab2 = ABx*ABx + ABy*ABy;
        float t = std::max(0.0f, std::min(1.0f, (APx*ABx + APy*ABy) / ab2));

        float closestX = A.first + t * ABx;
        float closestY = A.second + t * ABy;

        float dx = M_position->first - closestX;
        float dy = M_position->second - closestY;
        float dist2 = dx*dx + dy*dy;

        if (dist2 <= M_radius * M_radius && dist2 > 0.0001f) // Avoid divide by zero
        {
            float dist = std::sqrt(dist2);
            data.isColliding = true;
            data.normalX = dx / dist; // Normalize
            data.normalY = dy / dist; // Normalize
            data.penetration = M_radius - dist;
            return data; // Return the first hit we find
        }
    }
    return data;
}
