#pragma once

#include <memory>
#include <string>
#include <stack>
#include <glm/vec2.hpp>

namespace sg::ogl::scene
{
    class Scene;
}

namespace sg::city::map
{
    class Map;
    class RoadNetwork;
    class Astar;
    class Tile;
    class BuildingGenerator;
}

namespace sg::city::renderer
{
    class MapRenderer;
    class RoadNetworkRenderer;
    class BuildingsRenderer;
}

namespace sg::city::city
{
    class City
    {
    public:
        using MapSharedPtr = std::shared_ptr<map::Map>;
        using RoadNetworkSharedPtr = std::shared_ptr<map::RoadNetwork>;
        using BuildingGeneratorSharedPtr = std::shared_ptr<map::BuildingGenerator>;
        using AstarUniquePtr = std::unique_ptr<map::Astar>;

        using MapRendererUniquePtr = std::unique_ptr<renderer::MapRenderer>;
        using RoadNetworkRendererUniquePtr = std::unique_ptr<renderer::RoadNetworkRenderer>;
        using BuildingsRendererUniquePtr = std::unique_ptr<renderer::BuildingsRenderer>;

        using PathPositionContainer = std::stack<glm::vec2>;

        static constexpr auto BIRTH_RATE{ 0.00055f };
        static constexpr auto DEATH_RATE{ 0.00023f };
        static constexpr auto MOVE_POPULATION_RATE{ 4 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        City(std::string t_name, ogl::scene::Scene* t_scene, int t_mapSize = 128);

        City(const City& t_other) = delete;
        City(City&& t_other) noexcept = delete;
        City& operator=(const City& t_other) = delete;
        City& operator=(City&& t_other) noexcept = delete;

        ~City() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] const std::string& GetName() const noexcept;

        [[nodiscard]] const map::Map& GetMap() const noexcept;
        [[nodiscard]] map::Map& GetMap() noexcept;
        [[nodiscard]] MapSharedPtr GetMapPtr() const;

        [[nodiscard]] const map::RoadNetwork& GetRoadNetwork() const noexcept;
        [[nodiscard]] map::RoadNetwork& GetRoadNetwork() noexcept;
        [[nodiscard]] RoadNetworkSharedPtr GetRoadNetworkPtr() const;

        [[nodiscard]] const map::BuildingGenerator& GetBuildingGenerator() const noexcept;
        [[nodiscard]] map::BuildingGenerator& GetBuildingGenerator() noexcept;
        [[nodiscard]] BuildingGeneratorSharedPtr GetBuildingGeneratorPtr() const;

        [[nodiscard]] map::Astar& GetAstar() const noexcept;

        [[nodiscard]] float& GetTimePerDay();
        [[nodiscard]] int GetDay() const;
        [[nodiscard]] float GetPopulationPool() const;
        [[nodiscard]] float GetPopulation() const;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Update(double t_dt);
        void RenderMap() const;
        void RenderRoadNetwork() const;
        void RenderBuildings() const;

        //-------------------------------------------------
        // Path
        //-------------------------------------------------

        [[nodiscard]] PathPositionContainer Path(int t_fromMapX, int t_fromMapZ, int t_toMapX, int t_toMapZ) const;

    protected:

    private:
        /**
         * @brief The name of the City;
         */
        std::string m_name;

        /**
         * @brief The parent Scene object.
         */
        ogl::scene::Scene* m_scene{ nullptr };

        /**
         * @brief The Map of the City holding all Tiles.
         */
        MapSharedPtr m_map;

        /**
         * @brief The Road Network of the City.
         */
        RoadNetworkSharedPtr m_roadNetwork;

        /**
         * @brief Contains all buildings of the City.
         */
        BuildingGeneratorSharedPtr m_buildingGenerator;

        /**
         * @brief Renders the Map.
         */
        MapRendererUniquePtr m_mapRenderer;

        /**
         * @brief Renders the RoadNetwork.
         */
        RoadNetworkRendererUniquePtr m_roadNetworkRenderer;

        /**
         * @brief Renders all buildings.
         */
        BuildingsRendererUniquePtr m_buildingsRenderer;

        /**
         * @brief An Astar instance.
         */
        AstarUniquePtr m_astar;

        /**
         * @brief The real world time (in seconds) since the day updated.
         */
        float m_currentTime{ 0.0f };

        /**
         * @brief The amount of real world time each day should last.
         */
        float m_timePerDay{ 1.0f };

        /**
         * @brief Number of days.
         */
        int m_day{ 0 };

        /**
         * @brief Stores the number of people who do not have a home.
         *        We start with a small number of 50 people.
         */
        float m_populationPool{ 50.0f };

        /**
         * @brief The total population of the City.
         *        The sum of all the Tiles populations and the populationPool.
         */
        float m_population{ 0.0f };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init(ogl::scene::Scene* t_scene, int t_mapSize);
        void CreateMapEntity();
        void CreateRoadNetworkEntity();
        void CreateBuildingsEntity();

        //-------------------------------------------------
        // Distribute
        //-------------------------------------------------

        static void DistributePool(float& t_pool, map::Tile& t_tile, float t_rate = 0.0f);
    };
}
