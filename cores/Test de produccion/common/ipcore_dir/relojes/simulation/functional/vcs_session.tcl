gui_open_window Wave
gui_sg_create relojes_group
gui_list_add_group -id Wave.1 {relojes_group}
gui_sg_addsignal -group relojes_group {relojes_tb.test_phase}
gui_set_radix -radix {ascii} -signals {relojes_tb.test_phase}
gui_sg_addsignal -group relojes_group {{Input_clocks}} -divider
gui_sg_addsignal -group relojes_group {relojes_tb.CLK_IN1}
gui_sg_addsignal -group relojes_group {{Output_clocks}} -divider
gui_sg_addsignal -group relojes_group {relojes_tb.dut.clk}
gui_list_expand -id Wave.1 relojes_tb.dut.clk
gui_sg_addsignal -group relojes_group {{Counters}} -divider
gui_sg_addsignal -group relojes_group {relojes_tb.COUNT}
gui_sg_addsignal -group relojes_group {relojes_tb.dut.counter}
gui_list_expand -id Wave.1 relojes_tb.dut.counter
gui_zoom -window Wave.1 -full
