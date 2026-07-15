#include "Score.h"

void ScoreInfo::draw(Renderer& renderer, int value)
{
    std::string text = std::to_string(value);


    float x = -renderer.getProjectionHalfHeight() * renderer.getRatio() + 0.3f;
    constexpr float step = 0.09f;

    for (size_t i = 0; i < text.size(); ++i)
    {
        int digit = text[i] - '0';

        digits[i].material.texture = textures[digit];
        digits[i].transform.position = {x + i * step, 1.8f, 0.0f};
        digits[i].transform.setScale(0.1f);

        renderer.draw(
                digits[i].mesh,
                digits[i].transform,
                digits[i].material);
    }
}