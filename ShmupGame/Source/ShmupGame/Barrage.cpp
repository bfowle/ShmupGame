#include "Barrage.h"

Barrage::Barrage() :
    m_parser(nullptr) {
    for (int i = 0; i < m_morphParser.size(); ++i) {
        m_morphParser[i] = nullptr;
    }
}