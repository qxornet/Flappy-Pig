#include "Shader.h"

#include "AndroidOut.h"
#include "Model.h"
#include "Utility.h"

Shader *Shader::loadShader(
        const std::string &vertexSource,
        const std::string &fragmentSource,
        const std::string &positionAttributeName,
        const std::string &uvAttributeName,
        const std::string &projectionMatrixUniformName,
        const std::string &objPositionUniformName,
        const std::string &objScaleUniformName) {
    Shader *shader = nullptr;

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader) {
        return nullptr;
    }

    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        return nullptr;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint logLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

            // Если не удалось слинковать шейдерную программу, выводим лог для отладки
            if (logLength) {
                GLchar *log = new GLchar[logLength];
                glGetProgramInfoLog(program, logLength, nullptr, log);
                aout << "Failed to link program with:\n" << log << std::endl;
                delete[] log;
            }

            glDeleteProgram(program);
        } else {
            // Получаем расположения атрибутов и uniform-переменных по их именам.
            // Также можно жестко задать индексы через layout= в шейдере,
            // но в данном примере это не используется.
            GLint positionAttribute = glGetAttribLocation(program, positionAttributeName.c_str());
            GLint uvAttribute = glGetAttribLocation(program, uvAttributeName.c_str());
            GLint projectionMatrixUniform = glGetUniformLocation(
                    program,
                    projectionMatrixUniformName.c_str());

            GLint objPositionUniform =
                    glGetUniformLocation(
                            program,
                            objPositionUniformName.c_str());

            GLint objScaleUniform =
                    glGetUniformLocation(
                            program,
                            objScaleUniformName.c_str());

            // Создаем объект шейдера только если найдены все необходимые атрибуты.
            if (positionAttribute != -1
                && uvAttribute != -1
                && projectionMatrixUniform != -1
                && objPositionUniform != -1
                && objScaleUniform != -1) {

                shader = new Shader(
                        program,
                        positionAttribute,
                        uvAttribute,
                        projectionMatrixUniform,
                        objPositionUniform,
                        objScaleUniform);
            } else {
                glDeleteProgram(program);
            }
        }
    }

    // После линковки программы отдельные объекты шейдеров больше не нужны.
    // Освобождаем выделенную для них память.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shader;
}

GLuint Shader::loadShader(GLenum shaderType, const std::string &shaderSource) {
    Utility::assertGlError();
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        auto *shaderRawString = (GLchar *) shaderSource.c_str();
        GLint shaderLength = shaderSource.length();
        glShaderSource(shader, 1, &shaderRawString, &shaderLength);
        glCompileShader(shader);

        GLint shaderCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);

        // Если шейдер не компилируется, выводим сообщение об ошибке в терминал для отладки
        if (!shaderCompiled) {
            GLint infoLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);

            if (infoLength) {
                auto *infoLog = new GLchar[infoLength];
                glGetShaderInfoLog(shader, infoLength, nullptr, infoLog);
                aout << "Failed to compile with:\n" << infoLog << std::endl;
                delete[] infoLog;
            }

            glDeleteShader(shader);
            shader = 0;
        }
    }
    return shader;
}

void Shader::activate() const {
    glUseProgram(program_);
}

void Shader::deactivate() const {
    glUseProgram(0);
}

void Shader::draw(std::shared_ptr<MeshComponent> mesh, const TransformComponent& transform, const MaterialComponent& material) const
{
    const auto& texture = material.texture;

    const auto& pos = transform.position;
    const auto& scale = transform.scale;

    glUniform3f(
            objScale_,
            scale.x,
            scale.y,
            scale.z);

    glUniform3f(
            objPosition_,
            pos.x,
            pos.y,
            pos.z);

    // Атрибут позиции состоит из 3 чисел с плавающей точкой (x, y, z)
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glVertexAttribPointer(
            position_, // атрибут
            3, // количество элементов
            GL_FLOAT, // тип данных float
            GL_FALSE, // не выполнять нормализацию
            sizeof(Vertex), // шаг между вершинами в байтах
            (void*)offsetof(Vertex, position) // данные начинаются с начала массива вершин
    );
    glEnableVertexAttribArray(position_);

    // Атрибут UV-координат состоит из 2 чисел с плавающей точкой (u, v)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glVertexAttribPointer(
            uv_, // атрибут
            2, // количество элементов
            GL_FLOAT, // тип данных float
            GL_FALSE, // не выполнять нормализацию
            sizeof(Vertex), // шаг между вершинами в байтах
            (void*)offsetof(Vertex, uv) // смещение после Vector3
    );
    glEnableVertexAttribArray(uv_);

    // Настраиваем текстуру
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->getTextureID());

    // Отрисовываем индексированные треугольники
    glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_SHORT, nullptr);

    glDisableVertexAttribArray(uv_);
    glDisableVertexAttribArray(position_);
}

void Shader::setProjectionMatrix(float *projectionMatrix) const {
    glUniformMatrix4fv(projectionMatrix_, 1, false, projectionMatrix);
}