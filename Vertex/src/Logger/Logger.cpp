#include "Logger.h"

namespace Vertex
{
    Logger                          Logger::s_Instance;
    std::shared_ptr<spdlog::logger> Logger::s_Logger;

    void Logger::Init()
    {
        s_Logger = spdlog::stdout_color_mt("VX_CLIENT");
        s_Logger->set_level(vx_log_level_info);
    }

    CoreLogger                      CoreLogger::s_Instance;
    std::shared_ptr<spdlog::logger> CoreLogger::s_Logger;

    void CoreLogger::Init()
    {
        s_Logger = spdlog::stdout_color_mt("VX_CORE");
        s_Logger->set_level(vx_log_level_debug);
    }
}