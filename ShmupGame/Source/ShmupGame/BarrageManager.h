// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"
#include "bulletml/bulletmlrunner.h"

#define BARRAGE_TYPE_NUM 1 //6
#define BARRAGE_PATTERN_MAX 32

#define NORMAL_BARRAGE 0
//#define REVERSIBLE_BARRAGE 1
//#define MORPH_BARRAGE 2
//#define SIMPLE_BARRAGE 3
//#define MORPH_HEAVY_BARRAGE 4
//#define PSY_MORPH_BARRAGE 5

typedef struct {
    BulletMLParser *m_bulletml;
    double m_rank, m_maxRank;
} Barrage;

extern Barrage barragePattern[BARRAGE_TYPE_NUM][BARRAGE_PATTERN_MAX];
extern int barragePatternNum[BARRAGE_TYPE_NUM];

extern "C" {
#include "brgmng_mtd.h"
}