# 새 계기판 위젯 추가하기

계기판 화면은 **위젯 여러 개**로 그려집니다. 각 위젯 = 순수 함수로, 공유 **1bpp 프레임버퍼**의
자기 영역에 그림만 그립니다. 하드웨어·CAN·전역상태를 모르므로 **노트북에서 그대로 테스트**됩니다.

## 1. 위젯 파일 만들기 (`src/modules/widgets/`)
`widget_range.h`
```cpp
#pragma once
#include "framebuffer.h"
void widget_range_draw(FrameBuffer &fb, int x, int y, int range_km);
```
`widget_range.cpp`
```cpp
// [FILL-IN] Edit this file. Draw your widget into the framebuffer.
#include "modules/widgets/widget_range.h"
void widget_range_draw(FrameBuffer &fb, int x, int y, int range_km) {
    fb_number(fb, x, y, range_km, 2);
}
```

## 2. 그리기 도구 (framebuffer.h)
- `fb_rect(fb, x, y, w, h, fill, on)` / `fb_hline` / `fb_vline`
- `fb_text(fb, x, y, "R", scale)` — 폰트 글리프는 `font5x7.cpp`에 있는 문자만( ` 0-9 - R N D %`). 새 문자가 필요하면 코어 담당에게 글리프 추가 요청.
- `fb_number(fb, x, y, value, scale)` / `fb_bar(fb, x, y, w, h, pct)`

## 3. 테스트 (하드웨어 없이)
`test/test_widget_range/test_widget_range.cpp` — 프레임버퍼에 예상 픽셀이 켜졌는지 단언.
실행: `pio test -e native -f test_widget_range`.
디버깅 팁: 실패 시 `fb.get(x,y)` 를 루프 돌려 `#`/`.` 로 찍어 화면을 눈으로 확인.

## 4. 화면에 배치 (코어 담당 영역)
`src/core/app_wiring.cpp` 의 `display_update()` 에 한 줄 추가:
```cpp
widget_range_draw(fb, 10, 60, computed_range);
```
좌표(영역)는 다른 위젯과 겹치지 않게 코어 담당이 조율.

## 규칙
- 위젯 `.cpp` 에서 `state.h` / `<Arduino.h>` include 금지 (컴파일 깨짐 = 의도된 방어).
- 자기 영역 밖으로 그려도 프레임버퍼가 클리핑하지만, 레이아웃은 지킬 것.
