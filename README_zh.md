# Xtune

Xtune 是一款适用于搭载 SuperH 3/4 处理器的卡西欧计算器的超频与性能基准测试工具。

## 功能特性

> 注意：受 SH3 处理器架构限制，Ftune 仅支持 PLL 倍频（1-4 倍）、内存延迟调节与基准测试。

* 实时频率监控 — 实时查看 CPU、SuperHyway、总线及外设频率
* 频率精细调节 — 通过 FLL（450-2047 倍）、PLL（1-64 倍）与分频器调整频率
* 内存延迟调节 — 配置 RAM、ROM 乃至 LCD 控制器的等待周期
* 内存检测 — 校验 RAM 与 ROM 完整性
* 性能测试 — CPU / 内存跑分、帧率计数器、Dhrystone 测试

## 版本发布

Xtune 提供两个版本：传统版（GPLv2+ 协议）和 gint 版（MIT 协议）。新功能将优先在 gint 版中上线。

传统版 | 适用机型 | 备注
--- | --- | ---
[Ftune](https://git.planet-casio.com/CalcLoverHK/Xtune/wiki/Ftune) | fx-9860G(II) SH3、Graph 75/85/95 SH3 |
[Ftune2](https://git.planet-casio.com/CalcLoverHK/Xtune/wiki/Ftune2) | fx-9860GII SH4A、Graph 75/95 SH4A |
[Ftune3](https://git.planet-casio.com/CalcLoverHK/Xtune/wiki/Ftune3) | fx-9750/9860GIII、Graph 35+EII | 兼容 Ftune2 支持的机型
[Ptune2](https://git.planet-casio.com/CalcLoverHK/Xtune/wiki/Ptune2) | fx-CG10/20 |
[Ptune3](https://git.planet-casio.com/CalcLoverHK/Xtune/wiki/Ptune3) | fx-CG50、Graph 90+E |

gint 版 | 适用机型 | 备注
--- | --- | ---
Ftune4 (WIP) | 所有 Ftune2/3 支持的机型 | 暂不支持 SH3 机型，原因：[gint 的按键输入在超频 SH3 计算器上会崩溃](https://git.planet-casio.com/Lephenixnoir/gint/pulls/50)
Ptune4 | 所有 Ptune2/3 支持的机型、fx-CG100、fx-1AU Graph、Graph Math+ | 提供通用版与机型专属版
CPtune4 | fx-CP400(+E)、fx-CG500 |
