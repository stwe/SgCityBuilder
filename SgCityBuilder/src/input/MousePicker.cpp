#include <Application.h>
#include <Window.h>
#include <camera/Camera.h>
#include <scene/Scene.h>
#include "MousePicker.h"
#include "map/Map.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::input::MousePicker::MousePicker(ogl::scene::Scene* t_scene, const MapSharedPtr& t_map)
    : m_scene{ t_scene }
    , m_map{ t_map }
{
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

glm::vec3 sg::city::input::MousePicker::GetCurrentMouseDirection() const
{
    return m_currentMouseDirection;
}

glm::vec3 sg::city::input::MousePicker::GetCurrentMapPoint() const
{
    return m_currentMapPoint;
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void sg::city::input::MousePicker::Update(const float t_mouseX, const float t_mouseY)
{
    m_currentMouseDirection = GetDirectionFromMouse(t_mouseX, t_mouseY);

    if (IntersectionInRange(m_currentMouseDirection, 0, RAY_RANGE))
    {
        m_currentMapPoint = BinarySearch(m_currentMouseDirection, 0, RAY_RANGE, 0);
    }
}

//-------------------------------------------------
// Mouse Ray
//-------------------------------------------------

glm::vec3 sg::city::input::MousePicker::GetDirectionFromMouse(const float t_mouseX, const float t_mouseY) const
{
    const auto& projectionOptions{ m_scene->GetApplicationContext()->GetProjectionOptions() };
    const auto& projectionMatrix{ m_scene->GetApplicationContext()->GetWindow().GetProjectionMatrix() };

    /*

    mouse cursor coordinates

        0,0            1024
        ----------------> x
        |
        |
        |
        |
        |
   768 \|/
        y

    */

    // 3d (2d) Normalised Device Coordinates
    // We don't need to specify a z
    // range: x [-1:1] y [-1:1]
    const auto x{ (2.0f * t_mouseX) / static_cast<float>(projectionOptions.width) - 1.0f };
    const auto y{ 1.0f - (2.0f * t_mouseY) / static_cast<float>(projectionOptions.height) };
    const auto rayNds{ glm::vec2(x, y) };

    /*
             y
             | 1
             |
             |
    -1       |          1
    --------------------- x
             |
             |
             |
          -1 |

    */

    // 4d Homogeneous Clip Coordinates
    // We want our ray's z to point forwards - this is usually the negative z direction in OpenGL style.
    // We can add a w, just so that we have a 4d vector.
    // Range [-1:1, -1:1, -1:1, -1:1]
    const auto rayClip{ glm::vec4(rayNds.x, rayNds.y, -1.0f, 1.0f) };

    // 4d Eye (Camera) Coordinates
    // To get into clip space from eye space we multiply the vector by a projection matrix.
    // We can go backwards by multiplying by the inverse of this matrix.
    // range [-x:x, -y:y, -z:z, -w:w]
    auto rayEye{ inverse(projectionMatrix) * rayClip };
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    // 4d World Coordinates
    // Same again, to go back another step in the transformation pipeline.
    // range [-x:x, -y:y, -z:z, -w:w]
    auto rayWorld{ glm::vec3(inverse(m_scene->GetCurrentCamera().GetViewMatrix()) * rayEye) };

    // normalise the vector - it's just a direction
    rayWorld = normalize(rayWorld);

    return rayWorld;
}

bool sg::city::input::MousePicker::IntersectionInRange(const glm::vec3& t_ray, const float t_start, float const t_finish)
{
    const auto startPoint{ GetPointOnRay(t_ray, t_start) };
    const auto endPoint{ GetPointOnRay(t_ray, t_finish) };

    const auto result{ !IsUnderGround(startPoint) && IsUnderGround(endPoint) };

    return result;
}

glm::vec3 sg::city::input::MousePicker::BinarySearch(const glm::vec3& t_ray, const float t_start, const float t_finish, const int t_count)
{
    const auto half{ t_start + ((t_finish - t_start) / 2.0f) };

    if (t_count >= RECURSION_COUNT) // end - return current point as result
    {
        auto result{ GetPointOnRay(t_ray, half) };
        result.x = glm::floor(result.x);
        result.y = 0.0f;
        result.z = abs(glm::floor(result.z)) - 1;

        return result;
    }

    // todo: try out different values
    if (m_outsideTheMap > 50) // end - it was clicked outside the map
    {
        m_outsideTheMap = 0;
        return glm::vec3(-999.0f); // -999 means that nothing was found
    }

    if (IntersectionInRange(t_ray, t_start, half))
    {
        return BinarySearch(t_ray, t_start, half, t_count + 1);
    }

    return BinarySearch(t_ray, half, t_finish, t_count + 1);
}

glm::vec3 sg::city::input::MousePicker::GetPointOnRay(const glm::vec3& t_ray, const float t_distance) const
{
    return m_scene->GetCurrentCamera().GetPosition() + (t_ray * t_distance);
}

bool sg::city::input::MousePicker::IsUnderGround(const glm::vec3& t_point)
{
    const auto mapSize{ static_cast<float>(m_map->GetMapSize()) };

    // a GetHeight() method of the map can be used here
    if (t_point.x <= mapSize && t_point.x >= 0.0f && t_point.z >= -mapSize && t_point.z <= 0.0)
    {
        // return t_point.y < m_map.GetHeight(t_point.x, t_point.z);
        return t_point.y < 0.0f;
    }

    // no map height can be requested here

    /*
                               Camera position
                              +
        -------------       +  <------- mouse direction * distance
        |     |     |     +
        |     |     |   +
        ------------- +
        |     |    +|
        |     |  +  |
        -------------
             +
           + <-------- t_point, but outside the map, no height can determined
    */

    // the search is cancelled at a certain value
    m_outsideTheMap++;

    return t_point.y < 0.0f;
}
