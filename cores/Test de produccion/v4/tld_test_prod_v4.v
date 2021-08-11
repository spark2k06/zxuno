`timescale 1ns / 1ps
`default_nettype none

//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    01:24:02 08/15/2016 
// Design Name: 
// Module Name:    tld_test_prod_v4 
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
module tld_test_prod_v4 (
   input wire clk50mhz,
   //---------------------------
   input wire clkps2,
   input wire dataps2,
   //---------------------------
   inout wire mousedata,
   inout wire mouseclk,
   //---------------------------
   input wire ear,
   //---------------------------
   output wire [20:0] sram_addr,
   inout wire [7:0] sram_data,
   output wire sram_we_n,
   //---------------------------
   input wire joyup,
   input wire joydown,
   input wire joyleft,
   input wire joyright,
   input wire joyfire1,
   input wire joyfire2,
   input wire joyfire3,
   //---------------------------
   output wire testled,
   //---------------------------
   output wire sd_cs_n,
   output wire sd_clk,
   output wire sd_mosi,
   input wire sd_miso,
   //---------------------------
   output wire flash_cs_n,
   output wire flash_clk,
   output wire flash_mosi,
   input wire flash_miso,
   //---------------------------
   output wire [2:0] r,
   output wire [2:0] g,
   output wire [2:0] b,
   output wire hsync,
   output wire vsync,
   output wire stdn,
   output wire stdnb,
   output wire audio_out_left,
   output wire audio_out_right
    );

   wire clk100, clk14, clk7;

   wire mode, vga;
   assign joyfire3 = 0;
   assign stdn = mode;
   assign stdnb = ~mode;	

   wire [2:0] r_to_vga, g_to_vga, b_to_vga;
   wire hsync_to_vga, vsync_to_vga, csync_to_vga;
   
   wire memtest_init_fast, memtest_init_slow, memtest_progress;
	wire [2:0] memtest_result;
   wire sdtest_init, sdtest_progress, sdtest_result;
   wire flashtest_init, flashtest_progress, flashtest_result;
   
   wire [7:0] earcode;
   wire [2:0] mousebutton;  // M R L
   wire mousetest_init;
   wire return_to_bios;
   
   wire [56:0] dna;
   get_dna dna_fpga (
      .clk(clk7),
      .dna(dna)
   );
   
   relojes instance_name (
    .CLK_IN1(clk50mhz),
    .CLK_OUT1(clk100),
    .CLK_OUT2(clk14),
    .CLK_OUT3(clk7)
    );

   switch_mode teclas (
      .clk(clk7),
      .clkps2(clkps2),
      .dataps2(dataps2),
      .mode(mode),
      .vga(vga),
      .memtestf(memtest_init_fast),
      .memtests(memtest_init_slow),
      .sdtest(sdtest_init),
      .flashtest(flashtest_init),
      .mousetest(mousetest_init),
      .return_to_bios(return_to_bios)
   );

   ramtest test_de_ram (
      .clkf(clk100),
      .clks(clk14),
      .rstf(memtest_init_fast),
      .rsts(memtest_init_slow),
      .sram_a(sram_addr[20:0]),
      .sram_d(sram_data),
      .sram_we_n(sram_we_n),
      .test_in_progress(memtest_progress),
      .test_result(memtest_result)
   );

   sdtest test_slot_sd (
      .clk(clk7),
      .rst(sdtest_init),
      .spi_clk(sd_clk),
      .spi_di(sd_mosi),
      .spi_do(sd_miso),
      .spi_cs(sd_cs_n),
      .test_in_progress(sdtest_progress),
      .test_result(sdtest_result)
   );

   flashtest test_winbond_spi_flash (
      .clk(clk7),
      .rst(flashtest_init),
      .spi_clk(flash_clk),
      .spi_di(flash_mosi),
      .spi_do(flash_miso),
      .spi_cs(flash_cs_n),
      .test_in_progress(flashtest_progress),
      .test_result(flashtest_result)
   );

   eartest test_ear (
      .clk(clk7),
      .ear(~ear),
      .vs(vsync_to_vga),
      .code(earcode)
   );

   mousetest test_raton (
      .clk(clk7),
      .rst(mousetest_init),
      .ps2clk(mouseclk),
      .ps2data(mousedata),
      .botones(mousebutton)
   );

   updater mensajes (
     .clk(clk7),
     .mode(mode),
     .vga(vga),
     
     .dna(dna),
     .memtest_progress(memtest_progress),
     .memtest_result(memtest_result),
     .joystick(~{joyup,joydown,joyleft,joyright,joyfire1,joyfire2}),
     .earcode(earcode),
     .sdtest_progress(sdtest_progress),
     .sdtest_result(sdtest_result),
     .flashtest_progress(flashtest_progress),
     .flashtest_result(flashtest_result),
     .mousebutton(mousebutton),
     
     .r(r_to_vga),
     .g(g_to_vga),
     .b(b_to_vga),
     .hsync(hsync_to_vga),
     .vsync(vsync_to_vga),
     .csync(csync_to_vga)
     );

   vga_scandoubler #(.CLKVIDEO(7000)) modo_vga (
      .clkvideo(clk7),
      .clkvga(clk14),
      .enable_scandoubling(vga),
      .disable_scaneffect(1'b0),
      .ri(r_to_vga),
      .gi(g_to_vga),
      .bi(b_to_vga),
      .hsync_ext_n(hsync_to_vga),
      .vsync_ext_n(vsync_to_vga),
      .csync_ext_n(csync_to_vga),
      .ro(r),
      .go(g),
      .bo(b),
      .hsync(hsync),
      .vsync(vsync)
   );
   
   audio_test audio (
      .clk(clk14),
      .left(audio_out_left),
      .right(audio_out_right),
      .led(testled)
   );

   multiboot volver_a_bios (
      .clk_icap(clk7),
      .boot(return_to_bios)
   );

endmodule
