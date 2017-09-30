#include "utils.h"

char* file_read(const char* filename, int* size) {
    SDL_RWops *rw = SDL_RWFromFile(filename, "rb");
    if(!rw) return NULL;

    Sint64 res_size = SDL_RWsize(rw);
    char* res = (char*)SDL_malloc(res_size + 1);

    Sint64 nb_read_total = 0, nb_read = 1;
    char* buf = res;
    while (nb_read_total < res_size && nb_read != 0) {
        nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
        nb_read_total += nb_read;
        buf += nb_read;
    }
    SDL_RWclose(rw);
    if (nb_read_total != res_size) {
        SDL_free(res);
        return NULL;
    }

    res[nb_read_total] = '\0';
    if (size)  *size = nb_read_total;
    return res;
}
void print_log(GLuint object) {
    GLint log_length = 0;
    if (glIsShader(object)) {
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    } else if (glIsProgram(object)) {
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    } else {
        SDL_Log("printlog: Not a shader or a program\n");
        return;
    }
    char* log = SDL_stack_alloc(char, log_length);

    if (glIsShader(object))
        glGetShaderInfoLog(object, log_length, NULL, log);
    else if (glIsProgram(object))
        glGetProgramInfoLog(object, log_length, NULL, log);

    SDL_Log("printlog: %s\n", log);
    SDL_stack_free(log);
}

GLuint create_shader(const char* filename, GLenum type) {
    const GLchar* source = file_read(filename, NULL);
    if (source == NULL) {
        SDL_Log("Error opening \"%s\": %s", filename, SDL_GetError());
        return 0;
    }
    GLuint res = glCreateShader(type);
    glShaderSource(res, 1, &source, NULL);
    SDL_free((void*)source);

    glCompileShader(res);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE) {
        SDL_Log("\"%s\":\n", filename);
        print_log(res);
        glDeleteShader(res);
        return 0;
    }
    return res;
}

GLuint create_program(const char *vertexfile, const char *fragmentfile) {
    GLuint program = glCreateProgram();
    GLuint shader;
    if(vertexfile) {
        shader = create_shader(vertexfile, GL_VERTEX_SHADER);
        if(!shader)
            return 0;
        glAttachShader(program, shader);
    }
    if(fragmentfile) {
        shader = create_shader(fragmentfile, GL_FRAGMENT_SHADER);
        if(!shader)
            return 0;
        glAttachShader(program, shader);
    }

    glLinkProgram(program);
    GLint link_ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        SDL_Log("glLinkProgram:");
        print_log(program);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

#ifdef _GL_GEOMETRY_SHADER
GLuint create_gs_program(const char *vertexfile, const char *geometryfile, const char *fragmentfile, GLint input, GLint output, GLint vertices) {
    GLuint program = glCreateProgram();
    GLuint shader;

    if(vertexfile) {
        shader = create_shader(vertexfile, GL_VERTEX_SHADER);
        if(!shader)
            return 0;
        glAttachShader(program, shader);
    }

    if(geometryfile) {
        shader = create_shader(geometryfile, GL_GEOMETRY_SHADER);
        if(!shader)
            return 0;
        glAttachShader(program, shader);

        glProgramParameteriEXT(program, GL_GEOMETRY_INPUT_TYPE_EXT, input);
        glProgramParameteriEXT(program, GL_GEOMETRY_OUTPUT_TYPE_EXT, output);
        glProgramParameteriEXT(program, GL_GEOMETRY_VERTICES_OUT_EXT, vertices);
    }

    if(fragmentfile) {
        shader = create_shader(fragmentfile, GL_FRAGMENT_SHADER);
        if(!shader)
            return 0;
        glAttachShader(program, shader);
    }

    glLinkProgram(program);
    GLint link_ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "glLinkProgram:");
        print_log(program);
        glDeleteProgram(program);
        return 0;
    }

    return program;
}
#else
GLuint create_gs_program(const char *vertexfile, const char *geometryfile, const char *fragmentfile, GLint input, GLint output, GLint vertices) {
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                   "Missing support for geometry shaders.");
    return 0;
}
#endif

