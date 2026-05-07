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


## 8. Lens / Focus API Extension Spec (Camera Remote SDK)

### 8.1 Goal

PyCrSDK をレンズ情報・フォーカス操作まで拡張し、Python から以下を安全に扱えるようにする。

- レンズ情報テーブル取得 (`CrLensInformation`)
- ズーム/フォーカスプリセット情報取得 (`CrZoomAndFocusPresetInfo`)
- フォーカスの絶対位置指定 / 現在位置取得 / キャンセル
- フォーカスの相対操作（簡易 Int8 と高分解能 Int16）
- Follow Focus 用の正規化位置取得 / 設定
- プリセット保存 / 呼び出し / zoom-only フラグ設定
- 距離単位系・変換に必要な補助情報取得


### 8.2 SDK Doc Facts (HTML + SDK header から確認した事実)

#### 8.2.1 LensInformation API は取得専用

- `RequestLensInformation(deviceHandle)`
	- レンズ情報取得要求を投げる API。
	- `OnWarning` に `CrWarning_RequestLensInformation_Result_*` が返る。

- `GetLensInformation(deviceHandle, CrLensInformation** list, CrInt32u* numOfList)`
	- `CrLensInformation` 配列を取得する API。
	- `ReleaseLensInformation()` と対で使う。

- `CrLensInformation`
	- `type: CrLensInformationType`
		- `Undefined`, `Feet`, `Meter`
	- `dataVersion: CrInt16u`
		- Data Version (100-fold value)
	- `normalizedValue: CrInt32u`
		- 正規化フォーカス位置値
	- `focusPosition: CrInt32u`
		- 実距離値
		- docs 例: `20 = 0.2 feet/meter`

- `op_how_to_use_lens_information.html`
	- `FollowFocusPositionSetting` / `FollowFocusPositionCurrentValue` を meters/feet に変換する表として `GetLensInformation()` を使う。
	- movie mode のみ有効。

評価:

- SDK には `SetLensInformation` のような API は存在しない。
- `CrLensInformation` 自体は read-only snapshot として扱うべき。


#### 8.2.2 Lens information availability

- `CrDeviceProperty_LensInformationEnableStatus`
	- docs: `Get the Lens Information Enable Status`
	- DataType: `CrDataType_UInt8Array`
	- `Enable/Disable`
	- movie mode のみ有効

評価:

- これはレンズ情報テーブルが利用可能かどうかの capability/status であり、設定 API ではない。


#### 8.2.3 Absolute focus

- `CrDeviceProperty_FocusPositionSetting`
	- docs: `Get/Set the Absolute Focus Position`
	- DataType: `CrDataType_UInt16Range`
	- min `0x0000`, max `0xFFFF`, step `0x0001`

- `CrDeviceProperty_FocusPositionCurrentValue`
	- docs: `Get the Absolute Focus Position Current Value`
	- DataType: `CrDataType_UInt16Range`

- `CrDeviceProperty_FocusDrivingStatus`
	- 駆動中かどうかの監視に利用可能

- `CrControlCode_CancelFocusPosition` / `CrCommandId_CancelFocusPosition`
	- 絶対フォーカス動作の cancel に利用可能
	- 既存実装でも `Down -> Up` 送信

評価:

- 絶対フォーカス操作は SDK 上で明確に settable。
- 既存 `CameraDevice::set_focus_position_setting()` / `execute_focus_position_cancel()` はこの系統の CLI 実装。


#### 8.2.4 Relative focus

- `CrDeviceProperty_Focus_Operation`
	- docs: `Execute the focus operation`
	- DataType: `CrDataType_Int8`
	- `CurrentValue` は常に 0、update only
	- `CrDeviceProperty_Focus_Speed_Range` の範囲で設定可能
	- 例:
		- `1`   = Tele/Far focus
		- `-3`  = Wide/Near focus (speed 3)
		- `0`   = Stop
	- docs 注記:
		- `CrDeviceProperty_FocalDistanceInMeter` または `CrDeviceProperty_FocalDistanceInFeet` が Enable のとき有効

- `CrDeviceProperty_Focus_Speed_Range`
	- docs: `Get the Focus Speed Range`
	- DataType: `CrDataType_Int8Range`
	- min/max/step が機種依存
	- `CurrentValue` は常に 0

