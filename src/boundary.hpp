#pragma once

#include <physics_object.hpp>

class Boundary
    : public PhysicsObject
{
public:
    Boundary( float x, float y ) : PhysicsObject(x,y, "boundary") { };

    Boundary( std::vector<PointType> const& points ) 
        : M_points(points)
    {
        M_classname = "boundary";
        M_barycenter = barycenter();
        setPosition(M_barycenter.first, M_barycenter.second);

        M_collider = std::make_unique<PolylineCollider>(&M_position, M_points);
    };
    
    PointType
    barycenter() const
    {
        PointType barycenter{0,0};

        for ( auto const& pt : M_points )
        {
            barycenter.first += pt.first;
            barycenter.second += pt.second;
        }

        barycenter.first /= M_points.size(); 
        barycenter.second /= M_points.size(); 

        return barycenter;
    }

    void
    update( float time ) override {};

    void
    render( ImDrawList* drawList ) const override
    {

        std::vector<ImVec2> pts;
        std::size_t nbPts = M_points.size();
        pts.resize(nbPts);
        for ( std::size_t i = 0; i < nbPts; ++i )
        {
            auto const& pt = M_points[i];
            pts[i] = ImVec2(pt.first,pt.second);
        }

        float thickness = 2;

        drawList->AddPolyline(pts.data(), nbPts, IM_COL32(255, 255, 255, 255), {}, thickness );
    }


    std::size_t nbPoints() const { return M_points.size(); }; 
    PointType const& point( std::size_t i ) const { return M_points[i%M_points.size()]; };

private:
    PointType M_barycenter; 
    std::vector<PointType> M_points ;

};
