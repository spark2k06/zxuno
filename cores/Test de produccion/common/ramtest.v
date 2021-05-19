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
module ramtest (
   input wire clkf,
   input wire clks,
   input wire rstf,
   input wire rsts,
   output reg [20:0] sram_a,
   inout wire [7:0] sram_d,
   output reg sram_we_n,
   output reg test_in_progress,
   output reg [1:0] test_result
   );
   
   initial begin
      sram_a = 21'h000000;
      sram_we_n = 1'b1;
      test_in_progress = 1'b1;
      test_result = 2'd0;		
   end
   
   reg [3:0] estado = INIT, retorno_de_read = INIT, retorno_de_write = INIT;
   parameter
      INIT = 4'd0,
      DATOINICIAL = 4'd1,
      INCWRITE = 4'd2,
      CHECK = 4'd3,      
      CHECK2 = 4'd4,
      INCCHECK = 4'd5,
      HALT = 4'd6,
      READ = 4'd7,           
      WRITE = 4'd8,
      WRITE1 = 4'd9,
      WRITE2 = 4'd10
      ;
      
   parameter
      ENDADDRESS = 21'h1FFFFF,
		// SRAM Checksums
		CHECKSUM512KB = 32'h11000000, // 512KB		
		CHECKSUM1024KB = 32'h0CC00000, // 1024KB
		CHECKSUM2048KB = 32'h07F80000; // 2048KB
      
	reg [31:0] checksum;
   reg [7:0] sram_dout, data;
   wire [7:0] sram_din = sram_d;	
   assign sram_d = (sram_we_n == 1'b0)? sram_dout : 8'hZZ;
   
   wire clk;
   reg fastslow = 1'b0;
   BUFGMUX (
      .I0(clkf),
      .I1(clks),
      .O(clk),
      .S(fastslow)
      );
   
   always @(posedge clk) begin	   
      case (estado)
		// Fase 1. Volcado de datos
         INIT:
            begin
               test_in_progress <= 1'b1;
               test_result <= 2'd0;
					checksum <= 32'h00000000;
               sram_a <= 21'h000000;
               sram_dout <= 8'b00010001;					
               estado <= DATOINICIAL;
            end
         DATOINICIAL:
            begin
               estado <= WRITE;
               retorno_de_write <= INCWRITE;
            end
         INCWRITE:
            begin
               if (sram_a == ENDADDRESS) begin
					   sram_a <= 21'h000000;
                  estado <= CHECK;
               end				   
               else if ((sram_a & 21'h07FFFF) == 21'h07FFFF) begin						
						sram_dout <= sram_dout << 1;
						sram_a <= sram_a + 21'd1;
                  estado <= DATOINICIAL;
               end
               else begin
                  sram_a <= sram_a + 1'd1;
                  estado <= DATOINICIAL;
               end
            end			
			
			// Fase 2. Lectura de datos, obtencion de checksum y determinacion de modelo de SRAM
         CHECK:
            begin
               estado <= READ;
               retorno_de_read <= CHECK2;
            end
			CHECK2:
            begin				   
               checksum <= checksum + data;					
               estado <= INCCHECK;               
            end
         INCCHECK:
            begin
				   if (sram_a == ENDADDRESS) begin						
						if (checksum == CHECKSUM512KB) begin
							test_result <= 2'd1;
						end
						else if (checksum == CHECKSUM1024KB) begin
							test_result <= 2'd2;
						end
						else if (checksum == CHECKSUM2048KB) begin
							test_result <= 2'd3;
						end						
						else begin
							test_result <= 2'd0;                  
						end		
						test_in_progress <= 1'b0;
						estado <= HALT;						
               end
               else begin                  
						sram_a <= sram_a + 21'd1;
						estado <= CHECK;
               end
            end
         
         HALT:
            begin
               if (rstf == 1'b1) begin
                  fastslow <= 1'b0;
                  estado <= INIT;
               end
               else if (rsts == 1'b1) begin
                  fastslow <= 1'b1;
                  estado <= INIT;
               end
            end
				
         READ:			
            begin
               data <= sram_din;					
               estado <= retorno_de_read;					
            end
         
         WRITE:			
            begin
               sram_we_n <= 1'b0;
               estado <= WRITE2;
            end
         WRITE2:			
            begin
               sram_we_n <= 1'b1;
               estado <= retorno_de_write;					
            end

      endcase
   end
endmodule
