# UltraControlSystem

UltraControlSystem（UCS）是一个面向 Unreal Engine 的相机、输入、消息分发和 UMG 页面控制插件。插件通过统一的 `FUCS_LogicStruct` 消息，把相机聚焦、Actor/蓝图逻辑、场景 Widget 显示和主菜单页面切换连接起来，并附带可选的异步加载界面。

## 功能概览

- `AUCS_PawnBase`：提供基于 Enhanced Input 的平移、旋转和缩放；支持移动/旋转/缩放开关、Pitch/Yaw 限制，以及 Lerp/Flash 两种相机切换方式。
- `AUCS_Camera`：场景中的可聚焦相机目标。相机通过 `Info.LogicMap["Key"]` 或 Actor Tag 标识，支持目标位置、弹簧臂长度、旋转限制和运动能力配置。
- `AUCS_PlayerController`：自动加载插件内置输入映射，绑定移动、旋转、缩放动作，并按项目设置创建主菜单 Widget。
- `UUCS_GameInstanceSubsystem`：全局消息入口。`CallMessanger` 会依次尝试聚焦相机、广播 `LogicInterface`、显示场景 Widget，并驱动主菜单页面。
- UMG 基类：`UUCS_MenuBase`、`UUCS_WidgetBase`、`UUCS_ButtonBase` 和 `UUCS_WidgetSwitcher`，用于页面动画、按钮消息和按 Key 切换子页面。
- `AUCS_Widget`：屏幕空间 Widget Actor，默认隐藏，可按消息中的 Key/Tag 显示，并支持屏幕空间缩放与 Pivot 调整。
- `LoadingScreen` 模块：通过过渡地图和异步加载目标地图，显示可由蓝图实现的加载进度 UI。

## 模块与依赖

插件包含三个模块：

| 模块 | 类型 | 作用 |
| --- | --- | --- |
| `UltraControlSystem` | Runtime | 相机、Pawn、Controller、消息、UMG 和蓝图函数库 |
| `LoadingScreen` | Runtime | 过渡地图与异步加载界面 |
| `UltraControlSystemEditor` | Editor | Project Settings 中的 UCS 配置和 Actor Details 定制 |

插件描述文件声明并启用以下依赖插件：`EnhancedInput`、`JsonBlueprintUtilities`。

## 安装与基础配置

1. 将 `UltraControlSystem` 放入项目的 `Plugins` 目录，启用插件并重新生成项目文件。
2. 在项目中使用 `AUCS_GameModeBase`，并将 Pawn/Controller 分别设置为 `AUCS_PawnBase` 与 `AUCS_PlayerController`（也可以从它们派生蓝图）。
3. 打开 **Project Settings > Plugins > UCS**，设置 `Widget Path` 为 `UUCS_MenuBase` 子类的 Widget Blueprint 资产路径。默认值指向插件内容中的 `WBP_UCS_MenuBase`。
4. `AUCS_PlayerController` 使用插件内置资产：
   - `/UltraControlSystem/GameMode/Input/UCS_IMC_PawnInputBase`
   - `/UltraControlSystem/GameMode/Input/IA_Move`
   - `/UltraControlSystem/GameMode/Input/IA_Rotation`
   - `/UltraControlSystem/GameMode/Input/IA_Zoom`
5. 在项目 `Content/Settings/GlobalSettings.json` 创建全局移动/旋转/缩放速度配置（该文件应在运行前存在）：

   ```json
   {
     "GlobalMovementSpeed": 500.0,
     "GlobalRotationSpeed": 1.0,
     "GlobalZoomSpeed": 1.0
   }
   ```

   运行时可通过 `UUCS_FunctionLibrary::ReadGlobalSettingsJson` 检查解析结果。文件不存在或字段缺失会输出错误日志，且全局设置无法初始化。

## 输入与 Pawn

`AUCS_PlayerController` 在 `BeginPlay` 中添加默认 Mapping Context，并将三个动作绑定到 Pawn 的 `IUCS_Interface` 实现：

| 输入动作 | 接口 | 说明 |
| --- | --- | --- |
| `IA_Move` | `MoveInterface(FVector2D)` | 按控制器 Yaw 方向移动 |
| `IA_Rotation` | `RotationInterface(FVector2D)` | 修改控制器 Pitch/Yaw，并应用 Clamp |
| `IA_Zoom` | `ZoomInterface(float)` | 修改 Spring Arm 长度 |

可在派生 Pawn 中重写接口，或直接调整 `bCanMove`、`bCanRotation`、`bCanZoom`、`ClampPitch`、`ClampYaw` 等属性。

## 消息分发

`FUCS_LogicStruct` 包含两个 `TMap<FString, FString>`：

- `InfoMap`：显示名称、Widget 尺寸等附加信息。
- `LogicMap`：控制逻辑，`Key` 是 UCS 内置查找约定。

