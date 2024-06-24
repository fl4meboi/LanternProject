// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// 함수 호출 정보
#define CALLINFO (FString(__FUNCTION__) + (TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

// 정보 출력 매크로
#define LOGTEXT(fmt, ...) \
		(TEXT("[") + CALLINFO + TEXT("] ") + FString::Printf(fmt, ##__VA_ARGS__))
