#ifndef BOARDEXCEPTION_H
#define BOARDEXCEPTION_H

#include <exception> // std::exception
#include <string>    // std::string

template <typename Key, typename Position>
class BoardException : public std::exception {
private:
  std::string board;

public:
  BoardException(std::string board) : board(board) {}
  virtual ~BoardException() {}
  std::string getBoardName() const noexcept { return board; }

  const char *what() const noexcept {
    return "An error ocurred when using the Board";
  }
};

template <typename Key, typename Position>
class IDException : public BoardException<Key, Position> {
private:
  Key id;

public:
  IDException(std::string board, Key id)
      : BoardException<Key, Position>(board), id(id) {}
  virtual ~IDException() {}

  Key getID() const noexcept { return id; }
  const char *what() const noexcept { return "Problem with ID"; }
};

template <typename Key, typename Position>
class IDNonExistent : public IDException<Key, Position> {
public:
  IDNonExistent(std::string board, Key id)
      : IDException<Key, Position>(board, id) {}
  virtual ~IDNonExistent() {}

  const char *what() const noexcept {
    return "Provided ID doesn't match any element";
  }
};

template <typename Key, typename Position>
class IDInUse : public IDException<Key, Position> {
public:
  IDInUse(std::string board, Key id) : IDException<Key, Position>(board, id) {}
  virtual ~IDInUse() {}

  const char *what() const noexcept {
    return "Provided ID is already being used by another element";
  }
};

template <typename Key, typename Position>
class IDMultiSet : public IDException<Key, Position> {
public:
  IDMultiSet(std::string board, Key id)
      : IDException<Key, Position>(board, id) {}
  virtual ~IDMultiSet() {}

  const char *what() const noexcept {
    return "Action could not be resolved because the element is set in "
           "different positions of the Board";
  }
};

template <typename Key, typename Position>
class IDMonoSet : public IDException<Key, Position> {
public:
  IDMonoSet(std::string board, Key id)
      : IDException<Key, Position>(board, id) {}
  virtual ~IDMonoSet() {}

  const char *what() const noexcept {
    return "The element is already set and can't be set in various positions "
           "at the same time";
  }
};

template <typename Key, typename Position>
class IDOnBoard : public IDException<Key, Position> {
public:
  IDOnBoard(std::string board, Key id)
      : IDException<Key, Position>(board, id) {}
  virtual ~IDOnBoard() {}

  const char *what() const noexcept {
    return "The provided element is already set on the board";
  }
};

template <typename Key, typename Position>
class IDNotOnBoard : public IDException<Key, Position> {
public:
  IDNotOnBoard(std::string board, Key id)
      : IDException<Key, Position>(board, id) {}
  virtual ~IDNotOnBoard() {}

  const char *what() const noexcept {
    return "The provided element, while existing, is not set on the board";
  }
};

template <typename Key, typename Position>
class PositionException : public BoardException<Key, Position> {
private:
  Position position;

public:
  PositionException(std::string board, Position position)
      : BoardException<Key, Position>(board), position(position) {}
  virtual ~PositionException() {}

  Position getPosition() { return position; }
  const char *what() const noexcept { return "Problem with position"; }
};

template <typename Key, typename Position>
class PositionOutLimits : public PositionException<Key, Position> {
public:
  PositionOutLimits(std::string board, Position position)
      : PositionException<Key, Position>(board, position) {}
  virtual ~PositionOutLimits() {}

  const char *what() const noexcept {
    return "Provided position is out of the limits of the Board";
  }
};

template <typename Key, typename Position>
class PositionEmpty : public PositionException<Key, Position> {
public:
  PositionEmpty(std::string board, Position position)
      : PositionException<Key, Position>(board, position) {}
  virtual ~PositionEmpty() {}

  const char *what() const noexcept {
    return "Provided position doesn't contain any element";
  }
};

template <typename Key, typename Position>
class PositionOccupied : public PositionException<Key, Position> {
public:
  PositionOccupied(std::string board, Position position)
      : PositionException<Key, Position>(board, position) {}
  virtual ~PositionOccupied() {}

  const char *what() const noexcept {
    return "Provided position is already occupied by another element";
  }
};

#endif /* end of include guard: BOARDEXCEPTION_H */