蓝图中可通过 `Get_UCS_GameInstanceSubsystem` 获取子系统，并调用：

```text
CallMessanger(Logic)
```

消息处理规则如下：

1. 在已注册的 `AUCS_Camera` 中查找 Tag 等于 `LogicMap["Key"]` 的相机，并让当前 Pawn 聚焦该相机。
2. 查找场景中实现 `IUCS_Interface` 的 Actor，调用其 `LogicInterface`。
3. 在已注册的 `AUCS_Widget` 中查找同名 Tag，并调用 `ShowSelf`。
4. 将消息转发给 PlayerController 创建的 `UUCS_MenuBase`，由菜单的 `ShowPage` 播放页面动画。

因此，同一个 Key 可以同时驱动相机、场景 Widget、Actor 逻辑和菜单页面；不需要参与某一环节时，可以只配置对应对象。

## 相机与场景 Widget

### `AUCS_Camera`

- 将 `Info.LogicMap["Key"]` 设置为唯一标识；BeginPlay 时会把该 Key 加入 Actor Tags。
- 在 Details 面板中编辑 `TargetLocation`，插件会自动换算 Spring Arm 的旋转和长度。
- `CameraType = Lerp` 使用插值过渡；`CameraType = Flash` 使用黑屏淡入淡出过渡。
- `SetCameraActive` 和 `TEST` 提供编辑器/调试辅助操作。

### `AUCS_Widget`

- 设置 `WidgetClass` 为 `UUCS_WidgetBase` 子类，并可通过 `Groups` 添加多个查找 Tag。
- Widget 默认在 BeginPlay 时隐藏；收到匹配消息后由 `ShowSelf` 显示。
- `SetScreenSpaceWidgetScaleAndPivot` 可同时设置组件 Pivot 和内部 UMG 缩放。

## UMG 页面控件

- `UUCS_MenuBase`：设置 `Logic.LogicMap["Key"]`，实现或绑定名为 `PageAnimation` 的动画；收到消息后会从头播放该动画，并兼容旧的 `StartPageAnimation` 事件。
- `UUCS_ButtonBase`：设置按钮的 `Logic`，按下时自动调用 `CallMessanger` 和主菜单消息转发。
- `UUCS_WidgetSwitcher`：使用 `SetActiveWidgetByKey` 或 `SetActiveWidgetByLogic`，按子菜单 `LogicMap["Key"]` 精确匹配并切换页面。
- `UUCS_WidgetBase`：通用 Widget 基类，提供 `Info`、`InitWidget` 和 `bCanChili`（保留现有 API 名称）扩展点。

## 加载界面

`ULoadingScreenSubsystem` 是 Game Instance Subsystem。蓝图节点 `OpenLevelWithLoadScreen`（对应 C++ 的 `K2_OpenLevelWithLoadScreen`）需要传入：

- 目标地图 `Map`；
- 空白或轻量过渡地图 `TransitionMap`；
- `ULoadScreenUserWidget` 子类 `LoadScreenUserWidgetClass`。

加载界面通过 `GetLoadPercent` 获取异步加载百分比，并在 `ULoadScreenUserWidget` 中实现 `OnReceiveLoadPercent`（蓝图事件名：`OnReceiveLoadPercent`）更新进度显示。加载完成后插件自动打开目标地图并移除加载 Widget。

## 蓝图函数库

`UUCS_FunctionLibrary` 提供以下常用入口：

- `Get_UCS_GameInstanceSubsystem`：获取 UCS 全局子系统。
- `Get_UCS_GlobalSettings`：获取已解析的全局速度配置。
- `ReadGlobalSettingsJson`：读取并解析 `Content/Settings/GlobalSettings.json`。
- `Load_UCS_UMG_ByConfig`：按 Project Settings 中的 `WidgetPath` 创建主菜单。
- `PrintUCSLog`：输出对象、消息类型和 `FUCS_LogicStruct` 内容。

## 目录结构

```text
Source/
  UltraControlSystem/        # Runtime 核心模块
  LoadingScreen/              # Runtime 加载界面模块
  UltraControlSystemEditor/   # Editor 配置与 Details 定制
Content/
  GameMode/Input/             # 内置 Enhanced Input 资产
  Menu/                       # 主菜单 Widget 示例
  Widget/                     # Widget Actor/样式示例
Config/
  DefaultUltraControlSystem.ini
```

## 注意事项

- 相机和场景 Widget 的 Key 查找是大小写敏感的；`LogicMap` 中必须包含 `Key`。
- `AUCS_PawnBase` 需要由 `AUCS_PlayerController` 控制，且其 BeginPlay 会读取 Game Instance Subsystem 和全局设置。
- 自定义主菜单必须继承 `UUCS_MenuBase`，否则 `WidgetPath` 加载会失败并输出日志。
- 当前仓库未包含 `Content/Settings/GlobalSettings.json`，使用默认速度配置前请在项目 Content 目录创建该文件。
