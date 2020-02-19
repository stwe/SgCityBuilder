#include <SgOgl.h>
#include <SgOglEntryPoint.h>
#include "GameState.h"

class CityApplication final : public sg::ogl::Application
{
public:
    //-------------------------------------------------
    // Ctors. / Dtor.
    //-------------------------------------------------

    CityApplication() = delete;

    explicit CityApplication(const std::string& t_configFileName)
        : Application{ t_configFileName }
    {
        SG_OGL_LOG_DEBUG("[CityApplication::CityApplication()] Create CityApplication.");
    }

    CityApplication(const CityApplication& t_other) = delete;
    CityApplication(CityApplication&& t_other) noexcept = delete;
    CityApplication& operator=(const CityApplication& t_other) = delete;
    CityApplication& operator=(CityApplication&& t_other) noexcept = delete;

    ~CityApplication() noexcept override
    {
        SG_OGL_LOG_DEBUG("[CityApplication::~CityApplication()] Destruct CityApplication.");
    }

protected:
    //-------------------------------------------------
    // Override
    //-------------------------------------------------

    void RegisterStates() override
    {
        SG_OGL_LOG_DEBUG("[CityApplication::RegisterStates()] Register State: GameState as Game.");
        GetStateStack().RegisterState<GameState>(sg::ogl::state::GAME);
    }

    void Init() override
    {
        SG_OGL_LOG_DEBUG("[CityApplication::Init()] Init (Push) Game State.");
        GetStateStack().PushState(sg::ogl::state::GAME);
    }

private:

};

//-------------------------------------------------
// EntryPoint
//-------------------------------------------------

std::unique_ptr<sg::ogl::Application> sg::ogl::create_application()
{
    return std::make_unique<CityApplication>("res/config/Config.xml");
}
