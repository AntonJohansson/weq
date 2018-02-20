#pragma once

namespace weq{

class EntityManager;

class Entity{
public:
  static const u64 INVALID;

  Entity(EntityManager* manager, u64 id)
    : _id(id),
      _manager(manager){}

  Entity(const Entity& other)             = default;
  Entity& operator= (const Entity& other) = default;



  operator bool() const{
    return valid();
  }

  bool operator== (const Entity& other) const{
    return other._manager == _manager && other._id == _id;
  }

  bool operator!= (const Entity& other) const{
    return !(other == *this);
  }

  bool operator< (const Entity& other) const{
    return other._id < _id;
  }




  bool valid(){
    return _manager && _manager->valid(_id);
  }

  void invalidate(){
    _id = INVALID;
    _manager = nullptr;
  }

  u64 id(){return _id;}


private:
  friend class EntityManager;

  EntityManager* _manager = nullptr;
  u64 _id = INVALID;
};

// @TODO move to cpp
const u64 Entity::INVALID = -1;

}
