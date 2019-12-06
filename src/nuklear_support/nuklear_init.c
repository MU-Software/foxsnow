#include "nuklear_init.h"

// void FS_nk_consoleInit(SDL_Window* window) {
//     fs_nk_context = nk_sdl_init(window);
//     {
//         nk_sdl_font_stash_begin(&fs_nk_font_atlas);
//         struct nk_font *roboto  = nk_font_atlas_add_from_file(fs_nk_font_atlas, "./resources/font/Roboto-Regular.ttf", 16, 0);
//         nk_sdl_font_stash_end();
        
//         //nk_style_load_all_cursors(ctx, atlas->cursors);
//         nk_style_set_font(fs_nk_context, &roboto->handle);

//         fs_nk_context->style.window.background = nk_rgba(38,38,38,128);
//         fs_nk_context->style.window.header.normal = nk_style_item_color(nk_rgba(38,38,38,128));
//         fs_nk_context->style.window.header.hover  = nk_style_item_color(nk_rgba(38,38,38,192));
//         fs_nk_context->style.window.header.active = nk_style_item_color(nk_rgba(38,38,38,224));
//         fs_nk_context->style.window.header.minimize_button.normal = nk_style_item_color(nk_rgba(38,38,38,0));
//         fs_nk_context->style.window.header.minimize_button.hover  = nk_style_item_color(nk_rgba(38,38,38,0));
//         fs_nk_context->style.window.header.minimize_button.active = nk_style_item_color(nk_rgba(38,38,38,0));
//         fs_nk_context->style.window.header.minimize_button.border_color = nk_rgba(38,38,38,224);
//         fs_nk_context->style.window.header.label_active = nk_rgb(212,212,212);
//         fs_nk_context->style.window.fixed_background = nk_style_item_color(nk_rgba(38,38,38,192));
//         fs_nk_context->style.window.border_color = nk_rgb(18,86,133);
//         // ctx->style.window.contextual_border_color = nk_rgb(255,165,0);
//         // ctx->style.window.menu_border_color = nk_rgb(255,165,0);
//         // ctx->style.window.group_border_color = nk_rgb(255,165,0);
//         // ctx->style.window.tooltip_border_color = nk_rgb(255,165,0);
//         // ctx->style.window.scrollbar_size = nk_vec2(16,16);
//         // ctx->style.window.border_color = nk_rgba(0,0,0,0);
//         // ctx->style.window.border = 1;

//         fs_nk_context->style.button.normal = nk_style_item_color(nk_rgba(38,38,38,0));
//         fs_nk_context->style.button.hover = nk_style_item_color(nk_rgba(38,38,38,192));
//         fs_nk_context->style.button.border_color = nk_rgb(0,122,204);
//         fs_nk_context->style.button.text_normal = nk_rgb(212,212,212);
//         fs_nk_context->style.button.text_hover = nk_rgb(212,212,212);
//         fs_nk_context->style.button.text_active = nk_rgb(212,212,212);

//         fs_nk_context->style.text.color = nk_rgb(212,212,212);

//         fs_nk_context->style.edit.normal = nk_style_item_color(nk_rgba(38,38,38,192));
//         fs_nk_context->style.edit.hover  = nk_style_item_color(nk_rgba(38,38,38,192));
//         fs_nk_context->style.edit.active = nk_style_item_color(nk_rgba(38,38,38,192));
//         fs_nk_context->style.edit.border_color = nk_rgb(18,86,133);
//         fs_nk_context->style.edit.text_normal = nk_rgb(212,212,212);
//         fs_nk_context->style.edit.text_hover  = nk_rgb(212,212,212);
//         fs_nk_context->style.edit.text_active = nk_rgb(212,212,212);
//         fs_nk_context->style.edit.selected_normal = nk_rgb(212,212,212);
//         fs_nk_context->style.edit.selected_hover  = nk_rgb(212,212,212);
//         fs_nk_context->style.edit.selected_text_normal = nk_rgb(212,212,212);
//         fs_nk_context->style.edit.selected_text_hover  = nk_rgb(212,212,212);
//         // ctx->style.button.active = nk_style_item_color(nk_rgb(220,10,0));
//         // ctx->style.button.text_background = nk_rgb(0,0,0);
//     }
// }
