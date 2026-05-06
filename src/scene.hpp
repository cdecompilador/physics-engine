#pragma once

#include <vector>
#include <memory>

#include <imgui.hpp>

#include <scene_object.hpp>
#include <physics_object.hpp>


class PhysicsScene
{
public:

    void addObject( std::unique_ptr<PhysicsObject> && obj ) { M_physicsObjects.push_back(std::move(obj)); };

    void render( ImDrawList * drawList );

    void play() noexcept { M_isPlaying = true; };
    void pause() noexcept { M_isPlaying = false; };
    void reset(); 

    void checkCollisions();

private:
    std::vector<std::unique_ptr<PhysicsObject>> M_physicsObjects;

    bool M_isPlaying = false;
    
};

class Scene
{
public:
    Scene() = default;

    void render( ImDrawList * drawList ) ;

    void addObject( std::unique_ptr<SceneObject> && sceneObj ) { M_sceneObjects.push_back(std::move(sceneObj)); }

    void setPhysicsScene( std::unique_ptr<PhysicsScene> && physicsScene ) { M_physicsScene = std::move( physicsScene ); }



private:
    std::vector<std::unique_ptr<SceneObject>> M_sceneObjects;
    std::unique_ptr<PhysicsScene> M_physicsScene;


};

