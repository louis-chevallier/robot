
$(warning $(CURDIR))

PGM?=ws

x : start

run :
	arduino-cli core update-index
	arduino-cli board list
	arduino-cli board listall mkr
	arduino-cli core search esp8266
	arduino-cli core search NodeMCU
	arduino-cli core install esp8266:esp8266
#	arduino-cli compile --fqbn arduino:samd:mkr1000 MyFirstSketch

start : compile upload

compile :

	-bin2c --ultra -H $(PGM)/code.h $(PGM)/$(PGM).js
	-bin2c --ultra -H $(PGM)/page.h $(PGM)/$(PGM).html

	arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 $(PGM) --library $(CURDIR)/$(PGM) --library $(CURDIR)

upload :
	-arduino-cli core list
	arduino-cli upload -p /dev/ttyUSB0 --fqbn esp8266:esp8266:nodemcuv2 $(PGM)
	plink /dev/ttyUSB0  -serial -sercfg 115200,8,n,1,N

lib :
#	arduino-cli lib search HC_SR04
#	arduino-cli lib install FTDebouncer
	arduino-cli lib list --json > libs.json
	a=`json-reader libs.json 'installed_libraries,*,library,name'`; for e in $$a; do echo $$e; done






