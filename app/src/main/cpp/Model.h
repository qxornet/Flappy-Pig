#ifndef ANDROIDGLINVESTIGATIONS_MODEL_H
#define ANDROIDGLINVESTIGATIONS_MODEL_H

#include <vector>
#include "Global.h"
#include "Components.h"

class Model {
public:
    inline Model(
            std::shared_ptr<MeshComponent> e_mesh,
            std::shared_ptr<TextureAsset> e_texture)
            : mesh(std::move(e_mesh)),
              material(std::move(e_texture)) {}


    const Vector3& getPosition() const {
        return transform.position;
    }

    void setX(float x) {
        transform.position.x = x;
    }

    void setY(float y) {
        transform.position.y = y;
    }

    void moveX(float dx) {
        transform.position.x += dx;
    }

    void moveY(float dy) {
        transform.position.y += dy;
    }

    void setDepth(float depth) {
        transform.position.z = depth;
    }

    const Vector3& getScale() const {
        return transform.scale;
    }

    void setScale(float scale) {
        transform.scale = {scale, scale, scale};
    }

    const TransformComponent &getTransform() {
        return transform;
    }

    std::shared_ptr<MeshComponent> getMesh() {
        return mesh;
    }

    const MaterialComponent &getMaterial() {
        return material;
    }

private:
    std::shared_ptr<MeshComponent> mesh;
    TransformComponent transform;
    MaterialComponent material;
};

#endif //ANDROIDGLINVESTIGATIONS_MODEL_H