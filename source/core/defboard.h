#ifndef DEFBOARD_H
#define DEFBOARD_H

////////////////////////////////////////////
// para referenciar objetos normales o por defecto, la Key del board padre tiene
// que ser una tupla {tipo,id} en la que tipo indique si es por defecto o no
///////////////////////////////////////////////

#include "board.h"
#include "boardexception.h"

template <typename Key, typename T, unsigned int Dimensions>
class DefBoard : public Board<std::tuple<Key, bool>, T, Dimensions + 1> {
protected:
  typedef Board<std::tuple<Key, bool>, T, Dimensions + 1>
      Base; /* Alias for the underlying board */

public:
  typedef typename Board<Key, T, Dimensions>::Position
      Position; /**< Alias for the tuples that represent
                        positions on the defboard */

protected:
  Key defaultid;

  enum PositionType {
    PositionTypeLimit = 0,
    PositionTypeDefault,
    PositionTypeNormal
  };
  enum ElementType { ElementTypeDefault = false, ElementTypeNormal = true };

  typename Base::Position aux_ToInnerPosition(PositionType type,
                                              const Position &position) {
    typename Base::Position newposition;
    newposition[0] = type;
    for (unsigned int i = 0; i < Dimensions; i++) {
      newposition[i + 1] = position[i];
    }
    return newposition;
  }

  Position aux_ToPublicPosition(const typename Base::Position &position) {
    Position newposition;
    for (uint i = 0; i < Dimensions; i++) {
      newposition[i] = position[i + 1];
    }
    return newposition;
  }

public:
  template <typename Derived>
  DefBoard(std::string name, Position limits, const Key &defaultid,
           const Derived &defaultelement)
      : Base(name, aux_ToInnerPosition(PositionTypeLimit, limits)) {
    this->defaultid = defaultid;
    this->Base::aux_addElem(std::make_tuple(defaultid, ElementTypeDefault),
                            defaultelement);
  };
  virtual ~DefBoard(){};

  Position getLimits() { return aux_ToPublicPosition(this->getLimits()); }

  /** Adds a copy of element to the element pool of the Board
   *  Derived must be a descendant from type T.
   *  Throws IDInUse if the id is already assigned.
   */
  template <typename Derived>
  void addElement(const Key &id, const Derived &element) {
    auto id_w = std::make_tuple(id, ElementTypeNormal);
    if (std::get<0>(this->Base::aux_checkIDExists(id_w)))
      throw IDInUse<Key, Position>(this->name, id);

    this->Base::aux_addElem(id_w, element);
  }

  /** Adds a copy of element to the default pool of the Board
   *  Derived must be a descendant from type T.
   *  Throws IDInUse if the id is already assigned.
   */
  template <typename Derived>
  void addDefault(const Key &id, const Derived &element) {
    auto id_w = std::make_tuple(id, ElementTypeDefault);
    if (std::get<0>(this->Base::aux_checkIDExists(id_w)))
      throw IDInUse<Key, Position>(this->name, id);

    this->Base::aux_addElem(id_w, element);
  }

  /** Updates the item identified by id with a copy of element
   *  Derived must be a descendant from type T.
   *  Throws IDNonExistent if there is no element with that id.
   */
  template <typename Derived>
  void updateElement(const Key &id, const Derived &element) {
    auto id_w = std::make_tuple(id, ElementTypeNormal);
    if (!std::get<0>(this->Base::aux_checkIDExists(id_w)))
      throw IDNonExistent<Key, Position>(this->name, id);

    this->Base::aux_addElem(id_w, element);
  }

  /** Updates the item identified by id with a copy of element
   *  Derived must be a descendant from type T.
   *  Throws IDNonExistent if there is no element with that id.
   */
  template <typename Derived>
  void updateDefault(const Key &id, const Derived &element) {
    auto id_w = std::make_tuple(id, ElementTypeDefault);
    if (!std::get<0>(this->Base::aux_checkIDExists(id_w)))
      throw IDNonExistent<Key, Position>(this->name, id);

    this->Base::aux_addElem(id_w, element);
  }

