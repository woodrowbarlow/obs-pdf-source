/******************************************************************************
    Copyright (C) 2016 by Hugh Bailey <obs.jim@gmail.com>
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#pragma once

#include <graphics/graphics.h>

#ifdef __cplusplus
extern "C" {
#endif

struct gs_pdf_file {
    gs_texture_t *texture;
    enum gs_color_format format;
    uint32_t cx;
    uint32_t cy;
    bool loaded;

    uint8_t *texture_data;
};

typedef struct gs_pdf_file gs_pdf_file_t;

void gs_pdf_file_init(gs_pdf_file_t *pdf, const char *file);
void gs_pdf_file_free(gs_pdf_file_t *pdf);

void gs_pdf_file_init_texture(gs_pdf_file_t *pdf);
bool gs_pdf_file_tick(gs_pdf_file_t *pdf, uint64_t elapsed_time_ns);
void gs_pdf_file_update_texture(gs_pdf_file_t *pdf);

#ifdef __cplusplus
}
#endif