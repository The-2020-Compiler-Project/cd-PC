#include "Symbol.h"
#include "Symtab.h"

Var* Var::getVoid() {
    return SymTab::voidVar;
}