GLint get_attrib(GLuint program, const char *name) {
    GLint attribute = glGetAttribLocation(program, name);
    if(attribute == -1)
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                       "Could not bind attribute %s", name);
    return attribute;
}

GLint get_uniform(GLuint program, const char *name) {
    GLint uniform = glGetUniformLocation(program, name);
    if(uniform == -1)
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                       "Could not bind uniform %s", name);
    return uniform;
}

void print_opengl_info() {
    int major, minor, profile;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);
    const char* profile_str = "";
    if (profile & SDL_GL_CONTEXT_PROFILE_CORE)
        profile_str = "CORE";
    if (profile & SDL_GL_CONTEXT_PROFILE_COMPATIBILITY)
        profile_str = "COMPATIBILITY";
    if (profile & SDL_GL_CONTEXT_PROFILE_ES)
        profile_str = "ES";

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                   "OpenGL %d.%d %s", major, minor, profile_str);
}


const char *info_get_sdl_event_type(int type) {
    switch(type) {
        case SDL_FIRSTEVENT: return "SDL_FIRSTEVENT";
        case SDL_QUIT: return "SDL_QUIT";
        case SDL_APP_TERMINATING: return "SDL_APP_TERMINATING";
        case SDL_APP_LOWMEMORY: return "SDL_APP_LOWMEMORY";
        case SDL_APP_WILLENTERBACKGROUND: return "SDL_APP_WILLENTERBACKGROUND";
        case SDL_APP_DIDENTERBACKGROUND: return "SDL_APP_DIDENTERBACKGROUND";
        case SDL_APP_WILLENTERFOREGROUND: return "SDL_APP_WILLENTERFOREGROUND";
        case SDL_APP_DIDENTERFOREGROUND: return "SDL_APP_DIDENTERFOREGROUND";
        case SDL_WINDOWEVENT: return "SDL_WINDOWEVENT";
        case SDL_SYSWMEVENT: return "SDL_SYSWMEVENT";
        case SDL_KEYDOWN: return "SDL_KEYDOWN";
        case SDL_KEYUP: return "SDL_KEYUP";
        case SDL_TEXTEDITING: return "SDL_TEXTEDITING";
        case SDL_TEXTINPUT: return "SDL_TEXTINPUT";
        case SDL_KEYMAPCHANGED: return "SDL_KEYMAPCHANGED";
        case SDL_MOUSEMOTION: return "SDL_MOUSEMOTION";
        case SDL_MOUSEBUTTONDOWN: return "SDL_MOUSEBUTTONDOWN";
        case SDL_MOUSEBUTTONUP: return "SDL_MOUSEBUTTONUP";
        case SDL_MOUSEWHEEL: return "SDL_MOUSEWHEEL";
        case SDL_JOYAXISMOTION: return "SDL_JOYAXISMOTION";
        case SDL_JOYBALLMOTION: return "SDL_JOYBALLMOTION";
        case SDL_JOYHATMOTION: return "SDL_JOYHATMOTION";
        case SDL_JOYBUTTONDOWN: return "SDL_JOYBUTTONDOWN";
        case SDL_JOYBUTTONUP: return "SDL_JOYBUTTONUP";
        case SDL_JOYDEVICEADDED: return "SDL_JOYDEVICEADDED";
        case SDL_JOYDEVICEREMOVED: return "SDL_JOYDEVICEREMOVED";
        case SDL_CONTROLLERAXISMOTION: return "SDL_CONTROLLERAXISMOTION";
        case SDL_CONTROLLERBUTTONDOWN: return "SDL_CONTROLLERBUTTONDOWN";
        case SDL_CONTROLLERBUTTONUP: return "SDL_CONTROLLERBUTTONUP";
        case SDL_CONTROLLERDEVICEADDED: return "SDL_CONTROLLERDEVICEADDED";
        case SDL_CONTROLLERDEVICEREMOVED: return "SDL_CONTROLLERDEVICEREMOVED";
        case SDL_CONTROLLERDEVICEREMAPPED: return "SDL_CONTROLLERDEVICEREMAPPED";
        case SDL_FINGERDOWN: return "SDL_FINGERDOWN";
        case SDL_FINGERUP: return "SDL_FINGERUP";
        case SDL_FINGERMOTION: return "SDL_FINGERMOTION";
        case SDL_DOLLARGESTURE: return "SDL_DOLLARGESTURE";
        case SDL_DOLLARRECORD: return "SDL_DOLLARRECORD";
        case SDL_MULTIGESTURE: return "SDL_MULTIGESTURE";
        case SDL_CLIPBOARDUPDATE: return "SDL_CLIPBOARDUPDATE";
        case SDL_DROPFILE: return "SDL_DROPFILE";
        case SDL_DROPTEXT: return "SDL_DROPTEXT";
        case SDL_DROPBEGIN: return "SDL_DROPBEGIN";
        case SDL_DROPCOMPLETE: return "SDL_DROPCOMPLETE";
        case SDL_AUDIODEVICEADDED: return "SDL_AUDIODEVICEADDED";
        case SDL_AUDIODEVICEREMOVED: return "SDL_AUDIODEVICEREMOVED";
        case SDL_RENDER_TARGETS_RESET: return "SDL_RENDER_TARGETS_RESET";
        case SDL_RENDER_DEVICE_RESET: return "SDL_RENDER_DEVICE_RESET";
        case SDL_USEREVENT: return "SDL_USEREVENT";
        case SDL_LASTEVENT: return "SDL_LASTEVENT";
    }
    return "???";
}
void info_sdl_event_window(SDL_WindowEvent *pEvent) {
    switch (pEvent->event) {
        case SDL_WINDOWEVENT_SHOWN:
            SDL_Log("Window %d shown", pEvent->windowID);
            break;
        case SDL_WINDOWEVENT_HIDDEN:
            SDL_Log("Window %d hidden", pEvent->windowID);
            break;
        case SDL_WINDOWEVENT_EXPOSED:
            SDL_Log("Window %d exposed", pEvent->windowID);
            break;
        case SDL_WINDOWEVENT_MOVED:
            SDL_Log("Window %d moved to %d,%d",
                    pEvent->windowID, pEvent->data1,
                    pEvent->data2);
            break;
        case SDL_WINDOWEVENT_RESIZED:
            SDL_Log("Window %d resized to %dx%d",
                    pEvent->windowID, pEvent->data1,
                    pEvent->data2);
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            SDL_Log("Window %d size changed to %dx%d",
                    pEvent->windowID, pEvent->data1,
                    pEvent->data2);
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            SDL_Log("Window %d minimized", pEvent->windowID);
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            SDL_Log("Window %d maximized", pEvent->windowID);
            break;
        case SDL_WINDOWEVENT_RESTORED:
            SDL_Log("Window %d restored", pEvent->windowID);
            break;
        case SDL_WINDOWEVENT_ENTER:
            SDL_Log("Mouse entered window %d",
                    pEvent->windowID);
            break;
        case SDL_WINDOWEVENT_LEAVE:
            SDL_Log("Mouse left window %d", pEvent->windowID);
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            SDL_Log("Window %d gained keyboard focus",
                    pEvent->windowID);
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            SDL_Log("Window %d lost keyboard focus",
                    pEvent->windowID);
            break;
        case SDL_WINDOWEVENT_CLOSE:
            SDL_Log("Window %d closed", pEvent->windowID);
            break;
        case SDL_WINDOWEVENT_TAKE_FOCUS:
            SDL_Log("Window %d is offered a focus", pEvent->windowID);
            break;
        case SDL_WINDOWEVENT_HIT_TEST:
            SDL_Log("Window %d has a special hit test", pEvent->windowID);
            break;
        default:
            SDL_Log("Window %d got unknown event %d",
                    pEvent->windowID, pEvent->event);
            break;
        }
}
