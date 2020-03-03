#pragma once

#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>

class GameObject;
class ComponentData;

template<typename T>
struct ComponentID
{
	std::string id;
};

class Component
{
public:
	friend class ComponentManager;
	// Devuelve el componente de tipo indicado suponiendo que existe en el mismo owner,
	// si no, devuelve un nullptr  
	template<typename T>
	T* getComponent();

	// Busca a partir de la referencia de Scene desde el owner un objeto en la misma escena
	// con el nombre indicado
	template<typename T>
	GameObject* findGameObjectWithName(std::string name);

	// Busca a partir de la referencia de Scene desde el owner un objeto en la misma escena
	// con el tag indicado
	template<typename T>
	GameObject* findGameObjectWithTag(std::string tag);

	//Da valor a las variables del componente segun el ComponentData que reciba
	//Usando if else para elegir la propiedad que modifica if(ComponentData->name == name)
	//Usando sstream se da valor a la variable ->                sstream>>variable
	virtual void handleData(ComponentData* data);

protected:
	Component(GameObject* gameObject);


public:
	void setActive(bool active);
	bool isActive();

	template<typename T>
	static const std::string& getID();

private:
	GameObject* gameObject;

	template<typename T>
	static ComponentID<T> nameID;

	bool active;
};


template<typename T>
ComponentID<T> Component::nameID = { "not assigned" };

template<typename T>
inline const std::string& Component::getID()
{
	return nameID<T>.id;
}

#endif
