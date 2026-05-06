#pragma once

class Inspector
{
public:

    static Inspector * instance();

    struct Property
    {
        std::string label;
        enum Type { Float, Int, Bool } type;
        void * data;
        float min = 0.;
        float max = 1.;
    };
    
    void registerObject( std::string const& objectName, std::vector<Property> props ) { M_objects[objectName] = props; }


    void render();

private:
    Inspector() = default;

private:
    static std::unique_ptr<Inspector> S_instance;
    static std::once_flag S_onceFlag;

    std::unordered_map<std::string, std::vector<Property>> M_objects;

};