- `CrDeviceProperty_FocusOperationWithInt16`
	- docs: `Execute focus operation by specifying direction and speed`
	- DataType: `CrDataType_Int16Range`
	- range: `-32768 .. 32767`, step `1`
	- `0x0001..0x7FFF` = Far focus
	- `0xFFFF..0x8000` = Near focus
	- `0x0000` = Stop

- `CrDeviceProperty_FocusOperationWithInt16EnableStatus`
	- DataType: `CrDataType_UInt8Array`
	- Enable 時のみ Int16 relative focus が実行可能

評価:

- Relative focus は `Int8` と `Int16` の 2 ルートを両方提供可能。
- `Int16` は新機種向けの高分解能 API として独立公開するのが適切。


#### 8.2.5 Follow Focus position

- `CrDeviceProperty_FollowFocusPositionSetting`
	- docs: `Get/Set the Follow Focus Position`
	- DataType: `CrDataType_UInt16Range`
	- `CurrentValue` は要求値
	- 実際の位置確認は `FollowFocusPositionCurrentValue` を使う

- `CrDeviceProperty_FollowFocusPositionCurrentValue`
	- docs: `Get the Follow Focus Position Current Value`
	- DataType: `CrDataType_UInt16Range`
	- `GetLensInformation()` のテーブルで meters/feet に変換可能
	- movie recording 向け focus drive

評価:

- Follow Focus は absolute focus とは別の正規化位置系として API を分けるべき。
- 実距離換算は `LensInformation` テーブル依存なので、変換 API も別レイヤで提供する。


#### 8.2.6 Zoom & Focus preset information

- `RequestZoomAndFocusPreset()` / `GetZoomAndFocusPreset()` / `ReleaseZoomAndFocusPreset()`
	- プリセット一覧取得 API
	- `CrZoomAndFocusPresetInfo` 配列を返す

- `CrZoomAndFocusPresetInfo`
	- `isExists`
	- `lensModelName`
	- `zoomDistance` (0.001 mm)
	- `focalDistance` (0.001 m, infinity の特殊値ありうる)
	- `zoomOnlyEnableStatus`
	- `zoomOnlyValue`
		- docs: `CrDeviceProperty_ZoomAndFocusPresetZoomOnly_Set` で変更可能

- `CrDeviceProperty_ZoomAndFocusPresetZoomOnly_Set`
	- docs: `Set the Zoom&Focus Preset Zoom-only`
	- DataType: `CrDataType_UInt16`
	- upper 8 bits = preset number
	- lower 8 bits = `CrZoomAndFocusPresetZoomOnlyValue_Off/On`

- `CrDeviceProperty_ZoomAndFocusPosition_Save`
	- 保存対象 preset number を設定する property

- `CrDeviceProperty_ZoomAndFocusPosition_Load`
	- 呼び出し対象 preset number を設定する property

- `CrControlCode_SaveZoomAndFocusPosition`
- `CrControlCode_LoadZoomAndFocusPosition`

評価:

- `CrZoomAndFocusPresetInfo` 自体は read-only snapshot。
- ただし preset の save/load と zoom-only flag は SDK 上で settable。


### 8.3 Existing PyCrSDK Audit (現状の評価)

- `CameraDevice` には CLI 向けとして以下が既に存在する:
	- `get_focus_position_setting()`
	- `set_focus_position_setting()`
	- `execute_focus_position_cancel()`
	- `execute_request_zoom_and_focus_preset()`
	- `execute_get_zoom_and_focus_preset()`

- しかし Python wrapper (`PyCrSDK.hpp/.cpp`, `pybind_module.cpp`) には未公開。

- `LensInformation` 取得系は `CameraDevice` にもまだラッパメソッドが無い。

整理方針:

- 既存 CLI 実装は活かす。
- Python 向けには「取得 snapshot」と「操作 command/property」を分けて新設する。
- インタラクティブ CLI 前提の `set_*()` は、非対話の引数型メソッドへ分解する。


### 8.4 Recommended Python API (最適案)

#### 8.4.1 Lens information snapshot API

- `get_lens_information_enable_status(no: int) -> int`
	- `CrLensInformationEnableStatus`
	- movie mode 以外や非対応時は `-1`

- `request_lens_information(no: int) -> bool`
	- `RequestLensInformation()` を実行

- `get_lens_information(no: int) -> list[dict]`
	- `GetLensInformation()` の結果を list 化
	- 各要素:
		- `type`
		- `data_version`
		- `normalized_value`
		- `focus_position`

