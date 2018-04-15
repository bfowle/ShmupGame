#ifndef BARRAGEMANAGER_H
#define BARRAGEMANAGER_H

#include "CoreMinimal.h"

#include <string>
#include <vector>

class BulletMLParserTinyXML;

class BarrageManager {
public:
    void loadBulletMLFiles();
    void unloadBulletMLFiles();

public:
    enum {
        MORPH, SMALL, SMALL_MOVE, SMALL_SIDE_MOVE,
        MEDIUM, MEDIUM_SUB, MEDIUM_MOVE, MEDIUM_BACK_MOVE,
        LARGE, LARGE_MOVE,
        MORPH_LOCK, SMALL_LOCK, MEDIUM_SUB_LOCK,
    };

    enum {
        BARRAGE_TYPE = 13,
        BARRAGE_MAX = 64
    };

    std::vector<BulletMLParserTinyXML *> m_parser;
};

#endif