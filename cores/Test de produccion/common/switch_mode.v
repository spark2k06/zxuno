`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    05:20:24 08/15/2016 
// Design Name: 
// Module Name:    switch_mode 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module switch_mode (
   input wire clk,
   input wire clkps2,
   input wire dataps2,
   output reg mode,
   output reg vga,
   output reg memtestf,
   output reg memtests,
   output reg sdtest,
   output reg flashtest,
   output reg mousetest,
   output reg return_to_bios
   );

   wire key_event, released, extended;
   wire [7:0] scancode;

   ps2_port teclado (
      .clk(clk),  // se recomienda 1 MHz <= clk <= 600 MHz
      .enable_rcv(1'b1),  // habilitar la maquina de estados de recepcion
      .kb_or_mouse(1'b0),
      .ps2clk_ext(clkps2),
      .ps2data_ext(dataps2),
      .kb_interrupt(key_event),  // a 1 durante 1 clk para indicar nueva tecla recibida
      .scancode(scancode), // make o breakcode de la tecla
      .released(released),  // soltada=1, pulsada=0
      .extended(extended)  // extendida=1, no extendida=0
   );
    
   initial begin
      mode = 1'b0;
      vga = 1'b0;
      memtestf = 1'b0;
      memtests = 1'b0;
      sdtest = 1'b0;
      flashtest = 1'b0;
      mousetest = 1'b0;
      return_to_bios = 1'b0;
   end
   always @(posedge clk) begin
      if (memtestf == 1'b1)
         memtestf <= 1'b0;
      if (memtests == 1'b1)
         memtests <= 1'b0;
      if (sdtest == 1'b1)
         sdtest <= 1'b0;
      if (flashtest == 1'b1)
         flashtest <= 1'b0;
      if (mousetest == 1'b1)
         mousetest <= 1'b0;
      if (return_to_bios == 1'b1)
         return_to_bios <= 1'b0;
         
      if (key_event == 1'b1) begin
         if (extended == 1'b0 && released == 1'b1) begin
            case (scancode)
               8'h16,8'h69: begin  // tecla 1
                              mode <= 1'b0;
                              vga <= 1'b0;
                            end
               8'h1E,8'h72: begin  // tecla 2
                              mode <= 1'b1;
                              vga <= 1'b0;
                            end
               8'h26,8'h7A: begin // tecla 3
                              mode <= 1'b1;
                              vga <= 1'b1;
                            end
               8'h25,8'h6B: begin // tecla 4
                              memtestf <= 1'b1;
                            end
               8'h2E,8'h73: begin // tecla 5
                              memtests <= 1'b1;
                            end
               8'h3D,8'h6C: begin // tecla 7
                              sdtest <= 1'b1;
                            end
               8'h3E,8'h75: begin // tecla 8
                              flashtest <= 1'b1;
                            end
               8'h46,8'h7D: begin // tecla 9
                              mousetest <= 1'b1;
                            end
               8'h66      : begin // tecla BKSPACE
                              return_to_bios <= 1'b1;
                            end
            endcase
         end
      end
   end
endmodule
