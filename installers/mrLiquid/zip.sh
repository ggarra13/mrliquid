#!/bin/bash --norc

rm *.zip &> /dev/null
cd output

soft=mrLiquid
version=0.8.0
RLM=/home/gga/code/licensing/RLM/installers/rlm/output/rlm*
rubyMEL=/home/gga/code/Maya/rubyMEL/installers/rubyMEL/output/rubyMEL*
web=/home/gga/www/filmaura/rails/orders/software/mrLiquid_PRO

rm ${RLM##*/} &> /dev/null

for i in $RLM
do
  ln -s $i .
done

for i in $rubyMEL
do
  ln -s $i .
done

cat <<EOF
----------------------------
$soft-demo $version
----------------------------
EOF
command zip -9v -D ../v${version}_trial.zip $soft-demo* ${RLM##*/} rubyMEL-demo*


cat <<EOF
----------------------------
$soft $version
----------------------------
EOF
command zip -9v -D ../v${version}.zip $soft* ${RLM##*/} rubyMEL*  -x "${soft}-demo*" "rubyMEL-demo*"

rm ${RLM##*/}
rm ${rubyMEL##*/}

cd ..

for i in v*.zip
do
    cp $i $web/
done
