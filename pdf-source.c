#include <obs-module.h>
#include <util/platform.h>
#include <util/dstr.h>
#include <sys/stat.h>

#include "pdf-file.h"

#define blog(log_level, format, ...)                    \
    blog(log_level, "[pdf_source: '%s'] " format, \
         obs_source_get_name(context->source), ##__VA_ARGS__)

#define debug(format, ...) blog(LOG_WARNING, format, ##__VA_ARGS__)
#define info(format, ...) blog(LOG_INFO, format, ##__VA_ARGS__)
#define warn(format, ...) blog(LOG_WARNING, format, ##__VA_ARGS__)


struct pdf_source {
    obs_source_t *source;

    char *file;
    bool persistent;
    time_t file_timestamp;
    float update_time_elapsed;
    uint64_t last_time;
    bool active;

    gs_pdf_file_t pdf;
};

static time_t get_modified_timestamp(const char *filename)
{
    struct stat stats;
    if (os_stat(filename, &stats) != 0)
        return -1;
    return stats.st_mtime;
}

static const char *pdf_source_get_name(void *unused)
{
    UNUSED_PARAMETER(unused);
    return obs_module_text("PdfSlideShow");
}

static void pdf_source_load(struct pdf_source *context)
{
    char *file = context->file;

    obs_enter_graphics();
    gs_pdf_file_free(&context->pdf);
    obs_leave_graphics();

    if (file && *file) {
        context->file_timestamp = get_modified_timestamp(file);
        gs_pdf_file_init(&context->pdf, file);
        context->update_time_elapsed = 0;

        obs_enter_graphics();
        gs_pdf_file_init_texture(&context->pdf);
        obs_leave_graphics();

        if (!context->pdf.loaded)
            warn("failed to load texture '%s'", file);
    }
}

static void pdf_source_unload(struct pdf_source *context)
{
    obs_enter_graphics();
    gs_pdf_file_free(&context->pdf);
    obs_leave_graphics();
}

static void pdf_source_update(void *data, obs_data_t *settings)
{
    struct pdf_source *context = data;
    const char *file = obs_data_get_string(settings, "file");
    const bool unload = obs_data_get_bool(settings, "unload");

    if (context->file)
        bfree(context->file);
    context->file = bstrdup(file);
    context->persistent = !unload;

    /* Load the image if the source is persistent or showing */
    if (context->persistent || obs_source_showing(context->source))
        pdf_source_load(data);
    else
        pdf_source_unload(data);
}

static void pdf_source_defaults(obs_data_t *settings)
{
    obs_data_set_default_bool(settings, "unload", false);
}

static void pdf_source_show(void *data)
{
    struct pdf_source *context = data;

    if (!context->persistent)
        pdf_source_load(context);
}

static void pdf_source_hide(void *data)
{
    struct pdf_source *context = data;

    if (!context->persistent)
        pdf_source_unload(context);
}

static void *pdf_source_create(obs_data_t *settings, obs_source_t *source)
{
    struct pdf_source *context = bzalloc(sizeof(struct pdf_source));
    context->source = source;

    pdf_source_update(context, settings);
    return context;
}

static void pdf_source_destroy(void *data)
{
    struct pdf_source *context = data;

    pdf_source_unload(context);

    if (context->file)
        bfree(context->file);
    bfree(context);
}

static uint32_t pdf_source_getwidth(void *data)
{
    struct pdf_source *context = data;
    return context->pdf.cx;
}

static uint32_t pdf_source_getheight(void *data)
{
    struct pdf_source *context = data;
    return context->pdf.cy;
}

static void pdf_source_render(void *data, gs_effect_t *effect)
{
    struct pdf_source *context = data;

    if (!context->pdf.texture)
        return;

    gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"),
                          context->pdf.texture);
    gs_draw_sprite(context->pdf.texture, 0, context->pdf.cx,
                   context->pdf.cy);
}

static void pdf_source_tick(void *data, float seconds)
{
    struct pdf_source *context = data;
    uint64_t frame_time = obs_get_video_frame_time();

    context->update_time_elapsed += seconds;

    if (context->update_time_elapsed >= 1.0f) {
        time_t t = get_modified_timestamp(context->file);
        context->update_time_elapsed = 0.0f;

        if (context->file_timestamp != t) {
            pdf_source_load(context);
        }
    }

    if (obs_source_active(context->source)) {
        if (!context->active) {
            context->active = true;
        }
    } else {
        if (context->active) {
            context->active = false;
        }
        return;
    }

    context->last_time = frame_time;
}

static const char *file_filter =
    "PDF Files (*.pdf);;"
    "All Files (*.*)";

static obs_properties_t *pdf_source_properties(void *data)
{
    struct pdf_source *s = data;
    struct dstr path = {0};

    obs_properties_t *props = obs_properties_create();

    if (s && s->file && *s->file) {
        const char *slash;

        dstr_copy(&path, s->file);
        dstr_replace(&path, "\\", "/");
        slash = strrchr(path.array, '/');
        if (slash)
            dstr_resize(&path, slash - path.array + 1);
    }

    obs_properties_add_path(props, "file", obs_module_text("PdfSlideShow.File"),
                OBS_PATH_FILE, file_filter, path.array);
    obs_properties_add_bool(props, "unload",
                obs_module_text("PdfSlideShow.UnloadWhenNotShowing"));
    dstr_free(&path);

    return props;
}

static struct obs_source_info pdf_source_info = {
    .id = "pdf_source",
    .type = OBS_SOURCE_TYPE_INPUT,
    .output_flags = OBS_SOURCE_VIDEO,
    .get_name = pdf_source_get_name,
    .create = pdf_source_create,
    .destroy = pdf_source_destroy,
    .update = pdf_source_update,
    .get_defaults = pdf_source_defaults,
    .show = pdf_source_show,
    .hide = pdf_source_hide,
    .get_width = pdf_source_getwidth,
    .get_height = pdf_source_getheight,
    .video_render = pdf_source_render,
    .video_tick = pdf_source_tick,
    .get_properties = pdf_source_properties,
    .icon_type = OBS_ICON_TYPE_IMAGE,
};

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("pdf-source", "en-US")
MODULE_EXPORT const char *obs_module_description(void)
{
    return "PDF sources";
}

bool obs_module_load(void)
{
    obs_register_source(&pdf_source_info);
    return true;
}