add_library( physics_engine_lib renderer.cpp
                                scene.cpp
                                inspector.cpp
                                scene_object.cpp
                                collider.cpp
                                particle.cpp
                                boundary.cpp
)

target_link_libraries( physics_engine_lib PRIVATE imgui )

add_executable( physics-engine main.cpp )



