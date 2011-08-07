# Pins
SCHEMATIC_LAYERS = Nets Busses Symbols Names Values Info Guide
BOARD_TOP_LAYERS = Top Pads Vias
BOARD_BOTTOM_LAYERS = Bottom Pads Vias

all: generated/quad-esc.pdf

generated/quad-esc.pdf: generated/quad-esc-schematic.pdf generated/quad-esc-board-top.pdf generated/quad-esc-board-bottom.pdf
	@echo Combing all PDFs
	@pdftk $^ cat output $@

generated/quad-esc-schematic.pdf: board/quad-esc.sch
generated/quad-esc-board-bottom.pdf: board/quad-esc.brd
generated/quad-esc-board-top.pdf: board/quad-esc.brd

generated/%: board/%
	@mkdir -p `dirname $@`
	@mv $< $@

%-schematic.pdf: %.sch
	@echo 'Generating schema from $<'
	@eagle -X -dPS_DINA3 -o./tmp $< $(SCHEMATIC_LAYERS) > /dev/null
	@ps2pdf tmp $@
	@rm tmp

%-board-top.pdf: %.brd
	@echo 'Generating top layer from $<'
	@eagle -X -dPS -o./tmp $< $(BOARD_TOP_LAYERS) > /dev/null
	@ps2pdf tmp $@
	@rm tmp

%-board-bottom.pdf: %.brd
	@echo 'Generating bottom layer from $<'
	@eagle -X -dPS -o./tmp $< $(BOARD_BOTTOM_LAYERS) > /dev/null
	@ps2pdf tmp $@
	@rm tmp

clean:
	@rm -rf generated
	@mkdir generated
	@rm -f $(wildcard board/*.pdf)
	@rm -f $(wildcard board/*.b#?) # board backups
	@rm -f $(wildcard board/*.s#?) # schema backups
	@rm -f $(wildcard eagle-library/*.l#?) # library backups