  /** Sets an element of the pool on the Board's surface
   *  Throws IDNonExistent if the id is not in the pool.
   *  Throws IDMonoSet if multiple_positions is false and the element is already
   * set once or more.
   *  Throws PositionOutLimits if the position is not inside the limits set at
   * construction.
   *  Throws PositionOccupied if there is already an element at the position.
   */
  void setElement(const Key &id, const Position &position,
                  bool multiple_positions = false) {
    auto id_w = std::make_tuple(id, ElementTypeNormal);
    auto position_w = aux_ToInnerPosition(PositionTypeNormal, position);

    auto info = this->Base::aux_checkIDExists(id_w);
    if (!std::get<0>(info))
      throw IDNonExistent<Key, Position>(this->name, id);

    if (!(multiple_positions || std::get<3>(info).empty()))
      throw IDMonoSet<Key, Position>(this->name, id);

    if (!this->Base::aux_checkLimits(position_w))
      throw PositionOutLimits<Key, Position>(this->name, position);

    info = this->Base::aux_checkOccupied(position_w);
    if (std::get<0>(info))
      throw PositionOccupied<Key, Position>(this->name, position);

    this->Base::aux_put(id_w, position_w);
  }

  /** Unsets an element from the Board's surface
   *  Throws IDNonExistent if the id is not in the pool.
   *  Throws IDNotOnBoard if the element is not set.
   *  Throws IDMultiSet if unset_all is false and the element is more than once.
   */
  void unSetElement(const Key &id, bool unset_all = false) {
    auto id_w = std::make_tuple(id, ElementTypeNormal);
    auto info = this->Base::aux_checkIDExists(
        id_w); /* Position 2 of this tuple contains a list of the
                  positions the element is on */
    if (!std::get<0>(info))
      throw IDNonExistent<Key, Position>(this->name, id);

    if (std::get<3>(info).empty()) {
      throw IDNotOnBoard<Key, Position>(this->name, id);
    } else if (!unset_all && std::get<3>(info).size() > 1) {
      throw IDMultiSet<Key, Position>(this->name, id);
    }

    for (auto i : std::get<3>(info))
      this->Base::aux_unput(i);
  }

  /** Unsets an element from the Board's surface
   *  Throws PositionOutLimits if the position is not inside the limits set at
   * construction.
   *  Throws PositionEmpty if there is no element.
   */
  void unSetElement(const Position &position) {
    auto position_w = aux_ToInnerPosition(PositionTypeNormal, position);

    if (!this->Base::aux_checkLimits(position_w))
      throw PositionOutLimits<Key, Position>(this->name, position);

    auto info = this->Base::aux_checkOccupied(position_w);
    if (!std::get<0>(info))
      throw PositionEmpty<Key, Position>(this->name, position);

    this->Base::aux_unput(position_w);
  }

  /** Sets an element of the pool on the Board's surface
   *  Throws IDNonExistent if the id is not in the pool.
   *  Throws IDMonoSet if multiple_positions is false and the element is already
   * set once or more.
   *  Throws PositionOutLimits if the position is not inside the limits set at
   * construction.
   *  Throws PositionOccupied if there is already an element at the position.
   */
  void setDefault(const Key &id, const Position &position) {
    auto id_w = std::make_tuple(id, ElementTypeDefault);
    auto position_w = aux_ToInnerPosition(PositionTypeDefault, position);

    auto info = this->Base::aux_checkIDExists(id_w);
    if (!std::get<0>(info))
      throw IDNonExistent<Key, Position>(this->name, id);

    if (!this->Base::aux_checkLimits(position))
      throw PositionOutLimits<Key, Position>(this->name, position);

    info = this->Base::aux_checkOccupied(position);
    if (std::get<0>(info))
      throw PositionOccupied<Key, Position>(this->name, position);

    this->Base::aux_put(id_w, position_w);
  }

  /** Unsets an element from the Board's surface
   *  Throws PositionOutLimits if the position is not inside the limits set at
   * construction.
   *  Throws PositionEmpty if there is no element.
   */
  void unSetDefault(const Position &position) {
    auto position_w = aux_ToInnerPosition(PositionTypeDefault, position);

    if (!this->Base::aux_checkLimits(position_w))
      throw PositionOutLimits<Key, Position>(this->name, position);

    auto info = this->Base::aux_checkOccupied(position_w);
    if (!std::get<0>(info))
      throw PositionEmpty<Key, Position>(this->name, position);

    this->Base::aux_unput(position_w);
  }

  /** Returns the element with the id and if it's real (true) or default (false)
   *  Throws IDNonExistent if the id is not in the pool.
   */
  std::tuple<bool, Key, std::shared_ptr<T>, std::list<Position>>
  getElement(const Key &id) {
    auto info =
        this->Base::aux_checkIDExists(std::make_tuple(id, ElementTypeNormal));
    if (!std::get<0>(info))
      throw IDNonExistent<Key, Position>(this->name, id);

    std::list<Position> positionlist;
    for (typename Base::Position i : std::get<3>(info)) {
      positionlist.push_back(aux_ToPublicPosition(i));
    }

    return std::make_tuple(
        std::get<0>(info),
        std::get<0>(std::get<1>(info) /*This returns (id,elemType)*/),
        std::get<2>(info), positionlist);
  }

