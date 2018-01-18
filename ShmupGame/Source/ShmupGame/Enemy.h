// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Bullet.h"

extern "C" {
#include "Math/Vector.h"
#include "enemy_mtd.h"
}

class SHMUPGAME_API Enemy : public Bullet {
public:
	Enemy();
	~Enemy();
};
