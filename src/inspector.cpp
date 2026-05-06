#include <inspector.hpp>




std::unique_ptr<Inspector> Inspector::S_instance = nullptr;
std::once_flag Inspector::S_onceFlag;

Inspector *
Inspector::instance()
{
    std::call_once(S_onceFlag, [](){
        S_instance = std::unique_ptr<Inspector>( new Inspector() );
    });

    return S_instance.get();
};



void
Inspector::render()
{
    for ( auto & [name, props] : M_objects )
    {
        if ( ImGui::TreeNode(name.c_str()) )
        {
            for ( Property & prop: props  )
            {
                switch( prop.type )
                {
                    case Property::Float:
                        ImGui::SliderFloat( prop.label.c_str(), (float*)prop.data, prop.min, prop.max );
                        break;

                    case Property::Int:
                        ImGui::SliderInt( prop.label.c_str(), (int*)prop.data, (int)prop.min, (int)prop.max );
                        break;
                    case Property::Bool:
                        ImGui::Checkbox( prop.label.c_str(), (bool*)prop.data );
                        break;

                }
            }

            ImGui::TreePop();

        }

    }
    
}


