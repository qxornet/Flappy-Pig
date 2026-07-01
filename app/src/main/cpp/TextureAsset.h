#ifndef ANDROIDGLINVESTIGATIONS_TEXTUREASSET_H
#define ANDROIDGLINVESTIGATIONS_TEXTUREASSET_H

#include <memory>
#include <android/asset_manager.h>
#include <GLES3/gl3.h>
#include <string>
#include <vector>

class TextureAsset {
public:
    /*!
     * Загружает текстуру из каталога assets/.
     *
     * @param assetManager менеджер ресурсов Android.
     * @param assetPath путь к файлу ресурса.
     * @return shared_ptr на объект текстуры.
     * Ресурсы будут автоматически освобождены после уничтожения объекта.
     */
    static std::shared_ptr<TextureAsset>
    loadAsset(AAssetManager *assetManager, const std::string &assetPath);

    ~TextureAsset();

    /*!
     * @return идентификатор текстуры для использования в OpenGL.
     */
    constexpr GLuint getTextureID() const { return textureID_; }

private:
    inline TextureAsset(GLuint textureId) : textureID_(textureId) {}

    GLuint textureID_;
};

#endif // ANDROIDGLINVESTIGATIONS_TEXTUREASSET_H