  /** If there is an element set at the position, returns true and the element;
   * if there isn't one, returns false and the default element in that position;
   * if there is no default set for that position, returns false and the default
   * element declared at initialization.
   *  Throws PositionOutLimits if the position is not inside the limits set at
   * construction.
   */
  std::tuple<bool, Key, std::shared_ptr<T>, std::list<Position>>
  getElement(const Position &position) {
    auto position_w = aux_ToInnerPosition(PositionTypeNormal, position);
    auto positiondef_w = aux_ToInnerPosition(PositionTypeDefault, position);
    auto defaultid_w = std::make_tuple(this->defaultid, ElementTypeDefault);
    bool isNormal = true;

    if (!this->Base::aux_checkLimits(position_w))
      throw PositionOutLimits<Key, Position>(this->name, position);

    /* Looks for a normal element */
    auto info = this->Base::aux_checkOccupied(position_w);
    if (!std::get<0>(info)) {
      /* If there is no normal element, looks for custom default element */
      isNormal = false;
      info = this->Base::aux_checkOccupied(positiondef_w);
      if (!std::get<0>(info)) {
        /* If there is neither a custom default element, uses the master default
         * element */
        info = this->Base::aux_checkIDExists(defaultid_w);
      }
    }

    std::list<Position> positionlist;
    for (typename Base::Position i : std::get<3>(info)) {
      positionlist.push_back(aux_ToPublicPosition(i));
    }

    return std::make_tuple(
        isNormal,
        std::get<0>(std::get<1>(info) /* Inner get() returns (id,elemType) */),
        std::get<2>(info), positionlist);
  }

  /** Changes the position of an element
   *  Throws PositionOutLimits if one of the positions is not inside the limits
   * set at construction.
   *  Throws PositionEmpty if there is no element.at origin.
   *  Throws PositionOccupied if there is an element at destiny and
   * override_previous is false.
   */
  void moveElement(const Position &origin, const Position &destiny,
                   bool override_previous = false) {
    auto origin_w = aux_ToInnerPosition(PositionTypeNormal, origin);
    auto destiny_w = aux_ToInnerPosition(PositionTypeNormal, destiny);

    if (!this->Base::aux_checkLimits(origin_w))
      throw PositionOutLimits<Key, Position>(this->name, origin);

    if (!this->Base::aux_checkLimits(destiny_w))
      throw PositionOutLimits<Key, Position>(this->name, destiny);

    auto info = this->Base::aux_checkOccupied(origin_w);
    if (!std::get<0>(info))
      throw PositionEmpty<Key, Position>(this->name, origin);

    if (std::get<0>(this->Base::aux_checkOccupied(destiny_w))) {
      if (!override_previous)
        throw PositionOccupied<Key, Position>(this->name, destiny);
      else
        this->Base::aux_unput(destiny_w);
    }

    this->Base::aux_unput(origin_w);
    this->Base::aux_put(std::get<1>(info), destiny_w);
  }

  /** Changes the position of an element
   *  Throws IDNonExistent if the id is not in the pool.
   *  Throws PositionOutLimits if one of the positions is not inside the limits
   * set at construction.
   *  Throws PositionOccupied if there is an element at destiny and
   * override_previous is false.
   */
  void moveElement(const Key &id, const Position &destiny,
                   bool override_previous = false) {
    auto id_w = std::make_tuple(id, ElementTypeNormal);
    auto destiny_w = aux_ToInnerPosition(PositionTypeNormal, destiny);

    auto info = this->Base::aux_checkIDExists(id_w);
    if (!std::get<0>(info))
      throw IDNonExistent<Key, Position>(this->name, id);

    if (std::get<3>(info).empty()) {
      throw IDNotOnBoard<Key, Position>(this->name, id);
    } else if (std::get<3>(info).size() > 1) {
      throw IDMultiSet<Key, Position>(this->name, id);
    }

    if (!this->Base::aux_checkLimits(destiny_w))
      throw PositionOutLimits<Key, Position>(this->name, destiny);

    if (std::get<0>(this->Base::aux_checkOccupied(destiny_w))) {
      if (!override_previous)
        throw PositionOccupied<Key, Position>(this->name, destiny);
      else
        this->Base::aux_unput(destiny_w);
    }

    this->Base::aux_unput(std::get<3>(info).front());
    this->Base::aux_put(std::get<1>(info), destiny_w);
  }
};

#endif /* end of include guard: DEFBOARD_H */
