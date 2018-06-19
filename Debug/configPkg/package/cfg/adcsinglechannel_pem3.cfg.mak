# invoke SourceDir generated makefile for adcsinglechannel.pem3
adcsinglechannel.pem3: .libraries,adcsinglechannel.pem3
.libraries,adcsinglechannel.pem3: package/cfg/adcsinglechannel_pem3.xdl
	$(MAKE) -f C:\Users\Ledx\Documents\TI_Projects\pruebasMecanica/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\Ledx\Documents\TI_Projects\pruebasMecanica/src/makefile.libs clean

