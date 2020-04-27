#include <magick/MagickCore.h>

#include <stdio.h>
#include "graphics-magick.h"

#define blog(level, format, ...) \
    blog(level, "%s: " format, __FUNCTION__, __VA_ARGS__)

uint8_t *gs_create_texture_pdf_data(const char *file, unsigned int page,
                                    enum gs_color_format *format,
                                    uint32_t *cx_out, uint32_t *cy_out)
{
    FILE *output = NULL;
    size_t x, y, c, i = 0;

    const char *map = "BGRA";
    uint8_t *data = NULL;
    ImageInfo *info = NULL;
    ExceptionInfo *exception = NULL;
    Image *image = NULL;
    size_t width = 0;
    size_t height = 0;
    size_t file_len = 0;

    if (!file || !*file)
        goto exit;

    file_len = strlen(file);

    info = CloneImageInfo(NULL);
    exception = AcquireExceptionInfo();

    strcpy(info->filename, file);
    sprintf(&info->filename[(int)file_len], "[%u]", page);

    blog(LOG_WARNING, "Reading %s", info->filename);

    image = ReadImage(info, exception);
    if (exception->severity != UndefinedException) {
        blog(LOG_WARNING, "%s", exception->reason);
        goto exit;
    }

    width = image->magick_columns;
    height = image->magick_rows;
    data = bmalloc(width * height * strlen(map));

    ExportImagePixels(image, 0, 0, width, height, map,
                      CharPixel, data, exception);
    if (exception->severity != UndefinedException) {
        bfree(data);
        data = NULL;
        blog(LOG_WARNING, "%s\n", exception->reason);
        goto exit;
    }

    *format = GS_BGRA;
    *cx_out = (uint32_t)width;
    *cy_out = (uint32_t)height;

    output = fopen("/tmp/wjbdebug", "w");

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            if (x != 0)
            {
                fprintf(output, " ");
            }
            fprintf(output, "[");
            for (c = 0; c < strlen(map); c++)
            {
                uint8_t v = data[i++];
                if (c != 0)
                {
                    fprintf(output, ",");
                }
                fprintf(output, "%c:%02x", map[c], v);
            }
            fprintf(output, "]");
        }
        fprintf(output, "\n");
    }

    fclose(output);

exit:
    if (image != NULL)
        DestroyImage(image);
    if (info != NULL)
        DestroyImageInfo(info);
    if (exception != NULL)
        DestroyExceptionInfo(exception);

    return data;
}