#ifndef ANDROIDGLINVESTIGATIONS_UTILITY_H
#define ANDROIDGLINVESTIGATIONS_UTILITY_H

#include <cassert>

class Utility {
public:
    static bool checkAndLogGlError(bool alwaysLog = false);

    static inline void assertGlError() { assert(checkAndLogGlError()); }

    /**
     * Создает матрицу ортографической проекции на основе половины высоты,
     * соотношения сторон, ближней и дальней плоскостей отсечения.
     *
     * @param outMatrix матрица, в которую будет записан результат
     * @param halfHeight половина высоты экрана
     * @param aspect отношение ширины экрана к его высоте
     * @param near расстояние до ближней плоскости отсечения
     * @param far расстояние до дальней плоскости отсечения
     * @return созданная матрица; совпадает с @a outMatrix, что позволяет
     *     при необходимости объединять вызовы в цепочку
     */
    static float *buildOrthographicMatrix(
            float *outMatrix,
            float halfHeight,
            float aspect,
            float near,
            float far);

    static float *buildIdentityMatrix(float *outMatrix);
};

#endif //ANDROIDGLINVESTIGATIONS_UTILITY_H