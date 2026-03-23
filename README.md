# Xtune

Xtune is an overclocking and benchmarking utility for CASIO calculators with SuperH 3/4 MPU.

## Features

> Note: Ftune only has PLL (1-4x), memory latency tuning and benchmarks due to SH3 MPU design.

* Real-time frequency monitoring — view CPU, SuperHyway, bus, and peripheral frequencies live
* Frequency fine-tuning — adjust frequencies with FLL (450-2047x), PLL (1-64x) and dividers
* Memory latency tuning — adjust wait cycles of RAM, ROM and even LCD controller
* Memory tests — verify RAM and ROM integrity
* Benchmarks — CPU/memory scores, FPS counter and Dhrystone

## Releases

Xtune comes in 2 flavors: Legacy (GPLv2+) and gint (MIT). New features will appear on gint version first.

Legacy | Calculators | Remarks
--- | --- | ---
[Ftune](https://git.planet-casio.com/CalcLoverHK/Xtune/wiki/Ftune) | fx-9860G(II) SH3, Graph 75/85/95 SH3 |
[Ftune2](https://git.planet-casio.com/CalcLoverHK/Xtune/wiki/Ftune2) | fx-9860GII SH4A, Graph 75/95 SH4A |
[Ftune3](https://git.planet-casio.com/CalcLoverHK/Xtune/wiki/Ftune3) | fx-9750/9860GIII, Graph 35+EII | Compatible with models supported by Ftune2
[Ptune2](https://git.planet-casio.com/CalcLoverHK/Xtune/wiki/Ptune2) | fx-CG10/20 |
[Ptune3](https://git.planet-casio.com/CalcLoverHK/Xtune/wiki/Ptune3) | fx-CG50, Graph 90+E |

gint | Calculators | Remarks
--- | --- | ---
Ftune4 (WIP) | All models supported by Ftune2/3 | Does not support SH3 models at the moment as [gint's getkey crashes in overclocked SH3 calculators](https://git.planet-casio.com/Lephenixnoir/gint/pulls/50)
Ptune4 | All models supported by Ptune2/3, fx-CG100, fx-1AU Graph, Graph Math+ | Comes with unified and model-specific versions
CPtune4 | fx-CP400(+E), fx-CG500 |
