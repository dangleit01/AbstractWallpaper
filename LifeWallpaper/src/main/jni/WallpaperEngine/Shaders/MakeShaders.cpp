#include <stdlib.h>

#include "LogGL.h"
#include "MakeShaders.h"

const char * MakeShaders::v_star_shader =   "attribute float a_ArrayAngle;"
                                            "attribute vec4 a_ArrayColor;"
                                            "uniform vec2 u_Center;"
                                            "uniform vec2 u_Radius;"
                                            "uniform vec4 u_Arguments;"
                                            "uniform float u_PointSize;"
                                            "uniform float u_TotalDeltaSpeed;"
                                            "varying vec4 v_Color;"
                                            "void main(){"
                                            "   float x_coord = u_Center.x + u_Radius.x * (cos(a_ArrayAngle + u_TotalDeltaSpeed) + cos(u_Arguments.x * (a_ArrayAngle + u_TotalDeltaSpeed)) / u_Arguments.y);"
                                            "   float y_coord = u_Center.y + u_Radius.y * (sin(a_ArrayAngle + u_TotalDeltaSpeed) - sin(u_Arguments.z * (a_ArrayAngle + u_TotalDeltaSpeed)) / u_Arguments.w);"
                                            "   v_Color = a_ArrayColor;"
                                            "   gl_PointSize = u_PointSize;"
                                            "   gl_Position = vec4(x_coord, y_coord, 1.0, 1.0);"
                                            "}";

const char * MakeShaders::f_star_shader =   "precision mediump float;"
                                            "varying vec4 v_Color;"
                                            "uniform sampler2D t_texture;"
                                            "void main(){"
                                            "    gl_FragColor = v_Color * texture2D(t_texture, gl_PointCoord);"
                                            "}";

GLuint MakeShaders::compileShader(GLenum shaderType, const char* pSource) {
    // Создаём шейдер и получаем ссылку него, где shaderType - тип шейдера
    GLuint shader = glCreateShader(shaderType);

    if(shader){
        // Привязываем шейдер к исходнику
        glShaderSource(shader, 1, &pSource, NULL);
        // Компилим шейдер
        glCompileShader(shader);
        GLint compiled = 0;
        // Проверяем статус компиляции шейдера
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        // Если скомпилить не удалось, то выводим полный лог и удаляем шейдер который создали
        if (compiled == 0) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char * buf = (char *) malloc(infoLen);
                if(buf){
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }

                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint MakeShaders::createProgram(const char* pVertexSource, const char* pFragmentSource) {
    // Компилим пару шейдеров "vertex" и "fragment"
    // Компилим "vertex" шейдер
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, pVertexSource);
    if (vertexShader == 0) {
        return 0;
    }

    // Компилим "fragment" шейдер
    GLuint pixelShader = compileShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (pixelShader == 0) {
        return 0;
    }

    // Создаем программу и получаем ссылку на неё
    GLuint program = glCreateProgram();
    if (program) {
        // Привязываем шейдеры к программе
        glAttachShader(program, vertexShader);
        glAttachShader(program, pixelShader);

        // Линкуем программу
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        // Если не удалось слинковать, получаем статус и удаляем программу
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char * buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program: \n%s\n", buf);
                    free(buf);
                }
            }

            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}