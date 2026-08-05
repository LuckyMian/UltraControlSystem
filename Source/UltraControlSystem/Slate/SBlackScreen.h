#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Animation/CurveSequence.h"

class SBlackScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBlackScreen) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// 重写 Tick 函数来驱动动画
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	// 启动动画的函数
	void StartFadeAnimation();

private:
	// 动画序列
	FCurveSequence FadeSequence;
    
	// 动画曲线（用于获取具体时间点的插值）
	FCurveHandle FadeCurve;

	// 当前控件的颜色和透明度
	FLinearColor CurrentColor;
};