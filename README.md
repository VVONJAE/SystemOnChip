# SystemOnChip

ZYNQ보드와 Verilog HDL를 이용해 두더지 잡기 게임을 구현

______

## LED
 [LED.v](LED.v)
 [LED_v1_0.v](LED_v1_0.v)
 [LED_v1_0_S00_AXI.v](LED_v1_0_S00_AXI.v)
 * ip를 직접 작성.
 * LED 8개를 껐다 킬 수 있음

______

## [main.c](main.c)
* LED가 랜덤으로 반짝이면 해당되는 버튼을 눌러 불빛을 끄고 점수를 획득
* 게임 난이도 설정 가능
* 게임 시간 60초