- `convert_follow_focus_to_distance(no: int, normalized_value: int, unit: str | None = None) -> tuple[int, int] | None`
	- `LensInformation` テーブルから follow-focus 正規化値の前後区間を返す
	- 返値案: `(lower_focus_position, upper_focus_position)`
	- `unit=None` なら現在の focal distance unit に合わせる


#### 8.4.2 Absolute focus API

- `get_focus_abs_position_current(no: int) -> int`
	- `CrDeviceProperty_FocusPositionCurrentValue.current`

- `get_focus_abs_position_range(no: int) -> tuple[int, int, int]`
	- `(min, max, step)`

- `set_focus_abs_position(no: int, position: int) -> bool`
	- `CrDeviceProperty_FocusPositionSetting` に設定
	- 範囲外は false

- `cancel_focus_abs_position(no: int) -> bool`
	- `CrCommandId_CancelFocusPosition` を `Down -> Up`

- `get_focus_driving_status(no: int) -> int`
	- `CrDeviceProperty_FocusDrivingStatus.current`


#### 8.4.3 Relative focus API

- `focus_start(no: int, speed: int) -> bool`
	- `CrDeviceProperty_Focus_Operation`
	- `speed > 0` = far/tele focus
	- `speed < 0` = near/wide focus
	- `speed == 0` は stop と同義でもよいが、API 上は `focus_stop()` を推奨
	- `Focus_Speed_Range` 範囲外は false

- `focus_stop(no: int) -> bool`
	- `CrDeviceProperty_Focus_Operation` に 0 を送る

- `focus_move_relative_int16(no: int, value: int) -> bool`
	- `CrDeviceProperty_FocusOperationWithInt16`
	- `[-32768, 32767]`
	- `0` = stop
	- `FocusOperationWithInt16EnableStatus != Enable` なら false

- `get_focus_speed_range(no: int) -> tuple[int, int, int]`
	- `CrDeviceProperty_Focus_Speed_Range`

- `get_focus_operation_int16_enable_status(no: int) -> int`
	- `CrDeviceProperty_FocusOperationWithInt16EnableStatus`


#### 8.4.4 Follow Focus API

- `get_follow_focus_position_current(no: int) -> int`
	- `CrDeviceProperty_FollowFocusPositionCurrentValue.current`

- `get_follow_focus_position_range(no: int) -> tuple[int, int, int]`
	- `CrDeviceProperty_FollowFocusPositionSetting.possible`

- `set_follow_focus_position(no: int, position: int) -> bool`
	- `CrDeviceProperty_FollowFocusPositionSetting`


#### 8.4.5 Zoom & Focus preset API

- `request_zoom_and_focus_presets(no: int) -> bool`
- `get_zoom_and_focus_presets(no: int) -> list[dict]`
	- 各要素:
		- `preset_no`
		- `is_exists`
		- `lens_model_name`
		- `zoom_distance`
		- `focal_distance`
		- `zoom_only_enable_status`
		- `zoom_only_value`

- `save_zoom_and_focus_preset(no: int, preset_no: int) -> bool`
	- `CrDeviceProperty_ZoomAndFocusPosition_Save`

- `load_zoom_and_focus_preset(no: int, preset_no: int) -> bool`
	- `CrDeviceProperty_ZoomAndFocusPosition_Load`

- `set_zoom_and_focus_preset_zoom_only(no: int, preset_no: int, enabled: bool) -> bool`
	- `CrDeviceProperty_ZoomAndFocusPresetZoomOnly_Set`
	- 上位 8bit に preset number、下位 8bit に on/off 値を詰める


#### 8.4.6 Unit / compatibility helper API

- `get_focal_distance_unit(no: int) -> int`
	- 既存 SDK property `CrDeviceProperty_FocalDistanceUnitSetting` が読めるなら expose
	- `Feet/Meter` 判定用

- `get_focus_distance_in_meter(no: int) -> int`
- `get_focus_distance_in_feet(no: int) -> int`
	- SDK property が既に parse 済みなら expose 候補
	- 未実装なら後回し可


#### 8.4.7 Live focus distance polling API

- `get_focus_distance_in_meter(no: int) -> int`
	- `CrDeviceProperty_FocalDistanceInMeter.current`
	- 単位は `0.001 m`
	- 取得失敗または非対応時は `-1`

- `get_focus_distance_in_feet(no: int) -> int`
	- `CrDeviceProperty_FocalDistanceInFeet.current`
	- 単位は `0.1 feet` 相当の docs 表記に従って扱う
	- 取得失敗または非対応時は `-1`

