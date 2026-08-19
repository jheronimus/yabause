/* Copyright (C) 2010-2018 The RetroArch team
 *
 * ---------------------------------------------------------------------------------------
 * The following license statement only applies to this libretro SDK code part
 * (glsym).
 * ---------------------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <glsym/glsym.h>
#include <glsym/rglgen.h>

#if defined(__APPLE__) || defined(ARCH_IS_MACOSX)
typedef void (*real_glTexImage2D_t)(GLenum, GLint, GLint, GLsizei, GLsizei,
                                    GLint, GLenum, GLenum, const GLvoid *);
static real_glTexImage2D_t real_glTexImage2D = NULL;

/* Apple Metal-GL does not allocate the backing Metal texture until at least
 * one pixel write has occurred.  Calling glTexImage2D with pixels==NULL leaves
 * the texture in an "unloadable" state that the driver rejects when the shader
 * tries to sample it.  Intercept the call and substitute a zeroed buffer so
 * Metal allocates the texture immediately. */
static void rgl_glTexImage2D_shim(GLenum target, GLint level,
                                  GLint internalformat, GLsizei width,
                                  GLsizei height, GLint border, GLenum format,
                                  GLenum type, const GLvoid *pixels) {
  if (!real_glTexImage2D)
    return;
  if (pixels == NULL && width > 0 && height > 0) {
    /* Compute bytes per pixel for the most common types used by the core. */
    int bpp = 4; /* default: GL_RGBA / GL_UNSIGNED_BYTE */
    if (type == 0x8367 /* GL_UNSIGNED_INT_8_8_8_8_REV */ ||
        type == 0x8035 /* GL_UNSIGNED_BYTE_3_3_2 */)
      bpp = 4;
    else if (type == 0x8363 /* GL_UNSIGNED_SHORT_5_6_5 */)
      bpp = 2;
    size_t sz = (size_t)width * (size_t)height * (size_t)bpp;
    void *zeros = calloc(1, sz);
    if (zeros) {
      real_glTexImage2D(target, level, internalformat, width, height, border,
                        format, type, zeros);
      free(zeros);
      return;
    }
  }
  real_glTexImage2D(target, level, internalformat, width, height, border,
                    format, type, pixels);
}

typedef void (*real_glShaderSource_t)(GLuint, GLsizei, const GLchar **,
                                      const GLint *);
static real_glShaderSource_t real_glShaderSource = NULL;

static void rgl_glShaderSource_shim(GLuint shader, GLsizei count,
                                    const GLchar **string,
                                    const GLint *length) {
  if (!real_glShaderSource)
    return;
  const GLchar **new_strings =
      (const GLchar **)malloc(sizeof(GLchar *) * count);
  char **allocated = (char **)calloc(count, sizeof(char *));
  for (GLsizei i = 0; i < count; i++) {
    if (string && string[i]) {
      const char *v430 = strstr(string[i], "#version 430");
      const char *v400 = strstr(string[i], "#version 400");
      int needs_rewrite = (v430 || v400);

      /* layout(binding=N) on samplers/UBOs is GLSL 4.20+; macOS only
       * exposes 4.10. The core sets samplers via glUniform1i() anyway. */
      const char *has_binding = strstr(string[i], "layout(binding");
      if (!has_binding)
        has_binding = strstr(string[i], "layout (binding");
      if (!has_binding)
        has_binding = strstr(string[i], "layout( binding");

      if (needs_rewrite || has_binding) {
        char *dup = strdup(string[i]);

        /* Pass 1 – version rewrite */
        if (needs_rewrite) {
          char *target = strstr(dup, "#version 430");
          if (!target)
            target = strstr(dup, "#version 400");
          if (target) {
            target[9] = '3';
            target[10] = '3';
            target[11] = '0';
          }
        }

        /* Pass 2 – strip layout(binding=N) from uniform declarations */
        if (has_binding) {
          char *p = dup;
          while (*p) {
            char *lay = strstr(p, "layout");
            if (!lay)
              break;
            char *eol = strchr(lay, '\n');
            size_t line_len = eol ? (size_t)(eol - lay) : strlen(lay);
            int has_bind = 0, has_uni = 0;
            for (size_t k = 0; k < line_len; k++) {
              if (!has_bind && strncmp(lay + k, "binding", 7) == 0)
                has_bind = 1;
              if (!has_uni && strncmp(lay + k, "uniform", 7) == 0)
                has_uni = 1;
            }
            if (has_bind && has_uni) {
              char *close_paren = strchr(lay, ')');
              if (close_paren && close_paren < lay + line_len) {
                size_t erase_len = (size_t)(close_paren - lay) + 1;
                memset(lay, ' ', erase_len);
                p = lay + erase_len;
                continue;
              }
            }
            p = lay + 6;
          }
        }

        new_strings[i] = dup;
        allocated[i] = dup;
      } else {
        new_strings[i] = string[i];
      }
    } else {
      new_strings[i] = NULL;
    }
  }
  real_glShaderSource(shader, count, new_strings, length);
  for (GLsizei i = 0; i < count; i++) {
    if (allocated[i])
      free(allocated[i]);
  }
  free(allocated);
  free(new_strings);
}


static void dummy_glPushDebugGroup(GLenum source, GLuint id, GLsizei length,
                                   const GLchar *message) {}
static void dummy_glPopDebugGroup(void) {}
#endif

void rglgen_resolve_symbols_custom(rglgen_proc_address_t proc,
                                   const struct rglgen_sym_map *map) {
  for (; map->sym; map++) {
    rglgen_func_t func = proc(map->sym);
#if defined(__APPLE__) || defined(ARCH_IS_MACOSX)
    if (strcmp(map->sym, "glShaderSource") == 0 ||
        strcmp(map->sym, "glShaderSourceARB") == 0) {
      real_glShaderSource = (real_glShaderSource_t)func;
      func = (rglgen_func_t)rgl_glShaderSource_shim;
    } else if (strcmp(map->sym, "glTexImage2D") == 0) {
      real_glTexImage2D = (real_glTexImage2D_t)func;
      func = (rglgen_func_t)rgl_glTexImage2D_shim;
    } else if (strcmp(map->sym, "glPushDebugGroup") == 0 ||
               strcmp(map->sym, "glPushDebugGroupKHR") == 0) {
      if (!func)
        func = (rglgen_func_t)dummy_glPushDebugGroup;
    } else if (strcmp(map->sym, "glPopDebugGroup") == 0 ||
               strcmp(map->sym, "glPopDebugGroupKHR") == 0) {
      if (!func)
        func = (rglgen_func_t)dummy_glPopDebugGroup;
    }
#endif
    memcpy(map->ptr, &func, sizeof(func));
  }
}

void rglgen_resolve_symbols(rglgen_proc_address_t proc) {
  if (!proc)
    return;

  rglgen_resolve_symbols_custom(proc, rglgen_symbol_map);
}
