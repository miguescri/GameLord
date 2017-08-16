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

  /** Throws PositionOutLimits if limits is out */
  void aux_checkLimits(const Position &position) {
    if (!insideSpace(position, this->limits)) {
      throw PositionOutLimits(*this, position);
    }
  }

  /** Throws PositionOccupied if needed */
  void aux_checkEmpty(const Position &position) {
    if (this->table.find(position) != this->table.end()) {
      throw PositionOccupied(*this, position);
    }
  }

  /** Throws PositionEmpty if needed and returns the element that occupies the
   * position */
  std::tuple<Key, std::shared_ptr<T>, std::list<Position>>
  aux_checkNotEmpty(const Position &position) {
    auto iter = this->table.find(position);
    if (iter == this->table.end()) {
      throw PositionEmpty(*this, position);
    }
    BoxRef info = iter->second;
    return std::make_tuple(info->first, info->second.element,
                           info->second.positions);
  }

  /** Throws IDNonExistent if needed and returns the element that has the ID*/
  std::tuple<Key, std::shared_ptr<T>, std::list<Position>>
  aux_checkIDExists(const Key &id) {
    BoxRef info = this->suitcase.find(id);
    if (info == this->suitcase.end()) {
      throw IDNonExistent(*this, id);
    }
    return std::make_tuple(info->first, info->second.element,
                           info->second.positions);
  }

  /** Throws IDInUse if needed */
  void aux_checkIDNotExists(const Key &id) {
    if (this->suitcase.find(id) != this->suitcase.end()) {
      throw IDInUse(*this, id);
    }
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
    aux_checkIDNotExists(id);
    aux_addElem(id, element);
  }

  /** Updates the item identified by id with a copy of element
   *  Derived must be a descendant from type T.
   *  Throws IDNonExistent if there is no element with that id.
   */
  template <typename Derived>
  void updateElement(const Key &id, const Derived &element) {
    aux_checkIDExists(id);
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
    if (!(multiple_positions || std::get<2>(info).empty())) {
      throw IDMonoSet(*this, id);
    }
    aux_checkLimits(position);
    aux_checkEmpty(position);
    aux_put(id, position);
  }

  /** Unsets an element from the Board's surface
   *  Throws IDNonExistent if the id is not in the pool.
   *  Throws IDNotOnBoard if the element is not set.
   *  Throws IDMultiSet if unset_all is false and the element is more than once.
   */
  void unSetElement(const Key &id, bool unset_all = false) {
    auto info = aux_checkIDExists(id); // Position 2 of this tuple contains a
                                       // list of the positions the element is
                                       // on
    if (std::get<2>(info).empty()) {
      throw IDNotOnBoard(*this, id);
    } else if (!unset_all && std::get<2>(info).size() > 1) {
      throw IDMultiSet(*this, id);
    }
    for (auto i : std::get<2>(info))
      aux_unput(i);
  }

  /** Unsets an element from the Board's surface
   *  Throws PositionOutLimits if the position is not inside the limits set at
   * construction.
   *  Throws PositionEmpty if there is no element.
   */
  void unSetElement(const Position &position) {
    aux_checkLimits(position);
    aux_checkNotEmpty(position);
    aux_unput(position);
  }

  /** Returns the id, a pointer to a copy and th positions in which is the
   * element searched
   *  Throws IDNonExistent if the id is not in the pool.
   */
  std::tuple<Key, std::shared_ptr<T>, std::list<Position>>
  getElement(const Key &id) {
    return aux_checkIDExists(id);
  }

  /** Returns the id, a pointer to a copy and th positions in which is the
   * element searched
   *  Throws PositionOutLimits if the position is not inside the limits set at
   * construction.
   *  Throws PositionEmpty if there is no element.
   */
  std::tuple<Key, std::shared_ptr<T>, std::list<Position>>
  getElement(const Position &position) {
    aux_checkLimits(position);
    return aux_checkNotEmpty(position);
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
    aux_checkLimits(origin);
    aux_checkLimits(destiny);
    auto info = aux_checkNotEmpty(origin);
    try {
      aux_checkEmpty(destiny);
    } catch (PositionOccupied &e) {
      if (!override_previous)
        throw;
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
    if (std::get<2>(info).empty()) {
      throw IDNotOnBoard(*this, id);
    } else if (std::get<2>(info).size() > 1) {
      throw IDMultiSet(*this, id);
    }
    aux_checkLimits(destiny);
    try {
      aux_checkEmpty(destiny);
    } catch (PositionOccupied &e) {
      if (!override_previous)
        throw;
      aux_unput(destiny);
    }
    aux_unput(std::get<2>(info).front());
    aux_put(std::get<0>(info), destiny);
  }

  // EXCEPTIONS ////////////////////////////////////////////////////////////////

  class BoardException : public std::exception {
  private:
    Board &board;

  public:
    BoardException(Board &board) : board(board) {}
    virtual ~BoardException() {}
    Board &getBoard() const noexcept { return board; }

    const char *what() const noexcept {
      return "An error ocurred when using the Board";
    }
  };

  class IDException : public BoardException {
  private:
    Key id;

  public:
    IDException(Board &board, Key id) : BoardException(board), id(id) {}
    virtual ~IDException() {}

    Key getID() const noexcept { return id; }
    const char *what() const noexcept { return "Problem with ID"; }
  };

  class IDNonExistent : public IDException {
  public:
    IDNonExistent(Board &board, Key id) : IDException(board, id) {}
    virtual ~IDNonExistent() {}

    const char *what() const noexcept {
      return "Provided ID doesn't match any element";
    }
  };

  class IDInUse : public IDException {
  public:
    IDInUse(Board &board, Key id) : IDException(board, id) {}
    virtual ~IDInUse() {}

    const char *what() const noexcept {
      return "Provided ID is already being used by another element";
    }
  };

  class IDMultiSet : public IDException {
  public:
    IDMultiSet(Board &board, Key id) : IDException(board, id) {}
    virtual ~IDMultiSet() {}

    const char *what() const noexcept {
      return "Action could not be resolved because the element is set in "
             "different positions of the Board";
    }
  };

  class IDMonoSet : public IDException {
  public:
    IDMonoSet(Board &board, Key id) : IDException(board, id) {}
    virtual ~IDMonoSet() {}

    const char *what() const noexcept {
      return "The element is already set and can't be set in various positions "
             "at the same time";
    }
  };

  class IDOnBoard : public IDException {
  public:
    IDOnBoard(Board &board, Key id) : IDException(board, id) {}
    virtual ~IDOnBoard() {}

    const char *what() const noexcept {
      return "The provided element is already set on the board";
    }
  };

  class IDNotOnBoard : public IDException {
  public:
    IDNotOnBoard(Board &board, Key id) : IDException(board, id) {}
    virtual ~IDNotOnBoard() {}

    const char *what() const noexcept {
      return "The provided element, while existing, is not set on the board";
    }
  };

  class PositionException : public BoardException {
  private:
    Position position;

  public:
    PositionException(Board &board, Position position)
        : BoardException(board), position(position) {}
    virtual ~PositionException() {}

    Position getPosition() { return position; }
    const char *what() const noexcept { return "Problem with position"; }
  };

  class PositionOutLimits : public PositionException {
  public:
    PositionOutLimits(Board &board, Position position)
        : PositionException(board, position) {}
    virtual ~PositionOutLimits() {}

    const char *what() const noexcept {
      return "Provided position is out of the limits of the Board";
    }
  };

  class PositionEmpty : public PositionException {
  public:
    PositionEmpty(Board &board, Position position)
        : PositionException(board, position) {}
    virtual ~PositionEmpty() {}

    const char *what() const noexcept {
      return "Provided position doesn't contain any element";
    }
  };

  class PositionOccupied : public PositionException {
  public:
    PositionOccupied(Board &board, Position position)
        : PositionException(board, position) {}
    virtual ~PositionOccupied() {}

    const char *what() const noexcept {
      return "Provided position is already occupied by another element";
    }
  };
};

#endif /* end of include guard: BOARD_H */
