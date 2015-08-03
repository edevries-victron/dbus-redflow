#!/bin/sh

# XXX, not to polite but want to use the version compiled against..
export PATH=/home/jeroen/QtSDK/Desktop/Qt/474/gcc/bin/:$PATH

qdbusxml2cpp -v -c VBusItemProxy -p v_busitem_proxy.h:v_busitem_proxy.cpp com.victron.busitem.xml
qdbusxml2cpp -c VBusItemAdaptor -a v_busitem_adaptor.h:v_busitem_adaptor.cpp com.victron.busitem.xml
