.PHONY: 

plot-a.plt: sim1.raw
	ngnutmeg $< < sim1.plots

sim1.raw:

plots: sim1.raw

%.raw: %.cir
	ngspice -r $@ -b $<
