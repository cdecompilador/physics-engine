#pragma once

#include <string>
#include <memory>

#include <imgui.h>

#include <collider.hpp>


using PointType = std::pair<float,float>;


class SceneObject
{
public:
    SceneObject( std::string const& classname = ""){}

    virtual ~SceneObject() = default;

    virtual void render( ImDrawList* drawList ) const = 0;
    virtual void update( float time ) = 0;


    std::string const& classname() const { return M_classname; };


protected:
    std::string M_classname;

};

class PhysicsObject
    : public SceneObject
{
public:
    PhysicsObject() = default;
    ~PhysicsObject() = default;
    

    PhysicsObject(float x, float y,  std::string const& classname = "")
        : SceneObject(classname), M_position(PointType(x,y)), M_initialPosition(PointType(x,y))
    {}

    void setPosition( float x, float y ){ M_position = PointType(x,y); };
    PointType const& position() const { return M_position; }; 

    virtual void reset() { M_position = M_initialPosition; };

    virtual void onCollision( SceneObject * other ) {};

    virtual void collisionDetected( CollisionData const& ) {};

    std::unique_ptr<ICollider> const& collider() const { return M_collider; };

protected:
    PointType M_position;
    PointType M_initialPosition;

    std::unique_ptr<ICollider> M_collider;

};







