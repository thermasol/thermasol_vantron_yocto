#!/bin/bash -

if [ -f layers/meta-vantron/meta-custom/imx-setup-release.sh ];then
	. layers/meta-vantron/meta-custom/imx-setup-release.sh $@
elif [ -f layers/meta-vantron/meta-vtlinux-base/setup-environment ];then
	. layers/meta-vantron/meta-vtlinux-base/setup-environment $@
fi
