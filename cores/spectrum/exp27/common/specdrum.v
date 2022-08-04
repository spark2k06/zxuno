`timescale 1ns / 1ps
`default_nettype none

//    This file is part of the ZXUNO Spectrum core. 
//    Creation date is 23:58:32 2017-02-03 by Miguel Angel Rodriguez Jodar
//    (c)2014-2020 ZXUNO association.
//    ZXUNO official repository: http://svn.zxuno.com/svn/zxuno
//    Username: guest   Password: zxuno
//    Github repository for this core: https://github.com/mcleod-ideafix/zxuno_spectrum_core
//
//    ZXUNO Spectrum core is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    ZXUNO Spectrum core is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with the ZXUNO Spectrum core.  If not, see <https://www.gnu.org/licenses/>.
//
//    Any distributed copy of this file must keep this notice intact.

module specdrum (
   input wire clk,
   input wire rst_n,
   input wire [15:0] a,
   input wire iorq_n,
   input wire wr_n,
   input wire [7:0] d,
   output wire[8:0] specdrum_out_left,
   output wire[8:0] specdrum_out_right
   );

wire specdrum    = a[7:0] == 8'hDF;
wire covox       = a[7:0] == 8'hFB;
wire soundrive_a = a[7:0] == 8'h0F;
wire soundrive_b = a[7:0] == 8'h1F;
wire soundrive_c = a[7:0] == 8'h4F;
wire soundrive_d = a[7:0] == 8'h5F;

reg[7:0] l0, l1, r0, r1;

always @(posedge clk, negedge rst_n)
  if(!rst_n) begin
    l0 <= 8'h00;
    l1 <= 8'h00;
    r0 <= 8'h00;
    r1 <= 8'h00;
  end
  else if(!iorq_n && !wr_n) begin
      if(specdrum || covox || soundrive_a) l0 <= d;
      if(specdrum || covox || soundrive_b) l1 <= d;
      if(specdrum || covox || soundrive_c) r0 <= d;
      if(specdrum || covox || soundrive_d) r1 <= d;
  end

assign specdrum_out_left = { 1'b0, l0 }+{ 1'b0, l1 };
assign specdrum_out_right = { 1'b0, r0 }+{ 1'b0, r1 };

endmodule
