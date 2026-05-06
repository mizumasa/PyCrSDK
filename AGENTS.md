# PyCrSDK Zoom API Extension Spec (Camera Remote SDK)

## 1. Goal

PyCrSDK をズーム操作に拡張し、Python から以下を安全に扱えるようにする。

- 相対ズーム操作（方向+速度、開始/停止）
- 絶対ズーム位置指定（ターゲット位置へ移動）
- 実行中の絶対ズームのキャンセル
- 状態/能力取得（実行可否、速度レンジ、現在位置など）


## 2. SDK Doc Facts (HTML から確認した事実)

### 2.1 Relative zoom

- `CrDeviceProperty_Zoom_Operation`
	- DataType: `CrDataType_Int8Range`
	- 値: `-1` (Wide), `0` (Stop), `1` (Tele) または `CrDeviceProperty_Zoom_Speed_Range` のレンジに連動
	- 前提: `CrDeviceProperty_Zoom_Operation_Status == Enable`

- `CrDeviceProperty_ZoomOperationWithInt16`
	- DataType: `CrDataType_Int16Range`
	- 値: `-32768..32767`, `0` は停止
	- 例: `0x0001..0x7FFF` Tele, `0xFFFF..0x8000` Wide
	- 前提: `CrDeviceProperty_Zoom_Operation_Status == Enable`

- `CrDeviceProperty_Zoom_Speed_Range`
	- DataType: `CrDataType_Int8Range`
	- min/max/step が機種依存
	- 正値=Zoom in(Tele), 負値=Zoom out(Wide)
	- `CrDeviceProperty_Remocon_Zoom_Speed_Type == Variable` 時に有効


### 2.2 Absolute zoom

- `CrDeviceProperty_ZoomPositionSetting`
	- DataType: `CrDataType_UInt16Range`
	- 絶対ズーム目標値を設定（target）
	- min/max/step は可変

- `CrDeviceProperty_ZoomPositionCurrentValue`
	- DataType: `CrDataType_UInt16Range`
	- 絶対ズームの現在値を取得

- `CrControlCode_CancelZoomPosition` / `CrCommandId_CancelZoomPosition`
	- 絶対ズーム動作キャンセル
	- Down -> Up の順で送信

- `CrDeviceProperty_ZoomDrivingStatus`
	- 絶対ズームの駆動状態監視に利用可能


### 2.3 Other zoom info (補助情報)

- `CrDeviceProperty_ZoomDistance` (`UInt32Range`): 焦点距離(0.001mm単位)。
	- これは「絶対ズーム位置(U16)」とは別概念。
- `CrDeviceProperty_Zoom_Bar_Information` (`UInt32`): UI用バー情報（total box/current box/box内位置）。
- `CrDeviceProperty_Zoom_Scale` / `CrDeviceProperty_DigitalZoomScale`: 倍率系。


## 3. Existing PyCrSDK Audit (現状の評価)

### 3.1 既存 get 系 zoom API

- `get_zoom_current_position`
- `get_zoom_max_position`
- `get_zoom_min_position`
- `get_zoom_position_step`

現実装は `m_prop.zoom_distance` を返している。

評価:

- 実装自体は `CrDeviceProperty_ZoomDistance` の仕様に合致。
- ただし API 名の "position" は docs 上の `ZoomPositionCurrentValue/Setting` と意味が異なる。
- 互換性のため即削除はしない。以下の方針で整理する。

整理方針:

- 既存 API は残す（deprecated 扱いコメント追加）。
- 新規で "distance" と "absolute position" を明確に分ける。
	- 例: `get_zoom_distance_*` と `get_zoom_abs_position_*`


### 3.2 既存 speed API

- `get_zoom_max_speed`, `get_zoom_min_speed`, `set_zoom_speed`

評価:

