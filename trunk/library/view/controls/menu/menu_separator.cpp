
#include "menu_separator.h"

#include <windows.h>
#include <uxtheme.h>
#include <vssym32.h>

#include "gfx/canvas.h"
#include "gfx/native_theme_win.h"

#include "menu_config.h"
#include "menu_item_view.h"

namespace view
{

    void MenuSeparator::OnPaint(gfx::Canvas* canvas)
    {
        const MenuConfig& config = MenuConfig::instance();
        // The gutter is rendered before the background.
        int start_x = 0;
        int start_y = height()/3 + 1; // +1 makes separator centered.
        HDC dc = canvas->BeginPlatformPaint();
        const gfx::NativeTheme* theme = gfx::NativeTheme::instance();
        // Delta is needed for non-classic to move separator up slightly.
        int delta = theme->IsClassicTheme(gfx::NativeTheme::MENU) ? 0 : 1;
        if(config.render_gutter)
        {
            // If render_gutter is true, we're on Vista and need to render the
            // gutter, then indent the separator from the gutter.
            RECT gutter_bounds =
            {
                MenuItemView::label_start()-config.gutter_to_label-config.gutter_width,
                0, 0, height()
            };
            gutter_bounds.right = gutter_bounds.left + config.gutter_width;
            theme->PaintMenuGutter(dc, MENU_POPUPGUTTER, MPI_NORMAL, &gutter_bounds);
            start_x = gutter_bounds.left + config.gutter_width;
            start_y = -delta;
        }

        RECT separator_bounds = { start_x, start_y, width(), height()-delta };
        theme->PaintMenuSeparator(
            dc, MENU_POPUPSEPARATOR, MPI_NORMAL, &separator_bounds);
        canvas->EndPlatformPaint();
    }

    gfx::Size MenuSeparator::GetPreferredSize()
    {
        return gfx::Size(10, // Just in case we're the only item in a menu.
            MenuConfig::instance().separator_height);
    }

} //namespace view