- `get_focal_distance_unit(no: int) -> int`
	- `CrDeviceProperty_FocalDistanceUnitSetting.current`
	- `Meter` / `Feet` 判定用
	- 取得失敗または非対応時は `-1`

用途:

- AF 中や MF 中に、ホスト側から一定間隔でポーリングして現在のフォーカス距離を読み続ける用途に使う。
- `LensInformation` は Follow Focus 正規化値の変換テーブルであり、ライブ距離取得 API とは分離する。


### 8.5 Return Type Design

- `LensInformation` / `ZoomAndFocusPresetInfo` は pybind で扱いやすい `list[dict]` を第一候補とする。
- C++ 側で専用 struct を新設して pybind class 化してもよいが、現段階では dict/list の方が変更コストが低い。
- 戻り値規約:
	- 取得失敗: `int=-1`, `tuple=(-1,-1,-1)`, `list=[]`, `None`
	- 実行失敗: `bool=false`


### 8.6 C++ Implementation Plan

#### 8.6.1 include/CameraDevice.h

追加宣言:

- lens info
	- `int get_lens_information_enable_status();`
	- `bool request_lens_information();`
	- `std::vector<std::tuple<int, int, int, int>> get_lens_information();`

- absolute focus
	- `int get_focus_abs_position_current();`
	- `bool get_focus_abs_position_range(int& min_pos, int& max_pos, int& step);`
	- `bool set_focus_abs_position(int position);`
	- `bool cancel_focus_abs_position();`
	- `int get_focus_driving_status();`

- relative focus
	- `bool focus_start(int speed);`
	- `bool focus_stop();`
	- `bool focus_move_relative_int16(int value);`
	- `bool get_focus_speed_range(int& min_speed, int& max_speed, int& step);`
	- `int get_focus_operation_int16_enable_status();`

- follow focus
	- `int get_follow_focus_position_current();`
	- `bool get_follow_focus_position_range(int& min_pos, int& max_pos, int& step);`
	- `bool set_follow_focus_position(int position);`

- focal distance helpers
	- `int get_focus_distance_in_meter();`
	- `int get_focus_distance_in_feet();`
	- `int get_focal_distance_unit();`

- preset
	- `bool request_zoom_and_focus_presets();`
	- `std::vector<std::tuple<int, std::wstring, int, int, int, int>> get_zoom_and_focus_presets();`
	- `bool save_zoom_and_focus_preset(int preset_no);`
	- `bool load_zoom_and_focus_preset(int preset_no);`
	- `bool set_zoom_and_focus_preset_zoom_only(int preset_no, bool enabled);`

注:

- C++ 内部 tuple 形は実装都合で可。`PyCrSDK.cpp` で dict 化してもよい。


#### 8.6.2 include/CameraDevice.cpp

実装方針:

1. `load_properties()` 後に writable / possible / enable status を必ず検証。
2. `RequestLensInformation()` / `GetLensInformation()` / `ReleaseLensInformation()` の 3 点をラップ。
3. `FocusPositionSetting` は既存 CLI 実装を非対話版に分解し再利用。
4. relative focus:
	 - `Focus_Operation` は `CrDataType_Int8` で送信。
	 - `FocusOperationWithInt16` は `CrDataType_Int16Range` に準拠した値で送信。
5. cancel focus は既存 `execute_focus_position_cancel()` の非対話ラッパを用意。
6. follow focus は `FollowFocusPositionSetting` / `CurrentValue` を直接 set/get。
7. preset:
	 - `RequestZoomAndFocusPreset()` / `GetZoomAndFocusPreset()` / `ReleaseZoomAndFocusPreset()` をラップ。
	 - `ZoomAndFocusPresetZoomOnly_Set` は `((preset_no - 1) << 8) | value` 形式でパックする。
	 - preset 番号の基準は docs と既存 sample の表示に合わせて Python 側で 1-based に統一する。
8. `save/load` はまず `CrDeviceProperty_ZoomAndFocusPosition_Save/Load` による property route を優先し、必要なら control code route を追加検証する。
9. live focus distance:
	- `CrDeviceProperty_FocalDistanceInMeter`
	- `CrDeviceProperty_FocalDistanceInFeet`
	- `CrDeviceProperty_FocalDistanceUnitSetting`
	を `load_properties()` で parse し、read-only getter として expose する。
	- リアルタイム取得は callback 依存にせず、まずは `load_properties()` ベースの polling API とする。