- min/max 取得は `CrDeviceProperty_Zoom_Speed_Range` に沿った実装で概ね妥当。
- ただし `set_zoom_speed` は `CrDeviceProperty_Zoom_Operation` を `CrDataType_UInt16Array` で送信しており、docs の型 (`Int8Range`) と不整合の可能性がある。
- 型と値域の正規化が必要。


## 4. Recommended Python API (最適案)

Python 側は「意図が明確」「型がわかる」「失敗時に扱いやすい」を優先する。

### 4.1 Relative zoom API

- `zoom_start(no: int, speed: int) -> bool`
	- speed > 0: Tele, speed < 0: Wide, speed == 0 はエラー扱い（`zoom_stop`を使う）
	- `Zoom_Operation_Status` が Disable なら false
	- speed は `get_zoom_speed_range` で得た min/max に clamp せず、範囲外は false

- `zoom_stop(no: int) -> bool`
	- 0 を送信（停止）

- `zoom_move_relative_int16(no: int, value: int) -> bool`
	- value in [-32768, 32767]
	- 0 は stop
	- 新機種向けに Int16 ルートを直接公開

- `get_zoom_speed_range(no: int) -> tuple[int, int, int]`
	- `(min, max, step)`
	- 非対応時は `(-1, 1, 1)` を返す（現実装互換）


### 4.2 Absolute zoom API

- `get_zoom_abs_position_current(no: int) -> int`
	- `CrDeviceProperty_ZoomPositionCurrentValue.current`

- `get_zoom_abs_position_range(no: int) -> tuple[int, int, int]`
	- `CrDeviceProperty_ZoomPositionSetting.possible` の `(min, max, step)`

- `set_zoom_abs_position(no: int, position: int) -> bool`
	- `CrDeviceProperty_ZoomPositionSetting` に値を設定
	- 範囲外は false

- `cancel_zoom_abs_position(no: int) -> bool`
	- `CrCommandId_CancelZoomPosition` を Down->Up 送信

- `get_zoom_driving_status(no: int) -> int`
	- `CrDeviceProperty_ZoomDrivingStatus.current`


### 4.3 Compatibility API (既存 API 温存)

- 既存 `get_zoom_*position*` は当面維持。
- ただし docstring/コメントで「これは ZoomDistance(mm系)」と明記。
- 新規 `get_zoom_distance_current/min/max/step` を追加して誤解を防ぐ。


## 5. C++ Implementation Plan

## 5.1 include/CameraDevice.h

追加宣言:

- `bool zoom_start(int speed);`
- `bool zoom_stop();`
- `bool zoom_move_relative_int16(int value);`
- `bool get_zoom_speed_range(int& min_speed, int& max_speed, int& step);`
- `int  get_zoom_abs_position_current();`
- `bool get_zoom_abs_position_range(int& min_pos, int& max_pos, int& step);`
- `bool set_zoom_abs_position(int position);`
- `bool cancel_zoom_abs_position();`
- `int  get_zoom_driving_status();`
- `int  get_zoom_distance_current();`
- `int  get_zoom_distance_min();`
- `int  get_zoom_distance_max();`
- `int  get_zoom_distance_step();`


## 5.2 include/CameraDevice.cpp

実装方針:

1. `load_properties()` 後に writable/possible を必ず検証。
2. Relative:
	 - 基本は `CrDeviceProperty_Zoom_Operation` を使用。
	 - 値は `int8` レンジに収める。
	 - value type は docs 準拠の型を使用（現状 `UInt16Array` を修正）。
3. Int16 relative:
	 - `CrDeviceProperty_ZoomOperationWithInt16` を直接 set。
4. Absolute:
	 - `CrDeviceProperty_ZoomPositionSetting` に値を set。
	 - range check は `possible[0..2]`。
5. Cancel absolute:
	 - `SDK::SendCommand(...CrCommandId_CancelZoomPosition, Down)`
	 - 直後に Up。
6. 既存 `set_zoom_speed` は `zoom_start/zoom_stop` の薄いラッパに寄せる。
7. 既存 get_position 系は内部的に `zoom_distance_*` にリネーム相当実装へ整理。


