#include <graphics/graphics.h>

uint8_t *gs_create_texture_pdf_data(const char *file, unsigned int page,
                                    enum gs_color_format *format,
                                    uint32_t *cx_out, uint32_t *cy_out);