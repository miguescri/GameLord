#include <iostream>
#include <string>

#include "../core/board.h"

int main(int argc, char const *argv[]) {
  Board<int, std::string, 2> b("my board",
                               Board<int, std::string, 2>::Position{{4, 4}});
  int key = 1;
  std::string initial = "Hello";
  std::string updated = "Bye";

  std::cout << "TESTING NORMAL BEHAVIOUR";

  std::cout << '\n' << "- Add element: ";
  try {
    b.addElement(key, initial);
    b.addElement(key + 1, initial);
    std::cout << "\t\t\tPASSED";
  } catch (decltype(b)::BoardException &e) {
    std::cout << "\t\t\tNOT PASSED" << '\n' << e.what();
  }

  std::cout << '\n' << "- Update element: ";
  try {
    b.updateElement(key, updated);
    std::cout << "\t\tPASSED";
  } catch (decltype(b)::BoardException &e) {
    std::cout << "\t\tNOT PASSED" << '\n' << e.what();
  }

  std::cout << '\n' << "- Set element: ";
  try {
    b.setElement(key, decltype(b)::Position{{3, 2}});
    std::cout << "\t\t\tPASSED";
  } catch (decltype(b)::BoardException &e) {
    std::cout << "\t\t\tNOT PASSED" << '\n' << e.what();
  }

  std::cout << '\n' << "- Unset element by id: ";
  try {
    b.unSetElement(key);
    std::cout << "\t\tPASSED";
  } catch (decltype(b)::BoardException &e) {
    std::cout << "\t\tNOT PASSED" << '\n' << e.what();
  }

  std::cout << '\n' << "- Unset element by position: ";
  try {
    b.setElement(key, decltype(b)::Position{{3, 2}});
    b.unSetElement(decltype(b)::Position{{3, 2}});
    std::cout << "\tPASSED";
  } catch (decltype(b)::BoardException &e) {
    std::cout << "\tNOT PASSED" << '\n' << e.what();
  }

  std::cout << '\n' << "- Multiset element: ";
  try {
    b.setElement(key, decltype(b)::Position{{3, 2}});
    b.setElement(key, decltype(b)::Position{{1, 2}}, true);
    std::cout << "\t\tPASSED";
  } catch (decltype(b)::BoardException &e) {
    std::cout << "\t\tNOT PASSED" << '\n' << e.what();
  }

  std::cout << '\n' << "- Multiunset all by id: ";
  try {
    b.unSetElement(key, true);
    std::cout << "\tPASSED";
  } catch (decltype(b)::BoardException &e) {
    std::cout << "\tNOT PASSED" << '\n' << e.what();
  }

  std::cout << '\n' << "- Multiunset one by position: ";
  try {
    b.setElement(key, decltype(b)::Position{{3, 2}});
    b.setElement(key, decltype(b)::Position{{1, 2}}, true);
    b.unSetElement(decltype(b)::Position{{1, 2}});
    std::cout << "\tPASSED";
  } catch (decltype(b)::BoardException &e) {
    std::cout << "\tNOT PASSED" << '\n' << e.what();
  }

  std::cout << '\n' << "- Move element by id: ";
  try {
    b.moveElement(key, decltype(b)::Position{{1, 1}});
    std::cout << "\t\tPASSED";
  } catch (decltype(b)::BoardException &e) {
    std::cout << "\t\tNOT PASSED" << '\n' << e.what();
  }

  std::cout << '\n' << "- Move element by position: ";
  try {
    b.moveElement(decltype(b)::Position{{1, 1}}, decltype(b)::Position{{3, 2}});
    std::cout << "\tPASSED";
  } catch (decltype(b)::BoardException &e) {
    std::cout << "\tNOT PASSED" << '\n' << e.what();
  }

  std::cout << '\n' << "- Move overriding by id: ";
  try {
    b.setElement(key + 1, decltype(b)::Position{{1, 1}});
    b.moveElement(key, decltype(b)::Position{{1, 1}}, true);
    std::cout << "\tPASSED";
  } catch (decltype(b)::BoardException &e) {
    std::cout << "\tNOT PASSED" << '\n' << e.what();
  }

  std::cout << '\n' << "- Move overriding by position: ";
  try {
    b.setElement(key + 1, decltype(b)::Position{{3, 2}});
    b.moveElement(decltype(b)::Position{{1, 1}}, decltype(b)::Position{{3, 2}},
                  true);
    std::cout << "\tPASSED";
  } catch (decltype(b)::BoardException &e) {
    std::cout << "\tNOT PASSED" << '\n' << e.what();
  }

  std::cout << "\n\nTESTING READING";

  try {
    std::cout << '\n' << "- Get from id: ";
    auto res = b.getElement(key);
    std::cout << "\t\t\tPASSED" << '\n';

    std::cout << "Key: " << std::get<0>(res) << '\n'
              << "String: " << *std::get<1>(res) << '\n'
              << "Positions: ";
    if (std::get<2>(res).empty()) {
      std::cout << "Not set" << '\n';
    } else {
      for (auto i : std::get<2>(res)) {
        std::cout << "\n\t";
        std::cout << std::string("( ");
        for (auto j : i) {
          std::cout << j << " ";
        }
        std::cout << std::string(")");
      }
      std::cout << '\n';
    }

  } catch (decltype(b)::BoardException &e) {
    std::cout << "\t\t\tNOT PASSED" << '\n' << e.what();
  }

  try {
    std::cout << '\n' << "- Get from position: ";
    auto res = b.getElement(decltype(b)::Position{{3, 2}});
    std::cout << "\t\tPASSED" << '\n';

    std::cout << "Key: " << std::get<0>(res) << '\n'
              << "String: " << *std::get<1>(res) << '\n'
              << "Positions: ";
    if (std::get<2>(res).empty()) {
      std::cout << "Not set" << '\n';
    } else {
      for (auto i : std::get<2>(res)) {
        std::cout << "\n\t";
        std::cout << std::string("( ");
        for (auto j : i) {
          std::cout << j << " ";
        }
        std::cout << std::string(")");
      }
      std::cout << '\n';
    }

  } catch (decltype(b)::BoardException &e) {
    std::cout << "\t\tNOT PASSED" << '\n' << e.what();
  }

  return 0;
}
