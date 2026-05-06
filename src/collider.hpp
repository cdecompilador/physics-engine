#pragma once


class PhysicsObject;
class PolylineCollider;
class SphereCollider;

struct CollisionData {
    PhysicsObject * other;
    bool isColliding = false;
    float normalX = 0.0f;
    float normalY = 0.0f;
    float penetration = 0.0f;
};

class ICollider
{
public:

    ICollider( PointType const* position )
        : M_position(position)
    {}
    virtual ~ICollider() = default;


    virtual CollisionData checkCollision(ICollider const& other) const = 0;

    virtual CollisionData checkCollisionWith(PolylineCollider const& other) const = 0;
    virtual CollisionData checkCollisionWith(SphereCollider const& other) const = 0;



protected:
    PointType const* M_position;


};



class PolylineCollider
    : public ICollider
{
public:
    PolylineCollider( PointType const* position, std::vector<PointType> const& polyline )
        : ICollider( position ), M_polyline(polyline)
    {}

    std::vector<PointType> const& polyline() const { return M_polyline; };

    CollisionData checkCollision(ICollider const& other) const override
    {
        return other.checkCollisionWith(*this);
    }

    CollisionData checkCollisionWith(PolylineCollider const& other) const override;

    CollisionData checkCollisionWith(SphereCollider const& other) const override;


private:
    std::vector<PointType> M_polyline;

};

class SphereCollider
    : public ICollider
{
public:
    SphereCollider( PointType const* position, float & radius )
        : ICollider(position), M_radius(radius)
    {}

    CollisionData checkCollision(ICollider const& other) const override
    {
        return other.checkCollisionWith(*this);
    }

    CollisionData checkCollisionWith( PolylineCollider const& other ) const override;

    CollisionData checkCollisionWith(SphereCollider const& other) const override;

protected:
    float & M_radius;
};

