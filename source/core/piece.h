#ifndef PIECE_H
#define PIECE_H

#include <string> // std::string

/** A basic boardgame piece
 *  This piece contains diverse information about the characteristics
 *  of a simple boardgame piece.
 */
class Piece {
private:
  int id;           /**< Unique identifier for a piece */
  std::string name; /**< Human-readable name for the piece. Can be repeated
                        among different pieces */
  int type;         /**< Identifier for the type of piece (e.g queen, knight) */
  int team;         /**< Identifier for the team the piece belongs to */

public:
  /** A constructor
   *  This is the only way to set the ID, since it must be unique
   */
  Piece(int id, std::string name, int type, int team)
      : id(id), name(name), type(type), team(team){};
  virtual ~Piece();
  void setName(std::string name) { this->name = name; }
  void setType(int type) { this->type = type; }
  void setTeam(int team) { this->team = team; }
  int getId() { return this->id; }
  std::string getName() { return this->name; }
  int getType() { return this->type; }
  int getTeam() { return this->team; }
};

#endif /* end of include guard: PIECE_H */
