# Copyright (c) 2008-2009 MStar Semiconductor, Inc.
# All rights reserved.

$2 == "ENABLE_MSTAR_TITANIA" { if($3==1) system("cd ../u-boot-1.1.6/;make titania_config;make -s")}
$2 == "ENABLE_MSTAR_TITANIA2" { if($3==1) system("cd ../u-boot-1.1.6/;make titania2_config;make -s")}
$2 == "ENABLE_MSTAR_TITANIA3" { if($3==1) system("cd ../u-boot-1.1.6/;make titania3_config;make -s")}
$2 == "ENABLE_MSTAR_EUCLID" { if($3==1) system("cd ../u-boot-1.1.6/;make euclid_config;make -s")}
$2 == "ENABLE_MSTAR_TITANIA4" { if($3==1) system("cd ../u-boot-1.1.6/;make titania4_config;make -s")}
$2 == "ENABLE_MSTAR_TITANIA7" { if($3==1) system("cd ../u-boot-1.1.6/;make titania7_config;make -s")}
$2 == "ENABLE_MSTAR_TITANIA8" { if($3==1) system("cd ../u-boot-1.1.6/;make titania8_config;make -s")}
$2 == "ENABLE_MSTAR_TITANIA12" { if($3==1) system("cd ../u-boot-1.1.6/;make titania12_config;make -s")}
$2 == "ENABLE_MSTAR_AMBER2" { if($3==1) system("cd ../u-boot-1.1.6/;make amber2_config;make -s")}
$2 == "ENABLE_MSTAR_AMBER3" { if($3==1) system("cd ../u-boot-2011.06/;make amber3_config;sh build.sh")}
$2 == "ENABLE_MSTAR_AGATE" { if($3==1) system("cd ../u-boot-2011.06/;make agate_config;sh build.sh")}
$2 == "ENABLE_MSTAR_AMBER5" { if($3==1) system("cd ../u-boot-2011.06/;make amber5_config;make -s")}
$2 == "ENABLE_MSTAR_TITANIA9" { if($3==1) system("cd ../u-boot-1.1.6/;make titania9_config;make -s")}
$2 == "ENABLE_MSTAR_TITANIA13" { if($3==1) system("cd ../u-boot-1.1.6/;make titania13_config;make -s")}
$2 == "ENABLE_MSTAR_AMBER1" { if($3==1) system("cd ../u-boot-1.1.6/;make amber1_config;make -s")}
$2 == "ENABLE_MSTAR_AMBER6" { if($3==1) system("cd ../u-boot-1.1.6/;make amber6_config;make -s")}
$2 == "ENABLE_MSTAR_AMBER7" { if($3==1) system("cd ../u-boot-1.1.6/;make amber7_config;make -s")}
$2 == "ENABLE_MSTAR_AMETHYST" { if($3==1) system("cd ../u-boot-2011.06/;make amethyst_config;make -s")}
$2 == "ENABLE_MSTAR_URANUS4" { if($3==1) system("cd ../u-boot-1.1.6/;make uranus4_config;make -s")}
#$2 == "ENABLE_MSTAR_JANUS" { if($3==1) system("cd ../u-boot-1.1.6/;make janus_config;make -s")}
$2 == "ENABLE_MSTAR_JANUS" { if($3==1) system("cd ../u-boot-1.1.6/;make janus_config;make -s")}
$2 == "ENABLE_MSTAR_JANUS2" { if($3==1) system("cd ../u-boot-1.1.6/;make janus2_config;make -s")}
$2 == "ENABLE_MSTAR_MARIA10" { if($3==1) system("cd ../u-boot-1.1.6/;make maria10_config;make -s")}
$2 == "ENABLE_MSTAR_KRONUS" { if($3==1) system("cd ../u-boot-1.1.6/;make kronus_config;make -s")}
$2 == "ENABLE_MSTAR_KAISERIN" { if($3==1) system("cd ../u-boot-1.1.6/;make kaiserin_config;make -s")}
$2 == "ENABLE_MSTAR_EAGLE" { if($3==1) system("cd ../u-boot-2011.06/;make eagle_config;sh build.sh")}
$2 == "ENABLE_MSTAR_EIFFEL" { if($3==1) system("cd ../u-boot-2011.06/;make eiffel_config;sh build.sh")}
$2 == "ENABLE_MSTAR_NIKE" { if($3==1) system("cd ../u-boot-2011.06/;make nike_config;sh build.sh")}
$2 == "ENABLE_MSTAR_EMERALD" { if($3==1) system("cd ../u-boot-1.1.6/;make emerald_config;make -s")}
$2 == "ENABLE_MSTAR_EDISON" { if($3==1) system("cd ../u-boot-2011.06/;make edison_config;make -s")}
$2 == "ENABLE_MSTAR_EINSTEIN" { if($3==1) system("cd ../u-boot-2011.06/;make einstein_config;make -s")}
