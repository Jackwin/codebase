/*
Copyright 2022 Chunjie Wang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

`timescale 1ps/1ps

module rom #(
    parameter ROM_ADDR_WIDTH = 8,
    parameter ROM_DATA_WIDTH = 32
) (
    input                         clk,
    input [ROM_DATA_WIDTH-1:0]    i_wr_data,
    input                         i_we,
    input [ROM_ADDR_WIDTH-1:0]    i_addr,

    output [ROM_DATA_WIDTH-1:0]   o_rd_data
);

localparam ROM_DEPTH = 2 ** ROM_DATA_WIDTH;

logic [ROM_DATA_WIDTH-1:0] rom_bank [ROM_DEPTH-1:0];

logic [ROM_DATA_WIDTH]-1:0] rom_wr_data_1r;
logic                       rom_we_1r;
logic [ROM_ADDR_WIDTH-1:0]  rom_addr_1r;


always @(posedge clk) begin
    rom_wr_data_1r <= i_wr_data;
    rom_we_1r <= i_we;
    rom_addr_1r <= i_addr;

    if (rom_we_1r) begin
        rom_bank[rom_addr_1r] <= rom_wr_data_1r;
    end
end

assign o_rd_data = rom_bank[rom_addr_1r];
    
endmodule