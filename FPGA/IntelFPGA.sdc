## VENDOR  "Intel Corporation"
## PROGRAM "Quartus Prime"
## VERSION "Version 17.0.0  Pro Edition"


## DEVICE  "10AX115S2F45I1SG"



# Automatically calculate clock uncertainty to jitter and other effects.
derive_clock_uncertainty

derive_pll_clocks -create_base_clocks

#create_clock -name {USR_CLK} -period 8.000 -waveform { 0.000 4.000 } [get_ports {USR_CLK}]
