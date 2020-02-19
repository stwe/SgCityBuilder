#pragma once

namespace sg::city::map
{
    class Map;
}

namespace sg::city::input
{
    class MousePicker
    {
    public:
        using MapSharedPtr = std::shared_ptr<map::Map>;

        static constexpr auto RECURSION_COUNT{ 200 };
        static constexpr auto RAY_RANGE{ 600.0f };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MousePicker() = delete;

        MousePicker(ogl::scene::Scene* t_scene, const MapSharedPtr& t_map);

        MousePicker(const MousePicker& t_other) = delete;
        MousePicker(MousePicker&& t_other) noexcept = delete;
        MousePicker& operator=(const MousePicker& t_other) = delete;
        MousePicker& operator=(MousePicker&& t_other) noexcept = delete;

        ~MousePicker() noexcept = default;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] glm::vec3 GetCurrentMouseDirection() const;
        [[nodiscard]] glm::vec3 GetCurrentMapPoint() const;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Update(float t_mouseX, float t_mouseY);

    protected:

    private:
        /**
         * @brief Pointer to the parent Scene to get the current Camera and the Window object.
         */
        ogl::scene::Scene* m_scene{ nullptr };

        /**
         * @brief Pointer to the Map.
         */
        MapSharedPtr m_map;

        glm::vec3 m_currentMouseDirection{ glm::vec3(0.0f) };
        glm::vec3 m_currentMapPoint{ glm::vec3(0.0f) };

        int m_outsideTheMap{ 0 };

        //-------------------------------------------------
        // Mouse Ray
        //-------------------------------------------------

        /**
         * @brief Takes mouse position on screen and return direction in world coords.
         * @see http://antongerdelan.net/opengl/raycasting.html
         * @param t_mouseX The x mouse position.
         * @param t_mouseY The y mouse position.
         * @return glm::vec3
         */
        [[nodiscard]] glm::vec3 GetDirectionFromMouse(float t_mouseX, float t_mouseY) const;

        //-------------------------------------------------
        // Raycasting
        //-------------------------------------------------

        [[nodiscard]] bool IntersectionInRange(const glm::vec3& t_ray, float t_start, float t_finish);
        [[nodiscard]] glm::vec3 BinarySearch(const glm::vec3& t_ray, float t_start, float t_finish, int t_count);

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        [[nodiscard]] glm::vec3 GetPointOnRay(const glm::vec3& t_ray, float t_distance) const;
        [[nodiscard]] bool IsUnderGround(const glm::vec3& t_point);
    };
}
