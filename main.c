/**
 * fb2png  Save screenshot into .png.
 *
 * Copyright (C) 2012  Kyan <kyan.ql.he@gmail.com>
 * Copyright (C) 2014  philz-cwm6 <phytowardt@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "fb2png.h"
#include "log.h"

#ifdef ANDROID
    #define DEFAULT_SAVE_PNG_PATH "/data/local/fbdump.png"
    #define DEFAULT_SAVE_JPEG_PATH "/data/local/fbdump.jpg"
#else
    #define DEFAULT_SAVE_PNG_PATH "fbdump.png"
    #define DEFAULT_SAVE_JPEG_PATH "fbdump.jpg"
#endif

int out_format = OUT_FORMAT_PNG;

void print_version() {
    D(
        "\n"
        "Android Screenshooter - fb2png\n"
        "Author: Kyan He <kyan.ql.he@gmail.com>\n"
        "Updated by Phil3759 & McKael @xda\n"
        "v1.0.0 <2014> \n"
        "\n"
    );
}

int print_usage() {
    print_version();
    D(
        "Usage: fb2png [-option=][path/to/output.png]\n"
        "   The default output path is /data/local/fbdump.png\n"
        "Options: \n"
	"   -format=png  png jpeg\n"
        "   -buffer=n  0:single 1:double... buffering (default=auto detect)\n"
        "\n"
    );

    return 0;
}

int parse_options(const char* option) {
    char buffer_opt[] = "-buffer=";
    char format_opt[] = "-format=";
    int found_option = 0;
    int value;

    if (strncmp(option, buffer_opt, strlen(buffer_opt)) == 0) {
        value = (int)atol(option + strlen(buffer_opt));
        if (value >= 0 && value <= MAX_ALLOWED_FB_BUFFERS) {
            user_set_buffers_num = value;
            found_option = 1;
        } else {
            D("Invalid buffer option (%d)\n", value);
            found_option = -1;
        }
    } else if (strncmp(option, format_opt, strlen(format_opt)) == 0) {
        char *svalue = (char*)(option + strlen(format_opt));
	if (strncmp(svalue, "jpeg", 4) == 0) {
	  out_format = OUT_FORMAT_JPEG;
	} else {
	  out_format = OUT_FORMAT_PNG;
	}
	found_option = 1;
    } else if (strcmp(option, "-help") == 0 || strcmp(option, "--help") == 0 || strcmp(option, "-h") == 0) {
        // print help and exit
        found_option = -1;
    }

    return found_option;
}

int main(int argc, char **argv)
{
    char *path = NULL;
    int ret = 0;
    int i = 1;

    while (i < argc && path == NULL) {
        ret = parse_options(argv[i]);
        if (ret == 0)
            path = argv[i];
        else if (ret < 0)
            return print_usage();
        ++i;
    }

    // all options must come before path
    if (i != argc)
        return print_usage();

    if (path == NULL)
    {
	if (out_format == OUT_FORMAT_JPEG)
	  path = DEFAULT_SAVE_JPEG_PATH;
	else
	  path = DEFAULT_SAVE_PNG_PATH;
    }

    if (strlen(path) >= PATH_MAX) {
        D("Output path too long!\n");
        return -1;
    }

    print_version();
    D("%s -buffer=%d%s -format=%d %s\n",
            argv[0],
            user_set_buffers_num,
            user_set_buffers_num < 0 ? " (auto)" : "",
	    out_format,
            path
    );

    ret = fb2png(out_format, path);
    if (!ret)
        D("Image saved to %s\n", path);

    return ret;
}
