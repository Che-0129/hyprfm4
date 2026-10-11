#define WLR_USE_UNSTABLE

#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/managers/input/InputManager.hpp>
#include <hyprland/src/desktop/state/FocusState.hpp>
#include <hyprland/src/desktop/state/ViewState.hpp>
#include <hyprland/src/desktop/view/Window.hpp>
#include <hyprland/src/event/EventBus.hpp>
#include <hyprland/src/output/Monitor.hpp>
#include <linux/input-event-codes.h>

inline HANDLE PHANDLE = nullptr;

static CHyprSignalListener g_listener;
static bool                g_swallowRelease = false;

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    g_listener = Event::bus()->m_events.input.mouse.button.listen(
        [](IPointer::SButtonEvent e, Event::SCallbackInfo& info) {
            if (e.button != BTN_LEFT)
                return;

            if (e.state == WL_POINTER_BUTTON_STATE_RELEASED) {
                if (g_swallowRelease) {
                    info.cancelled   = true;
                    g_swallowRelease = false;
                }
                return;
            }

            const auto pos = g_pInputManager->getMouseCoordsInternal();
            const auto win = Desktop::viewState()->hitTest().windowAt(pos, Desktop::View::ALLOW_FLOATING);

            if (!win || win == Desktop::focusState()->window())
                return;

            const auto mon = win->m_monitor.lock();
            if (!mon)
                return;

            const CBox wb = win->getWindowMainSurfaceBox();
            const CBox mb = mon->logicalBox();

            constexpr double TOL = 2.0;
            const bool overflow = wb.x < mb.x - TOL || wb.y < mb.y - TOL ||
                                  wb.x + wb.w > mb.x + mb.w + TOL || wb.y + wb.h > mb.y + mb.h + TOL;

            if (!overflow)
                return;

            Desktop::focusState()->fullWindowFocus(win, Desktop::FOCUS_REASON_CLICK);
            info.cancelled   = true;
            g_swallowRelease = true;
        });

    return {"hyprfm4", "Focus on click without passing it to the client", "che", "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    g_listener.reset();
}
