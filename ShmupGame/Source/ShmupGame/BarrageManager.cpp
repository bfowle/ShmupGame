// Fill out your copyright notice in the Description page of Project Settings.

#include "BarrageManager.h"

extern "C" {
#include <sys/types.h>
//#include "genmcr.h"
#include "brgmng_mtd.h"
}

Barrage barragePattern[BARRAGE_TYPE_NUM][BARRAGE_PATTERN_MAX];
int barragePatternNum[BARRAGE_TYPE_NUM];

static const char *BARRAGE_DIR_NAME[BARRAGE_TYPE_NUM] = {
    "normal",
    //"reversible",
    //"morph",
    //"simple",
    //"morph_heavy",
    //"psy",
};

static int readBulletMLFiles(const char *dirPath, Barrage brg[]) {
    int i = 0;
    /*
    char fileName[256];
    if ((dp = opendir(dirPath)) == NULL) {
        fprintf(stderr, "Can't open directory: %s\n", dirPath);
        exit(1);
    }
    while ((dir = readdir(dp)) != NULL) {
        // read .xml files.
        if (strcmp(strrchr(dir->d_name, '.'), ".xml") != 0) {
            continue;
        }
        strcpy(fileName, dirPath);
        strcat(fileName, "/");
        strncat(fileName, dir->d_name, sizeof(fileName) - strlen(fileName) - 1);
        brg[i].m_bulletml = new BulletMLParserTinyXML(fileName);
        brg[i].m_bulletml->build(); i++;
        printf("%s\n", fileName);
    }
    closedir(dp);
    */
    return i;
}

void initBarrageManager() {
    for (size_t i = 0; i < BARRAGE_TYPE_NUM; i++) {
        barragePatternNum[i] = readBulletMLFiles(BARRAGE_DIR_NAME[i], barragePattern[i]);
    }
}

void closeBarrageManager() {
    for (size_t i = 0; i < BARRAGE_TYPE_NUM; i++) {
        for (size_t j = 0; j < barragePatternNum[i]; j++) {
            delete barragePattern[i][j].m_bulletml;
        }
    }
}