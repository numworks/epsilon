#!/bin/bash

# Necessary packages: jq, inkscape

rm UpsilonIconsCUSTOM.svg
cp UpsilonIconsVAR_PrettyPrinted.svg UpsilonIconsCUSTOM.svg
file=UpsilonIconsCUSTOM.svg
k=1

mkdir themes/local/custom_theme
mkdir themes/local/custom_theme/apps
mkdir themes/local/custom_theme/probability
dir=./themes/local/custom_theme


#Checks if jq is installed then assigns returned variables to $k or uses command line args
if [ $(which jq | wc -l) -ge 1 ]; then
  hex=$(cat logocolors.json | jq -r '.[]')
  for param in $hex; do
    echo "COLORID$k => $param"
    sed -i 's%COLORID'$k'%'$param'%g' $file
    k=$(($k+1))
  done
else
  for param in $@; do
    echo "COLORID$k => $param"
    sed -i 's%COLORID'$k'%'$param'%g' $file
    k=$(($k+1))
  done
fi

if [ $(which inkscape | wc -l) -ge 1 ]; then
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/apps/calculation_icon.png --export-area=0:0:55:56
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/apps/rpn_icon.png --export-area=80:0:135:56
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/apps/graph_icon.png --export-area=160:0:215:56
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/apps/code_icon.png --export-area=0:87:55:143
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/apps/stat_icon.png --export-area=80:87:135:143
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/apps/probability_icon.png --export-area=160:87:215:143
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/apps/solver_icon.png --export-area=0:173:55:229
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/apps/atomic_icon.png --export-area=80:173:135:229
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/apps/sequence_icon.png --export-area=160:173:215:229
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/apps/graph_icon.png --export-area=160:0:215:56
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/apps/graph_icon.png --export-area=0:258:55:314
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/apps/regression_icon.png --export-area=160:0:215:56
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/apps/settings_icon.png --export-area=80:258:135:314
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/apps/external_icon.png --export-area=160:258:215:314
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/apps/xcas_icon.png --export-area=80:342:135:397
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/binomial_icon.png --export-area=273:0:308:19
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/chi_squared_icon.png --export-area=273:29:308:48
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/exponential_icon.png --export-area=273:58:308:77
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/fisher_icon.png --export-area=273:87:308:106
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/geometric_icon.png --export-area=273:116:308:135
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/normal_icon.png --export-area=273:145:308:164
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/poisson_icon.png --export-area=273:174:308:193
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/student_icon.png --export-area=273:203:308:222
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/uniform_icon.png --export-area=273:232:308:251
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/calcul1_icon.png --export-area=273:261:312:284
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/calcul2_icon.png --export-area=273:294:312:317
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/calcul3_icon.png --export-area=273:327:312:350
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/calcul4_icon.png --export-area=273:360:312:383
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/focused_binomial_icon.png --export-area=327:0:362:19
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/focused_chi_squared_icon.png --export-area=327:29:362:48
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/focused_exponential_icon.png --export-area=327:58:362:77
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/focused_fisher_icon.png --export-area=327:87:362:106
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/focused_geometric_icon.png --export-area=327:116:362:135
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/focused_normal_icon.png --export-area=327:145:362:164
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/focused_poisson_icon.png --export-area=327:174:362:193
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/focused_student_icon.png --export-area=327:203:362:222
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/focused_uniform_icon.png --export-area=327:232:362:251
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/focused_calcul1_icon.png --export-area=327:261:366:284
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/focused_calcul2_icon.png --export-area=327:294:366:317
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/focused_calcul3_icon.png --export-area=327:327:366:350
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/probability/focused_calcul4_icon.png --export-area=327:360:366:383
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/exam_icon.png --export-area=273:393:291:402
  inkscape ./UpsilonIconsCUSTOM.svg -o $dir/logo_icon.png --export-area=0:447:115:479
fi

