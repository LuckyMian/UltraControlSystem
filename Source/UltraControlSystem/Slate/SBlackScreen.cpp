#include "SBlackScreen.h"
#include "Widgets/Images/SImage.h"

void SBlackScreen::Construct(const FArguments& InArgs)
{
	CurrentColor = FLinearColor::Black;

	// 1. 设置动画曲线
	// 总时长 1.0s
	// AddCurve 第一个参数是起始时间（0.25s），第二个参数是持续时间（1.0s - 0.25s = 0.75s）
	// ECurveEaseFunction::Linear 表示线性渐变，也可以换成 QuadOut 等平滑曲线
	FadeCurve = FadeSequence.AddCurve(0.5f, 0.5f, ECurveEaseFunction::QuadOut);

	// 2. 构建 UI 结构：一个 SImage，其颜色绑定到我们类的 CurrentColor
	ChildSlot
	[
		SNew(SImage)
		.ColorAndOpacity_Lambda([this]() { return CurrentColor; })
	];

	// 3. 自动开始播放动画
	StartFadeAnimation();
}

void SBlackScreen::StartFadeAnimation()
{
	FadeSequence.Play(this->AsShared());
}

void SBlackScreen::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	// 如果动画正在播放
	if (FadeSequence.IsPlaying())
	{
		// FadeSequence.GetSequenceTime() 可以获取 0 到 1 秒的绝对时间
		float CurrentTime = FadeSequence.GetSequenceTime();

		if (CurrentTime < 0.25f)
		{
			// 0 - 0.25 秒：保持纯黑
			CurrentColor.A = 1.0f;
		}
		else
		{
			// 0.25 - 1.0 秒：从纯黑（1.0）渐变成纯透明（0.0）
			// FadeCurve.GetLerp() 会在 0.25s 到 1.0s 之间自动返回一个 0.0 到 1.0 的值
			float LerpValue = FadeCurve.GetLerp();
			CurrentColor.A = 1.0f - LerpValue; 
		}
	}
	else if (FadeSequence.IsAtEnd())
	{
		// 动画播放完毕，确保完全透明
		CurrentColor.A = 0.0f;
        
		// 提示：你可以在这里调用销毁逻辑，把这个 Widget 从父级或视口中移除
	}
}