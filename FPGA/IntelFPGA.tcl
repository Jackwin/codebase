# Arria 10 - 10AX115S2F45I1SG
set_location_assignment PIN_AY10 -to F_AD[15]
set_location_assignment PIN_AY11 -to F_AD[14]
set_location_assignment PIN_AW16 -to F_AD[13]
set_location_assignment PIN_BB11 -to F_AD[12]
set_location_assignment PIN_AY12 -to F_AD[11]
set_location_assignment PIN_BD12 -to F_AD[10]
set_location_assignment PIN_AY14 -to F_AD[9]
set_location_assignment PIN_BD14 -to F_AD[8]
set_location_assignment PIN_AV25 -to F_AD[7]
set_location_assignment PIN_AU25 -to F_AD[6]
set_location_assignment PIN_AY25 -to F_AD[5]
set_location_assignment PIN_AR26 -to F_AD[4]
set_location_assignment PIN_BA25 -to F_AD[3]
set_location_assignment PIN_BD26 -to F_AD[2]
set_location_assignment PIN_BC25 -to F_AD[1]
set_location_assignment PIN_BA24 -to F_AD[0]

set_location_assignment PIN_AW22 -to F_A[3]
set_location_assignment PIN_AR24 -to F_A[2]
set_location_assignment PIN_AP24 -to F_A[1]
set_location_assignment PIN_AU23 -to F_A[0]

set_location_assignment PIN_AN11 -to F_ADV_n
set_location_assignment PIN_AN13 -to F_RD_n
set_location_assignment PIN_AP11 -to F_WR_n
set_location_assignment PIN_AR10 -to F_WAIT_n
set_location_assignment PIN_AM12 -to F_CS_n
set_location_assignment PIN_AM11 -to F_CLK

set_location_assignment PIN_AN10 -to GPIO[1]
set_location_assignment PIN_AL10 -to GPIO[0]



for {set index 0} {$index < 16} {incr index} {
    set_instance_assignment -name IO_STANDARD 1.8V -to F_AD[$index]
}

for {set index 0} {$index < 4} {incr index} {
    set_instance_assignment -name IO_STANDARD 1.8V -to F_A[$index]
}

set_instance_assignment -name IO_STANDARD 1.8V -to F_ADV_n
set_instance_assignment -name IO_STANDARD 1.8V -to F_RD_n
set_instance_assignment -name IO_STANDARD 1.8V -to F_WR_n
set_instance_assignment -name IO_STANDARD 1.8V -to F_WAIT_n
set_instance_assignment -name IO_STANDARD 1.8V -to F_CS_n
set_instance_assignment -name IO_STANDARD 1.8V -to F_CLK

set_instance_assignment -name IO_STANDARD 1.8V -to GPIO[1]
set_instance_assignment -name IO_STANDARD 1.8V -to GPIO[0]


#---------------------------------------- LED --------------------------------
#LED7
set_location_assignment PIN_R12 -to C_LED[0]
#LED8
set_location_assignment PIN_R11 -to C_LED[1]
#LED9
set_location_assignment PIN_N10 -to C_LED[2]
#LED10
set_location_assignment PIN_M10 -to C_LED[3]
#LED11
set_location_assignment PIN_L12 -to C_LED[4]
#LED12
set_location_assignment PIN_M11 -to C_LED[5]

for {set index 0} {$index < 6} {incr index} {
    set_instance_assignment -name IO_STANDARD 1.8V -to C_LED[$index]
}

#---------------------------------------- CLK RST ------------------------------
set_location_assignment PIN_AU33 -to USR_CLK
set_instance_assignment -name IO_STANDARD 1.8V -to USR_CLK

set_location_assignment PIN_AU10 -to USR_RST_n
set_instance_assignment -name IO_STANDARD 1.8V -to USR_RST_n

