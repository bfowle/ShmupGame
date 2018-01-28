#ifndef BARRAGEMANAGER_H
#define BARRAGEMANAGER_H

#include "CoreMinimal.h"

#include <string>
#include <array>

class BulletMLParserTinyXML;

class BarrageManager {
public:
    void loadBulletMLFiles();
    void unloadBulletMLFiles();

public:
    enum {
        LARGE,
        LARGE_MOVE,
        MEDIUM,
        MEDIUM_MOVE,
        MEDIUM_BACK_MOVE,
        MEDIUM_SUB,
        MEDIUM_SUB_LOCK,
        MORPH,
        MORPH_LOCK,
        SMALL,
        SMALL_MOVE,
        SMALL_SIDE_MOVE,
        SMALL_LOCK
    };

    enum {
        BARRAGE_TYPE = 13,
        BARRAGE_MAX = 64
    };

    std::array<std::array<BulletMLParserTinyXML *, BARRAGE_MAX>, BARRAGE_TYPE> m_parser;
    std::array<int, BARRAGE_TYPE> m_parserNum;

private:
    static std::array<const std::string, BARRAGE_TYPE> m_dirName;
};

#endif