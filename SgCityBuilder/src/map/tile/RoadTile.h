// This file is part of the SgCityBuilder package.
// 
// Filename: RoadTile.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include "Tile.h"

namespace sg::city::automata
{
    class AutoTrack;
}

namespace sg::city::map::tile
{
    /**
     * @brief Possible Road Neighbours.
     *        Each flag can set by using the OR operator.
     */
    enum RoadNeighbours : uint8_t
    {
        NORTH = 1,
        EAST = 2,
        SOUTH = 4,
        WEST = 8
    };

    /**
     * @brief The value corresponds to the index in the texture atlas.
     */
    enum class RoadType
    {
        ROAD_V = 0,
        ROAD_H = 1,
        ROAD_C1 = 4,
        ROAD_T1 = 5,
        ROAD_C2 = 6,
        ROAD_T2 = 8,
        ROAD_X = 9,
        ROAD_T3 = 10,
        ROAD_C3 = 12,
        ROAD_T4 = 13,
        ROAD_C4 = 14,
    };

    class RoadTile : public Tile
    {
    public:
        using AutoTrackSharedPtr = std::shared_ptr<automata::AutoTrack>;
        using AutoTrackContainer = std::list<AutoTrackSharedPtr>;

        using StopPattern = std::vector<bool>;
        using StopPatternContainer = std::vector<StopPattern>;

        //-------------------------------------------------
        // Const
        //-------------------------------------------------

        static constexpr auto STOP{ 'X' };
        static constexpr auto NODES_PER_TILE{ 49 };

        //-------------------------------------------------
        // Public member
        //-------------------------------------------------

        /**
         * @brief The tile may have a Safe Track to spawn a car or a pedestrian.
         */
        AutoTrackSharedPtr safeAutoTrack;

        /**
         * @brief The orientation of the road.
         */
        RoadType roadType{ RoadType::ROAD_V };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        RoadTile() = delete;

        RoadTile(float t_mapX, float t_mapZ, TileType t_type, Map* t_map);

        RoadTile(const RoadTile& t_other) = delete;
        RoadTile(RoadTile&& t_other) noexcept = delete;
        RoadTile& operator=(const RoadTile& t_other) = delete;
        RoadTile& operator=(RoadTile&& t_other) noexcept = delete;

        virtual ~RoadTile() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] const AutoTrackContainer& GetAutoTracks() const noexcept;
        [[nodiscard]] AutoTrackContainer& GetAutoTracks() noexcept;

        [[nodiscard]] const StopPatternContainer& GetStopPatterns() const noexcept;
        [[nodiscard]] StopPatternContainer& GetStopPatterns() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Update() override;

        //-------------------------------------------------
        // Debug
        //-------------------------------------------------

        /**
         * @brief Create a Mesh from the AutoTracks.
         */
        void CreateAutoTracksMesh();

        /**
         * @brief Render the auto tracks.
         */
        void RenderAutoTracks() const;

    protected:

    private:
        /**
         * @brief Each RoadTile can have multiple Auto Tracks.
         */
        AutoTrackContainer m_autoTracks;

        /**
         * @brief Each RoadTile can have multiple StopPattern to regulate traffic.
         */
        StopPatternContainer m_stopPatterns;

        /**
         * @brief A Mesh holding the whole Auto Tracks for debug.
         */
        MeshUniquePtr m_autoTracksMesh;

        //-------------------------------------------------
        // Regulate traffic
        //-------------------------------------------------

        /**
         * @brief Recreates all Auto Tracks depending on the direction of the road.
         */
        void CreateAutoTracks();

        /**
         * @brief Recreates all Stop Patterns depending on the direction of the road.
         */
        void CreateStopPatterns();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * @brief Determines the correct RoadType for this Tile depending on the neighbors.
         */
        void DetermineRoadType();

        /**
         * @brief Clear the Auto Tracks and Stop Patterns from the Tile and Nodes.
         */
        void Clear();

        /**
         * @brief Creates a single Auto Track.
         * @param t_fromNodeIndex The From Node
         * @param t_toNodeIndex  The To Node.
         * @param t_safeCarAutoTrack True if the Track is safe.
         */
        void AddAutoTrack(int t_fromNodeIndex, int t_toNodeIndex, bool t_safeCarAutoTrack = false);

        StopPattern CreateStopPattern(std::string t_s);

        void ApplyStopPattern(int t_index);
    };
}
