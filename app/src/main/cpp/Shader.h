#ifndef ANDROIDGLINVESTIGATIONS_SHADER_H
#define ANDROIDGLINVESTIGATIONS_SHADER_H

#include <string>
#include <GLES3/gl3.h>
#include "Components.h"

class Model;

/*!
 * Класс, представляющий простую шейдерную программу. Она состоит из вершинного и
 * фрагментного шейдеров. Входными атрибутами являются позиция (Vector3) и UV-координаты
 * (Vector2). Также используется uniform-переменная, содержащая полную матрицу
 * Model/View/Projection. Шейдер ожидает одну текстуру для фрагментного шейдинга и не
 * выполняет никаких дополнительных вычислений освещения (поэтому не использует uniform'ы
 * источников света или атрибуты нормалей).
 */
class Shader {
public:
    /*!
     * Загружает шейдер по полному исходному коду и именам необходимых атрибутов и uniform'ов
     * для привязки. Возвращает корректный объект Shader при успехе или nullptr при ошибке.
     * Ресурсы шейдера автоматически освобождаются при уничтожении объекта.
     *
     * @param vertexSource Полный исходный код вершинного шейдера
     * @param fragmentSource Полный исходный код фрагментного шейдера
     * @param positionAttributeName Имя атрибута позиции в вершинном шейдере
     * @param uvAttributeName Имя атрибута UV-координат в вершинном шейдере
     * @param projectionMatrixUniformName Имя uniform-переменной матрицы Model/View/Projection
     * @return Корректный объект Shader при успехе, иначе nullptr.
     */
    static Shader *loadShader(
            const std::string &vertexSource,
            const std::string &fragmentSource,
            const std::string &positionAttributeName,
            const std::string &uvAttributeName,
            const std::string &projectionMatrixUniformName,
            const std::string &objPositionUniformName,
            const std::string &objScaleUniformName);

    inline ~Shader() {
        if (program_) {
            glDeleteProgram(program_);
            program_ = 0;
        }
    }

    /*!
     * Подготавливает шейдер к использованию.
     * Вызывайте этот метод перед выполнением любых команд отрисовки.
     */
    void activate() const;

    /*!
     * Завершает использование шейдера.
     * Вызывайте этот метод после выполнения всех команд отрисовки.
     */
    void deactivate() const;

    /*!
     * Отрисовывает одну модель.
     * @param model Модель для отрисовки
     */
    void draw(std::shared_ptr<MeshComponent> mesh, const TransformComponent &transform, const MaterialComponent &component) const;

    /*!
     * Устанавливает матрицу Model/View/Projection в шейдер.
     * @param projectionMatrix Массив из шестнадцати чисел с плавающей точкой,
     * расположенных в порядке столбцов (column-major), определяющий матрицу
     * проекции OpenGL.
     */
    void setProjectionMatrix(float *projectionMatrix) const;

private:
    /*!
     * Вспомогательная функция для загрузки шейдера заданного типа.
     * @param shaderType Тип шейдера OpenGL. Должен быть либо GL_VERTEX_SHADER,
     * либо GL_FRAGMENT_SHADER.
     * @param shaderSource Полный исходный код шейдера
     * @return Идентификатор шейдера, возвращаемый glCreateShader,
     * либо 0 в случае ошибки.
     */
    static GLuint loadShader(GLenum shaderType, const std::string &shaderSource);

    /*!
     * Создаёт новый экземпляр шейдера. Используйте метод @a loadShader.
     * @param program Идентификатор GL-программы шейдера
     * @param position Расположение (location) атрибута позиции
     * @param uv Расположение (location) атрибута UV-координат
     * @param projectionMatrix Расположение (location) uniform-переменной матрицы проекции
     */
    constexpr Shader(
            GLuint program,
            GLint position,
            GLint uv,
            GLint projectionMatrix,
            GLint objPosition,
            GLint objScale)
            : program_(program),
              position_(position),
              uv_(uv),
              projectionMatrix_(projectionMatrix),
              objPosition_(objPosition),
              objScale_(objScale){}

    GLuint program_;
    GLint position_;
    GLint uv_;
    GLint projectionMatrix_;
    GLint objPosition_;
    GLint objScale_;

};

#endif //ANDROIDGLINVESTIGATIONS_SHADER_H