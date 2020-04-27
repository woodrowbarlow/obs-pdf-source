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

#include "pdf-file.h"
#include "graphics-magick.h"

#define blog(level, format, ...) \
    blog(level, "%s: " format, __FUNCTION__, __VA_ARGS__)

void gs_pdf_file_init(gs_pdf_file_t *pdf, const char *file)
{
    if (!pdf)
        return;

    memset(pdf, 0, sizeof(*pdf));

    if (!file)
        return;

    pdf->texture_data = gs_create_texture_pdf_data(file, 0, &pdf->format,
                                                   &pdf->cx, &pdf->cy);

    pdf->loaded = !!pdf->texture_data;
    if (!pdf->loaded) {
        blog(LOG_WARNING, "Failed to load file '%s'", file);
        gs_pdf_file_free(pdf);
    }
}

void gs_pdf_file_free(gs_pdf_file_t *pdf)
{
    if (!pdf)
        return;

    if (pdf->loaded)
    {
        gs_texture_destroy(pdf->texture);
    }

    bfree(pdf->texture_data);
    memset(pdf, 0, sizeof(*pdf));
}

void gs_pdf_file_init_texture(gs_pdf_file_t *pdf)
{
    if (!pdf->loaded)
        return;

    pdf->texture = gs_texture_create(pdf->cx, pdf->cy, pdf->format, 1,
                                     (const uint8_t **)&pdf->texture_data, 0);
    bfree(pdf->texture_data);
    pdf->texture_data = NULL;
}

bool gs_pdf_file_tick(gs_pdf_file_t *pdf, uint64_t elapsed_time_ns)
{
    // TODO
    return false;
}

void gs_pdf_file_update_texture(gs_pdf_file_t *pdf)
{
    // TODO
}