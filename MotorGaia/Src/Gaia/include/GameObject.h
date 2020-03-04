#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <OgreSceneNode.h>
#include <string>
#include <map>

#include "Transform.h"

class Scene;

class GameObject {
    friend class SceneManager;
    friend class UserComponent;
public:
    GameObject(const std::string& name, const std::string& tag, Scene* scene);
    ~GameObject();

	Ogre::SceneNode* node = nullptr;
	Transform* transform = nullptr;

    template<typename T>
    T* addComponent();

    template<typename T>
    bool delComponent();

    template<typename T>
    T* getComponent();
    
	void addChildNode(GameObject* child);
    const std::string& getName() const;
    const std::string& getTag() const;
    Scene* getScene() const;

private:
    bool addComponent(const std::string& componentName, Component* component);

public:

private: 
    const std::string name;
    const std::string tag;

    Scene* myScene;
    std::map<std::string, Component*> components;
};

template<typename T>
T* GameObject::getComponent() {
    const std::string key = Component::nameID<T>.id;
    if (components.find(key) == components.end())
        return nullptr;

    return (T*)components[key];
}

#endif