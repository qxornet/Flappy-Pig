#include <android/imagedecoder.h>
#include "TextureAsset.h"
#include "AndroidOut.h"
#include "Utility.h"

std::shared_ptr<TextureAsset>
TextureAsset::loadAsset(AAssetManager *assetManager, const std::string &assetPath) {
    // Получаем изображение из менеджера ресурсов Android.
    auto pAndroidRobotPng = AAssetManager_open(
            assetManager,
            assetPath.c_str(),
            AASSET_MODE_BUFFER);

    // Создаём декодер для преобразования изображения в текстуру.
    AImageDecoder *pAndroidDecoder = nullptr;
    auto result = AImageDecoder_createFromAAsset(pAndroidRobotPng, &pAndroidDecoder);
    assert(result == ANDROID_IMAGE_DECODER_SUCCESS);

    // Убеждаемся, что данные будут декодированы в формат RGBA8888
    // (8 бит на каждый канал, порядок RGBA).
    AImageDecoder_setAndroidBitmapFormat(
            pAndroidDecoder,
            ANDROID_BITMAP_FORMAT_RGBA_8888);

    // Получаем информацию о заголовке изображения,
    // необходимую для дальнейшей настройки.
    const AImageDecoderHeaderInfo *pAndroidHeader = nullptr;
    pAndroidHeader = AImageDecoder_getHeaderInfo(pAndroidDecoder);

    // Получаем параметры изображения,
    // необходимые для загрузки в OpenGL.
    auto width = AImageDecoderHeaderInfo_getWidth(pAndroidHeader);
    auto height = AImageDecoderHeaderInfo_getHeight(pAndroidHeader);
    auto stride = AImageDecoder_getMinimumStride(pAndroidDecoder);

    // Получаем пиксельные данные изображения.
    auto upAndroidImageData =
            std::make_unique<std::vector<uint8_t>>(height * stride);

    auto decodeResult = AImageDecoder_decodeImage(
            pAndroidDecoder,
            upAndroidImageData->data(),
            stride,
            upAndroidImageData->size());

    assert(decodeResult == ANDROID_IMAGE_DECODER_SUCCESS);

    // Создаём текстуру OpenGL.
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Ограничиваем выборку краями текстуры.
    // Без этого при альфа-смешивании могут появляться артефакты.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Загружаем текстуру в видеопамять.
    glTexImage2D(
            GL_TEXTURE_2D,                 // Целевая текстура
            0,                             // Уровень mipmap
            GL_RGBA,                       // Внутренний формат хранения
            width,                         // Ширина текстуры
            height,                        // Высота текстуры
            0,                             // Граница (всегда 0)
            GL_RGBA,                       // Формат исходных данных
            GL_UNSIGNED_BYTE,              // Тип данных
            upAndroidImageData->data()     // Данные для загрузки
    );

    // Генерируем mip-уровни.
    // Для 2D-графики обычно не обязательны, но полезны.
    glGenerateMipmap(GL_TEXTURE_2D);

    // Освобождаем временные объекты.
    AImageDecoder_delete(pAndroidDecoder);
    AAsset_close(pAndroidRobotPng);

    // Создаём shared_ptr для автоматического управления временем жизни объекта.
    return std::shared_ptr<TextureAsset>(new TextureAsset(textureId));
}

TextureAsset::~TextureAsset() {
    // Освобождаем ресурсы текстуры.
    glDeleteTextures(1, &textureID_);
    textureID_ = 0;
}