#### 8.6.3 load_properties() / PropertyValueTable

確認・追加対象:

- 既に parse 済み:
	- `FocusPositionSetting`
	- `FocusPositionCurrentValue`
	- `FocusDrivingStatus`
	- `FollowFocusPositionSetting` / `FollowFocusPositionCurrentValue` は現状未格納なら追加が必要
	- `Focus_Speed_Range` は現状未格納なら追加が必要
	- `FocusOperationWithInt16EnableStatus` は現状未格納なら追加が必要
	- `LensInformationEnableStatus` は現状未格納なら追加が必要
	- `ZoomAndFocusPresetZoomOnly_Set` は setter 専用のため parse は必須ではない
	- `FocalDistanceInMeter` / `FocalDistanceInFeet` / `FocalDistanceUnitSetting` は live distance getter 用に追加が必要


#### 8.6.4 PyCrSDK.hpp

`CameraManager` に以下を追加:

- lens info
	- `int get_lens_information_enable_status(int no);`
	- `bool request_lens_information(int no);`
	- `py::list get_lens_information(int no);`

- focus absolute / relative / follow focus
	- `get_focus_abs_position_current`
	- `get_focus_abs_position_range`
	- `set_focus_abs_position`
	- `cancel_focus_abs_position`
	- `get_focus_driving_status`
	- `focus_start`
	- `focus_stop`
	- `focus_move_relative_int16`
	- `get_focus_speed_range`
	- `get_focus_operation_int16_enable_status`
	- `get_follow_focus_position_current`
	- `get_follow_focus_position_range`
	- `set_follow_focus_position`
	- `get_focus_distance_in_meter`
	- `get_focus_distance_in_feet`
	- `get_focal_distance_unit`

- preset
	- `request_zoom_and_focus_presets`
	- `get_zoom_and_focus_presets`
	- `save_zoom_and_focus_preset`
	- `load_zoom_and_focus_preset`
	- `set_zoom_and_focus_preset_zoom_only`


#### 8.6.5 PyCrSDK.cpp

追加内容:

1. `findTarget(no, camera, true)` で共通ガード。
2. `CameraDevice` 新規メソッドへ委譲。
3. list/dict 返却 API は pybind で扱いやすい形に整形。
4. 失敗時の空 list / false / -1 を統一。


#### 8.6.6 pybind_module.cpp

公開メソッド追加:

- `.def("get_lens_information_enable_status", ...)`
- `.def("request_lens_information", ...)`
- `.def("get_lens_information", ...)`
- `.def("get_focus_abs_position_current", ...)`
- `.def("get_focus_abs_position_range", ...)`
- `.def("set_focus_abs_position", ...)`
- `.def("cancel_focus_abs_position", ...)`
- `.def("get_focus_driving_status", ...)`
- `.def("focus_start", ...)`
- `.def("focus_stop", ...)`
- `.def("focus_move_relative_int16", ...)`
- `.def("get_focus_speed_range", ...)`
- `.def("get_focus_operation_int16_enable_status", ...)`
- `.def("get_follow_focus_position_current", ...)`
- `.def("get_follow_focus_position_range", ...)`
- `.def("set_follow_focus_position", ...)`
- `.def("get_focus_distance_in_meter", ...)`
- `.def("get_focus_distance_in_feet", ...)`
- `.def("get_focal_distance_unit", ...)`
- `.def("request_zoom_and_focus_presets", ...)`
- `.def("get_zoom_and_focus_presets", ...)`
- `.def("save_zoom_and_focus_preset", ...)`
- `.def("load_zoom_and_focus_preset", ...)`
- `.def("set_zoom_and_focus_preset_zoom_only", ...)`


### 8.7 Decision

最適構成は以下とする。

- `CrLensInformation` / `CrZoomAndFocusPresetInfo` は read-only snapshot API として expose
- 書き込み可能なものは snapshot API と分離し、専用 command/property API として expose
- focus は以下 3 系統を明示的に分離
	- absolute focus: `FocusPositionSetting`
	- relative focus: `Focus_Operation` / `FocusOperationWithInt16`
	- follow focus: `FollowFocusPositionSetting`
- lens distance conversion は `LensInformation` テーブルを用いる補助 API として扱う

この構成により、SDK の責務境界に沿って Python API を整理でき、将来の実装でも read-only snapshot と writable command を混同せずに保守できる。