## 5.3 PyCrSDK.hpp (宣言)

`CameraManager` に新規メソッドを追加:

- relative: `zoom_start`, `zoom_stop`, `zoom_move_relative_int16`, `get_zoom_speed_range`
- absolute: `get_zoom_abs_position_current`, `get_zoom_abs_position_range`, `set_zoom_abs_position`, `cancel_zoom_abs_position`, `get_zoom_driving_status`
- distance: `get_zoom_distance_current`, `get_zoom_distance_min`, `get_zoom_distance_max`, `get_zoom_distance_step`


## 5.4 PyCrSDK.cpp

追加内容:

1. `findTarget(no, camera, true)` による共通ガード。
2. `CameraManager` メソッドを `CameraDevice` へ委譲。
3. tuple 返却が必要な API (`*_range`) は `std::tuple<int,int,int>` を返す。
4. 既存 `set_zoom_speed` は残し、内部で `zoom_start/zoom_stop` と同じ検証を使う。
5. 戻り値規約:
	 - 取得失敗: int=-1 / tuple=(-1,-1,-1)
	 - 実行失敗: bool=false


## 5.5 pybind_module.cpp

公開メソッドを追加:

- `.def("zoom_start", &CameraManager::zoom_start, py::arg("no"), py::arg("speed"))`
- `.def("zoom_stop", &CameraManager::zoom_stop, py::arg("no"))`
- `.def("zoom_move_relative_int16", &CameraManager::zoom_move_relative_int16, py::arg("no"), py::arg("value"))`
- `.def("get_zoom_speed_range", &CameraManager::get_zoom_speed_range, py::arg("no"))`
- `.def("get_zoom_abs_position_current", &CameraManager::get_zoom_abs_position_current, py::arg("no"))`
- `.def("get_zoom_abs_position_range", &CameraManager::get_zoom_abs_position_range, py::arg("no"))`
- `.def("set_zoom_abs_position", &CameraManager::set_zoom_abs_position, py::arg("no"), py::arg("position"))`
- `.def("cancel_zoom_abs_position", &CameraManager::cancel_zoom_abs_position, py::arg("no"))`
- `.def("get_zoom_driving_status", &CameraManager::get_zoom_driving_status, py::arg("no"))`
- `.def("get_zoom_distance_current", &CameraManager::get_zoom_distance_current, py::arg("no"))`
- `.def("get_zoom_distance_min", &CameraManager::get_zoom_distance_min, py::arg("no"))`
- `.def("get_zoom_distance_max", &CameraManager::get_zoom_distance_max, py::arg("no"))`
- `.def("get_zoom_distance_step", &CameraManager::get_zoom_distance_step, py::arg("no"))`


## 6. Validation Plan

1. compile check (CMake build)
2. 実機テスト観点
	 - `zoom_start(+speed)` -> Tele 動作
	 - `zoom_start(-speed)` -> Wide 動作
	 - `zoom_stop()` -> 停止
	 - `set_zoom_abs_position()` -> `get_zoom_abs_position_current()` が追従
	 - 絶対動作中 `cancel_zoom_abs_position()` で停止
	 - `Zoom_Operation_Status=Disable` 時に false を返す
3. 後方互換
	 - 既存 `get_zoom_*position*` 呼び出しが壊れないこと


## 7. Decision

最適APIは「Relative と Absolute を別系統で明示的に提供」する構成とする。

- Relative: `zoom_start/zoom_stop/zoom_move_relative_int16`
- Absolute: `set_zoom_abs_position/get_zoom_abs_position_* / cancel_zoom_abs_position`
- 補助: `get_zoom_speed_range/get_zoom_driving_status`
- 互換: 既存 get 系は維持しつつ distance 名 API を新設

この構成により、ズームの絶対位置操作と相対位置操作を同時に扱え、既存利用者への破壊的変更も避けられる。
