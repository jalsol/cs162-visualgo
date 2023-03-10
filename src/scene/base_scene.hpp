#ifndef SCENE_BASE_SCENE_HPP_
#define SCENE_BASE_SCENE_HPP_

namespace scene::internal {

class BaseScene {
public:
    BaseScene() = default;
    BaseScene(const BaseScene&) = delete;
    BaseScene(BaseScene&&) = delete;
    BaseScene& operator=(const BaseScene&) = delete;
    BaseScene& operator=(BaseScene&&) = delete;

    virtual ~BaseScene() = default;

    virtual void render() {}
    virtual void interact(){};
};

}  // namespace scene::internal

#endif  // SCENE_BASE_SCENE_HPP_
