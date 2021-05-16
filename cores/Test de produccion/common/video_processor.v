`timescale 1ns / 1ps
`default_nettype none
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    19:31:14 10/18/2012 
// Design Name: 
// Module Name:    dummy_ula 
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

module background (
    input wire clk,
    input wire mode,
    output reg [2:0] r,
    output reg [2:0] g,
    output reg [2:0] b,
    output wire [8:0] hc,
    output wire [8:0] vc,
    output wire hsync,
    output wire vsync,
    output wire csync    
    );

    wire blank;

    sync_generator_pal_ntsc sincronismos (
    .clk(clk),   // 7 MHz
    .in_mode(mode),  // 0: PAL, 1: NTSC
    .csync_n(csync),
    .hsync_n(hsync),
    .vsync_n(vsync),
    .hc(hc),
    .vc(vc),
    .blank(blank)
    );
    
    always @* begin
        if (blank == 1'b1) begin
            {g,r,b} = 9'b000000000;
        end
        else begin
            {g,r,b} = 9'b000000000;
            if (hc >= (39*0) && hc <= (39*1-1)) begin
                {g,r,b} = 9'b100000000;
            end
            if (hc >= (39*1) && hc <= (39*2-1)) begin
                {g,r,b} = 9'b010000000;
            end
            if (hc >= (39*2) && hc <= (39*3-1)) begin
                {g,r,b} = 9'b001000000;
            end
            if (hc >= (39*3) && hc <= (39*4-1)) begin
                {g,r,b} = 9'b000100000;
            end
            if (hc >= (39*4) && hc <= (39*5-1)) begin
                {g,r,b} = 9'b000010000;
            end
            if (hc >= (39*5) && hc <= (39*6-1)) begin
                {g,r,b} = 9'b000001000;
            end
            if (hc >= (39*6) && hc <= (39*7-1)) begin
                {g,r,b} = 9'b000000100;
            end
            if (hc >= (39*7) && hc <= (39*8-1)) begin
                {g,r,b} = 9'b000000010;
            end
            if (hc >= (39*8) && hc <= (39*9-1)) begin
                {g,r,b} = 9'b000000001;
            end
        end
    end        
endmodule

module window_on_background (
    input wire clk,
    input wire mode,
    input wire [8:0] addr,
    input wire [7:0] data,
    input wire we,
    output reg [2:0] r,
    output reg [2:0] g,
    output reg [2:0] b,
    output wire hsync,
    output wire vsync,
    output wire csync
    );
   
    parameter
      BEGINX = 9'd40,
      ENDX = 9'd296,
      BEGINY = 9'd64,
      ENDY = 9'd192;

    wire [2:0] rb,gb,bb;
    wire [8:0] hc,vc;
   
    background bg (
    .clk(clk),
    .mode(mode),
    .r(rb),
    .g(gb),
    .b(bb),
    .hc(hc),
    .vc(vc),
    .hsync(hsync),
    .vsync(vsync),
    .csync(csync)
    );
    
    reg [7:0] charrom[0:2047];
    initial begin
      $readmemh ("CP437.hex", charrom);
    end
    
    wire in_text_window = (hc >= BEGINX && hc < ENDX && vc >= BEGINY && vc < ENDY);
    wire showing_text_window = (hc >= (BEGINX+9'd8) && hc < (ENDX+9'd8) && vc >= BEGINY && vc < ENDY);
    
    reg [7:0] chc = 8'h00;
    reg [6:0] cvc = 7'h00;
    reg [7:0] shiftreg;
    reg [7:0] character;
    wire [7:0] dout;
    reg [8:0] charaddr = 9'd0;

   screenfb buffer_pantalla (
      .clk(clk),
      .addr_read(charaddr),
      .addr_write(addr),
      .we(we),
      .din(data),
      .dout(dout)
   );

   always @(posedge clk) begin
      // H and C counters for text window
      if (hc == (BEGINX-9'd1)) begin  // empezamos a contar 8 pixeles antes, para tener ya el shiftreg cargado cuando comencemos de verdad
         chc <= 8'd0;
         if (vc == BEGINY)
            cvc <= 7'd0;
         else
            cvc <= cvc + 7'd1;
      end
      else begin
         chc <= chc + 8'd1;
      end

      // char generator
      if (in_text_window) begin
         if (chc[2:0] == 3'b010) begin
            charaddr <= {cvc[6:3],5'b00000} + {2'b00,chc[7:3]};
         end
         if (chc[2:0] == 3'b100) begin         
            character <= dout; // lee el caracter siguiente
         end
         if (chc[2:0] == 3'b111) begin
            shiftreg <= charrom[{character,cvc[2:0]}];
         end
      end
      if (showing_text_window && chc[2:0] != 3'b111) begin
         shiftreg <= {shiftreg[6:0],1'b0};
      end
    end
    
    always @* begin
      {r,g,b} = {rb,gb,bb};
      if (showing_text_window)  // ventana de 32x16 caracteres de 8x8
         {r,g,b} = {9{shiftreg[7]}};  // texto blanco sobre fondo negro
    end
endmodule

module screenfb (
   input wire clk,
   input wire [8:0] addr_read,
   input wire [8:0] addr_write,
   input wire we,
   input wire [7:0] din,
   output reg [7:0] dout
   );
   
   reg [7:0] screenrom[0:511];
   initial begin
     $readmemh ("texto_inicial.hex", screenrom);
   end
    
   always @(posedge clk) begin
      dout <= screenrom[addr_read];
      if (we)
         screenrom[addr_write] <= din;
   end
endmodule

module teletype (
   input wire clk,
   input wire mode,
   input wire [7:0] chr,
   input wire we,
   output reg busy,
   output wire [2:0] r,
   output wire [2:0] g,
   output wire [2:0] b,
   output wire hsync,
   output wire vsync,
   output wire csync
   );

   reg [8:0] addr = 9'd0;
   reg [7:0] data = 8'h00, dscreen = 8'h00;
   reg wescreen = 1'b0;
   initial busy = 1'b0;
    
   window_on_background screen (
    .clk(clk),
    .mode(mode),
    .addr(addr),
    .data(dscreen),
    .we(wescreen),
    .r(r),
    .g(g),
    .b(b),
    .hsync(hsync),
    .vsync(vsync),
    .csync(csync)
    );
    
   parameter
      IDLE = 4'd0,
      PCOMMAND = 4'd1,
      ATR = 4'd3,
      ATC = 4'd4,
      CLS = 4'd5,
      PUTCHAR = 4'd6      
      ;
      
   parameter
      AT = 8'd22,
      CR = 8'd13,
      HOME = 8'd12
      ;
            
   reg [2:0] estado = IDLE;
   reg [3:0] row = 4'd0;
    
   always @(posedge clk) begin
      case (estado)
         IDLE,ATR,ATC: 
            begin
               if (we) begin
                  data <= chr;
                  if (estado == ATR) begin
                     row <= chr[3:0];
                     estado <= ATC;
                  end
                  else if (estado == ATC) begin
                     addr <= {row,chr[4:0]};
                     estado <= IDLE;
                  end
                  else begin
                     busy <= 1'b1;
                     estado <= PCOMMAND;
                  end                  
               end
            end
         PCOMMAND:
            begin
               if (data == AT) begin
                  busy <= 1'b0;
                  estado <= ATR;               
               end
               else if (data == HOME) begin
                  addr <= 9'd0;
                  wescreen <= 1'b1;
                  dscreen <= 8'h20;
                  estado <= CLS;
               end
               else if (data == CR) begin
                  addr <= {(addr[8:5] + 4'd1),5'b0000};
                  busy <= 1'b0;
                  estado <= IDLE;
               end
               else begin
                  dscreen <= data;
                  wescreen <= 1'b1;
                  estado <= PUTCHAR;
               end
            end
         CLS:
            begin
               if (addr == 9'h1FF) begin
                  busy <= 1'b0;
                  estado <= IDLE;
                  wescreen <= 1'b0;
                  addr <= 9'd0;
               end
               else
                  addr <= addr + 9'd1;
            end
         PUTCHAR:
            begin
               wescreen <= 1'b0;
               busy <= 1'b0;
               addr <= addr + 9'd1;
               estado <= IDLE;
            end
      endcase
   end
endmodule    

module updater (
   input wire clk,
   input wire mode,
   //--------------------------
   input wire vga,
   input wire [56:0] dna,
   input wire memtest_progress,
   input wire [1:0] memtest_result,
   input wire [5:0] joystick,
   input wire [7:0] earcode,
   input wire sdtest_progress,
   input wire sdtest_result,
   input wire flashtest_progress,
   input wire flashtest_result,
   input wire [2:0] mousebutton,
   //--------------------------
   output wire [2:0] r,
   output wire [2:0] g,
   output wire [2:0] b,
   output wire hsync,
   output wire vsync,
   output wire csync
   );
      
   reg [7:0] chr = 8'd0;
   reg we = 1'b0;
   wire busy;
   
   teletype teletipo (
     .clk(clk),
     .mode(mode),
     .chr(chr),
     .we(we),
     .busy(busy),
     .r(r),
     .g(g),
     .b(b),
     .hsync(hsync),
     .vsync(vsync),
     .csync(csync)
     );
   
   reg [59:0] regdna = 60'h000000000000000;
   reg [3:0] cntdigitsdna = 4'd0;
   reg [7:0] hexvalues[0:15];
   reg [7:0] stringlist[0:2047];
   integer i;
   initial begin
      for (i=0;i<10;i=i+1) begin
         hexvalues[i] = "0" + i;
      end
      for (i=10;i<16;i=i+1) begin
         hexvalues[i] = "A" - 10 + i;
      end
      for (i=0;i<2048;i=i+1) begin
         stringlist[i] = 8'hFF;
      end
      stringlist[0] = 8'd22;
      stringlist[1] = 8'd3;
      stringlist[2] = 8'd10;
      stringlist[3] = "V";
      stringlist[4] = "G";
      stringlist[5] = "A";
      stringlist[6] = " ";
      stringlist[7] = 8'hFF;
      
      stringlist[8] = 8'd22;
      stringlist[9] = 8'd3;
      stringlist[10] = 8'd10;
      stringlist[11] = "N";
      stringlist[12] = "T";
      stringlist[13] = "S";
      stringlist[14] = "C";
      stringlist[15] = 8'hFF;

      stringlist[16] = 8'd22;
      stringlist[17] = 8'd3;
      stringlist[18] = 8'd10;
      stringlist[19] = "P";
      stringlist[20] = "A";
      stringlist[21] = "L";
      stringlist[22] = " ";
      stringlist[23] = 8'hFF;
      
      stringlist[24] = 8'd22;
      stringlist[25] = 8'd4;
      stringlist[26] = 8'd10;
      stringlist[27] = 8'hFF;
      
      stringlist[28] = 8'd22;
      stringlist[29] = 8'd6;
      stringlist[30] = 8'd25;
      stringlist[31] = 8'hFF;
      
      stringlist[32] = "O";
      stringlist[33] = "K";
      stringlist[34] = " ";
      stringlist[35] = " ";
      stringlist[36] = " ";
      stringlist[37] = 8'hFF;
      
      stringlist[38] = "E";
      stringlist[39] = "R";
      stringlist[40] = "R";
      stringlist[41] = "O";
      stringlist[42] = "R";
		stringlist[43] = " ";
      stringlist[44] = 8'hFF;
      
      stringlist[45] = "w";
      stringlist[46] = "a";
      stringlist[47] = "i";
      stringlist[48] = "t";
      stringlist[49] = " ";
		stringlist[50] = " ";
      stringlist[51] = 8'hFF;
      
      stringlist[52] = 8'd22;
      stringlist[53] = 8'd7;
      stringlist[54] = 8'd25;
      stringlist[55] = "U";
      stringlist[56] = "D";
      stringlist[57] = "L";
      stringlist[58] = "R";
      stringlist[59] = "1";
      stringlist[60] = "2";
      stringlist[61] = 8'hFF;

      stringlist[62] = 8'd22;
      stringlist[63] = 8'd8;
      stringlist[64] = 8'd25;
      stringlist[65] = 8'hFF;

      stringlist[66] = 8'd22;
      stringlist[67] = 8'd11;
      stringlist[68] = 8'd25;
      stringlist[69] = " ";
      stringlist[70] = " ";
      stringlist[71] = " ";
      stringlist[72] = 8'hFF;

      stringlist[78] = 8'd22;
      stringlist[79] = 8'd9;
      stringlist[80] = 8'd25;
      stringlist[81] = 8'hFF;

      stringlist[82] = 8'd22;
      stringlist[83] = 8'd10;
      stringlist[84] = 8'd25;
      stringlist[85] = 8'hFF;
		
		stringlist[86] = "5";
      stringlist[87] = "1";
      stringlist[88] = "2";
      stringlist[89] = "K";
      stringlist[90] = "B";
		stringlist[91] = " ";		
      stringlist[92] = 8'hFF;
		
		stringlist[93] = "1";
      stringlist[94] = "0";
      stringlist[95] = "2";
      stringlist[96] = "4";
      stringlist[97] = "K";
		stringlist[98] = "B";		
      stringlist[99] = 8'hFF;
		
      stringlist[100] = "2";
      stringlist[101] = "0";
      stringlist[102] = "4";
      stringlist[103] = "8";
      stringlist[104] = "K";
		stringlist[105] = "B";		
      stringlist[106] = 8'hFF;
		
		
   end
   reg [10:0] addrstr = 11'd0;

   parameter
      ADDRVGA = 11'd0,
      ADDRNTSC = 11'd8,
      ADDRPAL = 11'd16,
      ADDRATDNA = 11'd24,
      ADDRATMEM = 11'd28,
      ADDROK = 11'd32,
      ADDRERROR = 11'd38,
      ADDRINPROGRESS = 11'd45,
      ADDRJOYSTATE = 11'd52,
      ADDREAR = 11'd62,
      ADDRMOUSE = 11'd66,
      ADDRATSD = 11'd78,
      ADDRATFLASH = 11'd82,
		ADDR512KB = 11'd86,
		ADDR1024KB = 11'd93,
		ADDR2048KB = 11'd100
      ;
   
   reg [4:0] estado = PUTVIDEO, 
             retorno_de_sendchar = PUTVIDEO, 
             retorno_de_sendstr = PUTVIDEO;
   parameter
      PUTVIDEO = 5'd0,
      PUTDNA = 5'd1,
      PUTDNA1 = 5'd2,
      PUTRAMTEST = 5'd3,
      PUTRAMTEST1 = 5'd4,
      PUTJOYTEST = 5'd5,
      PUTEARTEST = 5'd6,
      PUTSDTEST = 5'd7,
      PUTSDTEST1 = 5'd8,
      PUTFLASHTEST = 5'd9,
      PUTFLASHTEST1 = 5'd10,
      PUTEARTEST1 = 5'd11,
      PUTMOUSETEST = 5'd12,
      SENDCHAR = 5'd28,
      SENDCHAR1 = 5'd29,
      SENDSTR = 5'd30,
      SENDSTR1 = 5'd31
      ;
      
   always @(posedge clk) begin
      case (estado)
         PUTVIDEO:
            begin
               if (vga == 1'b1)
                  addrstr <= ADDRVGA;
               else if (mode == 1'b0)
                  addrstr <= ADDRPAL;
               else
                  addrstr <= ADDRNTSC;
               estado <= SENDSTR;
               retorno_de_sendstr <= PUTDNA;
            end
            
         PUTDNA:
            begin
               addrstr <= ADDRATDNA;
               estado <= SENDSTR;
               retorno_de_sendstr <= PUTDNA1;
               regdna <= {3'b000,dna};
               cntdigitsdna <= 4'd0;
            end
         PUTDNA1:
            begin
               if (cntdigitsdna == 4'd15)
                  estado <= PUTRAMTEST;
               else begin
                  cntdigitsdna <= cntdigitsdna + 4'd1;
                  chr <= hexvalues[regdna[59:56]];
                  regdna <= {regdna[55:0],4'b0000};
                  retorno_de_sendchar <= PUTDNA1;
                  estado <= SENDCHAR;
               end
            end
            
         PUTRAMTEST:
            begin
               addrstr <= ADDRATMEM;
               estado <= SENDSTR;
               retorno_de_sendstr <= PUTRAMTEST1;
            end         
         PUTRAMTEST1:
            begin
               if (memtest_progress == 1'b1)
                  addrstr <= ADDRINPROGRESS;
               else if (memtest_result == 2'd1)
                  addrstr <= ADDR512KB;
					else if (memtest_result == 2'd2)
                  addrstr <= ADDR1024KB;
					else if (memtest_result == 2'd3)
                  addrstr <= ADDR2048KB;
               else
                  addrstr <= ADDRERROR;
               estado <= SENDSTR;
               retorno_de_sendstr <= PUTJOYTEST;
            end
            
         PUTJOYTEST:
            begin
               stringlist[ADDRJOYSTATE+3] <= (joystick[5] == 1'b1)? "U" : " ";
               stringlist[ADDRJOYSTATE+4] <= (joystick[4] == 1'b1)? "D" : " ";
               stringlist[ADDRJOYSTATE+5] <= (joystick[3] == 1'b1)? "L" : " ";
               stringlist[ADDRJOYSTATE+6] <= (joystick[2] == 1'b1)? "R" : " ";
               stringlist[ADDRJOYSTATE+7] <= (joystick[1] == 1'b1)? "1" : " ";
               stringlist[ADDRJOYSTATE+8] <= (joystick[0] == 1'b1)? "2" : " ";
               addrstr <= ADDRJOYSTATE;
               estado <= SENDSTR;
               retorno_de_sendstr <= PUTEARTEST;
            end
            
         PUTEARTEST:
            begin
               addrstr <= ADDREAR;
               estado <= SENDSTR;
               retorno_de_sendstr <= PUTEARTEST1;
            end
         PUTEARTEST1:
            begin
               chr <= earcode;
               estado <= SENDCHAR;
               retorno_de_sendchar <= PUTSDTEST;
            end

         PUTSDTEST:
            begin
               addrstr <= ADDRATSD;
               estado <= SENDSTR;
               retorno_de_sendstr <= PUTSDTEST1;
            end
         PUTSDTEST1:
            begin
               if (sdtest_progress == 1'b1)
                  addrstr <= ADDRINPROGRESS;
               else if (sdtest_result == 1'b1)
                  addrstr <= ADDROK;
               else
                  addrstr <= ADDRERROR;
               estado <= SENDSTR;
               retorno_de_sendstr <= PUTFLASHTEST;
            end
            
         PUTFLASHTEST:
            begin
               addrstr <= ADDRATFLASH;
               estado <= SENDSTR;
               retorno_de_sendstr <= PUTFLASHTEST1;
            end
         PUTFLASHTEST1:
            begin
               if (flashtest_progress == 1'b1)
                  addrstr <= ADDRINPROGRESS;
               else if (flashtest_result == 1'b1)
                  addrstr <= ADDROK;
               else
                  addrstr <= ADDRERROR;
               estado <= SENDSTR;
               retorno_de_sendstr <= PUTMOUSETEST;
            end
            
         PUTMOUSETEST:
            begin
               stringlist[ADDRMOUSE+3] <= (mousebutton[0]==1'b1)? "L" : " ";
               stringlist[ADDRMOUSE+4] <= (mousebutton[2]==1'b1)? "M" : " ";
               stringlist[ADDRMOUSE+5] <= (mousebutton[1]==1'b1)? "R" : " ";
               addrstr <= ADDRMOUSE;
               estado <= SENDSTR;
               retorno_de_sendstr <= PUTVIDEO;
            end
               
         SENDSTR:
            begin
               chr <= stringlist[addrstr];
               addrstr <= addrstr + 11'd1;
               estado <= SENDSTR1;
            end
         SENDSTR1:
            begin
               if (chr == 8'hFF)
                  estado <= retorno_de_sendstr;
               else begin
                  estado <= SENDCHAR;
                  retorno_de_sendchar <= SENDSTR;
               end
            end
         
         SENDCHAR:
            begin
               if (busy == 1'b0) begin
                  we <= 1'b1;
                  estado <= SENDCHAR1;
               end
            end
         SENDCHAR1:
            begin
               we <= 1'b0;
               estado <= retorno_de_sendchar;
            end
      endcase
   end
endmodule

