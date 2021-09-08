`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    10:25:14 02/11/2016 
// Design Name: 
// Module Name:    memorycontroller 
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

// Asynchronous SRAM controller for byte access
// After outputting a byte to read, the result is available 70ns later.

module MemoryController  #(parameter RAMType = 0) (
  input clk,
  input read_a,             // Set to 1 to read from RAM
  input read_b,             // Set to 1 to read from RAM
  input write,              // Set to 1 to write to RAM
  input [21:0] addr,        // Address to read / write
  input [7:0] din,          // Data to write
  output reg [7:0] dout_a,  // Last read data a
  output reg [7:0] dout_b,  // Last read data b
  output reg busy,          // 1 while an operation is in progress

  output MemWR,         // Write Enable. WRITE when Low.
  output [20:0] MemAdr,
  inout [7:0] MemDB,
  input [13:0] debugaddr,
  output [15:0] debugdata
  );
					  
  reg MemOE;
  reg RamWR;
  reg sramWR  = 1'b1;
  
  reg [7:0] data_to_write;
  
  wire [7:0] vram_dout;
  wire [7:0] ram_dout;
  wire [7:0] prgrom_dout;
  wire [7:0] chrrom_dout;
  wire [7:0] prgram_dout;
  
  wire prgrom_ena;
  wire chrrom_ena;
  wire vram_ena;
  wire ram_ena;
  wire prgram_ena;  
  
// RAM en placa 512K o 2M
generate
  if (RAMType == 0) begin :R
	  assign prgrom_ena = addr[21:18] == 4'b0000;
	  assign chrrom_ena = addr[21:18] == 4'b1000;
	  assign vram_ena =   addr[21:18] == 4'b1100;
	  assign ram_ena =    addr[21:18] == 4'b1110;
	  assign prgram_ena = addr[21:18] == 4'b1111;
	  
	  parameter ROMsz = 17;
	  
	  assign MemAdr[20:19] = 2'b00;
	  
  end else begin :R
	  assign prgrom_ena = addr[21:20] == 2'b00;
	  assign chrrom_ena = addr[21:20] == 2'b10;
	  assign vram_ena =   addr[20:18] == 3'b100;
	  assign ram_ena =    addr[20:18] == 3'b110;
	  assign prgram_ena = addr[20:18] == 3'b111;
	  
	  parameter ROMsz = 19;	  
	  
  end
endgenerate	  
  
  reg [R.ROMsz+1:0] MemAdrReg;
  
  assign MemAdr[R.ROMsz+1:0] = MemAdrReg;

  wire [7:0] memory_dout = prgrom_ena ? prgrom_dout :
                           chrrom_ena ? chrrom_dout : 
                           vram_ena ? vram_dout : 
                           ram_ena ? ram_dout : prgram_dout;

  ram2k vram(clk, vram_ena, RamWR, addr[10:0], data_to_write, vram_dout); // VRAM in BRAM
  ram2k ram(clk, ram_ena, RamWR, addr[10:0], data_to_write, ram_dout); // RAM in BRAM
  ram8k prg_ram(clk, prgram_ena, RamWR, addr[12:0], data_to_write, prgram_dout); // Cart RAM in BRAM


  assign chrrom_dout = MemDB;
  assign prgrom_dout = MemDB;
  assign MemDB = (!sramWR) ? data_to_write : 8'bz;
//  assign MemAdr = MemAdrReg;
  assign MemWR = sramWR;

  reg [1:0] cycles;
  reg r_read_a;
  
  always @(posedge clk) begin
    // Initiate read or write
    if (!busy) begin
      if (read_a || read_b || write) begin
		  if (prgrom_ena) begin
		    MemAdrReg <= {1'b0, addr[R.ROMsz:0]}; // PRGROM in SRAM
//		    MemAdrReg <= {1'b0, addr[19:0]}; // PRGROM in SRAM
		  end else if (chrrom_ena) begin
		    MemAdrReg <= {1'b1, addr[R.ROMsz:0]}; // CHRROM in SRAM
//		    MemAdrReg <= {1'b1, addr[19:0]}; // CHRROM in SRAM
		  end
        RamWR <= write;
		  sramWR <= !((write == 1) && (prgrom_ena || chrrom_ena));
        MemOE <= !(write == 0);
        busy <= 1;
        data_to_write <= din;
        cycles <= 0;
        r_read_a <= read_a;
      end else begin
        MemOE <= 1;
        RamWR <= 0;
		  sramWR <= 1;
        busy <= 0;
        cycles <= 0;
      end
    end else begin
      if (cycles == 2) begin
        // Now we have waited 3x45 = 135ns, latch incoming data on read.
        if (!MemOE) begin
          if (r_read_a) dout_a <= memory_dout;
          else dout_b <= memory_dout;
        end
        MemOE <= 1; // Deassert Output Enable.
        RamWR <= 0; // Deassert Write
		  sramWR <= 1;
        busy <= 0;
        cycles <= 0;
      end else begin
        cycles <= cycles + 1;
      end
    end
  end
endmodule  // MemoryController
