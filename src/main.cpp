
#include <memory>

#include "renderer.hpp"
#include <particle.hpp>


int main( int argc, char const* argv[])
{


    Renderer renderer;

        
    auto boundary = std::make_unique<Boundary>( std::vector<PointType>{ {200,200}, {200,800}, {800,800}, {800,200}, {200,200} } );

    auto scene = std::make_unique<Scene>();
    auto physicsScene = std::make_unique<PhysicsScene>();

    for ( int i = 0; i < 10; i++ )
        for ( int j = 0; j < 2; j++ )
            physicsScene->addObject( std::make_unique<Particle>(300+ i*50+2*j, 500+100*j, 20) );

    physicsScene->addObject( std::move(boundary) );
   
    scene->setPhysicsScene(std::move(physicsScene));
    renderer.setScene(std::move(scene));

    renderer.run();
        
    
    return 0;
}
