`timescale 1ns / 1ps
`default_nettype none

//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    04:05:57 08/18/2016 
// Design Name: 
// Module Name:    ramtest 
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

module flashtest (
   input wire clk,
   input wire rst,
   output wire spi_clk,    //
   output wire spi_di,     // Interface SPI
   input wire spi_do,      //
   output reg spi_cs,     //
   output reg test_in_progress,
   output reg test_result
   );
   
   initial test_in_progress = 1'b1;
   initial test_result = 1'b0;
   
   reg send_data = 1'b0;
   reg receive_data = 1'b0;
   reg [7:0] data_to_flash = 8'hFF;
   wire [7:0] data_from_flash;
   wire ready;
   initial spi_cs = 1'b1;
   reg [15:0] delay = 16'h0000;
   
   spi chipflash (
      .clk(clk),         // 7MHz
      .enviar_dato(send_data), // a 1 para indicar que queremos enviar un dato por SPI
      .recibir_dato(receive_data), // a 1 para indicar que queremos recibir un dato
      .din(data_to_flash),   // del bus de datos de salida de la CPU
      .dout(data_from_flash),  // al bus de datos de entrada de la CPU
      .wait_n(ready),
      .spi_clk(spi_clk),   // Interface SPI
      .spi_di(spi_di),     //
      .spi_do(spi_do)      //
   );
   
   reg [3:0] estado = INIT, retorno_de_sendspi = INIT, retorno_de_recvspi = INIT;
   parameter
      INIT = 4'd0,
      SENDJEDECID = 4'd1,
      CHECK = 4'd4,
      HALT = 4'd6,
      SENDSPI = 4'd7,
      WAIT1CLKSEND = 4'd9,
      WAITSEND = 4'd10,
      RECVSPI = 4'd11,
      WAIT1CLKRECV = 4'd13,
      WAITRECV = 4'd14
      ;
   always @(posedge clk) begin
      case (estado)
         INIT:
            begin
               delay <= delay + 16'd1;
               if (delay == 16'hFFFF)
                  estado <= SENDJEDECID;
            end
         SENDJEDECID:
            begin
               spi_cs <= 1'b0;
               test_in_progress <= 1'b1;
               test_result <= 1'b0;
               data_to_flash <= 8'h9F; // comando READ JEDEC ID
               estado <= SENDSPI;
               retorno_de_sendspi <= RECVSPI;
               retorno_de_recvspi <= CHECK;
            end         
         CHECK:
            begin
               spi_cs <= 1'b1;
               if (data_from_flash != 8'hEF) begin  // EF = Winbond
                  test_in_progress <= 1'b0;
                  test_result <= 1'b0;
                  estado <= HALT;
               end
               else begin
                  test_in_progress <= 1'b0;
                  test_result <= 1'b1;
                  estado <= HALT;
               end
            end
         HALT:
            if (rst == 1'b1)
               estado <= INIT;
            
         SENDSPI:
            begin
               send_data <= 1'b1;
               estado <= WAIT1CLKSEND;
            end
         WAIT1CLKSEND:
            begin
               send_data <= 1'b0;
               estado <= WAITSEND;
            end
         WAITSEND:
            begin
               if (ready == 1'b1) begin
                  estado <= retorno_de_sendspi;
               end
            end
            
         RECVSPI:
            begin
               receive_data <= 1'b1;
               estado <= WAIT1CLKRECV;
            end
         WAIT1CLKRECV:
            begin
               receive_data <= 1'b0;
               estado <= WAITRECV;
            end
         WAITRECV:
            begin
               if (ready == 1'b1) begin
                  estado <= retorno_de_recvspi;
               end
            end
      endcase
   end      
endmodule
