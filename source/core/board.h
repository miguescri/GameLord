#ifndef BOARD_H
#define BOARD_H

#include <array>       // std::array
#include <exception>   // std::exception
#include <functional>  // std::ref
#include <list>        // std::list
#include <map>         // std::map
#include <memory>      // std::shared_ptr
#include <string>      // std::string
#include <tuple>       // std::tuple
#include <type_traits> // std::is_base_of std::is_same

#include "boardexception.h"

/** A N-dimensional board where put stuff
 *  A container of elements that inherit drom type T. These elements can be set
 * over a N-dimensional space in which each cell is identified by a N-index.
 */
template <typename Key, typename T, unsigned int Dimensions> class Board {
public:
  typedef std::array<unsigned int, Dimensions>
      Position; /**< A N-tuple that determines a point in a N-space.
                        Values in each dimension must be greater than 0
                        (0 means infinite and only has sense when setting
                        limits) */

  bool insideSpace(const Position &point, const Position &space) const {
    for (auto p = point.begin(), s = space.begin(); p < point.end(); p++, s++) {
      if ((*s != 0 && *s < *p) /*point outside space*/ ||
          *p == 0 /*point undefined in infinite*/) {
        return false;
      }
    }
    return true;
  }

private:
  /** Internal info container
   *  Every element is stored with all its current positions
   */
  struct Box {
    std::shared_ptr<T> element;
    std::list<Position> positions;
  };

  typedef decltype(std::map<Key, Box>().find(Key()))
      BoxRef; /**< This is the result of looking for an element in the
                       map elements, and al the info that will be stored about
                       an element in the board with a position */

  std::map<Key, Box> suitcase; /**< Every element stored, be it set on the board
                                 or not, identified by a unique ID */
  std::map<Position, BoxRef>
      table; /**< Contains the elements that are set to a specific position */
  Position limits; /**< Max value that can take every index. If 0, unlimited. */

protected:
  std::string name; /**< Name of the board */

  // Methods to interface the data structures

  /** True if position is inside limits */
  bool aux_checkLimits(const Position &position) {
    return insideSpace(position, this->limits);
  }

  /** If position contains an element, true and the element */
  std::tuple<bool, Key, std::shared_ptr<T>, std::list<Position>>
  aux_checkOccupied(const Position &position) {
    std::tuple<bool, Key, std::shared_ptr<T>, std::list<Position>> result;

    auto iter = this->table.find(position);
    if (iter == this->table.end()) {
      std::get<0>(result) = false;
    } else {
      BoxRef info = iter->second;
      result = std::make_tuple(true, info->first, info->second.element,
                               info->second.positions);
    }

    return result;
  }

  /** If id refers to an element, true and the element */
  std::tuple<bool, Key, std::shared_ptr<T>, std::list<Position>>
  aux_checkIDExists(const Key &id) {
    std::tuple<bool, Key, std::shared_ptr<T>, std::list<Position>> result;

    BoxRef info = this->suitcase.find(id);
    if (info == this->suitcase.end()) {
      std::get<0>(result) = false;
    } else {
      result = std::make_tuple(true, info->first, info->second.element,
                               info->second.positions);
    }

    return result;
  }

  /** Adds or overwrites a copy of an element with the id
   *  Derived must be a descendant from type T.
   */
  template <typename Derived>
  void aux_addElem(const Key &id, const Derived &element) {
    // Check that the type can be stored; otherwise compile error
    static_assert(std::is_same<T, Derived>::value ||
                      std::is_base_of<T, Derived>::value,
                  "Derived must inherit from T or be the same");
    Box new_box;
    std::shared_ptr<T> new_elem = std::make_shared<Derived>(element);
    new_box.element = new_elem;
    auto result = this->suitcase.insert(std::make_pair(id, new_box));
    if (!result.second) { // There was no insertion due to a previous element
      result.first->second.element = new_elem; // Update element
    }
  }

  /** Sets the element with id (if there is) on the position of the board (may
   * be out of the limits) unless it's occupied. Undefined behaviour if there is
   * not an element with id */
  void aux_put(const Key &id, const Position &position) {
    BoxRef info = this->suitcase.find(id);
    if (info != this->suitcase.end()) {
      info->second.positions.push_back(position);
      this->table.insert(std::make_pair(position, info));
    }
  }

  /** The element that used to be in position (if there was) is no more there */
  void aux_unput(const Position &position) {
    auto iter = this->table.find(position);
    if (iter != this->table.end()) {
      this->table.erase(iter);
      iter->second->second.positions.remove(position);
    }
  }

public:
  // Constructor and destructor
  Board(const std::string &name = "Unnamed board", const Position &limits = {0})
      : limits(limits), name(name){};
  virtual ~Board(){};

  // Methods
  std::string getName() { return this->name; }

  Position getLimits() { return this->limits; }

  /** Adds a copy of element to the element pool of the Board
   *  Derived must be a descendant from type T.
   *  Throws IDInUse if the id is already assigned.
   */
  template <typename Derived>
  void addElement(const Key &id, const Derived &element) {
    if (std::get<0>(aux_checkIDExists(id)))
      throw IDInUse<Key, Position>(this->name, id);

    aux_addElem(id, element);
  }

  /** Updates the item identified by id with a copy of element
   *  Derived must be a descendant from type T.
   *  Throws IDNonExistent if there is no element with that id.
   */
  template <typename Derived>
  void updateElement(const Key &id, const Derived &element) {
    if (!std::get<0>(aux_checkIDExists(id)))
      throw IDNonExistent<Key, Position>(this->name, id);

    aux_addElem(id, element);
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
    auto info = aux_checkIDExists(id);
    if (!std::get<0>(info))
      throw IDNonExistent<Key, Position>(this->name, id);

    if (!(multiple_positions || std::get<3>(info).empty()))
      throw IDMonoSet<Key, Position>(this->name, id);

    if (!aux_checkLimits(position))
      throw PositionOutLimits<Key, Position>(this->name, position);

    info = aux_checkOccupied(position);
    if (std::get<0>(info))
      throw PositionOccupied<Key, Position>(this->name, position);

    aux_put(id, position);
  }

  /** Unsets an element from the Board's surface
   *  Throws IDNonExistent if the id is not in the pool.
   *  Throws IDNotOnBoard if the element is not set.
   *  Throws IDMultiSet if unset_all is false and the element is more than once.
   */
  void unSetElement(const Key &id, bool unset_all = false) {
    auto info = aux_checkIDExists(id); // Position 3 of this tuple contains a
                                       // list of the positions the element is
                                       // on
    if (!std::get<0>(info))
      throw IDNonExistent<Key, Position>(this->name, id);

    if (std::get<3>(info).empty()) {
      throw IDNotOnBoard<Key, Position>(this->name, id);
    } else if (!unset_all && std::get<3>(info).size() > 1) {
      throw IDMultiSet<Key, Position>(this->name, id);
    }

    for (auto i : std::get<3>(info))
      aux_unput(i);
  }

  /** Unsets an element from the Board's surface
   *  Throws PositionOutLimits if the position is not inside the limits set at
   * construction.
   *  Throws PositionEmpty if there is no element.
   */
  void unSetElement(const Position &position) {
    if (!aux_checkLimits(position))
      throw PositionOutLimits<Key, Position>(this->name, position);

    auto info = aux_checkOccupied(position);
    if (!std::get<0>(info))
      throw PositionEmpty<Key, Position>(this->name, position);

    aux_unput(position);
  }

  /** If there is an element with id, return true and the element
   *  Throws IDNonExistent if the id is not in the pool.
   */
  std::tuple<bool, Key, std::shared_ptr<T>, std::list<Position>>
  getElement(const Key &id) {
    auto info = aux_checkIDExists(id);
    if (!std::get<0>(info))
      throw IDNonExistent<Key, Position>(this->name, id);

    return info;
  }

  /** If there is an element at position, return true and the element
   *  Throws PositionOutLimits if the position is not inside the limits set at
   * construction.
   */
  std::tuple<bool, Key, std::shared_ptr<T>, std::list<Position>>
  getElement(const Position &position) {
    if (!aux_checkLimits(position))
      throw PositionOutLimits<Key, Position>(this->name, position);

    return aux_checkOccupied(position);
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
    if (!aux_checkLimits(origin))
      throw PositionOutLimits<Key, Position>(this->name, origin);

    if (!aux_checkLimits(destiny))
      throw PositionOutLimits<Key, Position>(this->name, destiny);

    auto info = aux_checkOccupied(origin);
    if (!std::get<0>(info))
      throw PositionEmpty<Key, Position>(this->name, origin);

    if (std::get<0>(aux_checkOccupied(destiny))) {
      if (!override_previous)
        throw PositionOccupied<Key, Position>(this->name, destiny);
      else
        aux_unput(destiny);
    }

    aux_unput(origin);
    aux_put(std::get<0>(info), destiny);
  }

  /** Changes the position of an element
   *  Throws IDNonExistent if the id is not in the pool.
   *  Throws IDNotOnBoard if the element is not set.
   *  Throws IDMultiSet if the element is set more than once.
   *  Throws PositionOutLimits if one of the positions is not inside the limits
   * set at construction.
   *  Throws PositionOccupied if there is an element at destiny and
   * override_previous is false.
   */
  void moveElement(const Key &id, const Position &destiny,
                   bool override_previous = false) {
    auto info = aux_checkIDExists(id);
    if (!std::get<0>(info))
      throw IDNonExistent<Key, Position>(this->name, id);

    if (std::get<3>(info).empty()) {
      throw IDNotOnBoard<Key, Position>(this->name, id);
    } else if (std::get<3>(info).size() > 1) {
      throw IDMultiSet<Key, Position>(this->name, id);
    }

    if (!aux_checkLimits(destiny))
      throw PositionOutLimits<Key, Position>(this->name, destiny);

    if (std::get<0>(aux_checkOccupied(destiny))) {
      if (!override_previous)
        throw PositionOccupied<Key, Position>(this->name, destiny);
      else
        aux_unput(destiny);
    }

    aux_unput(std::get<3>(info).front());
    aux_put(std::get<1>(info), destiny);
  }
};

#endif /* end of include guard: BOARD_H */
