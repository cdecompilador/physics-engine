#include <scene.hpp>



void
PhysicsScene::render( ImDrawList * drawList )
{

    float dt = 0;
    if ( M_isPlaying )
        dt = ImGui::GetIO().DeltaTime;

    for ( auto & obj : M_physicsObjects )
    {
        if ( M_isPlaying )
            obj->update( dt );
        obj->render( drawList );
    }
    
    if ( M_isPlaying )
        this->checkCollisions();

}


void
PhysicsScene::reset()
{
    for ( auto & obj : M_physicsObjects )
    {
        obj->reset();
    }

}; 


void
Scene::render( ImDrawList * drawList )
{
    if( ImGui::Button("Play") )
        M_physicsScene->play();
    if( ImGui::Button("pause") )
        M_physicsScene->pause();
    if( ImGui::Button("Reset") )
    {
        M_physicsScene->pause();
        M_physicsScene->reset();
    }

    for ( auto & sceneObj : M_sceneObjects )
    {
        sceneObj->update( ImGui::GetTime() );
        sceneObj->render( drawList );
    }
    
    M_physicsScene->render( drawList );

}



void
PhysicsScene::checkCollisions()
{
    for ( size_t i = 0; i < M_physicsObjects.size(); ++i )
    {
        auto & obj1 = M_physicsObjects[i];
        if ( !obj1->collider() ) continue;
        for ( size_t j = i + 1; j < M_physicsObjects.size(); ++j )
        {
            auto & obj2 = M_physicsObjects[j];

            if ( !obj2->collider() ) continue;

            CollisionData data = obj1->collider()->checkCollision(*obj2->collider());
            data.other = obj2.get(); // TODO: Warning can get dangling

            if ( data.isColliding )
            {
                obj1->collisionDetected(data);

                CollisionData flippedData = data;
                flippedData.normalX = -data.normalX;
                flippedData.normalY = -data.normalY;
                obj2->collisionDetected(flippedData);
            }
        }
    }
}
