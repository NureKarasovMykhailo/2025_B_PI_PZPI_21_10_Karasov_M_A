// Fill out your copyright notice in the Description page of Project Settings.


#include "GOJGameModeBase.h"
#include "Player/GOJBaseCharacter.h"
#include "Player/GOJPlayerController.h"
#include "UI/GOJGameHUD.h"

AGOJGameModeBase::AGOJGameModeBase()
{
    DefaultPawnClass = AGOJBaseCharacter::StaticClass();
    PlayerControllerClass = AGOJPlayerController::StaticClass();
    HUDClass = AGOJGameHUD::StaticClass();
}