FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append = " \
    file://0001-arm-dts-add-osd32mp1-brk-mcm-board.patch \
    file://0002-ARM-dts-stm32-reserve-MCM-shared-DDR-region.patch \
"

KERNEL_DEVICETREE:append:osd32mp1-brk = " \
    st/stm32mp157f-osd32mp1-brk-mcm.dtb \
"
